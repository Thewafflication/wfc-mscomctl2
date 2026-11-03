# wfc-mscomctl2 roadmap

Library: **Windows Common Controls-2** (`wfcmscomctl2.dll`), the WFC
replacement for `mscomct2.ocx`. Current version **0.6.0**, targeting
**MSCOMCT2.OCX 6.00.8177 (Microsoft Common Controls-2, Visual Basic 6.0)**.

The version stays `0.<target major>.<target minor>` until the library matches
the Microsoft one functionally. A checked item is present, tested and passing;
an unchecked one is planned.

## 0.6.x: where it is

- [x] The member tables of the five controls, generated from the installed
      component's type library.
- [x] Version and target stamped into the DLL; `DllRegisterServer` and
      `DllUnregisterServer` (WFC ProgIDs and class ids; the Microsoft ones on
      request).
- [x] A C program tests the DLL; a table test checks the classes.
- [x] Standalone build; CI workflow for Windows, Winelib and WebAssembly.
- [x] The library has a repository of its own (`wfc-mscomctl2`) and is a
      submodule of WFC.

## 0.7: the controls in the evaluator

One control at a time, each with a corpus program, a GUI example and a
requirement:

- [x] UpDown (Value, Min, Max, Increment, Wrap, Orientation, `Change`,
      `UpClick`, `DownClick`; REQ-0308, corpus 211, `examples/updown`).
- [x] UpDown buddy: `BuddyControl`, `BuddyProperty`, `SyncBuddy`, `AutoBuddy` (corpus 213).
- [x] FlatScrollBar (Min, Max, Value, LargeChange, SmallChange, Orientation,
      `Change`, `Scroll`; REQ-0309, corpus 212, `examples/flatscrollbar`).
- [ ] FlatScrollBar drawn in the flat style (`Appearance`, `Arrows`).
- [x] MonthView (Value, the date range, selections, `DateClick`, `DateDblClick`,
      `SelChange`; REQ-0310, corpus 214, `examples/monthview`).
- [x] MonthView `GetDayBold` (corpus 218).
- [x] MonthView `HitTest`, `ComputeControlSize` (corpus 220).
- [ ] MonthView several months (`MonthRows`, `MonthColumns`).
- [x] DTPicker (Value, Format, CustomFormat, CheckBox, UpDown, `Change`,
      `CloseUp`, `DropDown`; REQ-0311, corpus 215, `examples/dtpicker`).
- [x] DTPicker callback fields (`Format`, `FormatSize`, `CallbackKeyDown`; corpus
      219; the host's drawing of the text is not verified).
- [x] Calendar colours of MonthView and DTPicker (corpus 221).
- [x] Animation (Open, Play, Stop, Close, AutoPlay, Center; AVI files; REQ-0312,
      corpus 217, `examples/animation`).
- [x] `DllGetClassObject` returns a class factory; its objects are late-bound
      (IDispatch) controls built from the member table, without the WFC
      evaluator (`include/wfc/ocx/class_factory.hpp`, tested by
      `wfc_check_create`). The behaviour is still the evaluator's.
- [x] Behaviour moves into the library: UpDown first. `updown.hpp` holds the
      rules (steps, Wrap, the position of the arrows, Min, Max and Value,
      error 380); the evaluator and the COM object both use it
      (`class_factory.hpp` takes a `Behaviour` per class). Tested by
      `updown_test` and, through COM, by `mscomctl2_dll_test`.
- [x] Events through COM: a control offers a connection point
      (`IConnectionPointContainer`); an UpDown raises `Change` to an advised
      sink when Value changes (tested with a C sink).
- [ ] UpDown `UpClick`/`DownClick` (they come from the window, which a COM
      object does not have yet) and the buddy through COM.
- [x] MonthView behaviour in the library (`monthview.hpp`: calendar
      arithmetic, the range, selection and MultiSelect rules, the parts of
      Value, VisibleDays, Week, bold days). The evaluator and the COM object
      use it; `class_factory.hpp` passes the index of a parameterized
      property (`DayBold(date)`). Tested by `monthview_test` and through COM.
- [ ] MonthView events and the user's selection through COM (they come from
      the window).
- [x] DTPicker behaviour in the library (`dtpicker.hpp`: the date and time,
      the range, Null with CheckBox, the parts of Value; the calendar
      arithmetic moved to `calendar.hpp`, shared with MonthView). The
      evaluator and the COM object use it; a COM DTPicker raises `Change`
      to an advised sink. Tested by `dtpicker_test` and through COM.
- [ ] DTPicker callback fields, the drop-down calendar and the check box
      click through COM (they come from the window).
- [x] FlatScrollBar behaviour in the library (`scrollbar.hpp`: Min may be
      above Max, Value inside the range or error 380, the position the user
      moves to, SmallChange and LargeChange steps). The evaluator uses it for
      the intrinsic HScrollBar and VScrollBar as well; the COM object keeps
      the properties and raises `Change`. Tested by `scrollbar_test` and
      through COM.
- [ ] Animation behaviour (it is the system animation host; nothing to move
      yet), then wfc-mscomctl, wfc-comdlg32 and wfc-mswinsck.

## 0.8: Microsoft ids verified

- [x] The class ids and ProgIDs come from the installed component's type
      library.
- [x] The class ids, ProgIDs and server of every class match the registration
      of the installed component (`scripts/Test-MicrosoftIds.ps1`).
- [ ] Registration over the Microsoft ids tested against the real component
      (the round trip is tested in the current user's registry only).
- [ ] A program built against the Microsoft type library loads this library.

## 0.9: behaviour parity, 1.0: Microsoft parity

- [ ] Every member of [COMPATIBILITY.md](COMPATIBILITY.md) done or not
      applicable, each with a test; differences documented.
- [ ] Version takes the Microsoft version number; signed release and package.

## Platform track

- [ ] Windows x86, x64, ARM64 (MSVC) passing in CI.
- [ ] Linux x86, x64 and ARM through Winelib (`cmake/winelib.cmake`).
- [ ] WebAssembly with Emscripten: the member table and its page
      (`web/index.html`).

## Toolchain track

- [ ] Built with WFC's own C++ compiler and C++ library (neither exists yet).

## WSP track

- [x] WSP baseline (see [docs/WSP-ADOPTION.md](docs/WSP-ADOPTION.md)).
- [x] The `wsp` submodule (pinned like the other libraries).
- [ ] 80-column source style for the library sources (the generated tables are
      exempt).
- [ ] Requirements and traceability (`requirements/`).
