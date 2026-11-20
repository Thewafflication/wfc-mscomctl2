# wfc-mscomctl2 Repository Instructions

This library (Windows Common Controls-2, `mscomct2.ocx`) is one of the WFC
libraries. It follows WSP (the `wsp/` submodule); the adoption status is in
`docs/WSP-ADOPTION.md`.

## Authorization

- Ordinary file edits within this repository and the current task scope do not
  require asking the owner for approval each time.
- Do not rewrite Git history, force-push, tag, release or publish without the
  owner's explicit request. Commit and push only when the owner asks.
- Do not edit the `wsp/` submodule; WSP changes belong in the WSP repository.
- When WFC checks this library out as `lib/wfc-mscomctl2`, treat that copy as
  read-only reference; work in a separate clone.

## Build and checks

- The standalone build is plain CMake (`cmake -S . -B out/build`, then
  `cmake --build out/build` and `ctest --test-dir out/build`). Build output
  goes under `out/` (ignored). Local verification is x64 only; CI covers x86,
  ARM64, Winelib and WebAssembly.
- Install the pinned tools with
  `python -m pip install --user --requirement requirements-dev.txt`.
- The commit gate (`pre-commit install`, then plain `git commit`) runs four
  hooks in order: `scripts/Start-CommitGate.ps1`, `scripts/Invoke-Lint.ps1`
  (WSP physical style at 80 columns, clang-format, gersemi, traceability,
  all in check mode), `scripts/Invoke-Build.ps1` (x64 Debug, static and
  shared) and `scripts/Invoke-Tests.ps1` (all 16 cases). Never skip it with
  `--no-verify`; do not run its stages by hand in place of it.
- Only one gate runs at a time on the machine: the first hook takes
  `wfc-commit-gate.lock` in the temp folder, the same lock as the WFC gate
  (a library gate and a WFC gate never overlap), and opens the always-on-top
  test progress window (`scripts/Show-TestProgress.ps1`, which follows
  `out/test-progress.log`). Do not suppress or replace the window. A commit
  refused because a gate is running waits for it; do not start a second
  commit, build or `ctest` in the same checkout meanwhile.
- The corpus cases (TC-0007 to TC-0016) run through the `wfc` executable:
  `Invoke-Build.ps1` finds it through `WFC_EXE`, `wfc` on PATH,
  `C:\wfc\wfc.exe` or a WFC checkout next to this one, and fails without
  it (`-NoCorpus` builds without).
- Format with the pinned tools (`.clang-format`, `.gersemirc`); do not
  hand-tune around them. Sources are UTF-8 with LF line endings.

## Process

- Requirements are `requirements/req-NNNN-<title>.md` (`REQ-NNNN`); test
  cases are `TC-NNNN`: the specification `tests/tc-NNNN-<title>.tex`, the
  runner `tests/run-tc-NNNN.ps1` and the CTest name `TC-NNNN-<short-name>`.
  Numbers run from 0001 in this repository's own namespace and are never
  reused or renumbered; `docs/id-map.md` maps the old WFC identifiers.
  `wsp/tools/Test-Traceability.ps1` checks the links (run by the lint stage).
  Update the requirement when behavior changes.
- The version is in `VERSION` (`0.<target major>.<target minor>`); see
  `ROADMAP.md`.
