<#
.SYNOPSIS
Runs TC-0006; the specification is tc-0006-dll.tex.
.DESCRIPTION
Runs the CTest case TC-0006-dll of the
build in -BuildDir and fails when it fails or is not registered.
.PARAMETER BuildDir
The configured and built CMake folder (the shared build).
#>
[CmdletBinding()]
param(
    [string]$BuildDir = (Join-Path $PSScriptRoot '\..\out\build-shared')
)

$ErrorActionPreference = 'Stop'
$runner = Join-Path $PSScriptRoot 'Invoke-TestCase.ps1'
& $runner -Id 'TC-0006' -BuildDir $BuildDir
exit $LASTEXITCODE
