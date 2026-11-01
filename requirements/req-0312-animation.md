# REQ-0312 — Animation control (`MSComCtl2.Animation`)

**Content type:** Project requirement

**Status:** Proposed (no VB6 reference probe; behavior follows the documented
control)

## Statement

- **Class.** `Begin MSComCtl2.Animation` forms (and `Dim a As
  MSComCtl2.Animation`) resolve to a native control; no OCX is needed. Every
  member of the type library of the installed component exists.
- **Open.** `Open(file)` loads a silent AVI clip. A file that does not exist
  raises 53 ("File not found"); a file that is not an AVI file (it must start
  with `RIFF` and `AVI `) raises 5 ("Invalid procedure call or argument").
  With `AutoPlay` True the clip starts at once.
- **Play, Stop, Close.** `Play(RepeatCount, StartFrame, EndFrame)` starts the
  loaded clip (repeat -1, the default, repeats for ever; frames count from 0 and
  an end of -1 is the last). `Play` with no clip loaded does nothing. `Stop`
  halts it and `Close` unloads the file.
- **Appearance.** `Center` centres the clip in the control; `BackStyle` 0 draws
  it with a transparent background. Both default to the VB6 values (False, 1).
- **Events.** `Click` and `DblClick` are the ordinary mouse events.
- **Win32 host.** The control is the system animation control
  (`SysAnimate32`) with `ACS_CENTER` and `ACS_TRANSPARENT` from the properties;
  `Open`, `Play`, `Stop` and `Close` send `ACM_OPEN`, `ACM_PLAY` and `ACM_STOP`.

## Known limits

The system animation control plays uncompressed and run-length-encoded clips
only, as in VB6; other compressions load and do not draw.

## Verification

Corpus `217-mscomctl2-animation` (defaults, `Open` errors, `Play`/`Stop`/`Close`,
`AutoPlay`, `Click`, `DblClick`); the example `examples/animation`.
