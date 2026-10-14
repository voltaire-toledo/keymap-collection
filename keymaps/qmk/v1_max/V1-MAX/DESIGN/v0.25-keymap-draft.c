/* V1 Max ANSI Encoder — Keymap v0.25 (DRAFT)
 *
 * Derived from B1Pro ZMK v0.25 design. 4-layer Mac/Win skeleton.
 * SYM / NAV / UAT / MCRO layers deferred to v0.26.
 *
 * Deploy to: keyboards/keychron/v1_max/ansi_encoder/keymaps/default/keymap.c
 * Build:     qmk compile -kb keychron/v1_max/ansi_encoder -km default
 */

#include QMK_KEYBOARD_H
#include "keychron_common.h"

/* ===== Layer indices ===== */
enum layers {
    MAC_BASE,   // 0
    MAC_FUNC,   // 1
    WIN_BASE,   // 2  — DIP inactive; must be at 2, not 6 → dip_switch_update_user below
    WIN_FUNC,   // 3
};

/* ===== Custom keycodes ===== */
enum custom_keycodes {
    ESC_CAPS = SAFE_RANGE,
};

/* ===== Tap dance IDs ===== */
enum tap_dance_id {
    TD_FN_MAC,
    TD_FN_WIN,
    TD_BOOT,
    TD_MINS,
    TD_EQL,
    TD_PAREN,
    TD_BRACE,
    TD_BRACK,
    TD_ANGLE,
};

/* ===== Home-row mod aliases ===== */
#define HRM_LCTL LCTL_T(KC_A)
#define HRM_LGUI LGUI_T(KC_S)
#define HRM_LALT LALT_T(KC_D)
#define HRM_LSFT LSFT_T(KC_F)
#define HRM_RSFT RSFT_T(KC_J)
#define HRM_RALT RALT_T(KC_K)
#define HRM_RGUI RGUI_T(KC_L)
#define HRM_RCTL RCTL_T(KC_SCLN)

/* ===== TO() exit flag (Behavior 3 RGB indicator) ===== */
static bool g_to_layer_active = false;

/* ─────────────────────────────────────────────
 *  Tap Dance — FN key (hold = MO layer, tap = KC_APP)
 * ───────────────────────────────────────────── */
static void td_fn_mac_finished(tap_dance_state_t *state, void *user_data) {
    if (state->pressed) {
        layer_on(MAC_FUNC);
    }
}
static void td_fn_mac_reset(tap_dance_state_t *state, void *user_data) {
    layer_off(MAC_FUNC);
    if (state->count == 1 && !state->pressed) {
        tap_code(KC_APP);  // v0.26: change to oneshot MCRO
    }
}

static void td_fn_win_finished(tap_dance_state_t *state, void *user_data) {
    if (state->pressed) {
        layer_on(WIN_FUNC);
    }
}
static void td_fn_win_reset(tap_dance_state_t *state, void *user_data) {
    layer_off(WIN_FUNC);
    if (state->count == 1 && !state->pressed) {
        tap_code(KC_APP);
    }
}

/* ─────────────────────────────────────────────
 *  Tap Dance — TD_BOOT (hold 3 s → reset)
 * ───────────────────────────────────────────── */
static void td_boot_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1 && state->pressed) {
        reset_keyboard();
    }
}

/* ─────────────────────────────────────────────
 *  Tap Dance — symbol pairs (MINS, EQL, brackets)
 * ───────────────────────────────────────────── */
static void td_symbols_finished(tap_dance_state_t *state, void *user_data) {
    switch (TAP_DANCE_KEYCODE(state)) {
        case TD(TD_MINS):
            if (state->count == 1) { tap_code(KC_MINS); }
            else                   { tap_code16(KC_UNDS); }
            break;
        case TD(TD_EQL):
            if (state->count == 1) { tap_code(KC_EQL); }
            else                   { tap_code16(KC_PLUS); }
            break;
        case TD(TD_PAREN):
            if      (state->count == 1) { tap_code16(KC_LPRN); }
            else if (state->count == 2) { tap_code16(KC_RPRN); }
            else { tap_code16(KC_LPRN); tap_code16(KC_RPRN); tap_code(KC_LEFT); }
            break;
        case TD(TD_BRACE):
            if      (state->count == 1) { tap_code16(KC_LCBR); }
            else if (state->count == 2) { tap_code16(KC_RCBR); }
            else { tap_code16(KC_LCBR); tap_code16(KC_RCBR); tap_code(KC_LEFT); }
            break;
        case TD(TD_BRACK):
            if      (state->count == 1) { tap_code(KC_LBRC); }
            else if (state->count == 2) { tap_code(KC_RBRC); }
            else { tap_code(KC_LBRC); tap_code(KC_RBRC); tap_code(KC_LEFT); }
            break;
        case TD(TD_ANGLE):
            if      (state->count == 1) { tap_code16(KC_LABK); }
            else if (state->count == 2) { tap_code16(KC_RABK); }
            else { tap_code16(KC_LABK); tap_code16(KC_RABK); tap_code(KC_LEFT); }
            break;
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_FN_MAC]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_fn_mac_finished, td_fn_mac_reset),
    [TD_FN_WIN]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_fn_win_finished, td_fn_win_reset),
    [TD_BOOT]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_boot_finished,   NULL),
    [TD_MINS]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
    [TD_EQL]     = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
    [TD_PAREN]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
    [TD_BRACE]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
    [TD_BRACK]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
    [TD_ANGLE]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
};

