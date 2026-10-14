# Keychron V1 Max QMK Workspace

This directory holds the project specification, test-ready candidate records,
and approved V1 Max release evidence. Firmware source belongs in the selected
QMK target directory, not here.

## Current state

No custom V1 Max candidate exists. The active physical target is the ANSI
encoder model:

| Unit | QMK target | Layout |
| --- | --- | --- |
| ANSI encoder | `keychron/v1_max/ansi_encoder` | `LAYOUT_ansi_82` |

The intended custom map is
`keyboards/keychron/v1_max/ansi_encoder/keymaps/default/`. Start it from the
ANSI vendor `keychron` map after the unit is confirmed.

`legacy-b1pro-zmk/` preserves incompatible B1 Pro material. It is neither a
build input nor a V1 Max QA baseline.
