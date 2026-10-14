# V1 Max QMK WSL Preparation

This is the working WSL preparation procedure for the Keychron V1 Max ANSI
project. Build from a Linux-native copy of this fork; use the Windows `D:`
mount only as the one-way source of that copy. QMK warns that building directly
under `/mnt/d` can be very slow.

This preparation procedure does **not** build firmware, create a `.bin`, enter
DFU mode, or flash the keyboard. The ANSI candidate must be reviewed, hashed,
and committed before a later build gate.

## Main instruction set

### 1. Confirm the WSL distribution and Windows source mount

In Windows PowerShell, confirm that the intended Ubuntu distribution is WSL 2:

```powershell
wsl --list --verbose
```

The validated environment is `Ubuntu-24.04`. In Ubuntu, confirm that the
Windows checkout is mounted:

```bash
test -d /mnt/d/CODE/VT-Keymaps/VT-QMK-V1Max/qmk_firmware-fork-keychron \
  && echo 'Windows source mount is available'
```

### 2. Install the verified Linux dependencies

In Ubuntu, install the packages used by the successful preparation:

```bash
sudo apt-get update
sudo env DEBIAN_FRONTEND=noninteractive apt-get install -y \
  gcc-arm-none-eabi \
  binutils-arm-none-eabi \
  libnewlib-arm-none-eabi \
  rsync \
  dfu-util
```

The ARM newlib support consumes substantial disk space (approximately 3.1 GB
in the validated setup). It is needed for QMK's ARM toolchain support.

### 3. Install or expose the QMK CLI

The validated installation uses QMK CLI 1.2.0 from `~/.local/bin/qmk`. Ensure
that directory is on `PATH`:

```bash
export PATH="$HOME/.local/bin:$PATH"
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
```

If `qmk --version` is not available, install the QMK launcher with the Python
tool manager available in this environment:

```bash
uv tool install qmk
```

Then confirm the CLI is available:

```bash
qmk --version
```

### 4. Create the native Linux copy

Make the WSL source parent and copy the Windows worktree into it. This copy
intentionally includes the reviewed but uncommitted candidate work.

```bash
mkdir -p ~/src
rsync -a --info=progress2 \
  --exclude='.build/' \
  --exclude='.git/index.lock' \
  /mnt/d/CODE/VT-Keymaps/VT-QMK-V1Max/qmk_firmware-fork-keychron/ \
  ~/src/qmk_firmware-fork-keychron/
```

Confirm it is actually on the Linux filesystem:

```bash
cd ~/src/qmk_firmware-fork-keychron
/bin/pwd
findmnt -T .
```

The path must be `/home/<your-user>/src/qmk_firmware-fork-keychron` and its
mount must be the Linux root filesystem, not `/mnt/d`.

### 5. Configure QMK for this existing fork and verify it

Use `qmk setup` with `-H` to configure the existing native fork. Do not run
bare `qmk setup`, because that normally selects or clones the upstream QMK
repository rather than this Keychron fork.

```bash
export QMK_HOME="$HOME/src/qmk_firmware-fork-keychron"
echo 'export QMK_HOME="$HOME/src/qmk_firmware-fork-keychron"' >> ~/.bashrc

qmk setup -H "$QMK_HOME" -y
qmk config user.qmk_home
qmk doctor
```

The validated result reported QMK home as the native path and passed checks for
ARM GCC, ARM binutils, `dfu-util`, dependencies, and submodules. `qmk doctor`
may report uncommitted project work; that is expected before the candidate is
committed. Its compiler probes are environment diagnostics, not a V1 Max
firmware build.

### 6. Verify the copied candidate identity before a later build gate

The current candidate record lists SHA-256 values for `keymap.c`, `config.h`,
and `rules.mk`. Check the native copies before any build work:

```bash
cd "$QMK_HOME"
sha256sum \
  keyboards/keychron/v1_max/ansi_encoder/keymaps/default/keymap.c \
  keyboards/keychron/v1_max/ansi_encoder/keymaps/default/config.h \
  keyboards/keychron/v1_max/ansi_encoder/keymaps/default/rules.mk
git status --short
```

For the reviewed v0.24 candidate, the verified hashes are recorded in
`V1-MAX/QC-IN_PROGRESS/v0.24-b1-intent-v1max-ansi.md`. Do not build until the
exact candidate has been committed under the release discipline in `AGENTS.md`.

### 7. Sync future Windows edits into WSL safely

Before a future build, sync Windows to WSL again:

```bash
rsync -a --info=progress2 \
  --exclude='.build/' \
  --exclude='.git/index.lock' \
  /mnt/d/CODE/VT-Keymaps/VT-QMK-V1Max/qmk_firmware-fork-keychron/ \
  ~/src/qmk_firmware-fork-keychron/
```

Do not add `--delete`, and do not automatically sync the WSL tree back to
Windows. Either action could remove or overwrite source, records, or artifacts.

## Attempts that did not work, with root cause and resolution

This section is a troubleshooting record only. These are not steps in the main
instruction set.

### `qmk doctor` before configuring QMK home

**Observed result:** `qmk doctor` was rejected with an "invalid choice:
'doctor'" message and only setup-oriented subcommands were shown.

**Root cause:** QMK CLI 1.2.0 was installed, but it had not yet been configured
with a QMK Firmware home. In that state its launcher exposes its bootstrap
commands, not the fork's firmware commands.

**Resolution:** Run `qmk setup -H "$QMK_HOME" -y` against the existing native
fork. The command wrote `~/.config/qmk/qmk.ini`, recognized the fork, and then
ran QMK Doctor successfully. The initial guess that this was an unrelated
third-party executable was incorrect; `qmk-cli` is the QMK command launcher.

### First `rsync` ended with a vanished `.git/index.lock`

**Observed result:** The first copy transferred the tree but exited before the
QMK configuration step with `file has vanished` for `.git/index.lock`.

**Root cause:** A Git lock file is transient and disappeared while `rsync` was
reading the Windows worktree, likely because another process had completed a
Git operation.

**Resolution:** Re-run the source-to-WSL copy while excluding
`.git/index.lock`. Never copy Git lock files. The re-sync completed and the
native candidate source hashes matched the reviewed record.

### A path check appeared to show `/mnt/d` after changing to the native copy

**Observed result:** One cross-shell test printed a Windows-mounted path,
creating the appearance that the native copy was not being used.

**Root cause:** PowerShell-to-Bash quoting and variable expansion affected the
diagnostic command, not the location of the copied tree.

**Resolution:** Verify inside WSL with `/bin/pwd` and `findmnt -T .`. Those
checks confirmed `/home/jigs/src/qmk_firmware-fork-keychron` on ext4.

### Historical WSL access-denied preflight

**Observed result:** The previous candidate record reported
`Wsl/EnumerateDistros/Service/E_ACCESS_DENIED`.

**Root cause:** That result came from an earlier execution context and did not
show that WSL was missing or unavailable to the interactive Windows account.

**Resolution:** In the available context, `Ubuntu-24.04` was enumerated and
used successfully. Treat a future access-denied result as a runtime-context
problem first; verify with `wsl --list --verbose` before changing the WSL
installation.
