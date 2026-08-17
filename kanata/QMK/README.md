---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Tap-Hold Timeouts Per Key](#tap-hold-timeouts-per-key)
3. [Permissive Hold](#permissive-hold)
4. [Retro Tapping](#retro-tapping)
5. [Build and Flash Firmware](#build-and-flash-firmware)
6. [Quick Reference](#quick-reference)
7. [Further Reading](#further-reading)

---

## Prerequisites

### 1. Clone Keychron's QMK fork

```bash
git clone https://github.com/Keychron/qmk_firmware.git --recurse-submodules
cd qmk_firmware
```

### 2. Set up QMK environment

Follow the [QMK setup guide](https://docs.qmk.fm/newbs_getting_started) for your OS.  
On Windows, use **QMK MSYS** (recommended) or **WSL2**.

```bash
qmk setup
```

### 3. Create (or copy) your keymap

```bash
qmk new-keymap -kb keychron/v1_max/ansi_encoder -km <your_keymap_name>
```

This creates your keymap at:
```
keyboards/keychron/v1_max/ansi_encoder/keymaps/<your_keymap_name>/
```

Your keymap folder will typically contain:
- `keymap.c` — key layout definitions
- `config.h` — compile-time settings
- `rules.mk` — feature flags

---

## Tap-Hold Timeouts Per Key

Mod-tap and layer-tap keys have a global tapping term (default **200 ms**). If you hold a key longer than the tapping term, QMK treats it as a hold (modifier); a shorter press is a tap (regular key).

### Global setting (`config.h`)

```c
// keyboards/keychron/v1_max/ansi_encoder/keymaps/<your_keymap>/config.h
#define TAPPING_TERM 200   // milliseconds; adjust to taste
```

### Per-key override (`keymap.c`)

Enable per-key control first:

```c
// config.h
#define TAPPING_TERM_PER_KEY
```

Then implement the callback in `keymap.c`:

```c
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Home-row mod on A — give it extra time to avoid accidental Ctrl
        case LCTL_T(KC_A):
            return 250;

        // Thumb cluster shift — snappier tap detection
        case RSFT_T(KC_ENT):
            return 150;

        default:
            return TAPPING_TERM;  // fall back to global value
    }
}
```

> **Tip:** Start with the global `TAPPING_TERM` that feels comfortable for *most* keys, then tune outliers with `get_tapping_term`.

---

## Permissive Hold

By default, QMK waits for the tapping term to decide whether a mod-tap key is a hold or a tap, regardless of what other keys are pressed in that window. **Permissive hold** changes this: if another key is *pressed **and released*** while the mod-tap is held, QMK immediately resolves the mod-tap as a hold.

This is useful for home-row mods when you type fast and want `SFT_T(KC_F)` + `KC_J` to produce `Shift+J` even if you release `KC_J` before the tapping term expires.

### Global setting

```c
// config.h
#define PERMISSIVE_HOLD
```

With this defined, *all* mod-tap and layer-tap keys use permissive hold behavior.

To **disable** it, simply remove or comment out the `#define`.

### Per-key override

When you want permissive hold only on specific keys, leave the global flag off and use the callback instead:

```c
// config.h
#define PERMISSIVE_HOLD_PER_KEY
```

```c
// keymap.c
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Use permissive hold for home-row mods
        case LSFT_T(KC_F):
        case RSFT_T(KC_J):
        case LCTL_T(KC_D):
        case RCTL_T(KC_K):
            return true;   // permissive hold ON for these keys

        default:
            return false;  // permissive hold OFF for everything else
    }
}
```

> **Note:** `PERMISSIVE_HOLD` and `PERMISSIVE_HOLD_PER_KEY` are mutually exclusive — define only one.

---

## Retro Tapping

When a mod-tap key is held past the tapping term but *no other key is pressed during that hold*, QMK normally registers **only the modifier** and sends nothing when you release. **Retro tapping** changes this: the tap key is sent on release even if the tapping term was exceeded, as long as no other key was pressed while it was held.

This prevents "lost" keystrokes when you press a mod-tap key slowly on its own.

### Global setting

```c
// config.h
#define RETRO_TAPPING
```

With this defined, all mod-tap keys will send their tap key on release if nothing else was pressed during the hold.

To **disable** it, remove or comment out the `#define`.

### Per-key override

```c
// config.h
#define RETRO_TAPPING_PER_KEY
```

```c
// keymap.c
bool get_retro_tapping(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Enable retro tapping for space/enter thumb keys only
        case LGUI_T(KC_SPC):
        case RGUI_T(KC_ENT):
            return true;

        default:
            return false;
    }
}
```

> **Tip:** Retro tapping is most helpful on keys you sometimes tap alone (like Space or Enter). Avoid enabling it globally on all home-row mods, as it can cause unexpected characters when you accidentally hold them slightly too long.

---

## Build and Flash Firmware

### Build

From the root of the `qmk_firmware` directory:

```bash
make keychron/v1_max/ansi_encoder:<your_keymap_name>
```

Replace `<your_keymap_name>` with the name of your keymap folder (e.g., `default`, `via`, or a custom name you created).

Example output file: `keychron_v1_max_ansi_encoder_<your_keymap_name>.bin`

Using the QMK CLI (alternative):

```bash
qmk compile -kb keychron/v1_max/ansi_encoder -km <your_keymap_name>
```

### Flash

Put the keyboard into **bootloader/DFU mode**:

1. Unplug the keyboard.
2. Hold the **Escape** key (top-left) while plugging in the USB cable.  
   *(The keyboard is now in DFU mode — no LEDs will light.)*

Then flash with the QMK CLI:

```bash
qmk flash -kb keychron/v1_max/ansi_encoder -km <your_keymap_name>
```

Or use `make` with the flash target:

```bash
make keychron/v1_max/ansi_encoder:<your_keymap_name>:flash
```

QMK will compile, detect the bootloader, and flash automatically.

> **Windows users:** Use **QMK Toolbox** as an alternative GUI flasher.  
> Download: <https://github.com/qmk/qmk_toolbox/releases>

---

## Quick Reference

| Feature | `config.h` define | Per-key callback | Behavior |
|---|---|---|---|
| Tap-hold timeout | `TAPPING_TERM 200` | `get_tapping_term()` with `TAPPING_TERM_PER_KEY` | Hold duration threshold for tap vs. hold |
| Permissive hold (global) | `PERMISSIVE_HOLD` | — | Any nested key press+release triggers hold |
| Permissive hold (per key) | `PERMISSIVE_HOLD_PER_KEY` | `get_permissive_hold()` | Same, but controlled per keycode |
| Retro tapping (global) | `RETRO_TAPPING` | — | Tap key fires on release if held alone past term |
| Retro tapping (per key) | `RETRO_TAPPING_PER_KEY` | `get_retro_tapping()` | Same, but controlled per keycode |
| Build firmware | — | — | `make keychron/v1_max/ansi_encoder:<keymap>` |
| Flash firmware | — | — | `make keychron/v1_max/ansi_encoder:<keymap>:flash` |

---

## Further Reading

- [QMK Tap-Hold Configuration](https://docs.qmk.fm/tap_hold)
- [QMK Mod-Tap](https://docs.qmk.fm/mod_tap)
- [Keychron QMK fork](https://github.com/Keychron/qmk_firmware)
- [QMK Getting Started](https://docs.qmk.fm/newbs_getting_started)
- [QMK Toolbox (GUI Flasher)](https://github.com/qmk/qmk_toolbox/releases)
- [Home Row Mods guide (precondition.ca)](https://precondition.github.io/home-row-mods)

---

# Keychron V1 Max / QMK Firmware
- Keychron QMK fork: https://github.com/Keychron/qmk_firmware
- QMK Getting Started: https://docs.qmk.fm/newbs_getting_started
- QMK Tap-Hold Configuration: https://docs.qmk.fm/tap_hold
- QMK Mod-Tap: https://docs.qmk.fm/mod_tap
- QMK Toolbox (GUI Flasher): https://github.com/qmk/qmk_toolbox/releases
- Home Row Mods guide: https://precondition.github.io/home-row-mods
- Keychron V1 Max QMK Customization Guide: https://github.com/voltaire-toledo/Mello.Ops.Local/blob/main/docs/keychron-v1-max-qmk.md

