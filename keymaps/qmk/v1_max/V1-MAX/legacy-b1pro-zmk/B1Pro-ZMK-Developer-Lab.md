# B1 Pro ZMK developer lab

Audience: new firmware interns  
Time: 60–90 minutes  
Outcome: prepare a repeatable Windows ZMK environment, make one safe B1 Pro keymap change, and produce a locally verified UF2.

## Lab rules

This lab builds firmware only. It does not authorize flashing a keyboard, changing an approved release, or promoting a release.

- Work in a disposable lab checkout under a physical D:\CODE\... path. Do not use the C:\Users\Jigs\CODE junction; CMake records absolute paths.
- Do not use Docker. This B1 Pro fork uses the native Windows tools below. WSL is allowed only when it is available and approved by the instructor.
- Never edit B1PRO/releases/, an archived UF2, or an approved candidate in place.
- Do not change positions 77–81. They are hidden hardware controls, not spare keys. Preserve Fn + minus boot recovery.
- Do not copy a whole checkout together with build, .isolated-builds, .venv, or zephyr-sdk.zip. That is the common cause of multi-gigabyte lab folders.

## 1. Required tools

| Requirement        | Lab standard                               | Check                                |
| ------------------ | ------------------------------------------ | ------------------------------------ |
| Shell              | PowerShell 7                               | pwsh --version                       |
| Source control     | Git                                        | git --version                        |
| Python environment | Repository .venv                           | .\.venv\Scripts\python.exe --version |
| Build tools        | West, CMake, Ninja in .venv                | Step 4                               |
| Toolchain          | Zephyr SDK 0.15.2                          | arm-zephyr-eabi-gcc.exe --version    |
| Firmware source    | Keychron keychron_bpro fork plus ESB patch | git status --short                   |

The current lab image keeps the SDK at:

```
<repo>\.isolated-builds\sdk-r1\zephyr-sdk-0.15.2
```

If it is absent, stop and ask for the approved SDK bundle/path. Do not substitute a newer SDK because it happens to be installed.

## 2. Obtain a clean lab checkout

For a new vendor checkout, run the fork’s documented bootstrap sequence from a physical directory such as D:\CODE\B1Pro-Intern-Lab:

```powershell
New-Item -ItemType Directory -Path D:\CODE\B1Pro-Intern-Lab -Force | Out-Null
Set-Location D:\CODE\B1Pro-Intern-Lab
git clone -b keychron_bpro https://github.com/keychron/zmk.git ZMK
Set-Location .\ZMK
py -3.11 -m venv .venv
.\.venv\Scripts\python.exe -m pip install --upgrade pip
.\.venv\Scripts\python.exe -m pip install west
& .\.venv\Scripts\python.exe -m west init -l app
& .\.venv\Scripts\python.exe -m west update
Set-Location zephyr
git am ..\0001-esb-nrf-fix.patch
git apply ..\0002-zephyr-fix-clang-format-requirement.patch
Set-Location ..
```

The manifest is app/west.yml; .west/config points West at app and the downloaded
zephyr tree. Both patch commands are one-time clean-checkout operations. If a
patch is already applied, do not force it.

Install the remaining Python build tools. If the instructor supplies a known
good .venv, verify it rather than rebuilding it:

```powershell
.\.venv\Scripts\python.exe -m pip install west ninja cmake
.\.venv\Scripts\python.exe -m pip install -r app\scripts\requirements.txt
```

Python 3.11 is the lab default. Use the supplied .venv when one exists; do not rebuild it merely to “clean” it.

## 3. Find the B1 Pro files

From the repository root:

```powershell
rg --files app\boards\shields\keychron\b1\us
rg -n "layer_base|bindings|LAYER_BASE" app\boards\shields\keychron\b1\us\keychron_b1_us.keymap
```

