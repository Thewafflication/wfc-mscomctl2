/* Black-box check of wfcmscomctl2.dll: exports, version resource and the
 * registration round trip (see dll_check.h). Run by CTest with the DLL,
 * the version and the target as arguments. */
#include "dll_check.h"

#include <ocidl.h>

static const struct wfc_check_class kClasses[] = {
    {L"WFC.MSComCtl2.Animation", L"{7F0C74C8-0C87-5AE2-A134-DAD91A6F99F7}",
     L"MSComCtl2.Animation.2", L"{B09DE715-87C1-11D1-8BE3-0000F8754DA1}"},
    {L"WFC.MSComCtl2.UpDown", L"{14C1B9B8-1541-5BCE-A4B9-7137848B6708}",
     L"MSComCtl2.UpDown.2", L"{603C7E80-87C2-11D1-8BE3-0000F8754DA1}"},
    {L"WFC.MSComCtl2.MonthView", L"{AB9666ED-78F0-5E96-AFB3-A8667C357E80}",
     L"MSComCtl2.MonthView.2", L"{232E456A-87C3-11D1-8BE3-0000F8754DA1}"},
    {L"WFC.MSComCtl2.DTPicker", L"{D79C5747-8AEF-5741-9F7E-5445F5777595}",
     L"MSComCtl2.DTPicker.2", L"{20DD1B9E-87C4-11D1-8BE3-0000F8754DA1}"},
    {L"WFC.MSComCtl2.FlatScrollBar", L"{E5EE73B3-49BC-5CF6-82F1-39EEC8F37260}",
     L"MSComCtl2.FlatScrollBar.2", L"{FE38753A-44A3-11D1-B5B7-0000C09000C4}"},
};

/* The behaviour of an UpDown made through COM: the defaults of the control, a
 * Value outside Min and Max refused with error 380, and Value kept inside the
 * range when it narrows. */
static long wfc_updown_get(IDispatch* control, const wchar_t* name,
                           HRESULT* hr) {
    OLECHAR* names[1];
    DISPID id = 0;
    DISPPARAMS none = {NULL, NULL, 0, 0};
    VARIANT value;
    names[0] = (OLECHAR*)name;
    VariantInit(&value);
    *hr = control->lpVtbl->GetIDsOfNames(control, &IID_NULL, names, 1, 0, &id);
    if (*hr == S_OK) {
        *hr = control->lpVtbl->Invoke(control, id, &IID_NULL, 0,
                                      DISPATCH_PROPERTYGET, &none, &value, NULL,
                                      NULL);
    }
    return *hr == S_OK ? value.lVal : -1;
}

static HRESULT wfc_updown_put(IDispatch* control, const wchar_t* name,
                              long number, EXCEPINFO* exception) {
    OLECHAR* names[1];
    DISPID id = 0;
    DISPID named = DISPID_PROPERTYPUT;
    VARIANT value;
    DISPPARAMS put;
    names[0] = (OLECHAR*)name;
    VariantInit(&value);
    value.vt = VT_I4;
    value.lVal = number;
    put.rgvarg = &value;
    put.rgdispidNamedArgs = &named;
    put.cArgs = 1;
    put.cNamedArgs = 1;
    if (control->lpVtbl->GetIDsOfNames(control, &IID_NULL, names, 1, 0, &id) !=
        S_OK) {
        return DISP_E_UNKNOWNNAME;
    }
    return control->lpVtbl->Invoke(control, id, &IID_NULL, 0,
                                   DISPATCH_PROPERTYPUT, &put, NULL, exception,
                                   NULL);
}

static void wfc_free_exception(EXCEPINFO* exception) {
    SysFreeString(exception->bstrSource);
    SysFreeString(exception->bstrDescription);
    memset(exception, 0, sizeof(*exception));
}

/* An event sink that counts the events it is sent. */
struct wfc_sink {
    IDispatchVtbl* lpVtbl;
    long count;
    DISPID last;
};

static HRESULT __stdcall wfc_sink_query(IDispatch* self, REFIID iid,
                                        void** object) {
    if (IsEqualIID(iid, &IID_IUnknown) || IsEqualIID(iid, &IID_IDispatch)) {
        *object = self;
        return S_OK;
    }
    *object = NULL;
    return E_NOINTERFACE;
}

