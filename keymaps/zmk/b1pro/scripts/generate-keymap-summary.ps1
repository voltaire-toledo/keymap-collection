<#
.SYNOPSIS
    Generates a structured Keymap Summary report (*-SUMMARY.md) analyzing all
    keyboard combinations, combos, tap-dances, hold-taps, and macros defined in a ZMK keymap.

.PARAMETER KeymapPath
    Path to the .keymap file to analyze.

.PARAMETER OutputPath
    Optional output markdown path. If omitted, defaults to <ReleaseDir>/<Release>-SUMMARY.md.

.PARAMETER AllReleases
    When specified, scans B1PRO/releases and B1PRO/QC-IN_PROGRESS for all .keymap files
    and generates companion *-SUMMARY.md reports for each.
#>
[CmdletBinding()]
param(
    [Parameter(ParameterSetName = 'Single', Mandatory = $true, Position = 0)]
    [string]$KeymapPath,

    [Parameter(ParameterSetName = 'Single')]
    [string]$OutputPath,

    [Parameter(ParameterSetName = 'Batch', Mandatory = $true)]
    [switch]$AllReleases
)

$ErrorActionPreference = 'Stop'

function Get-FriendlyKeyName {
    param([int]$pos)
    # 82-key matrix mapping for Keychron B1 Pro
    $map = @{
        0 = "Esc"; 1 = "F1"; 2 = "F2"; 3 = "F3"; 4 = "F4"; 5 = "F5"; 6 = "F6"; 7 = "F7"; 8 = "F8"; 9 = "F9"; 10 = "F10"; 11 = "F11"; 12 = "F12"; 13 = "Del";
        14 = "``~"; 15 = "1"; 16 = "2"; 17 = "3"; 18 = "4"; 19 = "5"; 20 = "6"; 21 = "7"; 22 = "8"; 23 = "9"; 24 = "0"; 25 = "-"; 26 = "="; 27 = "Bksp";
        28 = "Tab"; 29 = "Q"; 30 = "W"; 31 = "E"; 32 = "R"; 33 = "T"; 34 = "Y"; 35 = "U"; 36 = "I"; 37 = "O"; 38 = "P"; 39 = "["; 40 = "]"; 41 = "\";
        42 = "Caps/Fn"; 43 = "A"; 44 = "S"; 45 = "D"; 46 = "F"; 47 = "G"; 48 = "H"; 49 = "J"; 50 = "K"; 51 = "L"; 52 = ";"; 53 = "'"; 54 = "Enter";
        55 = "LShift"; 56 = "Z"; 57 = "X"; 58 = "C"; 59 = "V"; 60 = "B"; 61 = "N"; 62 = "M"; 63 = ","; 64 = "."; 65 = "/"; 66 = "RShift";
        67 = "LCtrl"; 68 = "LAlt/Win"; 69 = "LCmd/Alt"; 70 = "Space"; 71 = "RCmd/Alt"; 72 = "Fn"; 73 = "Left"; 74 = "Up"; 75 = "Down"; 76 = "Right";
        77 = "Direct_Win_Mac"; 78 = "Direct_BLE"; 79 = "Direct_24G"; 80 = "Direct_Chg"; 81 = "Direct_Chgd"
    }
    if ($map.ContainsKey($pos)) { return $map[$pos] }
    return "pos $pos"
}

