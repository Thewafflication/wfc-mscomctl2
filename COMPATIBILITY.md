# wfc-mscomctl2: compatibility with mscomct2.ocx

Target: **MSCOMCT2.OCX 6.00.8177 (Microsoft Common Controls-2, Visual Basic 6.0)**. Library version: **0.6.0**.

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
| UpDown | Implemented ([REQ-0308](requirements/req-0308-updown.md)): defaults, range and `Change`, `UpClick`/`DownClick`, `Increment`, `Wrap`, both orientations, the Win32 up-down control. The buddy (`BuddyControl`, `BuddyProperty`, `SyncBuddy`, `AutoBuddy`) follows `Value`. |
| FlatScrollBar | Implemented ([REQ-0309](requirements/req-0309-flatscrollbar.md)): behaves as the intrinsic scroll bars (range, `Change`, `Scroll`, both orientations) on a system scroll bar; `Appearance` and `Arrows` are stored, the flat style is not drawn yet. |
| MonthView | Implemented ([REQ-0310](requirements/req-0310-monthview.md)): `Value` and its parts, `MinDate`/`MaxDate`, `MultiSelect` with `MaxSelCount`, `VisibleDays`, `DayBold`, `SelChange` (with `Cancel`), `DateClick`, `DateDblClick`, the system month calendar. `GetDayBold`, `HitTest` and `ComputeControlSize` do nothing yet. |
| DTPicker, Animation | None yet: a form that contains one is refused as an unsupported control. |

The behaviour follows the documented controls; there is no reference probe of
the Microsoft ones yet. The order is in [ROADMAP.md](ROADMAP.md).

## Registration

| Item | WFC | Microsoft |
| --- | --- | --- |
| ProgID and class id | `WFC.MSComCtl2.<Class>`, a UUID derived from the ProgID | `MSComCtl2.<Class>.2`, the class id above |
| Selected by | the default | `WFC_REGISTER_MICROSOFT_IDS=1` (the previous server is saved and restored on unregister) |
| COM creation | `DllGetClassObject` reports the class as not available | n/a |

## How it is checked

- A C program (`tests/mscomctl2_dll_test.c`) loads the DLL, checks its COM entry
  points and version resource, and registers and unregisters it.
- A table test (`tests/mscomctl2_tables_test.cpp`) checks the five classes and
  their member kinds.
- The corpus programs `211-mscomctl2-updown`, `212-mscomctl2-flatscrollbar`, `213-mscomctl2-updown-buddy` and `214-mscomctl2-monthview` (run by WFC) test the UpDown and the FlatScrollBar.
- WFC compares the table with the evidence
  (`TC-MP0010-mscomctl2-tables-current`).