static ULONG __stdcall wfc_sink_ref(IDispatch* self) {
    (void)self;
    return 2;
}

static HRESULT __stdcall wfc_sink_count(IDispatch* self, UINT* count) {
    (void)self;
    *count = 0;
    return S_OK;
}

static HRESULT __stdcall wfc_sink_type(IDispatch* self, UINT i, LCID l,
                                       ITypeInfo** info) {
    (void)self;
    (void)i;
    (void)l;
    *info = NULL;
    return E_NOTIMPL;
}

static HRESULT __stdcall wfc_sink_names(IDispatch* self, REFIID iid,
                                        LPOLESTR* names, UINT n, LCID l,
                                        DISPID* ids) {
    (void)self;
    (void)iid;
    (void)names;
    (void)n;
    (void)l;
    (void)ids;
    return E_NOTIMPL;
}

static HRESULT __stdcall wfc_sink_invoke(IDispatch* self, DISPID id, REFIID iid,
                                         LCID l, WORD flags, DISPPARAMS* p,
                                         VARIANT* r, EXCEPINFO* e, UINT* a) {
    struct wfc_sink* sink = (struct wfc_sink*)self;
    (void)iid;
    (void)l;
    (void)flags;
    (void)p;
    (void)r;
    (void)e;
    (void)a;
    ++sink->count;
    sink->last = id;
    return S_OK;
}

static IDispatchVtbl wfc_sink_vtbl = {
    wfc_sink_query, wfc_sink_ref,   wfc_sink_ref,   wfc_sink_count,
    wfc_sink_type,  wfc_sink_names, wfc_sink_invoke};

/* Change (dispatch id 1) reaches an advised sink when Value changes, and not
 * after Unadvise. */
static void wfc_check_updown_events(IDispatch* control, const wchar_t* dll) {
    struct wfc_sink sink = {&wfc_sink_vtbl, 0, 0};
    IConnectionPointContainer* container = NULL;
    IConnectionPoint* point = NULL;
    DWORD cookie = 0;
    HRESULT hr;
    if (control->lpVtbl->QueryInterface(control, &IID_IConnectionPointContainer,
                                        (void**)&container) != S_OK ||
        container->lpVtbl->FindConnectionPoint(container, &IID_IDispatch,
                                               &point) != S_OK ||
        point->lpVtbl->Advise(point, (IUnknown*)&sink, &cookie) != S_OK) {
        wfc_check_fail("no connection point for the events of", dll);
    }
    wfc_updown_put(control, L"Value", 3, NULL);
    if (sink.count != 1 || sink.last != 1) {
        wfc_check_fail("Change was not raised for a new Value in", dll);
    }
    wfc_updown_put(control, L"Value", 3, NULL);
    if (sink.count != 1) {
        wfc_check_fail("Change was raised though Value did not change in", dll);
    }
    wfc_updown_put(control, L"Max", 2, NULL);
    if (sink.count != 2 || wfc_updown_get(control, L"Value", &hr) != 2) {
        wfc_check_fail("Change was not raised when Max moved Value in", dll);
    }
    if (point->lpVtbl->Unadvise(point, cookie) != S_OK) {
        wfc_check_fail("Unadvise failed in", dll);
    }
    wfc_updown_put(control, L"Value", 0, NULL);
    if (sink.count != 2) {
        wfc_check_fail("Change reached a sink that had unadvised in", dll);
    }
    point->lpVtbl->Release(point);
    container->lpVtbl->Release(container);
}

