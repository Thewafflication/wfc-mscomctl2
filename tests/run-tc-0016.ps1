<#
.SYNOPSIS
Runs TC-0016; the specification is tc-0016-corpus-calendar-colors.tex.
.DESCRIPTION
Runs the CTest case TC-0016-corpus-calendar-colors of the
build in -BuildDir and fails when it fails or is not registered.
.PARAMETER BuildDir
The configured and built CMake folder.
#>
[CmdletBinding()]
param(
    [string]$BuildDir = (Join-Path $PSScriptRoot '\..\out\build')
)

$ErrorActionPreference = 'Stop'
$runner = Join-Path $PSScriptRoot 'Invoke-TestCase.ps1'
& $runner -Id 'TC-0016' -BuildDir $BuildDir
exit $LASTEXITCODE
