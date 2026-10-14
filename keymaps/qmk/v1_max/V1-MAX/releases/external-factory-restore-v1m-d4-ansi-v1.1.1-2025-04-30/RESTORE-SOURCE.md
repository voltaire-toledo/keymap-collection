# External factory-restore cache — incomplete

This directory is an external recovery cache for the Keychron V1 Max ANSI
knob variant, requested for V1M-D4. It is **not** a custom candidate or a
project release, and it carries no build, flash, or hardware-QC claim.

## Official factory artifacts located

Keychron's V Max firmware page identifies its V1 Max Knob ANSI firmware as
version `1.1.1`, updated 2025-04-30. The linked binary is:

`https://cdn.shopify.com/s/files/1/0059/0630/1017/files/v1_max_ansi_encoder_v1.1.1_2504301037.bin?v=1746612974`

Keychron's page also links its ANSI VIA keymap JSON ZIP:

`https://cdn.shopify.com/s/files/1/0059/0630/1017/files/v1_max_ansi_knob_v1.0.json.zip?v=1698741749`

The current execution environment could not connect to Keychron's CDN on
2026-08-03, so neither remote file is present in this directory. Download the
two files on a networked machine, hash them, and record those hashes here
before treating either as a usable restore artifact. Do not substitute a
locally compiled binary for Keychron's factory binary.

## Locally archived QMK reference snapshot

The files in `qmk-source/` and the sibling JSON are an immutable copy from
this checkout, not the remote factory downloads. They are retained to inspect
the V1 Max ANSI QMK/VIA configuration while the official downloads are
unavailable.

- checkout commit: `4efcc8c9bf87687c9004079743c80d9a023d531c`
- locally known `origin/2025q3`: `07bfc38a4b11b8dac7ab758dfc5868b4229499ca`
- `qmk-source/keyboard.json` SHA-256:
  `245715d8ff14645b9694b84fc38a178114f6c7cb1dbf3b1fec68dea2593920ea`
- `qmk-source/keymap.c` SHA-256:
  `7939d7160ba080719f81ea61f58866e9e442231eb61458df3144bc15a016feae`
- `qmk-source/rules.mk` SHA-256:
  `1fa1a7e298f4d1f53c25059e106b19aece952b3a7a103b4b8384e05feddc9524`
- `v1_max_ansi_encoder-via-keymap-local-2025q3.json` SHA-256:
  `47ffa98e0a969d80207a64ac6436a757034419f08e88673391b21b1e2919ab60`

## Restore safety

Confirm that the physical test unit is the ANSI knob model before using the
official `.bin`. Keychron's factory-reset guidance says to use Cable mode,
enter DFU with the reset control beneath Space, flash the matching firmware,
and then perform the documented factory reset. No flash is authorized or
performed by this archive operation.
