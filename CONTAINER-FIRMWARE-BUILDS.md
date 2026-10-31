# Build QMK and ZMK firmware in containers

These procedures build the existing Keychron V1 Max ANSI encoder QMK overlay
and B1 Pro ANSI/US ZMK v0.25.8 candidate. They generate build artifacts only;
compilation is not hardware QC or approval to flash. Keep the candidate/release
rules in [FIRMWARE-WORKFLOW.md](FIRMWARE-WORKFLOW.md).

## 1. Provision the host

Use Linux x86-64 with Bash, Git, and a running Docker Engine, or Docker Desktop
with Linux containers on macOS/Windows. On Windows run these commands in WSL2
with Docker Desktop integration enabled. No host ARM compiler, Python, QMK CLI,
or west installation is needed. Start with 2 CPU cores, 8 GiB RAM, and 25 GiB
free disk; dependency downloads need HTTPS access to GitHub and Docker Hub.
Docker must be usable by your current account; do not make its socket public.

If the host is not provisioned yet:

1. On Linux, install Git and Bash with your distribution package manager, then
   follow [Docker Engine installation](https://docs.docker.com/engine/install/)
   for that distribution and its
   [Linux post-installation steps](https://docs.docker.com/engine/install/linux-postinstall/).
   Docker group membership grants administrator-equivalent access; use it only
   on a trusted build host. Log back in after changing group membership.
2. On Windows, install WSL2 and
   [Docker Desktop with WSL integration](https://docs.docker.com/desktop/features/wsl/).
   Open your WSL Linux terminal and install Git there. On macOS, install Git and
   [Docker Desktop](https://docs.docker.com/desktop/setup/install/mac-install/).
   These procedures were tested on Linux x86-64; other hosts are not verified.
3. Start Docker, then run `git --version`, `bash --version`, and `docker info`.
   Resolve any missing command or daemon/permission error before continuing.
   An ARM host may require x86-64 emulation and can be substantially slower;
   use an x86-64 runner to match the tested environment.

```bash
git clone https://github.com/voltaire-toledo/keymap-collection.git
cd keymap-collection
docker info
```

The sources live under `keymaps/`; this repository has no QMK/ZMK submodules.
The build scripts fetch pinned upstream commits into `.firmware-build/`.
Images supply compiler toolchains and are selected by immutable digest.
Builds default to two parallel jobs; set
`BUILD_JOBS=1` on a memory-constrained host.

## 2. Build QMK (Keychron V1 Max)

```bash
bash scripts/build-firmware.sh qmk
```

The script checks out Keychron QMK commit
`2b493ff95f2665db719b1e95c91babb5036e72f3` (2025q3), initializes its pinned
all top-level QMK submodules at their recorded gitlinks (including ChibiOS,
LUFA, and printf), copies `keymaps/qmk/v1_max/qmk_overlay/`, and builds
`keychron/v1_max/ansi_encoder:default` in the QMK image.

Expected files:

- `build_output/qmk/keychron_v1_max_ansi_encoder_default.bin`
- `build_output/qmk/keychron_v1_max_ansi_encoder_default.bin.sha256`

## 3. Build ZMK (Keychron B1 Pro)

```bash
bash scripts/build-firmware.sh zmk
```

The script checks out the vendor fork commit
`9df42d4e2c6362cf04df41218155e7aef4f02d18` (b1-pro-v.0.25.X), initializes
west, replaces its moving Zephyr branch with the locked manifest in
`scripts/zmk-west.yml`, downloads manifest dependencies, exports Zephyr, and
applies the two
Zephyr patches plus the portable DFU header from `keymaps/zmk/b1pro/patches/`.
A failed patch stops the build; only a verified already-applied patch is skipped.
It injects the existing `v0.25.8-candidate.keymap` and runs west with board
`keychron`, shield `keychron_b1_us`, `SOURCE_DATE_EPOCH=1791108125`, and
`BUILD_VERSION=c080d0853f98`.

Expected files:

- `build_output/zmk/zmk_b1pro.uf2`
- `build_output/zmk/zmk_b1pro.uf2.sha256`

The old reference hash is not a universal build acceptance criterion. This
procedure records the actual generated hash; it does not claim bit-for-bit
reproducibility across toolchains or validate keyboard behavior.

## 4. Verify and retain the artifacts

```bash
(cd build_output/qmk && sha256sum -c keychron_v1_max_ansi_encoder_default.bin.sha256)
(cd build_output/zmk && sha256sum -c zmk_b1pro.uf2.sha256)
```

On macOS, use `shasum -a 256 -c` in place of `sha256sum -c` for these
host-side checks; the container already performs its own checksum check.

Save these outputs and build logs outside an ephemeral runner. They are ignored
by Git. Do not overwrite archived firmware or promote these outputs to releases
without the repository's review and physical hardware checks.

## Proxies, failures, and reruns

Docker must have working registry and container HTTPS access. For a managed
runner whose Docker client already supplies the proxy, preserve that
configuration. If HTTPS is intercepted, mount a combined CA bundle:

```bash
export FIRMWARE_CA_BUNDLE=/etc/ssl/certs/ca-certificates.crt
bash scripts/build-firmware.sh qmk
bash scripts/build-firmware.sh zmk
```

The wrapper supplies that read-only bundle to Git, Python/Requests, and pip.
Do not disable TLS verification. Proxy reachability and registry access must
be configured by the host; the scripts do not install a VPN or change routes.

If `docker info` fails, start Docker or fix account/socket permissions first.
If a QMK compile reports a missing `lib/lufa/.../HIDClassCommon.h`, the
required QMK submodules were not initialized: use the complete script here,
which fetches LUFA even though this target is ARM-based.
If Git reports "dubious ownership" when a container reuses a bind-mounted
workspace, the maintained script adds exact repository paths under that
disposable workspace to its container-local safe-directory list. No host-wide
wildcard trust setting is needed. Use the updated script on reruns.
For download or compilation errors retain the log, correct the cause, and
rerun the same command. The dedicated workspace caches downloads; ZMK rebuilds
pristinely. Do not run the same target concurrently in the same checkout or
edit build scripts while a build is running.
Inspect only the expected output after a successful exit: a previous output
can remain after a failed run. For a fully clean test, remove that target's
`.firmware-build/<target>` and `build_output/<target>` directories (container
files may require administrator permissions), then rerun. Do not remove other
containers or workspaces. Both build directories are disposable, never source.

## Create your own keymap

First complete the supplied builds above. They establish that the toolchains
work before you introduce layout changes. Keep the hardware target unchanged:
these commands are for the V1 Max ANSI encoder and B1 Pro ANSI/US only.

Run each template-copy step once; back up edited copies before repeating it.

For QMK, make a personal copy of the complete overlay, then edit its
`keyboards/keychron/v1_max/ansi_encoder/keymaps/default/keymap.c`:

```bash
mkdir -p local-keymaps/qmk
cp -R keymaps/qmk/v1_max/qmk_overlay/. local-keymaps/qmk/
# Edit local-keymaps/qmk/keyboards/keychron/v1_max/ansi_encoder/keymaps/default/keymap.c
QMK_OVERLAY=local-keymaps/qmk bash scripts/build-firmware.sh qmk
```

For ZMK, copy the supplied candidate to a personal file and edit that copy:

```bash
mkdir -p local-keymaps
cp keymaps/zmk/b1pro/B1PRO/QC-IN_PROGRESS/v0.25.8-candidate.keymap local-keymaps/my-b1pro.keymap
# Edit local-keymaps/my-b1pro.keymap
ZMK_KEYMAP=local-keymaps/my-b1pro.keymap bash scripts/build-firmware.sh zmk
```

Both selectors are repository-relative paths. `local-keymaps/` is ignored so
personal layouts need not be committed to this collection. Keep backups or use
a separate private Git repository for your layouts. If you use another Git
repository, export/copy its source files into `local-keymaps/`; the QMK overlay
must not contain a `.git` directory or other repository metadata. To build the supplied
layout again, use a fresh target workspace as described above; cached QMK
overlays can otherwise retain extra personal files. Outputs use the same
paths as the supplied builds: save each result and its source before rebuilding.
Do not edit reviewed candidates or release archives to try a personal layout.
Preserve bootloader/recovery and wireless controls, and test on hardware before
relying on a new layout. See the firmware workflow for changes proposed here.

We recommend continuing to version the small source keymaps and dependency
locks, while distributing generated binaries through release downloads rather
than adding more binaries to Git. Source history provides rollback and review;
users can keep personal keymaps outside the shared version history. Existing
release archives are retained by this change.

## Source layout and deliberate dependency updates

Older documentation described root `QMK/` and `ZMK/` submodules and suggested
`git submodule update --remote`. Those instructions are obsolete: there is no
root `.gitmodules` or firmware gitlink in this checkout. Firmware inputs are
tracked in `keymaps/`; upstream compiler workspaces are disposable downloads.
Do not recreate the old submodules or follow branch heads during normal builds.
The QMK upstream's own submodules remain necessary and are checked out
at the gitlinks recorded by the pinned QMK commit, without `--remote`.

The build lock consists of:

| Input | Lock location | Selected revision |
| --- | --- | --- |
| Keychron QMK source | `scripts/container-build.sh` | `2b493ff95f2665db719b1e95c91babb5036e72f3` |
| Vendor ZMK source | `scripts/container-build.sh` | `9df42d4e2c6362cf04df41218155e7aef4f02d18` |
| Zephyr | `scripts/zmk-west.yml` | `1ae0eb5ce8adafcec993e6fb8f4eeb6f818a7772` |
| Zephyr modules | Imported manifest at the pinned Zephyr commit | Exact commit SHAs |
| QMK toolchain image | `scripts/build-firmware.sh` | `qmkfm/qmk_cli@sha256:b7d7fa8fb4432b569931de5ad59098cb788f440ed61a62c5126746b71aee0f4a` |
| ZMK toolchain image | `scripts/build-firmware.sh` | `zmkfirmware/zmk-build-arm@sha256:1fa70cf58594942f90a96059a3b135ac1843af3b4fb6a46c7bb39f29f9de661b` |

Branch names such as `2025q3` and `b1-pro-v.0.25.X`, and the original image tags
`latest` and `3.2`, explain where these inputs came from. They are not build
selectors. In particular, vendor ZMK's `v3.2.0+zmk-fixes` is a **branch**, not
an immutable release: the local manifest replaces it with the SHA above.

To upgrade dependencies:

1. Create a branch. Resolve the desired upstream revision to a full commit SHA
   and the desired image to a registry digest. Review upstream changes and
   architecture compatibility; do not substitute branch names or tags in locks.
2. Update the relevant script pin and this table together. For ZMK, compare the
   new vendor `app/west.yml` with `scripts/zmk-west.yml`, preserve needed project
   exclusions, pin Zephyr, and inspect every imported manifest for moving refs.
3. Use new/empty build workspaces for both targets. Follow this guide verbatim
   with an independent tester or agent. Correct the procedure when it fails,
   and repeat until both a fresh `.bin` and a fresh `.uf2` are generated.
4. Record the exact inputs, artifact sizes and SHA-256 values in the PR. Retain
   the generated firmware plus checksum sidecars as build evidence.
   A changed hash after changing inputs is evidence to review, not grounds to
   copy an old artifact or suppress a build error.
5. Review and merge the pins deliberately. Treat firmware behavior changes as
   separate reviewed candidates under `FIRMWARE-WORKFLOW.md`; compiling alone
   does not authorize flashing or release promotion.

A lock protects against upstream branch movement, not unavailable repositories
or images. For long-term/offline recovery, retain the selected images and Git
objects in your own trusted registry/mirror as well as the generated artifacts.

## PowerShell and CI

The existing PowerShell entry point now delegates to the same procedure:

```powershell
./scripts/build-firmware.ps1 -Target v1_max
./scripts/build-firmware.ps1 -Target b1_pro
```

The PowerShell wrapper is a convenience entry point; validation here covers the
Bash procedure on Linux, not a Windows or PowerShell execution.
On Windows it invokes WSL2; install Git/Bash there and enable Docker Desktop
integration for that distribution. For managed proxy CA configuration, prefer
running the Bash commands directly inside WSL so the Linux bundle path and
proxy environment are unambiguous.

Firmware CI and the scheduled link checker are disabled: their historical files
are in `.github/disabled-workflows/`, outside GitHub Actions discovery. Only the
GitHub Pages publishing workflows remain active. The archived firmware builds
still contain moving references and are not this validated procedure. Run the
Bash commands manually; upload the expected output plus its checksum only after
a successful exit. No interactive cloud session lifetime is assumed. Retain
artifacts externally before an ephemeral environment ends.

## Validation evidence

An independent reader followed this guide on Linux x86-64 with Docker and two
build jobs, reviewed each failure/correction, and verified generated artifacts
and checksum sidecars. Container commands used the primary session's Docker
access because the reader's permission requests stalled.

Observed supplied-layout outputs:

| Firmware | Bytes | SHA-256 |
| --- | ---: | --- |
| V1 Max QMK `.bin` | 112560 | `61a89bbc5a09511ec18c71b6e221e51f717077569b1b013a95bebd5d81fef818` |
| B1 Pro ZMK `.uf2` | 435712 | `d917b9b95a845dd03baffb582882254415ea239951d81b1246b93a9797612a76` |

The personal-copy commands also passed: QMK was built from an empty workspace,
and ZMK reused its dependency workspace with a pristine compilation. Copied
build-source hashes matched the selected personal files. The personal files
had identifying comments only; no shared keymap or hardware behavior was
changed. The personal QMK output hash differed from the supplied-layout output,
so these values are recorded evidence, not universal expected checksums or a
claim of bit-for-bit reproducibility. Both build commands exited successfully.
No firmware was flashed or promoted to a release. Windows/macOS and the
PowerShell convenience entry point were not executed during this validation.
