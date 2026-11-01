# REQ-0309 — FlatScrollBar control (`MSComCtl2.FlatScrollBar`)

**Content type:** Project requirement

**Status:** Proposed (no VB6 reference probe; behavior follows the documented
control and the intrinsic scroll bars)

## Statement

- **Class.** `Begin MSComCtl2.FlatScrollBar` forms (and `Dim f As
  MSComCtl2.FlatScrollBar`) resolve to a native control; no OCX is needed. Every
  member of the type library of the installed component exists.
- **Scroll bar behaviour.** The control behaves as the intrinsic `HScrollBar`
  and `VScrollBar` (REQ-0154 and the form-scroll-bar tests): `Value` is the
  default property and must lie between `Min` and `Max` (error 380 "Invalid
  property value" otherwise; `Min` may be above `Max`); `Change` is raised when
  `Value` changes; the user dragging the thumb raises `Scroll` and `Change`, a
  click on an arrow or in the track changes `Value` by `SmallChange` or
  `LargeChange` and raises `Change`.
- **Defaults.** `Min` 0, `Max` 32767, `Value` 0, `LargeChange` 1,
  `SmallChange` 1, `Orientation` 0 (`cc2OrientationVertical`), `Appearance` 0
  (flat), `Arrows` 0 (both).
- **Orientation.** `Orientation` is read when the control is created: 1 gives a
  horizontal bar, 0 a vertical one.
- **Win32 host.** The control is a system scroll bar. `Appearance`, `Arrows`
  and `MousePointer` are stored; the bar is drawn in the system style, not the
  flat style.
- **Scripts.** `scroll <control> <value>` (the thumb is dragged to the value)
  and `scrollby <control> <amount>` stand for the user.

## Verification

Corpus `212-mscomctl2-flatscrollbar`; the example `examples/flatscrollbar`.
