param(
    [Parameter(Mandatory = $true)]
    [string]$KeymapPath,
    [ValidateRange(1, 255)]
    [int]$ExpectedLayerCount = 12,
    [ValidateRange(1, 255)]
    [int]$ExpectedBindingsPerLayer = 82
)

$ErrorActionPreference = 'Stop'
$errors = [System.Collections.Generic.List[string]]::new()

function Add-LintError {
    param([string]$Message)
    $errors.Add($Message)
}

function Remove-Comments {
    param([string]$Text)
    $withoutBlocks = [regex]::Replace($Text, '(?s)/\*.*?\*/', '')
    [regex]::Replace($withoutBlocks, '(?m)//.*$', '')
}

if (-not (Test-Path -LiteralPath $KeymapPath -PathType Leaf)) {
    Add-LintError "Keymap does not exist: $KeymapPath"
}
else {
    $content = Get-Content -Raw -LiteralPath $KeymapPath

    $lineNumber = 0
    foreach ($line in (Get-Content -LiteralPath $KeymapPath)) {
        $lineNumber++
        if ($line -match '[\t]') { Add-LintError "Line $lineNumber contains a tab; use spaces." }
        if ($line -match '[ \t]+$') { Add-LintError "Line $lineNumber has trailing whitespace." }
    }

    $openBlocks = [regex]::Matches($content, '/\*').Count
    $closeBlocks = [regex]::Matches($content, '\*/').Count
    if ($openBlocks -ne $closeBlocks) { Add-LintError "Block comments are unbalanced: $openBlocks opening, $closeBlocks closing." }

    $code = Remove-Comments $content
    if ($code -match '\?') { Add-LintError 'Unresolved placeholder marker (?) found outside comments.' }
    foreach ($pair in @(@('{', '}'), @('<', '>'), @('(', ')'))) {
        $openCount = [regex]::Matches($code, [regex]::Escape($pair[0])).Count
        $closeCount = [regex]::Matches($code, [regex]::Escape($pair[1])).Count
        if ($openCount -ne $closeCount) { Add-LintError "Delimiters '$($pair[0])$($pair[1])' are unbalanced: $openCount opening, $closeCount closing." }
    }

    $declared = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
    foreach ($match in [regex]::Matches($code, '(?m)^\s*(?<name>[A-Za-z_][A-Za-z0-9_]*)\s*:')) {
        [void]$declared.Add($match.Groups['name'].Value)
    }
    foreach ($match in [regex]::Matches($code, 'ZMK_MACRO\(\s*(?<name>[A-Za-z_][A-Za-z0-9_]*)\s*,')) {
        [void]$declared.Add($match.Groups['name'].Value)
    }

    $builtIns = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
    foreach ($name in @('bootloader', 'bt', 'kp', 'lt', 'mo', 'mt', 'none', 'out', 'to', 'trans')) {
        [void]$builtIns.Add($name)
    }

    $layerPattern = '(?ms)^\s*(?<name>[A-Za-z_][A-Za-z0-9_]*)\s*\{\s*bindings\s*=\s*<(?<bindings>.*?)>;'
    $layers = @([regex]::Matches($content, $layerPattern))
    if ($layers.Count -ne $ExpectedLayerCount) {
        Add-LintError "Found $($layers.Count) binding layers; expected $ExpectedLayerCount."
    }

    $names = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
    foreach ($layer in $layers) {
        $name = $layer.Groups['name'].Value
        if (-not $names.Add($name)) { Add-LintError "Duplicate binding layer name: $name" }

        $bindingText = Remove-Comments $layer.Groups['bindings'].Value
        $entries = @(
            [regex]::Split($bindingText, '(?=&[A-Za-z_][A-Za-z0-9_]*)') |
                ForEach-Object { ($_ -replace '\s+', ' ').Trim() } |
                Where-Object { $_.StartsWith('&') }
        )
        if ($entries.Count -ne $ExpectedBindingsPerLayer) {
            Add-LintError "Layer $name has $($entries.Count) bindings; expected $ExpectedBindingsPerLayer."
        }

        foreach ($entry in $entries) {
            $behavior = [regex]::Match($entry, '^&(?<name>[A-Za-z_][A-Za-z0-9_]*)').Groups['name'].Value
            if (-not $builtIns.Contains($behavior) -and -not $declared.Contains($behavior)) {
                Add-LintError "Layer $name references undefined behavior &$behavior."
            }
        }
    }
}

if ($errors.Count -gt 0) {
    $errors | ForEach-Object { [Console]::Error.WriteLine($_) }
    exit 1
}

Write-Output "Keymap lint passed: $ExpectedLayerCount unique layers, $ExpectedBindingsPerLayer bindings per layer, balanced delimiters, declared behaviors, and no formatting or placeholder errors."
