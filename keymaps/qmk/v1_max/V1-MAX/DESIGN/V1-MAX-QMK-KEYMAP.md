# Keychron V1 Max QMK Keymap Specification

## Status

Discovery only. No custom layout or behavior is approved until the on-hand
ANSI encoder unit is confirmed and a stock-derived baseline passes hardware
testing.

## Firmware authority

| Concern | Authority |
| --- | --- |
| Selected layout and matrix | `keyboards/keychron/v1_max/ansi_encoder/keyboard.json` |
| Stock map to copy | `keyboards/keychron/v1_max/ansi_encoder/keymaps/keychron/keymap.c` |
| Board-level DIP behavior | `keyboards/keychron/v1_max/v1_max.c` |
| Board and wireless configuration | `keyboards/keychron/v1_max/{config.h,rules.mk,info.json}` |
| Custom implementation | `keyboards/keychron/v1_max/ansi_encoder/keymaps/default/` |

## Required retained behavior

- Mac and Windows base layers must retain the stock 0/2 relationship unless
  the board DIP callback changes deliberately and is fully retested.
- Fn access, wireless profile/2.4 GHz controls, battery indication, encoder,
  RGB, and STM32-DFU recovery are protected regression areas.
- The custom map must explicitly choose either the vendor VIA-enabled model or
  a fixed compiled map before changes are implemented.

## Deferred behavior migration

The previous B1 design is an idea source only. Translate each desired behavior
onto the selected V1 layout and test it independently:

- Layers: `MO()` or `LT()`.
- Home-row modifiers: `MT()` with Chordal Hold if same-hand-roll protection is
  required.
- Multi-tap behavior: QMK Tap Dance and a candidate-local `rules.mk` flag.
- Custom Esc/Caps behavior: a clearly specified QMK implementation, not a
  copied ZMK behavior.

---

## v0.25 Design Specification

Draft: `V1-MAX/DESIGN/v0.25-keymap-draft.c`
Target: `keyboards/keychron/v1_max/ansi_encoder/keymaps/default/`

### Layer plan (4 layers)

| Index | Name      | Entry             | Notes |
|-------|-----------|-------------------|-------|
| 0     | MAC_BASE  | DIP Mac / boot    | Full HRM + ESC_CAPS |
| 1     | MAC_FUNC  | Hold LT on Esc-row key; hold Fn | Media, numpad, wireless, RGB |
| 2     | WIN_BASE  | DIP Win           | Same layout, Win modifier order |
| 3     | WIN_FUNC  | Hold Fn (Win)     | Win-specific media keys |

SYM, NAV, UAT, MCRO layers are **deferred to v0.26**. In v0.25:
- G and H are plain `KC_G` / `KC_H` (no LT to SYM).
- Backslash is plain `KC_BSLS` (no LT to NAV).
- Space is plain `KC_SPC` (no LT to NAV).
- Fn tap is `KC_APP` (application key) as a temporary placeholder; v0.26 replaces with oneshot MCRO.

### DIP switch override — required

`v1_max.c` sets default layer to **6** for Win mode. With only 4 layers, WIN_BASE
is at index 2. The custom keymap must supply:

```c
bool dip_switch_update_user(uint8_t index, bool active) {
    if (index == 0) {
        default_layer_set(1UL << (active ? MAC_BASE : WIN_BASE));
    }
    return true;
}
```

### `config.h` settings

```c
#define CHORDAL_HOLD
#define PERMISSIVE_HOLD_PER_KEY
#define TAPPING_TERM_PER_KEY
#define QUICK_TAP_TERM_PER_KEY
#define FLOW_TAP_TERM          150
#define ONESHOT_TIMEOUT        1000
#define DYNAMIC_KEYMAP_LAYER_COUNT 4
```

### `rules.mk` settings

```
VIA_ENABLE     = yes
TAP_DANCE_ENABLE = yes
MOUSEKEY_ENABLE  = yes
```

### Home row mods (identical to B1Pro v0.25)

| Key | Modifier | Timing overrides |
|-----|----------|-----------------|
| A   | LCTL     | quick_tap=175ms, flow_tap=FLOW_TAP_TERM, permissive_hold=true |
| S   | LGUI     | same |
| D   | LALT     | same |
| F   | LSFT     | same |
| J   | RSFT     | same |
| K   | RALT     | same |
| L   | RGUI     | same |
| ;   | RCTL     | same |

Chordal Hold (`CHORDAL_HOLD`) configured with a bilateral `chordal_hold_layout`
matching the physical left/right split — same-hand rolls stay taps, opposite-hand
triggers the mod.

### ESC_CAPS custom keycode

```
tap  < 600 ms  →  Esc
hold ≥ 600 ms  →  CapsLock (held while key held, released on key release)
```

Timer reset on key press; handled in `process_record_user`.

### Tap dance inventory