static void wfc_check_updown(const wchar_t* dll_path) {
    typedef HRESULT(__stdcall * get_class_object)(REFCLSID, REFIID, void**);
    HMODULE module = LoadLibraryW(dll_path);
    get_class_object get;
    CLSID clsid;
    IClassFactory* factory = NULL;
    IDispatch* control = NULL;
    EXCEPINFO exception;
    HRESULT hr;
    if (module == NULL) {
        wfc_check_fail("cannot load", dll_path);
    }
    get = (get_class_object)(void*)GetProcAddress(module, "DllGetClassObject");
    CLSIDFromString(L"{14C1B9B8-1541-5BCE-A4B9-7137848B6708}", &clsid);
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (get == NULL ||
        get(&clsid, &IID_IClassFactory, (void**)&factory) != S_OK ||
        factory->lpVtbl->CreateInstance(factory, NULL, &IID_IDispatch,
                                        (void**)&control) != S_OK) {
        wfc_check_fail("cannot make an UpDown in", dll_path);
    }
    if (wfc_updown_get(control, L"Min", &hr) != 0 ||
        wfc_updown_get(control, L"Max", &hr) != 10 ||
        wfc_updown_get(control, L"Increment", &hr) != 1 ||
        wfc_updown_get(control, L"Value", &hr) != 0) {
        wfc_check_fail("the defaults of the UpDown are wrong in", dll_path);
    }
    memset(&exception, 0, sizeof(exception));
    hr = wfc_updown_put(control, L"Value", 11, &exception);
    if (hr != DISP_E_EXCEPTION || (exception.scode & 0xFFFF) != 380) {
        wfc_check_fail("Value above Max was not error 380 in", dll_path);
    }
    wfc_free_exception(&exception);
    if (wfc_updown_put(control, L"Max", 100, NULL) != S_OK ||
        wfc_updown_put(control, L"Value", 60, NULL) != S_OK ||
        wfc_updown_get(control, L"Value", &hr) != 60) {
        wfc_check_fail("Value did not follow the range in", dll_path);
    }
    if (wfc_updown_put(control, L"Max", 40, NULL) != S_OK ||
        wfc_updown_get(control, L"Value", &hr) != 40) {
        wfc_check_fail("Value was not brought inside a narrower range in",
                       dll_path);
    }
    /* Measured under VB6: Min above Max is accepted. */
    memset(&exception, 0, sizeof(exception));
    if (wfc_updown_put(control, L"Min", 50, &exception) != S_OK) {
        wfc_check_fail("Min above Max was refused in", dll_path);
    }
    wfc_free_exception(&exception);
    /* Back to a sensible range for the event checks. */
    wfc_updown_put(control, L"Min", 0, NULL);
    wfc_updown_put(control, L"Max", 40, NULL);
    wfc_updown_put(control, L"Value", 40, NULL);
    wfc_check_updown_events(control, dll_path);
    control->lpVtbl->Release(control);
    factory->lpVtbl->Release(factory);
    printf("UpDown: the range is kept through COM\n");
}

/* The behaviour of a FlatScrollBar made through COM: the defaults of the
 * control, and a Value outside Min and Max refused with error 380. */
static void wfc_check_flatscrollbar(const wchar_t* dll_path) {
    typedef HRESULT(__stdcall * get_class_object)(REFCLSID, REFIID, void**);
    HMODULE module = LoadLibraryW(dll_path);
    get_class_object get;
    CLSID clsid;
    IClassFactory* factory = NULL;
    IDispatch* control = NULL;
    EXCEPINFO exception;
    HRESULT hr;
    if (module == NULL) {
        wfc_check_fail("cannot load", dll_path);
    }
    get = (get_class_object)(void*)GetProcAddress(module, "DllGetClassObject");
    CLSIDFromString(L"{E5EE73B3-49BC-5CF6-82F1-39EEC8F37260}", &clsid);
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (get == NULL ||
        get(&clsid, &IID_IClassFactory, (void**)&factory) != S_OK ||
        factory->lpVtbl->CreateInstance(factory, NULL, &IID_IDispatch,
                                        (void**)&control) != S_OK) {
        wfc_check_fail("cannot make a FlatScrollBar in", dll_path);
    }
    if (wfc_updown_get(control, L"Min", &hr) != 0 ||
        wfc_updown_get(control, L"Max", &hr) != 32767 ||
        wfc_updown_get(control, L"SmallChange", &hr) != 1 ||
        wfc_updown_get(control, L"LargeChange", &hr) != 1 ||
        wfc_updown_get(control, L"Value", &hr) != 0) {
        wfc_check_fail("the defaults of the FlatScrollBar are wrong in",
                       dll_path);
    }
    memset(&exception, 0, sizeof(exception));
    if (wfc_updown_put(control, L"Max", 100, NULL) != S_OK) {
        wfc_check_fail("Max was refused by the FlatScrollBar in", dll_path);
    }
    hr = wfc_updown_put(control, L"Value", 101, &exception);
    if (hr != DISP_E_EXCEPTION || (exception.scode & 0xFFFF) != 380) {
        wfc_check_fail("Value above Max was not error 380 in", dll_path);
    }
    wfc_free_exception(&exception);
    if (wfc_updown_put(control, L"Value", 70, NULL) != S_OK ||
        wfc_updown_get(control, L"Value", &hr) != 70) {
        wfc_check_fail("Value did not follow the range in", dll_path);
    }
    control->lpVtbl->Release(control);
    factory->lpVtbl->Release(factory);
    printf("FlatScrollBar: the range is kept through COM\n");
}

