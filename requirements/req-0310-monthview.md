# REQ-0310 — MonthView control (`MSComCtl2.MonthView`)

**Content type:** Project requirement

**Status:** Proposed (no VB6 reference probe; behavior follows the documented
control)

## Statement

- **Class.** `Begin MSComCtl2.MonthView` forms (and `Dim m As
  MSComCtl2.MonthView`) resolve to a native control; no OCX is needed. Every
  member of the type library of the installed component exists. In a form file
  the date of the control is `CurrentDate` (a date serial).
- **Value.** `Value` (the default property) is the selected date; without a
  design value it is today. `Day`, `Month`, `Year`, `DayOfWeek` (1 is Sunday)
  and `Week` are the parts of `Value`; setting `Month` or `Year` keeps the day
  as far as the month allows, setting `Day` beyond the month's length raises
  380 ("Invalid property value").
- **Range.** `MinDate` (default 1 January 1601) and `MaxDate` (default 31
  December 9999): a date outside them, and a `MinDate` above `MaxDate`, raise
  380; moving a limit past `Value` moves `Value` to the limit.
- **Selection.** `SelStart` and `SelEnd` equal `Value` unless `MultiSelect` is
  True, which allows up to `MaxSelCount` days (default 7; a longer range
  raises 380). Setting `SelStart` leaves a one-day selection; setting `SelEnd`
  before `SelStart` moves `SelStart`.
- **Visible days.** `VisibleDays(1)` to `VisibleDays(42)` are the days shown for
  the month of `Value`, starting on `StartOfWeek` (1 Sunday to 7 Saturday;
  default 1) on or before the first of the month; another index raises 380.
  `DayBold(date)` reads and writes whether a day is bold.
- **Events.** A date chosen by the user raises `SelChange(StartDate, EndDate,
  Cancel)` when the selection changes (setting `Cancel` keeps the old
  selection), then `DateClick(DateClicked)`; a double click adds
  `DateDblClick(DateDblClicked)`. Setting `Value` from code raises no event.
- **Win32 host.** The control is the system month calendar (`SysMonthCal32`)
  with the range, the selection, `ShowToday`, `ShowWeekNumbers`,
  `MaxSelCount` and `StartOfWeek` of the model; choosing a date runs the same
  handling as the scripted `ui <control> select`.
- **GetDayBold.** `GetDayBold(StartDate, Count, State())` is raised for each
  stretch of days the calendar shows: `State` is a Boolean array from 0 to
  `Count - 1`, and the days the handler sets True are bold (as with `DayBold`)
  and the others of the stretch are not. The Win32 host asks when it shows or
  moves the calendar (`MCS_DAYSTATE`).
- **HitTest, ComputeControlSize.** `HitTest(x, y, Date)` (pixels, in the
  control) returns the area (`mvwCalendarBack` 0, `mvwCalendarDate` 1,
  `mvwCalendarDateNext` 2, `mvwCalendarDatePrev` 3, `mvwCalendarDay` 4,
  `mvwCalendarWeekNum` 5, `mvwNoWhere` 6, `mvwTitleBack` 7, `mvwTitleBtnNext` 8,
  `mvwTitleBtnPrev` 9, `mvwTitleMonth` 10, `mvwTitleYear` 11, `mvwTodayLink`
  12; the values of the installed type library) and sets `Date` when it is on a
  date. `ComputeControlSize(Rows, Columns, Width, Height)` sets the size in
  twips of that many months. Both ask the system calendar; without a window
  (scripts) `HitTest` is `mvwNoWhere` and the size is 0.
- **Scripts.** `ui <form>.<control> click|dblclick yyyy-mm-dd`, `select
  yyyy-mm-dd yyyy-mm-dd` and `getdaybold yyyy-mm-dd count`.

## Known limits

 `MonthRows` and `MonthColumns` are stored (the calendar shows one
month); `DateDblClick` comes only from scripts in the Win32 host; scrolling the
calendar is not reported.

## Verification

Corpus `214-mscomctl2-monthview` (parts of the date, ranges, selection, visible
days, bold days, `SelChange` with `Cancel`, `DateClick`, `DateDblClick`); the
example `examples/monthview`.