/* ─────────────────────────────────────────────
 *  Tapping-term overrides
 * ───────────────────────────────────────────── */
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD(TD_BOOT): return 3000;
        default:          return 200;
    }
}

uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case HRM_LCTL: case HRM_LGUI: case HRM_LALT: case HRM_LSFT:
        case HRM_RSFT: case HRM_RALT: case HRM_RGUI: case HRM_RCTL:
            return 175;
        default:
            return 0;
    }
}

uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case HRM_LCTL: case HRM_LGUI: case HRM_LALT: case HRM_LSFT:
        case HRM_RSFT: case HRM_RALT: case HRM_RGUI: case HRM_RCTL:
            return FLOW_TAP_TERM;
        default:
            return 0;
    }
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case HRM_LCTL: case HRM_LGUI: case HRM_LALT: case HRM_LSFT:
        case HRM_RSFT: case HRM_RALT: case HRM_RGUI: case HRM_RCTL:
            return true;
        default:
            return false;
    }
}

/* ─────────────────────────────────────────────
 *  Chordal Hold layout (bilateral HRM protection)
 *  'L' = left hand, 'R' = right hand, '*' = thumb/special (always hold)
 * ───────────────────────────────────────────── */
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = {
    "L L L L L L L R R R R R R R * R",
    "L L L L L L L R R R R R R R * R",
    "L L L L L L L R R R R R R R * R",
    "L L L L L L L R R R R R R * R",
    "L L L L L L L R R R R R * R",
    "* * * * * * * * * *",
};

/* ─────────────────────────────────────────────
 *  process_record_user — ESC_CAPS
 * ───────────────────────────────────────────── */
static uint16_t esc_caps_timer = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case ESC_CAPS:
            if (record->event.pressed) {
                esc_caps_timer = timer_read();
            } else {
                if (timer_elapsed(esc_caps_timer) < 600) {
                    tap_code(KC_ESC);
                } else {
                    tap_code(KC_CAPS);
                }
            }
            return false;
    }
    return true;
}

/* ─────────────────────────────────────────────
 *  DIP switch — map Win mode to WIN_BASE (layer 2), not layer 6
 * ───────────────────────────────────────────── */
bool dip_switch_update_user(uint8_t index, bool active) {
    if (index == 0) {
        default_layer_set(1UL << (active ? MAC_BASE : WIN_BASE));
    }
    return true;
}

