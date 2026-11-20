<#
.SYNOPSIS
Runs the library test stage (x64 Debug).
.DESCRIPTION
Runs every registered CTest test of the shared build (out/build-shared),
which holds all sixteen cases, including the DLL test. Zero registered
tests is an error. The build must exist (scripts/Invoke-Build.ps1). The
output goes to out/test-progress.log as ctest prints it, in lines such as
`12/16 Test  #12: name ... Passed`, which scripts/Show-TestProgress.ps1
follows live.
.PARAMETER BuildDir
The CMake build folder (default out/build-shared).
.PARAMETER JUnitPath
Optional path for a JUnit XML result file.
.EXAMPLE
pwsh -NoProfile -File scripts/Invoke-Tests.ps1
#>
[CmdletBinding()]
param(
    [string]$BuildDir = 'out/build-shared',
    [string]$JUnitPath = ''
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
Import-Module (Join-Path $PSScriptRoot 'WfcChecks.psm1') -Force

$root = Split-Path -Parent $PSScriptRoot
Push-Location $root
try {
    # The log is followed live by scripts/Show-TestProgress.ps1.
    $progressLog = Join-Path $root 'out/test-progress.log'
    New-Item -ItemType Directory -Force -Path (Split-Path $progressLog) |
        Out-Null
    $arguments = @('--test-dir', $BuildDir, '-C', 'Debug',
        '--output-on-failure', '--no-tests=error')
    if ($JUnitPath) {
        $arguments += @('--output-junit', $JUnitPath)
    }
    Write-Stage "[tests] ctest $BuildDir (x64 Debug)"
    # Each line goes to the log as soon as ctest prints it.
    & ctest @arguments 2>&1 | ForEach-Object {
        $line = "$_"
        [IO.File]::AppendAllText($progressLog, $line + "`n")
        $line
    }
    if ($LASTEXITCODE -ne 0) {
        throw "ctest $($arguments -join ' ') failed ($LASTEXITCODE)."
    }
    Write-Output '[PASS] tests x64 Debug'
}
finally {
    Pop-Location
}
