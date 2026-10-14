# ZMK Docker Build Process: Keychron B1 Pro (v0.25.x / v0.26)

This document specifies the exact, reproducible step-by-step procedure for compiling the Keychron B1 Pro ZMK firmware into a `.uf2` artifact using containerized tooling.

---

## 1. Prerequisites & Specifications

* **Target Hardware**: Keychron B1 Pro (ANSI / US layout)
* **Shield Name**: `keychron_b1_us`
* **Board Name**: `keychron`
* **Docker Container Image**: `zmkfirmware/zmk-build-arm:3.2`
* **Reference SHA-256 Benchmark (v0.25.8 Candidate)**:
  `d917b9b95a845dd03baffb582882254415ea239951d81b1246b93a9797612a76`
* **Reference Artifact Path**:
  `D:\CODE\VT-Mello-Keymaps\ZMK\B1PRO\QC-IN_PROGRESS\v0.25.8-candidate.uf2`

---

## 2. Variables To Determine Beforehand

Only define the target version number and the relative path to the keymap:

```powershell
$KeymapVersion = "v0.25.8"
$KeymapPath = "keymaps/zmk/b1pro/B1PRO/QC-IN_PROGRESS/v0.25.8-candidate.keymap"
```

---

## 3. Step-by-Step Containerized Build Instructions

Execute each step exactly as written in PowerShell. Do not modify commands or make ad-hoc decisions.

### Step 3.1: Clean and Prepare Staging Directory
```powershell
$StageDir = "D:\QODE\vtg.pub-keymap-collection\.zmk-docker-stage"
if (Test-Path $StageDir) { Remove-Item -Path $StageDir -Recurse -Force }
New-Item -ItemType Directory -Path "$StageDir\output" -Force | Out-Null
```

### Step 3.2: Initialize West Workspace and Apply Patches
Run the versioned Docker container `zmkfirmware/zmk-build-arm:3.2` to check out the vendor `b1-pro-v.0.25.X` branch, initialize West locally against `app`, update dependencies, apply kernel/driver patches, and inject the target keymap:

```powershell
docker run --rm `
  -v "${StageDir}:/workspace" `
  -v "D:\QODE\vtg.pub-keymap-collection\keymaps\zmk\b1pro:/keymaps:ro" `
  -v "D:\QODE\vtg.pub-keymap-collection:/repo:ro" `
  -w /workspace `
  zmkfirmware/zmk-build-arm:3.2 `
  bash -c "
    set -e
    git config --global --add safe.directory '*'
    git clone --depth 1 -b b1-pro-v.0.25.X https://github.com/voltaire-toledo/submod-zmk.git zmk
    cd zmk
    west init -l app
    west update -n
    west zephyr-export

    # 1. Apply Zephyr kernel fixes
    cd zephyr
    git apply ../0001-esb-nrf-fix.patch || true
    git apply ../0002-zephyr-fix-clang-format-requirement.patch || true
    cd ..

    # 2. Apply portable DFU header patch
    cp -f /keymaps/patches/tdfu_prv.h app/src/dfu/tdfu_prv.h

    # 3. Inject target keymap
    cp -f /repo/${KeymapPath} app/boards/shields/keychron/b1/us/keychron_b1_us.keymap
  "
```

### Step 3.3: Compile the Firmware
Run the compiler inside the container to build the `.uf2` binary with bit-for-bit deterministic reproducibility:

```powershell
docker run --rm `
  -v "${StageDir}\zmk:/workspace/zmk" `
  -v "${StageDir}\output:/output" `
  -w /workspace/zmk `
  zmkfirmware/zmk-build-arm:3.2 `
  bash -c "
    set -e
    git config --global --add safe.directory '*'
    export SOURCE_DATE_EPOCH=1791108125
    west build -s app -b keychron -d build/b1pro -p always -- -DSHIELD=keychron_b1_us -DBUILD_VERSION=c080d0853f98
    cp build/b1pro/zephyr/zmk.uf2 /output/zmk_b1pro.uf2
  "
```

### Step 3.4: Verify Generated Artifact Checksum
Compute the SHA-256 hash of the generated UF2 binary and compare it against the reference benchmark:

```powershell
$GeneratedUf2 = "$StageDir\output\zmk_b1pro.uf2"
if (!(Test-Path $GeneratedUf2)) {
    throw "UF2 artifact was not generated at $GeneratedUf2"
}
$ComputedHash = (Get-FileHash -Path $GeneratedUf2 -Algorithm SHA256).Hash.ToLower()
$ReferenceHash = "d917b9b95a845dd03baffb582882254415ea239951d81b1246b93a9797612a76"

Write-Host "Generated SHA256 : $ComputedHash"
Write-Host "Reference SHA256 : $ReferenceHash"

if ($ComputedHash -eq $ReferenceHash) {
    Write-Host "VERIFICATION SUCCESS: Output UF2 hash matches reference benchmark exactly!" -ForegroundColor Green
} else {
    Write-Warning "Checksum mismatch: Expected $ReferenceHash, got $ComputedHash"
}
```

---

## 4. Failure Protocol & Clean-Up Rule

If any step fails:
1. Immediately remove the stage directory:
   ```powershell
   Remove-Item -Path "D:\QODE\vtg.pub-keymap-collection\.zmk-docker-stage" -Recurse -Force -ErrorAction SilentlyContinue
   ```
2. Remove any hanging container instances:
   ```powershell
   docker ps -a --filter "ancestor=zmkfirmware/zmk-build-arm:3.2" -q | ForEach-Object { docker rm -f $_ }
   ```
3. Report the exact error output back to the orchestrating agent without altering commands on the fly.
4. Any correction must be delivered through an updated version of this document.
