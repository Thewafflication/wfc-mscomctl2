# wfc-mscomctl2: compatibility with mscomct2.ocx

Target: **MSCOMCT2.OCX 6.00.8177 (Microsoft Common Controls-2, Visual Basic 6.0)**. Library version: **0.6.1**.

Generated from the type library of the installed `MSCOMCT2.OCX` (file version
6.00.8177), retained in WFC as `evidence/reference/mscomct2-2.0-type-library.json`.

## Surface

| Class | Members | Events | ProgID | Class id |
| --- | ---: | ---: | --- | --- |
| Animation | 19 | 11 | `MSComCtl2.Animation.2` | `{B09DE715-87C1-11D1-8BE3-0000F8754DA1}` |
| UpDown | 32 | 12 | `MSComCtl2.UpDown.2` | `{603C7E80-87C2-11D1-8BE3-0000F8754DA1}` |
| MonthView | 77 | 18 | `MSComCtl2.MonthView.2` | `{232E456A-87C3-11D1-8BE3-0000F8754DA1}` |
| DTPicker | 57 | 20 | `MSComCtl2.DTPicker.2` | `{20DD1B9E-87C4-11D1-8BE3-0000F8754DA1}` |
| FlatScrollBar | 29 | 2 | `MSComCtl2.FlatScrollBar.2` | `{FE38753A-44A3-11D1-B5B7-0000C09000C4}` |

5 classes, 214 members and 63 events are in the table.
*Members* means the member exists with the Microsoft name, kind, type and
dispatch id.

## Behaviour

| Control | Status |
| --- | --- |
| UpDown | Implemented ([REQ-0001](requirements/req-0001-updown.md)): defaults, range and `Change`, `UpClick`/`DownClick`, `Increment`, `Wrap`, both orientations, the Win32 up-down control. The buddy (`BuddyControl`, `BuddyProperty`, `SyncBuddy`, `AutoBuddy`) follows `Value`. |
| FlatScrollBar | Implemented ([REQ-0002](requirements/req-0002-flatscrollbar.md)): behaves as the intrinsic scroll bars (range, `Change`, `Scroll`, both orientations) on a system scroll bar; `Appearance` and `Arrows` are stored, the flat style is not drawn yet. |
| MonthView | Implemented ([REQ-0003](requirements/req-0003-monthview.md)): `Value` and its parts, `MinDate`/`MaxDate`, `MultiSelect` with `MaxSelCount`, `VisibleDays`, `DayBold`, `GetDayBold`, `HitTest`, `ComputeControlSize`, the calendar colours, `SelChange` (with `Cancel`), `DateClick`, `DateDblClick`, the system month calendar. `HitTest` and `ComputeControlSize` ask the system calendar (corpus 220). |
| DTPicker | Implemented ([REQ-0004](requirements/req-0004-dtpicker.md)): `Value` (a Date or Null) and its parts, `MinDate`/`MaxDate`, `Format`, `CustomFormat`, `CheckBox`, `UpDown`, `Change`, `DropDown`, `CloseUp`, the system date-time picker. The callback events (`Format`, `FormatSize`, `CallbackKeyDown`) are raised; the calendar colours apply to the drop-down calendar. |
| Animation | Implemented ([REQ-0005](requirements/req-0005-animation.md)): `Open` (53 and 5 errors), `Play`, `Stop`, `Close`, `AutoPlay`, `Center`, `BackStyle`, `Click`, `DblClick`, the system animation control. |

The behaviour follows the documented controls; there is no reference probe of
the Microsoft ones yet. The order is in [ROADMAP.md](ROADMAP.md).

## Registration

| Item | WFC | Microsoft |
| --- | --- | --- |
| ProgID and class id | `WFC.MSComCtl2.<Class>`, a UUID derived from the ProgID | `MSComCtl2.<Class>.2`, the class id above |
| Selected by | the default | `WFC_REGISTER_MICROSOFT_IDS=1` (the previous server is saved and restored on unregister) |
| COM creation | `DllGetClassObject` reports the class as not available | n/a |

## How it is checked

Every test case is `TC-NNNN` with a specification in `tests/tc-NNNN-*.tex`,
and each requirement in `requirements/` lists its cases; the check is
`wsp/tools/Test-Traceability.ps1` (run by `scripts/Invoke-Lint.ps1`).

- TC-0001: the member table (`tests/mscomctl2_tables_test.cpp`) checks the five
  classes and their member kinds.
- TC-0002 to TC-0005: the models of UpDown, the scroll bar, MonthView and
  DTPicker (`tests/updown_test.cpp`, `scrollbar_test.cpp`, `monthview_test.cpp`,
  `dtpicker_test.cpp`).
- TC-0006: a C program (`tests/mscomctl2_dll_test.c`, shared build) loads the
  DLL, checks its COM entry points and version resource, registers and
  unregisters it, and makes the controls through COM.
- TC-0007 to TC-0016: the corpus programs `211` to `221` in `tests/corpus`,
  run by the `wfc` executable (`-DWFC_EXE=<path to wfc>`; WFC itself runs them
  when it mounts the library).
- WFC compares the table with the evidence
  (`TC-MP0010-mscomctl2-tables-current`, a test of the WFC repository).

The identifiers the controls had in WFC are in [docs/id-map.md](docs/id-map.md).
