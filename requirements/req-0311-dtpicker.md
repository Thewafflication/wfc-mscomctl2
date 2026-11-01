# REQ-0311 — DTPicker control (`MSComCtl2.DTPicker`)

**Content type:** Project requirement

**Status:** Proposed (no VB6 reference probe; behavior follows the documented
control)

## Statement

- **Class.** `Begin MSComCtl2.DTPicker` forms (and `Dim d As
  MSComCtl2.DTPicker`) resolve to a native control; no OCX is needed. Every
  member of the type library of the installed component exists. In a form file
  the date and time of the control are `CurrentDate` (a date serial with the
  time as its fraction), and `Format` carries flag bits above its number, which
  are ignored.
- **Value.** `Value` (the default property) is a Date, or Null while `CheckBox`
  is True and the box is cleared; without a design value it is now. `Day`,
  `Month`, `Year`, `Hour`, `Minute`, `Second` and `DayOfWeek` (1 is Sunday) are
  the parts of `Value`, and are Null when it is Null. Setting a part keeps the
  others as far as the month allows (`Month` 2 on the 31st gives the 28th or
  29th); a part out of its range (a day beyond the month's length, `Hour` 24)
  raises 380 ("Invalid property value"), as does setting a part while `Value` is
  Null.
- **Null.** Setting `Value` to Null with `CheckBox` False raises 380; with
  `CheckBox` True it clears the box (Change is raised once).
- **Range.** `MinDate` (default 1 January 1601) and `MaxDate` (default 31
  December 9999): a `Value` outside them, and a `MinDate` above `MaxDate`, raise
  380; moving a limit past `Value` moves `Value` to the limit, keeping the time.
- **Format.** `Format` is 0 (long date), 1 (short date), 2 (time) or 3
  (`CustomFormat`, with the codes `yyyy`, `MM`, `dd`, `HH`, `mm`, `ss`, `tt` and
  the others of the system date-time picker). `UpDown` True gives spin arrows in
  place of the drop-down calendar. `CheckBox` and `UpDown` are read when the
  window is made.
- **Events.** `Change` is raised when `Value` changes, by code or by the user
  (and when the box is ticked or cleared); `DropDown` and `CloseUp` bracket the
  calendar.
- **Callback fields.** A run of `X` in a custom format is a callback field.
  `Format(CallbackField, FormattedString)` gives its text,
  `FormatSize(CallbackField, Size)` its width in pixels and
  `CallbackKeyDown(KeyCode, Shift, CallbackField, CallbackDate)` runs when a key
  is pressed in it (setting `CallbackDate` changes `Value`, with `Change`).
  `CallbackField` is the whole run (`XX`). The Win32 host answers from the last
  answers of the program and asks again (events run after the notification), so
  the first drawing can lack the text.
- **Win32 host.** The control is the system date and time picker
  (`SysDateTimePick32`) with the value, the range, the format and the check
  box of the model; its notifications run the same handling as the scripted
  `ui` actions.
- **Scripts.** `ui <form>.<control> value yyyy-mm-dd[ hh:mm:ss]`,
  `check on|off`, `dropdown`, `closeup`, `format <field> <date> <time>`,
  `formatsize <field>` and `keydown <field> <key> <shift> <date> <time>`.

## Known limits

The calendar colours are stored and not applied to the drop-down calendar. In
the Win32 host the text of a callback field does not show in the screenshots
made here (the model, the answers and the widths reach the control; whether the
system control paints them is not verified).

## Verification

Corpus `215-mscomctl2-dtpicker` (parts of the value, ranges, Null and the
check box, `Change`, `DropDown`, `CloseUp`); the example `examples/dtpicker`.
