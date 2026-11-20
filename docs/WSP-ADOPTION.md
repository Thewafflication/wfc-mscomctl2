# WSP Adoption Record

**Content type:** Project adoption record

**Project:** wfc-mscomctl2 (Windows Common Controls-2)

**WSP release:** 1.4.0

**WSP baseline:** Immutable commit `f009399dd1406219571ac978bfee85e85bbdfeac`

**Submodule path:** `wsp/`

**Pinned commit:** `f009399dd1406219571ac978bfee85e85bbdfeac`

**Status:** In adoption; the style and lint baseline is in place, the gaps
are listed below

The library is one of the WFC libraries and follows the WFC adoption record
(`planning/wsp-adoption-record.md` in
[WFC](https://github.com/Thewafflication/wfc)); this record covers what the
library repository itself carries.

## Selected profiles

| Profile | Selected | Scope or rationale |
| --- | --- | --- |
| Security/DFS | Yes | Hardening flags (WSP-SEC-0015) from `wsp/tools/cmake/WspBuild.cmake` in the standalone build |
| C source style | No | The sources are C++; the C test programs follow the same layout |
| CMake style | Yes | `CMakeLists.txt` |
| Windows version resources | Yes | `version.rc.in`, one version resource per DLL |
| Windows code signing and Defender | Deferred | Until the first release |
| Documentation | Yes | `README.md`, `ROADMAP.md`, `COMPATIBILITY.md` |

## Gap analysis (WSP 1.4.0)

WSP asks of a project: the style baseline and a commit gate, controlled
requirements with stable identifiers, test cases traced to them, a recorded
adoption (this file), and a controlled release. Status as of this branch:

| Item | WSP source | Status | Action |
| --- | --- | --- | --- |
| WSP submodule pinned | `architecture/adr-0001`, requirements README | Done: 1.4.0, same as WFC | Move with WFC when WSP is updated |
| Formatter configs | `style/source-style.md`, `templates/source-style-configurations.md` (WSP-LANG-0002, -0010) | Done: `.clang-format`, `.gersemirc`, `.editorconfig`; the sources already conform | None |
| Physical style (80 columns, LF, UTF-8) | WSP-STYLE-0001 to 0004 | Done: `Test-SourceStyle.ps1` passes on 34 files (`tests/corpus` is out of scope) | None |
| Lint stage and commit gate | `processes/commit-checks.md` (WSP-CHECK-0001 to 0008) | Done: `.pre-commit-config.yaml` runs `Start-CommitGate.ps1` (machine-wide lock shared with the WFC gate, test progress window), `Invoke-Lint.ps1`, `Invoke-Build.ps1` and `Invoke-Tests.ps1`; x64 only locally; the blocking CI job runs the lint | `Test-PeHardening.ps1` in the build stage |
| Pinned tools | WSP-CHECK-0007 | Done: `requirements-dev.txt` | None |
| PowerShell and Python analysis | WSP-LANG-0007 | Not done: no PSScriptAnalyzer settings; two Python scripts | Add `PSScriptAnalyzerSettings.psd1` and a lint step when scripts grow |
| Requirement format | `requirements/requirements-management.md`, `style/requirements-writing.md` | Done: seven `REQ-NNNN` files (`REQ-0001` to `REQ-0007`) in the library's own namespace, each with Verification (test-case links) and Relationships; the status stays Proposed until the controls are probed against VB6 | Move to Approved with the first release |
| Test cases with stable IDs | `testing/test-strategy.md`, `style/artifact-identifiers.md` | Done: sixteen cases `TC-0001` to `TC-0016` (CTest `TC-NNNN-<name>`, `tests/tc-NNNN-*.tex`, `tests/run-tc-NNNN.ps1`); the old WFC identifiers are in [id-map.md](id-map.md) | None |
| Traceability check | `tools/Test-Traceability.ps1`, WSP-TEST traceability | Done: passes for 7 requirements, 16 specifications and 16 runners; run by `scripts/Invoke-Lint.ps1` | None |
| Test evidence and reports | `tools/Test-TestEvidence.ps1`, `New-TestReport.ps1` | Not done | Later, with the first release |
| Docs structure | `documentation/`, `style/documentation-style.md` | Partial: README, ROADMAP, COMPATIBILITY, `docs/`; no architecture or design records | Add an ADR for the table generation and the standalone/WFC split when needed |
| Source documentation | `style/source-documentation.md` | Not checked: no Doxygen | Decide whether C++ sources need `@file` and `@brief` blocks |
| Static analysis | `style/static-analysis.md` | Not done | Optional: clang-tidy via `wsp/tools/cmake` |
| Hardening and version resource | WSP-SEC-0015, `style/windows-version-resources.md` | Done (`WspBuild.cmake`, `version.rc.in`) | Add `Test-PeHardening.ps1` to the build stage |
| Release and versioning | `processes/release-readiness-template.md`, `security/windows-code-signing-and-defender.md` | Partial: `VERSION`, shared release workflow; no readiness record, no signing | Signing planned for the first release |
| AGENTS.md | WFC model | Done | None |

## Open items

| Item | Disposition |
| --- | --- |
| Build and test hooks | Closed: the gate has gate-start, lint, build and test hooks |
| Requirement and test-case numbering | Closed: `REQ-NNNN` and `TC-NNNN` from 0001 in this repository (`docs/id-map.md`) |
| Traceability check | Closed: part of the lint stage |
| Signing | Planned for the first release |
| CI screenshots (`render_terminal.py`, `capture_web.py`) | Open, owner decision: these files are identical in every WFC library (a WFC test checks the copies), so restyling them to the house look needs a change in all libraries at once |

The earlier open item "80-column style" is closed: the sources, including the
generated tables, already satisfy it; the only long lines were in `ci.yml` and
two Python scripts, now wrapped.