| File                                                     | Purpose                                                      | Lab action                                                 |
| -------------------------------------------------------- | ------------------------------------------------------------ | ---------------------------------------------------------- |
| app/boards/shields/keychron/b1/us/keychron_b1_us.keymap  | Shield keymap consumed by the B1 US build.                   | Stage a disposable candidate into it just before building. |
| app/boards/shields/keychron/b1/us/keychron_b1_us.overlay | Physical 82-position matrix order and five hidden positions. | Read only.                                                 |
| app/boards/shields/keychron/b1/us/keychron_b1_us.conf    | B1 Pro identity and fixed-keymap setting.                    | Read only.                                                 |
| app/boards/arm/keychron/keychron.dts                     | Board-level hardware definition.                             | Read only.                                                 |
| app/Kconfig and app/src/launcher                         | Vendor fixed-keymap support.                                 | Out of scope.                                              |
| B1PRO/DESIGN/B1Pro-ZMK-KEYMAP.md                         | Human-reviewed design illustrations.                         | Read only.                                                 |
| B1PRO/QC-IN_PROGRESS                                     | Candidates, release records, checklists, unpassed UF2s.      | Store the lab candidate here.                              |
| B1PRO/releases/                                          | Hardware-approved releases only.                             | Never edit.                                                |

The binding list is positional: position 0 is Esc, position 1 is physical F1, and so on. The matrix overlay comments are the physical-key authority; do not infer numbers from a keyboard photo.

## 4. Verify the environment

Run this preflight from the repository root. The real CMake package path is intentional: this Windows environment can have a non-working lightweight .venv\Scripts\cmake.exe wrapper.

```powershell
$Repo = (Resolve-Path .).Path
$Sdk = Join-Path $Repo '.isolated-builds\sdk-r1\zephyr-sdk-0.15.2'
$CMakeBin = Join-Path $Repo '.venv\Lib\site-packages\cmake\data\bin'
$Python = Join-Path $Repo '.venv\Scripts\python.exe'

Test-Path "$Sdk\cmake\Zephyr-sdkConfig.cmake"
Test-Path "$Sdk\arm-zephyr-eabi\bin\arm-zephyr-eabi-gcc.exe"
& $Python -m west --version
& "$CMakeBin\cmake.exe" --version
& (Join-Path $Repo '.venv\Scripts\ninja.exe') --version
```

Both Test-Path commands must return True. The remaining commands must print versions. Stop for help if preflight fails.

## 5. Safe configuration exercise

Change only Layer 0, position 1: physical F1. The factory binding sends brightness-decrease. The lab candidate will send F13, a harmless and easy-to-observe keycode. It does not touch a direct control, bootloader action, layer selector, or hold-tap.

Create a candidate copy first:

```powershell
$Candidate = 'B1PRO\QC-IN_PROGRESS\intern-<your-name>-f1-f13.keymap'
Copy-Item 'app\boards\shields\keychron\b1\us\keychron_b1_us.keymap' $Candidate
```

Open the candidate. In layer_base, its first bindings begin:

```dts
&esc_caps CAPS ESC &kp C_BRIGHTNESS_DEC &kp C_BRIGHTNESS_INC
```

Replace only the second binding and keep a position comment:

```dts
&esc_caps CAPS ESC /* p1: physical F1 - LAB ONLY */ &kp F13 &kp C_BRIGHTNESS_INC
```

Review the narrow change and capture the candidate identity:

```powershell
git diff --no-index -- app\boards\shields\keychron\b1\us\keychron_b1_us.keymap $Candidate
Get-FileHash -Algorithm SHA256 $Candidate
```

git diff --no-index returns exit code 1 when it finds the expected difference. That is not a failure. Verify that the only functional change is brightness-decrease to F13 at physical position 1.

## 6. Stage and compile the candidate

The shield expects a fixed keymap filename. Stage only in the disposable lab checkout:

```powershell
$ShieldKeymap = 'app\boards\shields\keychron\b1\us\keychron_b1_us.keymap'
Copy-Item $Candidate $ShieldKeymap -Force
```

