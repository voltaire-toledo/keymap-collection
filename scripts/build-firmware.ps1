<#
.SYNOPSIS
    Builds firmware through the shared Bash/Docker procedure.
    Windows requires WSL2 with Docker Desktop integration and Git/Bash in WSL.
#>
[CmdletBinding()]
param(
    [ValidateSet('v1_max', 'b1_pro')]
    [string]$Target = 'v1_max'
)
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$buildTarget = if ($Target -eq 'v1_max') { 'qmk' } else { 'zmk' }
if ($env:OS -eq 'Windows_NT') {
    & wsl.exe --cd $repoRoot bash scripts/build-firmware.sh $buildTarget
} else {
    Push-Location $repoRoot
    try { & bash scripts/build-firmware.sh $buildTarget }
    finally { Pop-Location }
}
if ($LASTEXITCODE -ne 0) { throw "Firmware build failed (exit $LASTEXITCODE)." }
