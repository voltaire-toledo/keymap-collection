# Keychron V1 Max QMK Developer Lab

## Purpose

Create one disposable, compile-only V1 Max experiment. This lab does not
authorize a flash or release promotion.

## Preconditions

1. Confirm the physical unit is the ANSI encoder model.
2. Install the fork's QMK dependencies; this checkout currently has no `qmk`,
   `arm-none-eabi-gcc`, or `dfu-util` available on `PATH`.
3. Use `keyboards/keychron/v1_max/ansi_encoder/`.
4. Do not use B1 Pro ZMK, West, Zephyr, UF2, or `NRF52BOOT` steps. WSL and
   Docker are permitted for the QMK build and development environment.

## Safe build exercise

Build the selected unmodified vendor map first. Do not create or compile a
custom candidate in this lab.

```text
make keychron/v1_max/ansi_encoder:keychron
```

Before a custom build, complete the candidate record, deterministic source
comparison, and required senior review described in `AGENTS.md`.

## Hardware gate

Before any reviewed artifact is ready, verify one non-destructive DFU entry
route and confirm the Windows host can see the bootloader with `dfu-util -l`.
After a revision entry and explicit approval, use the documented Cable-mode
recovery path to flash only the selected QMK target. Then test the changed
behavior plus Mac/Win DIP switching, Fn, wireless controls, encoder, RGB, and
normal USB input after recovery.

Do not run this lab's build output on the test unit. The custom-candidate flow
starts only after its immutable identity and review gates pass.
