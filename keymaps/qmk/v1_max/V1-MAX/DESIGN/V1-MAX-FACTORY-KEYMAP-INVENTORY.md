# V1 Max Factory Keymap Inventory

Source: `keyboards/keychron/v1_max/ansi_encoder/keymaps/keychron/keymap.c`

## Layer structure

| Index | Name      | Activated by                        |
|-------|-----------|-------------------------------------|
| 0     | MAC_BASE  | DIP active (Mac side)               |
| 1     | MAC_FN    | `FN_MAC` = `MO(1)` held             |
| 2     | WIN_BASE  | DIP inactive (Win side) — see note  |
| 3     | WIN_FN    | `FN_WIN` = `MO(3)` held             |

> **DIP note**: `v1_max.c:dip_switch_update_kb` sets `default_layer_set(1UL << (active ? 0 : 6))`.
> Active (Mac) → layer 0. Inactive (Win) → layer 6 (not layer 2).
> The factory keychron keymap works because `keychron_common.h` likely contains a
> `dip_switch_update_user` that remaps 6 → WIN_BASE(2). Any custom keymap with
> WIN_BASE at an index other than 6 **must** supply its own `dip_switch_update_user`.

## FN key macros

Defined at the top of the factory keymap (provided by `keychron_common.h`):
```c
#define FN_MAC MO(MAC_FN)   // MO(1)
#define FN_WIN MO(WIN_FN)   // MO(3)
```

## Layer 0 — MAC_BASE

| Row | Keys |
|-----|------|
| Fn  | KC_ESC, KC_BRID, KC_BRIU, KC_MCTL, KC_LAUNCHPAD, KC_SEARCH, KC_CRTA, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_DEL \| KC_MUTE (enc) |
| Num | KC_GRV, KC_1–KC_0, KC_MINS, KC_EQL, KC_BSPC \| KC_PGUP |
| Tab | KC_TAB, KC_Q–KC_P, KC_LBRC, KC_RBRC, KC_BSLS \| KC_PGDN |
| Cap | KC_CAPS, KC_A–KC_SCLN, KC_QUOT, KC_ENT \| KC_HOME |
| Sft | KC_LSFT, KC_Z–KC_SLSH, KC_RSFT \| KC_UP |
| Bot | KC_LCTL, KC_LOPTN, KC_LCMMD, KC_SPC, KC_RCMMD, **FN_MAC**, KC_LEFT, KC_DOWN, KC_RGHT |

## Layer 1 — MAC_FN

| Row | Notable keys |
|-----|-------------|
| Fn  | ___, KC_F1–KC_F12, ___, UG_TOGG (enc) |
| Num | ___, BT_HST1, BT_HST2, BT_HST3, P2P4G, (rest ___) |
| Tab | UG_TOGG, UG_NEXT, UG_VALU, UG_HUEU, UG_SATU, UG_SPDU, ___, ___, ___, ___, ___, UG_PREV, UG_VALD, ___ \| UG_SPDD |
| Cap | (all ___) |
| Sft | ___, ___, ___, ___, ___, BAT_LVL, (rest ___) |
| Bot | (all ___, encoder): CCW=UG_VALD, CW=UG_VALU |

## Layers 2/3 — WIN_BASE / WIN_FN

Mirrors of Mac layers with Windows-specific bottom row:
- Bottom: KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_RALT, **FN_WIN**, KC_LEFT, KC_DOWN, KC_RGHT

## Encoder maps

| Layer     | CCW      | CW       |
|-----------|----------|----------|
| MAC_BASE  | KC_VOLD  | KC_VOLU  |
| MAC_FN    | UG_VALD  | UG_VALU  |
| WIN_BASE  | KC_VOLD  | KC_VOLU  |
| WIN_FN    | UG_VALD  | UG_VALU  |

## Keycodes from `keychron_common.h`

| Keycode    | Function                              |
|------------|---------------------------------------|
| BT_HST1    | Switch to Bluetooth host 1            |
| BT_HST2    | Switch to Bluetooth host 2            |
| BT_HST3    | Switch to Bluetooth host 3            |
| P2P4G      | Switch to 2.4 GHz dongle             |
| BAT_LVL    | Flash battery level on number row     |
| UG_TOGG    | RGB toggle on/off                     |
| UG_NEXT    | Next RGB animation mode               |
| UG_PREV    | Previous RGB animation mode           |
| UG_VALU    | RGB brightness up                     |
| UG_VALD    | RGB brightness down                   |
| UG_HUEU    | RGB hue up                            |
| UG_HUED    | RGB hue down                          |
| UG_SATU    | RGB saturation up                     |
| UG_SATD    | RGB saturation down                   |
| UG_SPDU    | RGB animation speed up                |
| UG_SPDD    | RGB animation speed down              |
| KC_MCTL    | macOS Mission Control (F3)            |
| KC_LAUNCHPAD | macOS Launchpad (F4)               |
| KC_SEARCH  | macOS Spotlight (F5)                  |
| KC_CRTA    | macOS Lock Screen / Cortana (F6)      |
| KC_LOPTN   | Left Option (macOS Alt)               |
| KC_LCMMD   | Left Command (macOS GUI)              |
| KC_RCMMD   | Right Command (macOS GUI)             |
| KC_TASK    | Windows Task View                     |
| KC_FILE    | Windows File Explorer                 |

## Board-level behavior (v1_max.c)

- **DIP switch (index 0)**: `active=true` → `default_layer_set(layer 0)`, `active=false` → `default_layer_set(layer 6)`. Custom keymaps must override with `dip_switch_update_user` if WIN_BASE is not at layer 6.
- **Power-on LED**: Battery LED (BAT_LOW_LED_PIN, B12) lit for 3 s on connect, then extinguished.
- **LPM idle**: Enters low-power when power-on timer expired and RGB is not active.
- **Init**: Calls `keychron_common_init()` then `keyboard_post_init_user()`.

## Board-level RGB config (v1_max/config.h)

| Setting | Value |
|---------|-------|
| RGB driver | SNLED27351 over SPI (SPID1, SCK=A5, MISO=A6, MOSI=A7) |
| CS pins | B8, B9 |
| Scan phase | 9-channel |
| BT indicator LEDs | Matrix indices 15, 16, 17 (BT HST 1/2/3) |
| 2.4G indicator LED | Matrix index 18 |
| Battery level LEDs | Matrix indices 15–24 |
