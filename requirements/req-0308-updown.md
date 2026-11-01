# REQ-0308 — UpDown control (`MSComCtl2.UpDown`)

**Content type:** Project requirement

**Status:** Proposed (no VB6 reference probe; behavior follows the documented
control)

## Statement

- **Class.** `Begin MSComCtl2.UpDown` forms (and `Dim u As MSComCtl2.UpDown`)
  resolve to a native control; no OCX is needed. The `Object=` line of
  `mscomct2.ocx` is accepted. Every member of the type library of the
  installed component exists (`lib/wfc-mscomctl2/src/mscomctl2_tables.cpp`).
- **Defaults.** `Min` 0, `Max` 10, `Value` 0, `Increment` 1, `Wrap` False,
  `Orientation` 0 (`cc2OrientationVertical`), `Acceleration` 0, `AutoBuddy` and
  `SyncBuddy` False. `Value` is the default property.
- **Range.** `Value` outside `Min` to `Max`, and `Min` above `Max`, raise 380
  ("Invalid property value"). Changing `Min` or `Max` moves `Value` into the new
  range.
- **Change.** `Change` is raised when `Value` changes, whether by code or by a
  click.
- **Clicks.** A click on the up arrow raises `UpClick`, one on the down arrow
  `DownClick`, each followed by `Change` when `Value` changed. The up arrow adds
  `Increment`, the down arrow subtracts it. At `Max` the up arrow keeps `Value`
  (with `Wrap`: goes to `Min`); at `Min` the down arrow keeps it (with `Wrap`:
  goes to `Max`). `UpClick` and `DownClick` are raised even when `Value` does
  not change.
- **Buddy.** `BuddyControl` is a control (`Set`) or, in a form file, its name;
  `BuddyProperty` is the buddy's property by name (a dispatch id in a form file
  stands for the buddy's default property); with `AutoBuddy` the buddy is the
  control before the UpDown in the form's creation order. While `SyncBuddy` is
  True the buddy's property is set to `Value` when the form loads and whenever
  `Value` changes (text for `Text` and `Caption`, a number otherwise), and
  reading `Value` takes the number the buddy holds, kept inside `Min` and
  `Max` (a buddy that holds no number leaves `Value` as it is).
- **Win32 host.** The control is a system up-down control
  (`msctls_updown32`), vertical or horizontal (`Orientation` 1), with the range,
  the position, `Wrap` and `Increment` of the model; a click on an arrow runs
  the same handling as the scripted `ui <control> up|down`.
- **Scripts.** `ui <form>.<control> up` and `ui <form>.<control> down` stand for
  a click on an arrow.

## Known limits

`Acceleration` is stored and not used; the buddy follows `Value` when the form
loads and when `Value` changes, but a buddy that the user edits is only read
when `Value` is read (no event is raised); the order of `UpClick`/`DownClick`
and `Change` follows the documentation's wording, not a probe of the Microsoft
control.

## Verification

Corpus `211-mscomctl2-updown` (properties, range errors, clicks, wrap and the
event order) and `213-mscomctl2-updown-buddy` (the buddy); the example `examples/updown` shows both orientations.