/* Calls a member of a late-bound control. `args` are in COM order (the last
 * argument first); a property put has the value first and is named. */
static HRESULT wfc_call(IDispatch* control, const wchar_t* name, WORD flags,
                        VARIANT* args, UINT count, VARIANT* result,
                        EXCEPINFO* exception) {
    OLECHAR* names[1];
    DISPID id = 0;
    DISPID named = DISPID_PROPERTYPUT;
    DISPPARAMS params;
    names[0] = (OLECHAR*)name;
    if (control->lpVtbl->GetIDsOfNames(control, &IID_NULL, names, 1, 0, &id) !=
        S_OK) {
        return DISP_E_UNKNOWNNAME;
    }
    params.rgvarg = args;
    params.cArgs = count;
    params.rgdispidNamedArgs = (flags & DISPATCH_PROPERTYPUT) ? &named : NULL;
    params.cNamedArgs = (flags & DISPATCH_PROPERTYPUT) ? 1 : 0;
    return control->lpVtbl->Invoke(control, id, &IID_NULL, 0, flags, &params,
                                   result, exception, NULL);
}

static VARIANT wfc_date(double serial) {
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_DATE;
    v.date = serial;
    return v;
}

static VARIANT wfc_bool(int flag) {
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_BOOL;
    v.boolVal = flag ? VARIANT_TRUE : VARIANT_FALSE;
    return v;
}

static VARIANT wfc_long(long number) {
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_I4;
    v.lVal = number;
    return v;
}

/* A refused property: error 380 (Invalid property value). */
static int wfc_refused(HRESULT hr, EXCEPINFO* exception) {
    int refused = hr == DISP_E_EXCEPTION && (exception->scode & 0xFFFF) == 380;
    SysFreeString(exception->bstrSource);
    SysFreeString(exception->bstrDescription);
    memset(exception, 0, sizeof(*exception));
    return refused;
}

static double wfc_date_of(IDispatch* control, const wchar_t* name) {
    VARIANT result;
    VariantInit(&result);
    if (wfc_call(control, name, DISPATCH_PROPERTYGET, NULL, 0, &result, NULL) !=
            S_OK ||
        result.vt != VT_DATE) {
        return -1;
    }
    return result.date;
}

static long wfc_short_of(IDispatch* control, const wchar_t* name) {
    VARIANT result;
    VariantInit(&result);
    if (wfc_call(control, name, DISPATCH_PROPERTYGET, NULL, 0, &result, NULL) !=
            S_OK ||
        result.vt != VT_I2) {
        return -1;
    }
    return result.iVal;
}

/* The behaviour of a MonthView made through COM: the parts of Value, the range
 * and the selection rules, the bold days and the days shown. */
