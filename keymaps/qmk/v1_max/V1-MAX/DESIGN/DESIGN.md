# Keychron V1 Max QMK Design

## Architecture

The V1 Max is a QMK keyboard with an STM32F401, STM32 DFU bootloader, wireless
support, RGB, encoder, raw HID, and a Mac/Win DIP switch. The shared board
files are `keyboards/keychron/v1_max/{info.json,config.h,rules.mk,v1_max.c}`.
The ANSI encoder target contributes the physical layout and stock keymaps.

## Custom-map boundary

Place customization in
`keyboards/keychron/v1_max/ansi_encoder/keymaps/default/`. Do not alter shared
board or wireless code for a normal remap. A custom `rules.mk` enables features
such as Tap Dance; a custom `config.h` holds feature configuration such as
Chordal Hold. Any shared-source change receives an independent review.

## Build and recovery

The normal target is `keychron/v1_max/ansi_encoder:default`. QMK produces a
target-specific firmware artifact; its exact path and hash must be recorded
before DFU flashing. Enter DFU from Cable mode using Esc while connecting USB,
or the reset control under Space.

## Design documents in this directory

| File | Purpose |
|------|---------|
| `DESIGN.md` | This file — architecture overview |
| `V1-MAX-FACTORY-KEYMAP-INVENTORY.md` | Full inventory of factory keychron keymap, keycodes, board behavior |
| `V1-MAX-QMK-KEYMAP.md` | Keymap spec: firmware authority, required behavior, v0.25 design, RGB indicator spec |
| `ideation.md` | Unapproved ideas for future versions |
| `v0.25-keymap-draft.c` | Draft v0.25 keymap (4 layers, HRM, ESC_CAPS, RGB indicators) |
| `v0.25-config.h` | Companion config.h for v0.25 draft |
| `v0.25-rules.mk` | Companion rules.mk for v0.25 draft |
| `V1-MAX-VIA-KEYMAP-*.json` | VIA JSON exports |
