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
- The lint stage is `pwsh -NoProfile -File scripts/Invoke-Lint.ps1`: WSP
  physical style (80 columns), clang-format and gersemi in check mode. It
  changes no files. `pre-commit install` runs it as the commit gate; never
  skip it with `--no-verify`.
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