Configure and compile serially:

```powershell
$Repo = (Resolve-Path .).Path
$Sdk = Join-Path $Repo '.isolated-builds\sdk-r1\zephyr-sdk-0.15.2'
$CMakeBin = Join-Path $Repo '.venv\Lib\site-packages\cmake\data\bin'
$Python = Join-Path $Repo '.venv\Scripts\python.exe'
$BuildDir = 'build\intern-f1-f13'

$env:PATH = "$CMakeBin;$(Join-Path $Repo '.venv\Scripts');$env:PATH"
$env:ZEPHYR_TOOLCHAIN_VARIANT = 'zephyr'
$env:ZEPHYR_SDK_INSTALL_DIR = $Sdk
$env:GIT_CONFIG_COUNT = '1'
$env:GIT_CONFIG_KEY_0 = 'safe.directory'
$env:GIT_CONFIG_VALUE_0 = $Repo

& $Python -m west build -s app -b keychron -d $BuildDir -p always --cmake-only -- -DSHIELD=keychron_b1_us
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }

& ninja -C $BuildDir -j1
if ($LASTEXITCODE -ne 0) { throw 'Firmware compilation failed.' }
```

| Argument                | Purpose                                                                                  |
| ----------------------- | ---------------------------------------------------------------------------------------- |
| -s app                  | Uses the vendor app directory as firmware source.                                        |
| -b keychron             | Selects the Keychron board.                                                              |
| -DSHIELD=keychron_b1_us | Selects the B1 Pro US shield, overlay, config, and staged keymap.                        |
| -p always               | Makes the build directory pristine, preventing stale generated files from hiding errors. |
| -j1                     | Deliberate serialized Ninja; parallel builds have been unreliable in this Windows fork.  |

## 7. Verify the UF2

A successful build creates:

```
build\intern-f1-f13\zephyr\zmk.uf2
```

Verify it:

```powershell
$Uf2 = Join-Path $BuildDir 'zephyr\zmk.uf2'
Get-Item $Uf2 | Select-Object FullName, Length, LastWriteTime
Get-FileHash -Algorithm SHA256 $Uf2
ninja -C $BuildDir -j1
```

The final Ninja command should report no work to do. Record the candidate hash, UF2 hash, SDK version, and build result in the lab worksheet.

A compile proves syntax and link correctness only. Do not flash this UF2 without explicit instructor authorization. A real test-unit flash requires a revision entry in CHANGELOG.md, a release record, and a cumulative QC checklist. Promotion into `B1PRO/releases/` requires an explicit full hardware PASS.

## 8. Cleanup and hand-in

Keep the candidate and its hash for instructor review. When the generated output is no longer needed:

```powershell
Remove-Item -LiteralPath $BuildDir -Recurse -Force
git restore -- app\boards\shields\keychron\b1\us\keychron_b1_us.keymap
git status --short
```

Before removal, confirm that $BuildDir resolves to the lab-specific build\intern-f1-f13 directory. Never recursively delete the repository root, .isolated-builds, B1PRO, or an unresolved variable.

## Completion questions

1. Which file defines the physical order for B1 Pro positions 0–81?
2. Why is position 77 protected even though it is not a typing key?
3. Which exact source, board, and shield arguments select this B1 Pro build?
4. Why is Ninja run with -j1?
5. What evidence is still needed before a built UF2 can enter B1PRO/releases/?

### Instructor answer key

1. app/boards/shields/keychron/b1/us/keychron_b1_us.overlay.
2. It is a physical Win-switch direct control; changing it can strand layer access.
3. -s app, -b keychron, and -DSHIELD=keychron_b1_us.
4. This Windows fork has a verified parallel-build reliability problem; serialized Ninja is the validated fallback.
5. Approved artifact identity, release record, explicit flash authorization, and a complete cumulative hardware PASS.