| ID         | Tap             | Hold / 2× tap                             |
|------------|-----------------|-------------------------------------------|
| TD_FN_MAC  | KC_APP          | MO(MAC_FUNC) while held                   |
| TD_FN_WIN  | KC_APP          | MO(WIN_FUNC) while held                   |
| TD_BOOT    | KC_NO           | Hold 3 s → `reset_keyboard()`             |
| TD_MINS    | KC_MINS         | 2× → KC_UNDS                              |
| TD_EQL     | KC_EQL          | 2× → KC_PLUS                              |
| TD_PAREN   | KC_LPRN         | 2× → KC_RPRN   3× → macro `()`+cursor    |
| TD_BRACE   | KC_LCBR         | 2× → KC_RCBR   3× → macro `{}`+cursor    |
| TD_BRACK   | KC_LBRC         | 2× → KC_RBRC   3× → macro `[]`+cursor    |
| TD_ANGLE   | KC_LABK         | 2× → KC_RABK   3× → macro `<>`+cursor    |

TD_MINS through TD_ANGLE appear in MAC_FUNC / WIN_FUNC number row.

### VIA compatibility

- `DYNAMIC_KEYMAP_LAYER_COUNT 4` matches the 4-layer layout.
- Works with UseVIA.app + custom JSON (`V1-MAX-VIA-KEYMAP-2026-09-03.json`).
- Keychron Launcher compatibility is **not** a goal for the custom keymap.
- Custom keycodes (ESC_CAPS and above) must have VIA IDs declared if you want
  them remappable in VIA; for v0.25 they are compiled-in and not remappable.

---

## v0.25 RGB Indicator Specification

These are features unique to the V1 Max (QMK per-key RGB) with no B1Pro equivalent.
Implemented in `rgb_matrix_indicators_advanced_user`.

### LED index lookup

Use `g_led_config.matrix_co[row][col]` to resolve matrix position → LED index at
runtime. No hard-coded indices in the callback; use the macro helper from QMK:

```c
#define LED_AT(r, c)  g_led_config.matrix_co[(r)][(c)]
```

Key matrix positions (LAYOUT_ansi_82, 0-indexed rows/cols):
| Key   | Row | Col | Note |
|-------|-----|-----|------|
| ESC   | 0   | 0   | Also holds ESC_CAPS behavior |
| 1–9   | 1   | 1–9 | Number row for layer indicators |
| LSFT  | 4   | 0   | Physical left shift |
| RSFT  | 4   | 11  | Physical right shift |

### Behavior 1 — HRM Shift indicator

When either `MOD_BIT(KC_LSFT)` or `MOD_BIT(KC_RSFT)` is in `get_mods()`, light
the physical LSFT and RSFT keys in **orange** (0xFF, 0x80, 0x00).

This fires when F or J (homerow shift) is held — exactly the visual feedback target.

### Behavior 2 — Active layer number key

When `get_highest_layer(layer_state) > 0` and layer ≤ 9, light the number key at
column `layer` in **cyan** (0x00, 0xFF, 0xFF).

- Layer 1 (MAC_FUNC) active → `1` key lights cyan while Fn is held.
- Layer 2 (WIN_BASE) active (default layer) → not signaled (it is the base, not momentary).
- Layer 3 (WIN_FUNC) active → `3` key lights cyan.

Momentary layers (MO/LT) extinguish the indicator when the key is released. This
gives instant confirmation of which layer is engaged.

### Behavior 3 — TO() exit indicator

When `layer_state_cmp(layer_state, MAC_BASE)` is false AND `default_layer_state`
is not MAC_BASE or WIN_BASE — i.e., a `TG()`/`TO()` call has moved the base layer
— light the **ESC key red** (0xFF, 0x00, 0x00) as a persistent "you are locked into
a layer, press Esc or the marked key to return" indicator.

In v0.25 with only 4 layers and no TG()/TO() other than TO(MAC_BASE)/TO(WIN_BASE),
this indicator is dormant. It becomes active in v0.26 when OSL/TG layers are added.

Track via a module-static flag `g_to_layer_active` set in `process_record_user`:
```c
case TO(MAC_MCRO):  // example future layer
    g_to_layer_active = record->event.pressed;
    return true;
```

### RGB callback structure

```c
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // 1. HRM shift indicator
    if (get_mods() & (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT))) {
        RGB_MATRIX_INDICATOR_SET_COLOR(LED_AT(4, 0),  0xFF, 0x80, 0x00);
        RGB_MATRIX_INDICATOR_SET_COLOR(LED_AT(4, 11), 0xFF, 0x80, 0x00);
    }
    // 2. Layer number key
    uint8_t layer = get_highest_layer(layer_state);
    if (layer >= 1 && layer <= 9) {
        RGB_MATRIX_INDICATOR_SET_COLOR(LED_AT(1, layer), 0x00, 0xFF, 0xFF);
    }
    // 3. TO() exit indicator
    if (g_to_layer_active) {
        RGB_MATRIX_INDICATOR_SET_COLOR(LED_AT(0, 0), 0xFF, 0x00, 0x00);
    }
    return false;
}
```
