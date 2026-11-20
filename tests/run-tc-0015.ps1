<#
.SYNOPSIS
Runs TC-0015; the specification is tc-0015-corpus-monthview-hittest.tex.
.DESCRIPTION
Runs the CTest case TC-0015-corpus-monthview-hittest of the
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
& $runner -Id 'TC-0015' -BuildDir $BuildDir
exit $LASTEXITCODE
