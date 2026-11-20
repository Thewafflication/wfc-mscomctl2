<#
.SYNOPSIS
Runs the CTest case of one test-case identifier (the tc-NNNN runners call it).
.DESCRIPTION
Runs `ctest -R ^<Id>-` on a configured and built folder, with
--no-tests=error: a case that is not registered in that build is a failure.
A corpus case that CTest reports as skipped (no WFC_EXE) is not a failure.
.PARAMETER Id
The test-case identifier, for example TC-0002.
.PARAMETER BuildDir
The CMake build folder.
.PARAMETER Configuration
The configuration of a multi-configuration generator (default Debug).
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)][ValidatePattern('^TC-\d{4}$')][string]$Id,
    [Parameter(Mandatory)][string]$BuildDir,
    [string]$Configuration = 'Debug'
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
& ctest --test-dir $BuildDir -C $Configuration -R "^$Id-" --no-tests=error `
    --output-on-failure
exit $LASTEXITCODE
