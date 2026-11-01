<#
.SYNOPSIS
Checks the Microsoft class ids and ProgIDs of the library against the
installed MSCOMCT2.OCX.
.DESCRIPTION
Reads evidence/reference/mscomct2-2.0-type-library.json and compares each
class id, ProgID and server with the 32-bit registration of the installed
component. Skips (exit 0) when the component is not installed.
#>
[CmdletBinding()]
param(
    [string]$Reference = (Join-Path $PSScriptRoot `
        '../evidence/reference/mscomct2-2.0-type-library.json')
)
$ErrorActionPreference = 'Stop'
if (-not (Test-Path -LiteralPath $Reference)) {
    $Reference = Join-Path $PSScriptRoot `
        '../../../evidence/reference/mscomct2-2.0-type-library.json'
}
$json = Get-Content -LiteralPath $Reference -Raw | ConvertFrom-Json
$base = [Microsoft.Win32.RegistryKey]::OpenBaseKey('ClassesRoot', 'Registry32')
$problems = 0
foreach ($control in $json.controls) {
    $key = $base.OpenSubKey("CLSID\$($control.guid)")
    if ($null -eq $key) {
        Write-Host 'MSCOMCT2.OCX is not registered here: skipped.'
        exit 0
    }
    $progId = $key.OpenSubKey('ProgID').GetValue('')
    $server = $key.OpenSubKey('InprocServer32').GetValue('')
    if ($progId -ne $control.progId -or $server -notlike '*MSCOMCT2.OCX') {
        Write-Host "MISMATCH $($control.name): $progId $server"
        $problems++
    }
    else {
        Write-Host "ok $($control.name) $($control.guid) $progId"
    }
}
exit $(if ($problems -eq 0) { 0 } else { 1 })