function Generate-KeymapSummary {
    param(
        [string]$TargetKeymap,
        [string]$TargetOutput
    )

    $t = [char]96

    if (-not (Test-Path -LiteralPath $TargetKeymap)) {
        Write-Warning "File not found: $TargetKeymap"
        return
    }

    $raw = Get-Content -Raw -LiteralPath $TargetKeymap
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $TargetKeymap).Hash.ToLower()
    $fileName = Split-Path -Leaf $TargetKeymap
    $dirName = Split-Path -Parent $TargetKeymap

    # Derive release label
    $releaseLabel = $fileName -replace '\.keymap$', ''
    if ($releaseLabel -match '^(v\d+(\.\d+)*(-[a-zA-Z0-9_\-]+)?)') {
        $versionTitle = $matches[1]
    } elseif ($fileName -match 'factory|zmk_b1pro') {
        $versionTitle = "Factory v1.0.3"
    } else {
        $versionTitle = $releaseLabel
    }

    if (-not $TargetOutput) {
        $baseName = $releaseLabel -replace '-candidate$', '' -replace '-unified$', '' -replace '-sync-layers$', '' -replace '-mac-win-full-layers$', '' -replace '-HRM-QC_PASSED$', '' -replace '-esc-caps$', ''
        if ($fileName -match 'factory|zmk_b1pro') { $baseName = "factory" }
        $TargetOutput = Join-Path $dirName "$baseName-SUMMARY.md"
    }

    $sb = [System.Text.StringBuilder]::new()
    [void]$sb.AppendLine("# $versionTitle Keymap Combination & Feature Summary")
    [void]$sb.AppendLine("")
    [void]$sb.AppendLine("- **Keymap Source:** $t$fileName$t")
    [void]$sb.AppendLine("- **SHA-256:** $t$hash$t")
    [void]$sb.AppendLine("- **Generated:** $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')")
    [void]$sb.AppendLine("")

    # 1. Combos Section
    [void]$sb.AppendLine("## 1. Formal ZMK Combos (${t}combos${t} block)")
    [void]$sb.AppendLine("")
    $combos = [regex]::Matches($raw, '(?ms)(?:/\*(?<doc>.*?)\*/\s*)?(?<name>combo_[A-Za-z0-9_]+)\s*\{\s*(?<body>[^\}]+)\};')
    if ($combos.Count -gt 0) {
        [void]$sb.AppendLine("| Combo Identifier | Trigger Keys (Positions) | Active Layers | Timeout | Binding / Action | Description |")
        [void]$sb.AppendLine("| :--- | :--- | :--- | :--- | :--- | :--- |")

        foreach ($node in $combos) {
            $name = $node.Groups['name'].Value.Trim()
            $body = $node.Groups['body'].Value
            $doc = $node.Groups['doc'].Value.Trim()

            $timeout = ([regex]::Match($body, 'timeout-ms\s*=\s*<(?<v>\d+)>')).Groups['v'].Value
            if (-not $timeout) { $timeout = "default" } else { $timeout = "${timeout}ms" }

            $layers = ([regex]::Match($body, 'layers\s*=\s*<(?<v>[^>]+)>')).Groups['v'].Value.Trim()
            if (-not $layers) { $layers = "All" }

            $positionsRaw = ([regex]::Match($body, 'key-positions\s*=\s*<(?<v>[^>]+)>')).Groups['v'].Value
            $positionsClean = [regex]::Replace($positionsRaw, '(?s)/\*.*?\*/', '').Trim()
            $positions = @($positionsClean -split '\s+' | Where-Object { $_ -match '^\d+$' } | ForEach-Object { [int]$_ })

            $posDesc = @($positions | ForEach-Object { "$($_): $(Get-FriendlyKeyName $_)" }) -join " + "

            $binding = ([regex]::Match($body, 'bindings\s*=\s*<(?<v>[^>]+)>')).Groups['v'].Value.Trim()

            # Clean brief description from doc comment or binding
            $desc = ""
            if ($doc -and $doc -notmatch 'Copyright' -and $doc -notmatch 'Keychron B1 Pro v0\.') {
                $lines = $doc -split '\r?\n' | ForEach-Object { $_.Trim(' *') } | Where-Object { $_ -and -not $_.StartsWith('*') }
                foreach ($l in $lines) {
                    if ($l -match '^[A-Za-z0-9]') {
                        $desc = $l -replace '\|', '\|'
                        break
                    }
                }
            }
            if (-not $desc) {
                if ($name -match 'reset|recover|jz' -or $binding -match 'long_press_recover') { $desc = "Factory Reset: Erases BLE bonds and resets MCU" }
                elseif ($name -match 'bat|battery|^combo_b$' -or $binding -match 'OUT_BAT') { $desc = "Battery Level Query: Pulses battery LED" }
                elseif ($name -match 'xl|change' -or $binding -match 'long_press_change') { $desc = "Function/Media Row Exchange" }
                elseif ($name -match 'win' -or $binding -match 'long_press_fn_win') { $desc = "Windows Key Lock: Suppresses GUI keys globally" }
                elseif ($name -match 'bootloader' -or $binding -match 'bootloader') { $desc = "DFU Bootloader Entry" }
                elseif ($name -match '^combo_t\d+') { $desc = "Factory Production Test Chord" }
                else { $desc = $binding }
            }

            [void]$sb.AppendLine("| $t$name$t | $posDesc | $t$layers$t | $timeout | $t$binding$t | $desc |")
        }
        [void]$sb.AppendLine("")
    } else {
        [void]$sb.AppendLine("*No formal ${t}zmk,combos${t} block declared in this keymap.*")
        [void]$sb.AppendLine("")
    }

    # 2. Tap Dance Behaviors
    [void]$sb.AppendLine("## 2. Tap Dance Combinations (${t}zmk,behavior-tap-dance${t})")
    [void]$sb.AppendLine("")
    $tdMatches = [regex]::Matches($raw, '(?ms)(?:/\*(?<doc>.*?)\*/\s*)?(?://(?<lineComment>[^\n]*)\n\s*)?(?<name>[A-Za-z0-9_]+)\s*:\s*[A-Za-z0-9_]*\s*\{\s*compatible\s*=\s*"zmk,behavior-tap-dance";(?<body>.*?)\};')
    if ($tdMatches.Count -gt 0) {
        [void]$sb.AppendLine("| Tap Dance Name | Tapping Term | Outcomes (1-Tap / 2-Tap / 3-Tap) |")
        [void]$sb.AppendLine("| :--- | :--- | :--- |")

        foreach ($td in $tdMatches) {
            $name = $td.Groups['name'].Value.Trim()
            $body = $td.Groups['body'].Value
            $term = ([regex]::Match($body, 'tapping-term-ms\s*=\s*<(?<v>\d+)>')).Groups['v'].Value
            if ($term) { $term = "${term}ms" } else { $term = "default" }

            $bSec = ([regex]::Match($body, 'bindings\s*=\s*(?<bsec>.*?);')).Groups['bsec'].Value
            $bMatches = [regex]::Matches($bSec, '<(?<bseq>[^>]+)>')
            $bList = @($bMatches | ForEach-Object { $_.Groups['bseq'].Value.Trim() })

            $outcomeList = @()
            for ($i = 0; $i -lt $bList.Count; $i++) {
                $tapCount = $i + 1
                $b = $bList[$i]
                $outcomeList += "**${tapCount}-Tap:** $t$b$t"
            }
            $outcomes = $outcomeList -join "<br>"

            [void]$sb.AppendLine("| $t$name$t | $term | $outcomes |")
        }
        [void]$sb.AppendLine("")
    } else {
        [void]$sb.AppendLine("*No tap-dance behaviors declared.*")
        [void]$sb.AppendLine("")
    }

    # 3. Dual-Role Hold-Tap Behaviors
    [void]$sb.AppendLine("## 3. Dual-Role Hold-Tap Behaviors (${t}zmk,behavior-hold-tap${t})")
    [void]$sb.AppendLine("")
    $htMatches = [regex]::Matches($raw, '(?ms)(?:/\*(?<doc>.*?)\*/\s*)?(?://(?<lineComment>[^\n]*)\n\s*)?(?<name>[A-Za-z0-9_]+)\s*:\s*[A-Za-z0-9_]*\s*\{\s*compatible\s*=\s*"zmk,behavior-hold-tap";(?<body>.*?)\};')
    if ($htMatches.Count -gt 0) {
        [void]$sb.AppendLine("| Behavior Name | Flavor | Tapping Term | Quick Tap / Idle | Retro-Tap | Bindings (Hold / Tap) |")
        [void]$sb.AppendLine("| :--- | :--- | :--- | :--- | :--- | :--- |")

        foreach ($ht in $htMatches) {
            $name = $ht.Groups['name'].Value.Trim()
            $body = $ht.Groups['body'].Value
            $flavor = ([regex]::Match($body, 'flavor\s*=\s*"(?<v>[^"]+)"')).Groups['v'].Value
            $term = ([regex]::Match($body, 'tapping-term-ms\s*=\s*<(?<v>\d+)>')).Groups['v'].Value
            if ($term) { $term = "${term}ms" } else { $term = "-" }

            $quickTap = ([regex]::Match($body, 'quick-tap-ms\s*=\s*<(?<v>\d+)>')).Groups['v'].Value
            $idle = ([regex]::Match($body, 'require-prior-idle-ms\s*=\s*<(?<v>\d+)>')).Groups['v'].Value
            $timingDetails = @()
            if ($quickTap) { $timingDetails += "QT: ${quickTap}ms" }
            if ($idle) { $timingDetails += "Idle: ${idle}ms" }
            $timingStr = if ($timingDetails.Count -gt 0) { $timingDetails -join ", " } else { "-" }

            $hasRetro = if ($body -match 'retro-tap;') { "Yes" } else { "No" }

            $bSec = ([regex]::Match($body, 'bindings\s*=\s*(?<bsec>.*?);')).Groups['bsec'].Value
            $bMatches = [regex]::Matches($bSec, '<(?<bseq>[^>]+)>')
            $bList = @($bMatches | ForEach-Object { $_.Groups['bseq'].Value.Trim() })
            $bDesc = if ($bList.Count -ge 2) { "Hold: $t$($bList[0])$t<br>Tap: $t$($bList[1])$t" } else { "$t$bSec$t" }

            [void]$sb.AppendLine("| $t$name$t | $t$flavor$t | $term | $timingStr | $hasRetro | $bDesc |")
        }
        [void]$sb.AppendLine("")
    } else {
        [void]$sb.AppendLine("*No hold-tap behaviors declared.*")
        [void]$sb.AppendLine("")
    }

    # 4. Macros Section
    [void]$sb.AppendLine("## 4. Custom Macros (${t}ZMK_MACRO${t})")
    [void]$sb.AppendLine("")
    $macroMatches = [regex]::Matches($raw, '(?m)^\s*ZMK_MACRO1?\s*\(\s*(?<name>[A-Za-z0-9_]+)\s*,\s*bindings\s*=\s*<(?<bindings>[^>]+)>\s*;\s*\)')
    if ($macroMatches.Count -gt 0) {
        [void]$sb.AppendLine("| Macro Name | Sequence Output |")
        [void]$sb.AppendLine("| :--- | :--- |")
        foreach ($m in $macroMatches) {
            $mName = $m.Groups['name'].Value.Trim()
            $mBindings = $m.Groups['bindings'].Value.Trim()
            [void]$sb.AppendLine("| $t$mName$t | $t$mBindings$t |")
        }
        [void]$sb.AppendLine("")
    } else {
        [void]$sb.AppendLine("*No custom macros declared.*")
        [void]$sb.AppendLine("")
    }

    # 5. Layer Map Breakdown
    [void]$sb.AppendLine("## 5. Layer Structure & Special Activations")
    [void]$sb.AppendLine("")
    $layers = [regex]::Matches($raw, '(?ms)^\s*(?<name>[A-Za-z_][A-Za-z0-9_]*)\s*\{\s*bindings\s*=\s*<(?<bindings>.*?)>;')
    [void]$sb.AppendLine("- **Total Defined Layers:** $($layers.Count)")
    [void]$sb.AppendLine("| Index | Layer Identifier | Special Dual-Role & Switching Keys |")
    [void]$sb.AppendLine("| :--- | :--- | :--- |")
    for ($i = 0; $i -lt $layers.Count; $i++) {
        $layerName = $layers[$i].Groups['name'].Value
        $layerBindings = $layers[$i].Groups['bindings'].Value

        # Extract special activations in this layer
        $specialKeys = @()
        $matchesLt = [regex]::Matches($layerBindings, '&lt\s+[A-Za-z0-9_]+\s+[A-Za-z0-9_]+')
        foreach ($m in $matchesLt) { $specialKeys += "$t$($m.Value.Trim())$t" }

        $matchesMt = [regex]::Matches($layerBindings, '&mt\s+[^&>\n]+')
        foreach ($m in $matchesMt) { $specialKeys += "$t$($m.Value.Trim())$t" }

        $matchesTd = [regex]::Matches($layerBindings, '&td_[A-Za-z0-9_]+\s+\d+')
        foreach ($m in $matchesTd) { $specialKeys += "$t$($m.Value.Trim())$t" }

        $matchesHt = [regex]::Matches($layerBindings, '&(?:fn_esc|esc_caps|long_press)[A-Za-z0-9_]*\s+[^&>\n]+')
        foreach ($m in $matchesHt) { $specialKeys += "$t$($m.Value.Trim())$t" }

        $uniqueSpecial = ($specialKeys | Select-Object -Unique) -join ", "
        if (-not $uniqueSpecial) { $uniqueSpecial = "Standard Direct Mappings" }

        [void]$sb.AppendLine("| $i | $t$layerName$t | $uniqueSpecial |")
    }
    [void]$sb.AppendLine("")

    Set-Content -LiteralPath $TargetOutput -Value $sb.ToString() -Encoding utf8
    Write-Host "Generated summary: $TargetOutput"
}