/* ─────────────────────────────────────────────
 *  Keymap
 * ───────────────────────────────────────────── */
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  /* ==================== MAC_BASE (0) ====================
     ESC_CAPS  = tap<600ms→Esc, hold≥600ms→CapsLock
     HYPR_T    = tap→Tab, hold→Ctrl+Shift+Cmd+Opt
     LT(1,ESC) = tap→Esc, hold→MAC_FUNC (homerow access to FUNC)
     HRM_*     = home-row mods (bilateral chordal hold)
     TD(FN_MAC)= hold→MAC_FUNC, tap→KC_APP (placeholder, v0.26→OSL MCRO)
  */
    [MAC_BASE] = LAYOUT_ansi_82(
        ESC_CAPS,             KC_BRID,    KC_BRIU,    KC_MCTL,    KC_LAUNCHPAD, KC_SEARCH,   KC_CRTA,    KC_MPRV,    KC_MPLY,    KC_MNXT,    KC_MUTE,    KC_VOLD,  KC_VOLU,  KC_DEL,               KC_MUTE,
        KC_GRV,               KC_1,       KC_2,       KC_3,       KC_4,         KC_5,        KC_6,       KC_7,       KC_8,       KC_9,       KC_0,       KC_MINS,  KC_EQL,   KC_BSPC,              KC_PGUP,
        HYPR_T(KC_TAB),       KC_Q,       KC_W,       KC_E,       KC_R,         KC_T,        KC_Y,       KC_U,       KC_I,       KC_O,       KC_P,       KC_LBRC,  KC_RBRC,  KC_BSLS,              KC_PGDN,
        LT(MAC_FUNC, KC_ESC), HRM_LCTL,   HRM_LGUI,   HRM_LALT,   HRM_LSFT,     KC_G,        KC_H,       HRM_RSFT,   HRM_RALT,   HRM_RGUI,   HRM_RCTL,   KC_QUOT,  KC_ENT,       KC_HOME,
        KC_LSFT,              KC_Z,       KC_X,       KC_C,       KC_V,         KC_B,        KC_N,       KC_M,       KC_COMM,    KC_DOT,     KC_SLSH,    KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LOPTN,  KC_LCMMD,  KC_SPC,         KC_RCMMD,    TD(TD_FN_MAC), KC_LEFT,    KC_DOWN,    KC_RGHT),

  /* ==================== MAC_FUNC (1) ====================
     TO(0)  on ESC = return to MAC_BASE (also clears any stuck state)
     TD_BOOT on -  = hold 3 s to enter DFU bootloader
     Numpad         = right-hand 789/456/123/0 layout
     Screenshot     = Cmd+Shift+4 (Mac area screenshot)
  */
    [MAC_FUNC] = LAYOUT_ansi_82(
        TO(MAC_BASE),         KC_F1,      KC_F2,      KC_F3,      KC_F4,        KC_F5,       KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,     KC_F11,   KC_F12,   KC_DEL,               UG_TOGG,
        _______,              BT_HST1,    BT_HST2,    BT_HST3,    P2P4G,        _______,     _______,    KC_PAST,    KC_PSLS,    KC_PPLS,    KC_PMNS,    TD(TD_BOOT), LGUI(KC_S), UG_SATD,         _______,
        UG_TOGG,              UG_NEXT,    UG_VALU,    UG_HUEU,    UG_SATU,      UG_SPDU,     KC_DEL,     KC_P7,      KC_P8,      KC_P9,      KC_BSPC,    UG_PREV,  UG_VALD,  LGUI(LSFT(KC_4)),    UG_SPDD,
        _______,              KC_LCTL,    KC_LGUI,    KC_LALT,    KC_LSFT,      _______,     KC_TAB,     KC_P4,      KC_P5,      KC_P6,      KC_ENT,     UG_HUED,  _______,      KC_HOME,
        _______,              TD(TD_MINS),TD(TD_EQL), TD(TD_PAREN),TD(TD_BRACE),BAT_LVL,     KC_COMM,    KC_P1,      KC_P2,      KC_P3,      KC_PDOT,    TD(TD_ANGLE),           _______,
        _______,  _______,  _______,  KC_P0,           _______,     _______,      KC_RCTL,    KC_HOME,    KC_PGUP,    KC_PGDN),

  /* ==================== WIN_BASE (2) ====================
     Identical to MAC_BASE except:
     - Bottom row: LCTL / LGUI(Win) / LALT order (standard Windows layout)
     - TD_FN_WIN activates WIN_FUNC
  */
    [WIN_BASE] = LAYOUT_ansi_82(
        ESC_CAPS,             KC_F1,      KC_F2,      KC_F3,      KC_F4,        KC_F5,       KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,     KC_F11,   KC_F12,   KC_DEL,               KC_MUTE,
        KC_GRV,               KC_1,       KC_2,       KC_3,       KC_4,         KC_5,        KC_6,       KC_7,       KC_8,       KC_9,       KC_0,       KC_MINS,  KC_EQL,   KC_BSPC,              KC_PGUP,
        HYPR_T(KC_TAB),       KC_Q,       KC_W,       KC_E,       KC_R,         KC_T,        KC_Y,       KC_U,       KC_I,       KC_O,       KC_P,       KC_LBRC,  KC_RBRC,  KC_BSLS,              KC_PGDN,
        LT(WIN_FUNC, KC_ESC), HRM_LCTL,   HRM_LGUI,   HRM_LALT,   HRM_LSFT,     KC_G,        KC_H,       HRM_RSFT,   HRM_RALT,   HRM_RGUI,   HRM_RCTL,   KC_QUOT,  KC_ENT,       KC_HOME,
        KC_LSFT,              KC_Z,       KC_X,       KC_C,       KC_V,         KC_B,        KC_N,       KC_M,       KC_COMM,    KC_DOT,     KC_SLSH,    KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LGUI,  KC_LALT,   KC_SPC,         KC_RALT,     TD(TD_FN_WIN), KC_LEFT,    KC_DOWN,    KC_RGHT),

  /* ==================== WIN_FUNC (3) ====================
     Windows-specific: Task View (F3), File Explorer (F4),
     Win+S search (F5), Win+L lock (F6), Snipping Tool screenshot.
  */
    [WIN_FUNC] = LAYOUT_ansi_82(
        TO(WIN_BASE),         KC_BRID,    KC_BRIU,    KC_TASK,    KC_FILE,      LGUI(KC_S),  LGUI(KC_L), KC_MPRV,    KC_MPLY,    KC_MNXT,    KC_MUTE,    KC_VOLD,  KC_VOLU,  KC_DEL,               UG_TOGG,
        _______,              BT_HST1,    BT_HST2,    BT_HST3,    P2P4G,        _______,     _______,    KC_PAST,    KC_PSLS,    KC_PPLS,    KC_PMNS,    TD(TD_BOOT), LGUI(KC_S), UG_SATD,         _______,
        UG_TOGG,              UG_NEXT,    UG_VALU,    UG_HUEU,    UG_SATU,      UG_SPDU,     KC_DEL,     KC_P7,      KC_P8,      KC_P9,      KC_BSPC,    UG_PREV,  UG_VALD,  LGUI(LSFT(KC_S)),    UG_SPDD,
        _______,              KC_LCTL,    KC_LGUI,    KC_LALT,    KC_LSFT,      _______,     KC_TAB,     KC_P4,      KC_P5,      KC_P6,      KC_ENT,     UG_HUED,  _______,      KC_HOME,
        _______,              TD(TD_MINS),TD(TD_EQL), TD(TD_PAREN),TD(TD_BRACE),BAT_LVL,     KC_COMM,    KC_P1,      KC_P2,      KC_P3,      KC_PDOT,    TD(TD_ANGLE),           _______,
        _______,  _______,  _______,  KC_P0,           _______,     _______,      KC_RCTL,    KC_HOME,    KC_PGUP,    KC_PGDN),
};
// clang-format on

