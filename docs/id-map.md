# Identifier map

**Content type:** Project record

Requirements and test cases of this repository are numbered `REQ-NNNN` and
`TC-NNNN` from 0001 in the namespace of the repository (WSP
`style/artifact-identifiers.md`). Before the WSP adoption they carried
identifiers from the WFC repository, where the library was a folder and then a
submodule. WSP asks that references to historical identifiers be preserved, so
this file maps every old identifier to its new one. Old identifiers are not
reused here.

## Requirements

| Old ID (WFC) | New ID | Title |
| --- | --- | --- |
| REQ-0308 | [REQ-0001](../requirements/req-0001-updown.md) | UpDown control (`MSComCtl2.UpDown`) |
| REQ-0309 | [REQ-0002](../requirements/req-0002-flatscrollbar.md) | FlatScrollBar control (`MSComCtl2.FlatScrollBar`) |
| REQ-0310 | [REQ-0003](../requirements/req-0003-monthview.md) | MonthView control (`MSComCtl2.MonthView`) |
| REQ-0311 | [REQ-0004](../requirements/req-0004-dtpicker.md) | DTPicker control (`MSComCtl2.DTPicker`) |
| REQ-0312 | [REQ-0005](../requirements/req-0005-animation.md) | Animation control (`MSComCtl2.Animation`) |
| (none) | [REQ-0006](../requirements/req-0006-member-table.md) | Member table of the type library (new, split out of the per-control "every member exists" statements) |
| (none) | [REQ-0007](../requirements/req-0007-shared-library.md) | Shared library as a COM server (new; the registration and version rules of the README) |

## Test cases

The CTest names listed here are those of this repository; when another project (WFC) mounts the library they carry the prefix `wfc-mscomctl2-` (`WFC_MSCOMCTL2_TEST_PREFIX`).
The WFC names are CTest names. `TC-MP0010-mscomctl2-*` were registered by this
library's `CMakeLists.txt`; `TC-MP0002-corpus-<case>` are registered by the WFC
repository for every `*.expected` file it finds under `lib/*/tests/corpus`, so
the corpus cases keep that name inside WFC as well (WFC mounts the library and
registers them itself; this repository registers its own `TC-NNNN` names for
the same programs in the standalone build when `WFC_EXE` is given).

| Old name (WFC) | New ID | CTest name | Title |
| --- | --- | --- | --- |
| `TC-MP0010-mscomctl2-tables` | TC-0001 | `TC-0001-tables` | Member table of the five classes |
| `TC-MP0010-mscomctl2-updown` | TC-0002 | `TC-0002-updown-model` | UpDown model |
| `TC-MP0010-mscomctl2-scrollbar` | TC-0003 | `TC-0003-scrollbar-model` | Scroll bar model of the FlatScrollBar |
| `TC-MP0010-mscomctl2-monthview` | TC-0004 | `TC-0004-monthview-model` | MonthView model and calendar arithmetic |
| `TC-MP0010-mscomctl2-dtpicker` | TC-0005 | `TC-0005-dtpicker-model` | DTPicker model and calendar helpers |
| `TC-MP0010-mscomctl2-dll` | TC-0006 | `TC-0006-dll` | Shared library: entry points, version, registration, objects |
| `TC-MP0002-corpus-211-mscomctl2-updown` | TC-0007 | `TC-0007-corpus-updown` | Corpus 211: UpDown |
| `TC-MP0002-corpus-212-mscomctl2-flatscrollbar` | TC-0008 | `TC-0008-corpus-flatscrollbar` | Corpus 212: FlatScrollBar |
| `TC-MP0002-corpus-213-mscomctl2-updown-buddy` | TC-0009 | `TC-0009-corpus-updown-buddy` | Corpus 213: UpDown buddy |
| `TC-MP0002-corpus-214-mscomctl2-monthview` | TC-0010 | `TC-0010-corpus-monthview` | Corpus 214: MonthView |
| `TC-MP0002-corpus-215-mscomctl2-dtpicker` | TC-0011 | `TC-0011-corpus-dtpicker` | Corpus 215: DTPicker |
| `TC-MP0002-corpus-217-mscomctl2-animation` | TC-0012 | `TC-0012-corpus-animation` | Corpus 217: Animation |
| `TC-MP0002-corpus-218-mscomctl2-getdaybold` | TC-0013 | `TC-0013-corpus-getdaybold` | Corpus 218: MonthView GetDayBold |
| `TC-MP0002-corpus-219-mscomctl2-dtpicker-callback` | TC-0014 | `TC-0014-corpus-dtpicker-callback` | Corpus 219: DTPicker callback fields |
| `TC-MP0002-corpus-220-mscomctl2-monthview-hittest` | TC-0015 | `TC-0015-corpus-monthview-hittest` | Corpus 220: MonthView HitTest |
| `TC-MP0002-corpus-221-mscomctl2-calendar-colors` | TC-0016 | `TC-0016-corpus-calendar-colors` | Corpus 221: Calendar colours |

Not mapped: `TC-MP0010-mscomctl2-tables-current` belongs to the WFC repository
(it compares the table with the type-library evidence that only WFC holds) and
keeps its name there.

## Other renamed items

- The CTest labels `mp-0010` (a WFC milestone) were dropped; the labels
  `unit`, `integration`, `corpus` and `mscomctl2` remain.
- `lib/wfc-mscomctl2/src/mscomctl2_tables.cpp` in REQ-0308 became
  `src/mscomctl2_tables.cpp` (REQ-0001).
