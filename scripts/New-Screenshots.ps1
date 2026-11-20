<#
.SYNOPSIS
Regenerates the README screenshots of the examples in docs/images.
.DESCRIPTION
Runs each example (examples/<name>/<name>.vbp) with wfc --gui and
--screenshots, which saves a PNG of the window the program opened, and copies
the first picture of each to docs/images/<name>.png. Only the program's own
windows are captured. Each example opens a real window for a few seconds.
wfc is found by Resolve-WfcExecutable (WFC_EXE).
.PARAMETER ExitAfter
Milliseconds the program runs before it closes (default 3000).
.EXAMPLE
pwsh -NoProfile -File scripts/New-Screenshots.ps1
#>
[CmdletBinding()]
param([int]$ExitAfter = 3000)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
Import-Module (Join-Path $PSScriptRoot 'WfcChecks.psm1') -Force

$root = Split-Path -Parent $PSScriptRoot
$wfc = Resolve-WfcExecutable
if (-not $wfc) { throw 'No wfc executable: set WFC_EXE.' }
$work = Join-Path ([IO.Path]::GetTempPath()) 'wfc-mscomctl2-shots'
Remove-Item -Recurse -Force -ErrorAction SilentlyContinue $work
New-Item -ItemType Directory -Force -Path $work | Out-Null
$images = Join-Path $root 'docs/images'
foreach ($name in 'updown', 'flatscrollbar', 'monthview', 'dtpicker',
    'animation') {
    $folder = Join-Path $work $name
    $project = Join-Path $root "examples/$name/$name.vbp"
    Invoke-CheckedCommand $wfc @('run', $project, '--gui', '--screenshots',
        $folder, '--exit-after', "$ExitAfter")
    $shown = Get-ChildItem $folder -Filter '*-shown.png' |
        Select-Object -First 1
    if (-not $shown) { throw "No screenshot of $name." }
    Copy-Item $shown.FullName (Join-Path $images "$name.png") -Force
    Write-Output "docs/images/$name.png"
}