static void wfc_check_monthview(const wchar_t* dll_path) {
    typedef HRESULT(__stdcall * get_class_object)(REFCLSID, REFIID, void**);
    HMODULE module = LoadLibraryW(dll_path);
    get_class_object get;
    CLSID clsid;
    IClassFactory* factory = NULL;
    IDispatch* control = NULL;
    EXCEPINFO exception;
    VARIANT args[2];
    VARIANT result;
    HRESULT hr;
    if (module == NULL) {
        wfc_check_fail("cannot load", dll_path);
    }
    get = (get_class_object)(void*)GetProcAddress(module, "DllGetClassObject");
    CLSIDFromString(L"{AB9666ED-78F0-5E96-AFB3-A8667C357E80}", &clsid);
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (get == NULL ||
        get(&clsid, &IID_IClassFactory, (void**)&factory) != S_OK ||
        factory->lpVtbl->CreateInstance(factory, NULL, &IID_IDispatch,
                                        (void**)&control) != S_OK) {
        wfc_check_fail("cannot make a MonthView in", dll_path);
    }
    memset(&exception, 0, sizeof(exception));
    /* 45000 is 2023-03-15, a Wednesday. */
    args[0] = wfc_date(45000);
    if (wfc_call(control, L"Value", DISPATCH_PROPERTYPUT, args, 1, NULL,
                 NULL) != S_OK) {
        wfc_check_fail("cannot set Value in", dll_path);
    }
    if (wfc_short_of(control, L"Day") != 15 ||
        wfc_short_of(control, L"Month") != 3 ||
        wfc_short_of(control, L"Year") != 2023 ||
        wfc_short_of(control, L"DayOfWeek") != 4) {
        wfc_check_fail("the parts of Value are wrong in", dll_path);
    }
    args[0] = wfc_long(31);
    wfc_call(control, L"Day", DISPATCH_PROPERTYPUT, args, 1, NULL, NULL);
    args[0] = wfc_long(2);
    if (wfc_call(control, L"Month", DISPATCH_PROPERTYPUT, args, 1, NULL,
                 NULL) != S_OK ||
        wfc_short_of(control, L"Day") != 28) {
        wfc_check_fail("Month 2 did not bring Day to the month's end in",
                       dll_path);
    }
    args[0] = wfc_long(13);
    if (!wfc_refused(wfc_call(control, L"Month", DISPATCH_PROPERTYPUT, args, 1,
                              NULL, &exception),
                     &exception)) {
        wfc_check_fail("Month 13 was not error 380 in", dll_path);
    }
    args[0] = wfc_date(45000);
    wfc_call(control, L"Value", DISPATCH_PROPERTYPUT, args, 1, NULL, NULL);
    args[0] = wfc_date(45010);
    if (wfc_call(control, L"MinDate", DISPATCH_PROPERTYPUT, args, 1, NULL,
                 NULL) != S_OK ||
        wfc_date_of(control, L"Value") != 45010) {
        wfc_check_fail("MinDate did not bring Value up in", dll_path);
    }
    args[0] = wfc_date(45000);
    if (!wfc_refused(wfc_call(control, L"Value", DISPATCH_PROPERTYPUT, args, 1,
                              NULL, &exception),
                     &exception)) {
        wfc_check_fail("Value below MinDate was not error 380 in", dll_path);
    }
    /* DayBold(date): the date is the index, the value comes first. */
    args[0] = wfc_bool(1);
    args[1] = wfc_date(45012);
    if (wfc_call(control, L"DayBold", DISPATCH_PROPERTYPUT, args, 2, NULL,
                 NULL) != S_OK) {
        wfc_check_fail("cannot set DayBold in", dll_path);
    }
    args[0] = wfc_date(45012);
    VariantInit(&result);
    hr = wfc_call(control, L"DayBold", DISPATCH_PROPERTYGET, args, 1, &result,
                  NULL);
    if (hr != S_OK || result.vt != VT_BOOL || result.boolVal == VARIANT_FALSE) {
        wfc_check_fail("DayBold did not keep a bold day in", dll_path);
    }
    args[0] = wfc_date(45013);
    hr = wfc_call(control, L"DayBold", DISPATCH_PROPERTYGET, args, 1, &result,
                  NULL);
    if (hr != S_OK || result.boolVal != VARIANT_FALSE) {
        wfc_check_fail("DayBold made another day bold in", dll_path);
    }
    /* VisibleDays: 1 to 42, Sunday first. March 2023 starts on a Wednesday. */
    args[0] = wfc_long(1);
    VariantInit(&result);
    hr = wfc_call(control, L"VisibleDays", DISPATCH_PROPERTYGET, args, 1,
                  &result, NULL);
    /* Value is 45010 (2023-03-25): the calendar shows March, from 2023-02-26.
     */
    if (hr != S_OK || result.vt != VT_DATE || result.date != 44983) {
        wfc_check_fail("VisibleDays(1) is wrong in", dll_path);
    }
    args[0] = wfc_long(43);
    if (!wfc_refused(wfc_call(control, L"VisibleDays", DISPATCH_PROPERTYGET,
                              args, 1, &result, &exception),
                     &exception)) {
        wfc_check_fail("VisibleDays(43) was not error 380 in", dll_path);
    }
    /* MultiSelect with MaxSelCount 3. */
    args[0] = wfc_bool(1);
    wfc_call(control, L"MultiSelect", DISPATCH_PROPERTYPUT, args, 1, NULL,
             NULL);
    args[0] = wfc_long(3);
    wfc_call(control, L"MaxSelCount", DISPATCH_PROPERTYPUT, args, 1, NULL,
             NULL);
    args[0] = wfc_date(45012);
    wfc_call(control, L"SelStart", DISPATCH_PROPERTYPUT, args, 1, NULL, NULL);
    args[0] = wfc_date(45014);
    if (wfc_call(control, L"SelEnd", DISPATCH_PROPERTYPUT, args, 1, NULL,
                 NULL) != S_OK ||
        wfc_date_of(control, L"SelEnd") != 45014) {
        wfc_check_fail("a selection of three days was refused in", dll_path);
    }
    args[0] = wfc_date(45015);
    if (!wfc_refused(wfc_call(control, L"SelEnd", DISPATCH_PROPERTYPUT, args, 1,
                              NULL, &exception),
                     &exception)) {
        wfc_check_fail("a selection of four days was not error 380 in",
                       dll_path);
    }
    control->lpVtbl->Release(control);
    factory->lpVtbl->Release(factory);
    printf("MonthView: the dates and the selection are kept through COM\n");
}

