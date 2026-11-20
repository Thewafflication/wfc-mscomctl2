<#
.SYNOPSIS
Runs TC-0011; the specification is tc-0011-corpus-dtpicker.tex.
.DESCRIPTION
Runs the CTest case TC-0011-corpus-dtpicker of the
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
& $runner -Id 'TC-0011' -BuildDir $BuildDir
exit $LASTEXITCODE
