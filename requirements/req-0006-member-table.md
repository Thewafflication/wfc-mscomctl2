# REQ-0006 — Member table of the type library

**Content type:** Project requirement

**Status:** Proposed (the structure is tested here; the comparison with the
type library of the installed component is made in WFC)

## Statement

- **Classes.** The library shall list the five classes of `MSComCtl2`
  (Animation, UpDown, MonthView, DTPicker, FlatScrollBar, in that order)
  through `wfc::ui::mscomctl2_classes()`, in a static build and in the shared
  build.
- **Members.** Every class shall list at least one member; every member shall
  have a name and a kind that is a property get (`g`), a property let (`l`), a
  property set (`s`) or a method (`m`); every event shall be listed as a
  method (`m`).
- **Source.** The table is generated from the type library of the installed
  `MSCOMCT2.OCX` (`scripts/New-UiControlTables.ps1` in WFC); the file is not
  edited by hand. It holds 214 members and 63 events (see
  [COMPATIBILITY.md](../COMPATIBILITY.md)).

## Known limits

That the member names, kinds, types and dispatch ids equal those of the
installed type library is checked by a WFC test
(`TC-MP0010-mscomctl2-tables-current` in the WFC repository), which needs the evidence file of WFC and so does not
run in this repository.

## Verification

Method: automated test.

- [TC-0001](../tests/tc-0001-tables.tex): the classes and the shape of every
  row.

## Relationships

Every control requirement (REQ-0001 to REQ-0005) relies on it: "every member
of the type library exists".
