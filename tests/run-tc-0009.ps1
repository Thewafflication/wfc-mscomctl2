<#
.SYNOPSIS
Runs TC-0009; the specification is tc-0009-corpus-updown-buddy.tex.
.DESCRIPTION
Runs the CTest case TC-0009-corpus-updown-buddy of the
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
& $runner -Id 'TC-0009' -BuildDir $BuildDir
exit $LASTEXITCODE