static VARIANT wfc_null(void) {
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_NULL;
    return v;
}

/* The behaviour of a DTPicker made through COM: the value and its parts, the
 * range, Null with CheckBox, and Change for an advised sink. */
static void wfc_check_dtpicker(const wchar_t* dll_path) {
    typedef HRESULT(__stdcall * get_class_object)(REFCLSID, REFIID, void**);
    HMODULE module = LoadLibraryW(dll_path);
    get_class_object get;
    CLSID clsid;
    IClassFactory* factory = NULL;
    IDispatch* control = NULL;
    IConnectionPointContainer* container = NULL;
    IConnectionPoint* point = NULL;
    struct wfc_sink sink = {&wfc_sink_vtbl, 0, 0};
    DWORD cookie = 0;
    EXCEPINFO exception;
    VARIANT args[1];
    VARIANT result;
    if (module == NULL) {
        wfc_check_fail("cannot load", dll_path);
    }
    get = (get_class_object)(void*)GetProcAddress(module, "DllGetClassObject");
    CLSIDFromString(L"{D79C5747-8AEF-5741-9F7E-5445F5777595}", &clsid);
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (get == NULL ||
        get(&clsid, &IID_IClassFactory, (void**)&factory) != S_OK ||
        factory->lpVtbl->CreateInstance(factory, NULL, &IID_IDispatch,
                                        (void**)&control) != S_OK ||
        control->lpVtbl->QueryInterface(control, &IID_IConnectionPointContainer,
                                        (void**)&container) != S_OK ||
        container->lpVtbl->FindConnectionPoint(container, &IID_IDispatch,
                                               &point) != S_OK ||
        point->lpVtbl->Advise(point, (IUnknown*)&sink, &cookie) != S_OK) {
        wfc_check_fail("cannot make a DTPicker in", dll_path);
    }
    memset(&exception, 0, sizeof(exception));
    /* 45000.5 is 2023-03-15 12:00, a Wednesday. */
    args[0] = wfc_date(45000.5);
    if (wfc_call(control, L"Value", DISPATCH_PROPERTYPUT, args, 1, NULL,
                 NULL) != S_OK ||
        sink.count != 1 || sink.last != 2) {
        wfc_check_fail("setting Value did not raise Change in", dll_path);
    }
    wfc_call(control, L"Value", DISPATCH_PROPERTYPUT, args, 1, NULL, NULL);
    if (sink.count != 1) {
        wfc_check_fail("the same Value raised Change in", dll_path);
    }
    if (wfc_short_of(control, L"Day") != 15 ||
        wfc_short_of(control, L"Month") != 3 ||
        wfc_short_of(control, L"Year") != 2023 ||
        wfc_short_of(control, L"Hour") != 12 ||
        wfc_short_of(control, L"Minute") != 0 ||
        wfc_short_of(control, L"DayOfWeek") != 4) {
        wfc_check_fail("the parts of Value are wrong in", dll_path);
    }
    args[0] = wfc_long(13);
    if (wfc_call(control, L"Hour", DISPATCH_PROPERTYPUT, args, 1, NULL, NULL) !=
            S_OK ||
        sink.count != 2 || wfc_short_of(control, L"Hour") != 13) {
        wfc_check_fail("Hour did not change Value in", dll_path);
    }
    args[0] = wfc_long(25);
    if (!wfc_refused(wfc_call(control, L"Hour", DISPATCH_PROPERTYPUT, args, 1,
                              NULL, &exception),
                     &exception)) {
        wfc_check_fail("Hour 25 was not error 380 in", dll_path);
    }
    args[0] = wfc_null();
    if (!wfc_refused(wfc_call(control, L"Value", DISPATCH_PROPERTYPUT, args, 1,
                              NULL, &exception),
                     &exception)) {
        wfc_check_fail("Null without CheckBox was not error 380 in", dll_path);
    }
    args[0] = wfc_bool(1);
    wfc_call(control, L"CheckBox", DISPATCH_PROPERTYPUT, args, 1, NULL, NULL);
    args[0] = wfc_null();
    VariantInit(&result);
    if (wfc_call(control, L"Value", DISPATCH_PROPERTYPUT, args, 1, NULL,
                 NULL) != S_OK ||
        sink.count != 3 ||
        wfc_call(control, L"Value", DISPATCH_PROPERTYGET, NULL, 0, &result,
                 NULL) != S_OK ||
        result.vt != VT_NULL) {
        wfc_check_fail("Null with CheckBox was not kept in", dll_path);
    }
    VariantInit(&result);
    if (wfc_call(control, L"Day", DISPATCH_PROPERTYGET, NULL, 0, &result,
                 NULL) != S_OK ||
        result.vt != VT_NULL) {
        wfc_check_fail("a part of Null is not Null in", dll_path);
    }
    args[0] = wfc_date(44000);
    wfc_call(control, L"MaxDate", DISPATCH_PROPERTYPUT, args, 1, NULL, NULL);
    args[0] = wfc_date(45000);
    if (!wfc_refused(wfc_call(control, L"Value", DISPATCH_PROPERTYPUT, args, 1,
                              NULL, &exception),
                     &exception)) {
        wfc_check_fail("Value above MaxDate was not error 380 in", dll_path);
    }
    point->lpVtbl->Unadvise(point, cookie);
    point->lpVtbl->Release(point);
    container->lpVtbl->Release(container);
    control->lpVtbl->Release(control);
    factory->lpVtbl->Release(factory);
    printf("DTPicker: the value and its range are kept through COM\n");
}

int main(int argc, char** argv) {
    int checked = wfc_check_dll(argc, argv, L"mscomctl2", kClasses,
                                sizeof(kClasses) / sizeof(kClasses[0]));
    if (checked != 0) {
        return checked;
    }
    if (wfc_check_create(wfc_check_widen(argv[1]),
                         L"{14C1B9B8-1541-5BCE-A4B9-7137848B6708}",
                         L"Acceleration", L"AboutBox") != 0) {
        return 1;
    }
    wfc_check_updown(wfc_check_widen(argv[1]));
    wfc_check_flatscrollbar(wfc_check_widen(argv[1]));
    wfc_check_monthview(wfc_check_widen(argv[1]));
    wfc_check_dtpicker(wfc_check_widen(argv[1]));
    return 0;
}
