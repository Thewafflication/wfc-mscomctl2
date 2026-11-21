<#
.SYNOPSIS
Runs the library build stage (x64 Debug).
.DESCRIPTION
Configures and builds the library twice in out/: the static library
(out/build) and the shared library, mscomct2.ocx (out/build-shared),
which also holds the DLL test. The corpus cases (TC-0007 to TC-0016) run
through wfc, found by Resolve-WfcExecutable (WFC_EXE); without it the stage
fails unless -NoCorpus is given. Only x64 is built here; CI builds x86 and
ARM64.
.PARAMETER NoCorpus
Build without a wfc executable; the corpus cases then report as skipped.
.EXAMPLE
pwsh -NoProfile -File scripts/Invoke-Build.ps1
#>
[CmdletBinding()]
param([switch]$NoCorpus)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
Import-Module (Join-Path $PSScriptRoot 'WfcChecks.psm1') -Force

$root = Split-Path -Parent $PSScriptRoot
Push-Location $root
try {
    $wfc = Resolve-WfcExecutable
    if (-not $wfc -and -not $NoCorpus) {
        throw ('No wfc executable for the corpus cases: set WFC_EXE, or ' +
            'pass -NoCorpus.')
    }
    $corpus = @()
    if ($wfc) { $corpus = @("-DWFC_EXE=$wfc") }
    $builds = @(
        @{ Dir = 'out/build'; Options = @() },
        @{ Dir = 'out/build-shared';
            Options = @('-DWFC_MSCOMCTL2_SHARED=ON') }
    )
    foreach ($build in $builds) {
        Write-Stage "[build] configure $($build.Dir) (x64 Debug)"
        Invoke-CheckedCommand cmake (@('-S', '.', '-B', $build.Dir, '-A',
                'x64') + $build.Options + $corpus)
        Write-Stage "[build] compile $($build.Dir)"
        Invoke-CheckedCommand cmake @('--build', $build.Dir, '--config',
            'Debug')
    }
    Write-Output '[PASS] build x64 Debug'
}
finally {
    Pop-Location
}
