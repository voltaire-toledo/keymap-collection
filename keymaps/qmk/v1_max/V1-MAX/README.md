# Keychron V1 Max QMK Workspace

This directory holds the project specification, test-ready candidate records,
and approved V1 Max release evidence. Firmware source belongs in the selected
QMK target directory, not here.

## Current state

A custom build overlay is tracked in [`../qmk_overlay/`](../qmk_overlay/).
Follow the repository's [container build guide](../../../../CONTAINER-FIRMWARE-BUILDS.md)
to compile it. A successful build does not establish a hardware-QC baseline.
The active physical target is the ANSI encoder model:

| Unit | QMK target | Layout |
| --- | --- | --- |
| ANSI encoder | `keychron/v1_max/ansi_encoder` | `LAYOUT_ansi_82` |

The custom map is under
`../qmk_overlay/keyboards/keychron/v1_max/ansi_encoder/keymaps/default/`.
The overlay also includes V1 Max runtime/rules overrides; keep those inputs
with the map when building or making a personal copy.

`legacy-b1pro-zmk/` preserves incompatible B1 Pro material. It is neither a
build input nor a V1 Max QA baseline.
