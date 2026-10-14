<#
.SYNOPSIS
    Builds Keychron V1 Max (QMK) or B1 Pro (ZMK) firmware locally via Docker.
.PARAMETER Target
    Firmware target: 'v1_max' (default) or 'b1_pro'.
#>
[CmdletBinding()]
param(
    [ValidateSet('v1_max', 'b1_pro')]
    [string]$Target = 'v1_max'
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$outputDir = Join-Path $repoRoot 'build_output'
if (-not (Test-Path $outputDir)) { New-Item -ItemType Directory -Path $outputDir -Force | Out-Null }

if ($Target -eq 'v1_max') {
    Write-Host "==> Building Keychron V1 Max QMK Firmware via Docker..." -ForegroundColor Cyan

    $tempQmk = Join-Path ([System.IO.Path]::GetTempPath()) "qmk_build_$([System.Guid]::NewGuid().ToString('N'))"
    try {
        Write-Host "==> Cloning minimal Keychron QMK 2025q3..." -ForegroundColor DarkGray
        git clone --depth 1 --branch 2025q3 https://github.com/Keychron/qmk_firmware.git $tempQmk
        Set-Location $tempQmk
        git submodule update --init lib/chibios lib/chibios-contrib

        Write-Host "==> Applying local overlay from keymaps/qmk/v1_max/qmk_overlay..." -ForegroundColor DarkGray
        $overlaySrc = Join-Path $repoRoot 'keymaps\qmk\v1_max\qmk_overlay'
        Copy-Item -Path "$overlaySrc\*" -Destination $tempQmk -Recurse -Force

        Write-Host "==> Compiling in Docker container (qmkfm/qmk_cli:latest)..." -ForegroundColor Green
        docker run --rm `
            -v "${tempQmk}:/qmk_firmware" `
            -w /qmk_firmware `
            qmkfm/qmk_cli:latest `
            qmk compile -kb keychron/v1_max/ansi_encoder -km default

        $bin = Get-ChildItem -Path $tempQmk -Filter "*.bin" -Recurse | Select-Object -First 1
        if ($bin) {
            $dest = Join-Path $outputDir $bin.Name
            Copy-Item -Path $bin.FullName -Destination $dest -Force
            Write-Host "SUCCESS: Firmware built at: $dest" -ForegroundColor Green
        } else {
            Write-Error "Build finished but no .bin artifact was generated."
        }
    }
    finally {
        Set-Location $repoRoot
        if (Test-Path $tempQmk) { Remove-Item -Path $tempQmk -Recurse -Force -ErrorAction SilentlyContinue }
    }
}
elseif ($Target -eq 'b1_pro') {
    Write-Host "==> B1 Pro offline compilation requires Zephyr West workspace." -ForegroundColor Cyan
    Write-Host "==> Recommended: Push keymap changes to trigger GitHub Actions .github/workflows/build-zmk.yml for cloud compilation." -ForegroundColor Yellow
}
