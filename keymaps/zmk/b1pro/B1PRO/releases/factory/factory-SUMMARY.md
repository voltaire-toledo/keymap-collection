# Factory v1.0.3 Keymap Combination & Feature Summary

- **Keymap Source:** `zmk_b1pro_us_v1.0.3.keymap`
- **SHA-256:** `c7eb602f0744991ff18e095f7a2f9703a0f680f2431e155241703d300829f465`
- **Generated:** 2026-09-17 13:09:39

## 1. Formal ZMK Combos (`combos` block)

| Combo Identifier | Trigger Keys (Positions) | Active Layers | Timeout | Binding / Action | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `combo_jz` | 49: J + 56: Z | `1 3` | 1000ms | `&long_press_recover 0 0` | Factory Reset: Erases BLE bonds and resets MCU |
| `combo_xl` | 57: X + 51: L | `1 3` | 1000ms | `&long_press_change 0 0` | Function/Media Row Exchange |
| `combo_b` | 60: B | `1 3` | 1000ms | `&out OUT_BAT` | Battery Level Query: Pulses battery LED |
| `combo_win_l` | 68: LAlt/Win | `1 3` | 1000ms | `&long_press_fn_win 0 0` | Windows Key Lock: Suppresses GUI keys globally |
| `combo_t0` | 80: Direct_Chg + 90: pos 90 | `4` | default | `&to 3` | Factory Production Test Chord |
| `combo_t1` | 80: Direct_Chg + 91: pos 91 | `4` | default | `&tog 3` | Factory Production Test Chord |
| `combo_t2` | 80: Direct_Chg + 92: pos 92 | `4` | default | `&sl 3` | Factory Production Test Chord |
| `combo_t3` | 70: Space + 93: pos 93 | `4` | default | `&lt 3 SPACE` | Factory Production Test Chord |
| `combo_t4` | 81: Direct_Chgd + 94: pos 94 | `4` | default | `&sk LSHIFT` | Factory Production Test Chord |
| `combo_t5` | 81: Direct_Chgd + 93: pos 93 | `4` | default | `&mt LSHIFT SPACE` | Factory Production Test Chord |
| `combo_t6` | 81: Direct_Chgd + 93: pos 93 | `4` | default | `&gresc` | Factory Production Test Chord |

## 2. Tap Dance Combinations (`zmk,behavior-tap-dance`)

*No tap-dance behaviors declared.*

## 3. Dual-Role Hold-Tap Behaviors (`zmk,behavior-hold-tap`)

| Behavior Name | Flavor | Tapping Term | Quick Tap / Idle | Retro-Tap | Bindings (Hold / Tap) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `long_press_recover` | `tap-preferred` | 3000ms | - | No | Hold: `&recover`<br>Tap: `&none` |
| `long_press_change` | `tap-preferred` | 3000ms | - | No | Hold: `&change`<br>Tap: `&none` |
| `long_press_bootloader` | `tap-preferred` | 3000ms | - | No | Hold: `&en_bootloader`<br>Tap: `&none` |
| `long_press_fn_win` | `tap-preferred` | 3000ms | - | No | Hold: `&fn_win`<br>Tap: `&none` |
| `bt_pair_0` | `tap-preferred` | 3000ms | - | No | Hold: `&bt_pair0`<br>Tap: `&bt_recon_0` |
| `bt_pair_1` | `tap-preferred` | 3000ms | - | No | Hold: `&bt_pair1`<br>Tap: `&bt_recon_1` |
| `bt_pair_2` | `tap-preferred` | 3000ms | - | No | Hold: `&bt_pair2`<br>Tap: `&bt_recon_2` |
| `bt_pair_3` | `tap-preferred` | 3000ms | - | No | Hold: `&bt_pair3`<br>Tap: `&bt_recon_3` |

## 4. Custom Macros (`ZMK_MACRO`)

| Macro Name | Sequence Output |
| :--- | :--- |
| `recover` | `&out OUT_RECOVER` |
| `change` | `&out OUT_FN` |
| `fn_win` | `&out OUT_FN_WIN` |
| `en_bootloader` | `&bootloader` |
| `MA` | `&kp 0x770000` |
| `bt_pair0` | `&bt BT_PAIR 0` |
| `bt_pair1` | `&bt BT_PAIR 1` |
| `bt_pair2` | `&bt BT_PAIR 2` |
| `bt_pair3` | `&bt BT_PAIR 3` |
| `bt_recon_0` | `&bt BT_SEL 0` |
| `bt_recon_1` | `&bt BT_SEL 1` |
| `bt_recon_2` | `&bt BT_SEL 2` |
| `bt_recon_3` | `&bt BT_SEL 3` |

## 5. Layer Structure & Special Activations

- **Total Defined Layers:** 4
| Index | Layer Identifier | Special Dual-Role & Switching Keys |
| :--- | :--- | :--- |
| 0 | `default_layer` | Standard Direct Mappings |
| 1 | `layer_one` | Standard Direct Mappings |
| 2 | `layer_two` | Standard Direct Mappings |
| 3 | `layer_three` | Standard Direct Mappings |