/* ─────────────────────────────────────────────
 *  Encoder map
 * ───────────────────────────────────────────── */
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [MAC_BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [MAC_FUNC] = {ENCODER_CCW_CW(UG_VALD, UG_VALU)},
    [WIN_BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [WIN_FUNC] = {ENCODER_CCW_CW(UG_VALD, UG_VALU)},
};
#endif

/* ─────────────────────────────────────────────
 *  RGB indicators
 *  LED_AT(r,c) = g_led_config.matrix_co[r][c]
 *  Row/col are MATRIX positions, not physical positions.
 *  Verify with `qmk info -kb keychron/v1_max/ansi_encoder -m`.
 * ───────────────────────────────────────────── */
#if defined(RGB_MATRIX_ENABLE)
#define LED_AT(r, c) g_led_config.matrix_co[(r)][(c)]

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    /* 1. HRM Shift indicator — orange on physical LSFT and RSFT when
     *    homerow shift (F=LSFT or J=RSFT) is being held.               */
    if (get_mods() & (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT))) {
        RGB_MATRIX_INDICATOR_SET_COLOR(LED_AT(4, 0),  0xFF, 0x80, 0x00); // LSFT
        RGB_MATRIX_INDICATOR_SET_COLOR(LED_AT(4, 11), 0xFF, 0x80, 0x00); // RSFT
    }

    /* 2. Layer number key — cyan on number key matching active layer.
     *    E.g. MAC_FUNC (1) lights the "1" key while Fn is held.        */
    uint8_t layer = get_highest_layer(layer_state);
    if (layer >= 1 && layer <= 9) {
        RGB_MATRIX_INDICATOR_SET_COLOR(LED_AT(1, layer), 0x00, 0xFF, 0xFF);
    }

    /* 3. TO() exit indicator — ESC key red when stuck in a toggled layer.
     *    g_to_layer_active is set in process_record_user when TO() fires.
     *    Dormant in v0.25 (no TG/TO except back-to-base). Active in v0.26. */
    if (g_to_layer_active) {
        RGB_MATRIX_INDICATOR_SET_COLOR(LED_AT(0, 0), 0xFF, 0x00, 0x00); // ESC
    }

    return false;
}
#endif
