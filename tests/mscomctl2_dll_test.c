/* Black-box check of wfcmscomctl2.dll: exports, version resource and the
 * registration round trip (see dll_check.h). Run by CTest with the DLL,
 * the version and the target as arguments. */
#include "dll_check.h"

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
    if (wfc_updown_put(control, L"Min", 50, &exception) != DISP_E_EXCEPTION) {
        wfc_check_fail("Min above Max was accepted in", dll_path);
    }
    wfc_free_exception(&exception);
    control->lpVtbl->Release(control);
    factory->lpVtbl->Release(factory);
    printf("UpDown: the range is kept through COM\n");
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
    return 0;
}
