<#
.SYNOPSIS
Runs the library lint stage (WSP-CHECK-0001).
.DESCRIPTION
Checks the owned source and configuration scope:
- WSP physical style (80 columns, UTF-8, whitespace, final newline);
- clang-format check mode for C++ (WSP-LANG-0002);
- gersemi check mode for CMake (WSP-LANG-0010);
- pre-commit configuration validity.
Stops at the first failing check and exits nonzero. Changes no files.
tests/corpus holds VB programs and expected output, which are not source
text in a WSP language profile, so the style check leaves it out.
.PARAMETER Report
Runs every check and reports the findings without failing, for a baseline
count before the sources are reformatted.
.EXAMPLE
pwsh -NoProfile -File scripts/Invoke-Lint.ps1
#>
[CmdletBinding()]
param([switch]$Report)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
Import-Module (Join-Path $PSScriptRoot 'WfcChecks.psm1') -Force

$root = Split-Path -Parent $PSScriptRoot
Push-Location $root
try {
    Write-Stage '[lint] WSP physical source style' -Reset
    $styleScope = @(
        'src', 'include', 'cmake', 'scripts', 'tests', '.github',
        'CMakeLists.txt', '.clang-format', '.editorconfig', '.gersemirc',
        '.gitattributes', '.pre-commit-config.yaml'
    )
    $quoted = ($styleScope | ForEach-Object { "'$_'" }) -join ','
    $style = "& ./wsp/tools/Test-SourceStyle.ps1 -RepositoryRoot '$root' " +
        "-SourcePath @($quoted) -ExcludePath @('tests/corpus')"
    if ($Report) {
        & pwsh -NoProfile -Command $style
    } else {
        Invoke-CheckedCommand pwsh @('-NoProfile', '-Command', $style)
    }

    Write-Stage '[lint] clang-format (C++)'
    $clangFormat = Resolve-PinnedTool 'clang-format' '22.1.3'
    $cxx = @(Get-OwnedFile @('src', 'include', 'tests') @('.cpp', '.hpp', '.h',
            '.c'))
    $arguments = @('--dry-run', '--style=file') + $cxx
    if ($Report) {
        & $clangFormat @arguments 2>&1 |
            Select-String 'warning:' | ForEach-Object { $_.Line } |
            ForEach-Object { ($_ -split ':')[0..1] -join ':' } |
            Sort-Object -Unique | Measure-Object |
            ForEach-Object { Write-Output "$($_.Count) files would change" }
    } else {
        Invoke-CheckedCommand $clangFormat (@('--Werror') + $arguments)
    }

    Write-Stage '[lint] gersemi (CMake)'
    $gersemi = Resolve-PinnedTool 'gersemi' '0.29.2'
    $cmake = @('CMakeLists.txt') + (Get-OwnedFile @('cmake') @('.cmake'))
    # Definitions teach gersemi the project's own functions, so an unknown
    # command is a real finding.
    $check = @('--check', '--warnings-as-errors',
        '--definitions', 'cmake', 'wsp/tools/cmake', '--') + $cmake
    if ($Report) {
        & $gersemi @check
    } else {
        Invoke-CheckedCommand $gersemi $check
    }

    Write-Stage '[lint] pre-commit configuration'
    $preCommit = Resolve-PinnedTool 'pre-commit' '4.6.2'
    Invoke-CheckedCommand $preCommit @('validate-config')

    Write-Output '[PASS] lint'
}
finally {
    Pop-Location
}
