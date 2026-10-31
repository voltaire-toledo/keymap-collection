# Keymap Collection

Custom keyboard firmware, keymaps, design documents, and hardware QC records.

- **Keychron V1 Max (QMK, ANSI encoder):** `keymaps/qmk/v1_max/`
- **Keychron B1 Pro (ZMK, ANSI/US):** `keymaps/zmk/b1pro/`

## Developer quick start

```bash
git clone https://github.com/voltaire-toledo/keymap-collection.git
cd keymap-collection
bash scripts/build-firmware.sh qmk
bash scripts/build-firmware.sh zmk
```

Install Git, Bash, and Docker first. Follow the complete
[container provisioning and firmware build instructions](CONTAINER-FIRMWARE-BUILDS.md)
for requirements, pinned source revisions, generated files, proxy setup, and
troubleshooting. The firmware sources are directories in this repository,
not Git submodules. Generated firmware goes to `build_output/` and is not
committed automatically.

For prebuilt firmware, use GitHub Releases. A successful build is not hardware
QC; follow [the firmware candidate workflow](FIRMWARE-WORKFLOW.md) before
flashing or promoting a release.