if ($PSCmdlet.ParameterSetName -eq 'Single') {
    Generate-KeymapSummary -TargetKeymap $KeymapPath -TargetOutput $OutputPath
}
elseif ($AllReleases) {
    Write-Host "Scanning B1PRO/releases and B1PRO/QC-IN_PROGRESS for keymap files..."
    $targets = @()
    if (Test-Path 'B1PRO/releases') {
        $targets += Get-ChildItem -Recurse -Filter '*.keymap' 'B1PRO/releases'
    }
    if (Test-Path 'B1PRO/QC-IN_PROGRESS') {
        $targets += Get-ChildItem -Filter '*.keymap' 'B1PRO/QC-IN_PROGRESS'
    }

    foreach ($file in $targets) {
        # Skip duplicate backup files like "copy.keymap"
        if ($file.Name -match 'copy\.keymap$') { continue }
        # If there are multiple keymaps in a directory (like v0.8-HRM and v0.8-HRM-QC_PASSED), prefer QC_PASSED
        if ($file.Name -eq 'v0.8-HRM.keymap' -and (Test-Path (Join-Path $file.DirectoryName 'v0.8-HRM-QC_PASSED.keymap'))) { continue }
        Generate-KeymapSummary -TargetKeymap $file.FullName
    }
    Write-Host "All keymap summaries generated successfully."
}
