/*
 * Keychron V1 Max ANSI candidate.
 *
 * Behavioral intent was inferred from the archived B1 Pro v0.24 ZMK source.
 * The physical layout, wireless controls, DIP layer contract, and encoder are
 * native to the Keychron V1 Max ANSI QMK implementation.
 */

#include QMK_KEYBOARD_H
#include "keychron_common.h"
#include "keymap_us.h"

enum layers {
    MAC_BASE = 0,
    MAC_FUNC = 1,
    WIN_BASE = 2,
    WIN_FUNC = 3,
    MAC_SYM,
    MAC_NAV,
    MAC_UAT,
    MAC_MCRO,
    WIN_SYM,
    WIN_NAV,
    WIN_UAT,
    WIN_MCRO,
};

enum custom_keycodes {
    ESC_CAPS = SAFE_RANGE,
};

enum tap_dance_keys {
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

#define HRM_LCTL LCTL_T(KC_A)
#define HRM_LGUI LGUI_T(KC_S)
#define HRM_LALT LALT_T(KC_D)
#define HRM_LSFT LSFT_T(KC_F)
#define HRM_RSFT RSFT_T(KC_J)
#define HRM_RALT RALT_T(KC_K)
#define HRM_RGUI RGUI_T(KC_L)
#define HRM_RCTL RCTL_T(KC_SCLN)

static void td_fn_mac_finished(tap_dance_state_t *state, void *user_data);
static void td_fn_mac_reset(tap_dance_state_t *state, void *user_data);
static void td_fn_win_finished(tap_dance_state_t *state, void *user_data);
static void td_fn_win_reset(tap_dance_state_t *state, void *user_data);
static void td_boot_finished(tap_dance_state_t *state, void *user_data);
static void td_symbols_finished(tap_dance_state_t *state, void *user_data);

tap_dance_action_t tap_dance_actions[] = {
    [TD_FN_MAC] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_fn_mac_finished, td_fn_mac_reset),
    [TD_FN_WIN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_fn_win_finished, td_fn_win_reset),
    [TD_BOOT]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_boot_finished, NULL),
    [TD_MINS]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
    [TD_EQL]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
    [TD_PAREN]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
    [TD_BRACE]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
    [TD_BRACK]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
    [TD_ANGLE]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_symbols_finished, NULL),
};

static void td_fn_mac_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        if (state->pressed) {
            layer_on(MAC_FUNC);
        } else {
            set_oneshot_layer(MAC_MCRO, ONESHOT_START);
        }
    }
}

static void td_fn_mac_reset(tap_dance_state_t *state, void *user_data) {
    layer_off(MAC_FUNC);
}

static void td_fn_win_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1) {
        if (state->pressed) {
            layer_on(WIN_FUNC);
        } else {
            set_oneshot_layer(WIN_MCRO, ONESHOT_START);
        }
    }
}

static void td_fn_win_reset(tap_dance_state_t *state, void *user_data) {
    layer_off(WIN_FUNC);
}

static void td_boot_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count == 1 && state->pressed) {
        reset_keyboard();
    }
}

static void td_symbols_finished(tap_dance_state_t *state, void *user_data) {
    uint16_t dance = TAP_DANCE_KEYCODE(state);

    switch (dance) {
        case TD(TD_MINS):
            tap_code16(state->count == 1 ? KC_MINS : KC_UNDS);
            break;
        case TD(TD_EQL):
            tap_code16(state->count == 1 ? KC_EQL : KC_PLUS);
            break;
        case TD(TD_PAREN):
            if (state->count >= 3) {
                tap_code16(KC_LPRN);
                tap_code16(KC_RPRN);
            } else {
                tap_code16(state->count == 1 ? KC_LPRN : KC_RPRN);
            }
            break;
        case TD(TD_BRACE):
            if (state->count >= 3) {
                tap_code16(KC_LCBR);
                tap_code16(KC_RCBR);
            } else {
                tap_code16(state->count == 1 ? KC_LCBR : KC_RCBR);
            }
            break;
        case TD(TD_BRACK):
            if (state->count >= 3) {
                tap_code16(KC_LBRC);
                tap_code16(KC_RBRC);
            } else {
                tap_code16(state->count == 1 ? KC_LBRC : KC_RBRC);
            }
            break;
        case TD(TD_ANGLE):
            if (state->count >= 3) {
                tap_code16(KC_LABK);
                tap_code16(KC_RABK);
            } else {
                tap_code16(state->count == 1 ? KC_LABK : KC_RABK);
            }
            break;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t esc_caps_timer;

    if (keycode != ESC_CAPS) return true;

    if (record->event.pressed) {
        esc_caps_timer = timer_read();
    } else if (timer_elapsed(esc_caps_timer) >= 600) {
        tap_code(KC_CAPS);
    } else {
        tap_code(KC_ESC);
    }
    return false;
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    if (keycode == TD(TD_BOOT)) return 3000;
    return 200;
}

uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case HRM_LCTL:
        case HRM_LGUI:
        case HRM_LALT:
        case HRM_LSFT:
        case HRM_RSFT:
        case HRM_RALT:
        case HRM_RGUI:
        case HRM_RCTL:
            return 175;
    }
    return 0;
}

uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prev_keycode) {
    switch (keycode) {
        case HRM_LCTL:
        case HRM_LGUI:
        case HRM_LALT:
        case HRM_LSFT:
        case HRM_RSFT:
        case HRM_RALT:
        case HRM_RGUI:
        case HRM_RCTL:
            return FLOW_TAP_TERM;
    }
    return 0;
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case HRM_LCTL:
        case HRM_LGUI:
        case HRM_LALT:
        case HRM_LSFT:
        case HRM_RSFT:
        case HRM_RALT:
        case HRM_RGUI:
        case HRM_RCTL:
            return true;
    }
    return false;
}

const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM = LAYOUT_ansi_82(
    '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*',
    'L', 'L', 'L', 'L', 'L', 'L', 'L', 'R', 'R', 'R', 'R', 'R', 'R', 'R', '*',
    'L', 'L', 'L', 'L', 'L', 'L', 'L', 'R', 'R', 'R', 'R', 'R', 'R', 'R', '*',
    'L', 'L', 'L', 'L', 'L', 'L', 'R', 'R', 'R', 'R', 'R', 'R', '*', '*',
    'L',      'L', 'L', 'L', 'L', 'L',      'R', 'R', 'R', 'R', 'R',      'R', '*',
    '*', '*', '*',                     '*',                     '*', '*', '*', '*', '*', '*'
);

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
   
  /* ==================== MAC_BASE ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [MAC_BASE] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        ESC_CAPS,             KC_BRID,    KC_BRIU,    KC_MCTRL,   KC_LNPAD, LGUI(KC_SPC),      LGUI(LCTL(KC_Q)),  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD, KC_VOLU, KC_DEL,               KC_MUTE,
        KC_GRV,               KC_1,       KC_2,       KC_3,       KC_4,     KC_5,              KC_6,              KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS, KC_EQL,  KC_BSPC,              KC_PGUP,
        HYPR_T(KC_TAB),       KC_Q,       KC_W,       KC_E,       KC_R,     KC_T,              KC_Y,              KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC, KC_RBRC, LT(MAC_NAV, KC_BSLS), KC_PGDN,
        LT(MAC_FUNC, KC_ESC), HRM_LCTL,   HRM_LGUI,   HRM_LALT,   HRM_LSFT, LT(MAC_SYM, KC_G), LT(MAC_SYM, KC_H), HRM_RSFT, HRM_RALT, HRM_RGUI, HRM_RCTL, KC_QUOT, KC_ENT,       KC_HOME,
        KC_LSFT,              KC_Z,       KC_X,       KC_C,       KC_V,     KC_B,              KC_N,              KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,          KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,   KC_SPC,        KC_RCMMD,      TD(TD_FN_MAC), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

   
  /* ==================== MAC_FUNC ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [MAC_FUNC] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        TO(MAC_BASE),         KC_F1,      KC_F2,      KC_F3,      KC_F4,    KC_F5,             KC_F6,             KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,  KC_F12,  KC_DEL,               UG_TOGG,
        _______,              BT_HST1,    BT_HST2,    BT_HST3,    P2P4G,    _______,           _______,           KC_PAST,  KC_PSLS,  KC_PPLS,  KC_PMNS,  TD(TD_BOOT),LGUI(KC_SPC),UG_SATD,              _______,
        UG_TOGG,              UG_NEXT,    UG_VALU,    UG_HUEU,    UG_SATU,  UG_SPDU,           KC_DEL,            KC_P7,    KC_P8,    KC_P9,    KC_BSPC,  UG_PREV, UG_VALD, LGUI(LSFT(KC_4)),     UG_SPDD,
        _______,              KC_LCTL,    KC_LGUI,    KC_LALT,    KC_LSFT,  _______,           KC_TAB,            KC_P4,    KC_P5,    KC_P6,    KC_ENT,   UG_HUED, _______,      KC_HOME,
        _______,              _______,    _______,    _______,    _______,  BAT_LVL,           KC_COMM,           KC_P1,    KC_P2,    KC_P3,    KC_PDOT,  LGUI(KC_SPC),     _______,
        _______,  _______,  _______,    KC_P0,         _______,       _______,       KC_RCTL,  KC_HOME,  KC_PGUP,  KC_PGDN),

   
  /* ==================== WIN_BASE ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [WIN_BASE] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        ESC_CAPS,             KC_F1,      KC_F2,      KC_F3,      KC_F4,    KC_F5,             KC_F6,             KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,  KC_F12,  KC_DEL,               KC_MUTE,
        KC_GRV,               KC_1,       KC_2,       KC_3,       KC_4,     KC_5,              KC_6,              KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS, KC_EQL,  KC_BSPC,              KC_PGUP,
        HYPR_T(KC_TAB),       KC_Q,       KC_W,       KC_E,       KC_R,     KC_T,              KC_Y,              KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC, KC_RBRC, LT(WIN_NAV, KC_BSLS), KC_PGDN,
        LT(WIN_FUNC, KC_ESC), HRM_LCTL,   HRM_LGUI,   HRM_LALT,   HRM_LSFT, LT(WIN_SYM, KC_G), LT(WIN_SYM, KC_H), HRM_RSFT, HRM_RALT, HRM_RGUI, HRM_RCTL, KC_QUOT, KC_ENT,       KC_HOME,
        KC_LSFT,              KC_Z,       KC_X,       KC_C,       KC_V,     KC_B,              KC_N,              KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,          KC_UP,
        KC_LCTL,  KC_LGUI,  KC_LALT,    KC_SPC,        KC_RALT,       TD(TD_FN_WIN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

   
  /* ==================== WIN_FUNC ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [WIN_FUNC] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        TO(WIN_BASE),         KC_BRID,    KC_BRIU,    KC_TASK,    KC_FILE,  LGUI(KC_S),        LGUI(KC_L),        KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD, KC_VOLU, KC_DEL,               UG_TOGG,
        _______,              BT_HST1,    BT_HST2,    BT_HST3,    P2P4G,    _______,           _______,           KC_PAST,  KC_PSLS,  KC_PPLS,  KC_PMNS,  TD(TD_BOOT),LGUI(KC_S),UG_SATD,              _______,
        UG_TOGG,              UG_NEXT,    UG_VALU,    UG_HUEU,    UG_SATU,  UG_SPDU,           KC_DEL,            KC_P7,    KC_P8,    KC_P9,    KC_BSPC,  UG_PREV, UG_VALD, LGUI(LSFT(KC_S)),     UG_SPDD,
        _______,              KC_LCTL,    KC_LGUI,    KC_LALT,    KC_LSFT,  _______,           KC_TAB,            KC_P4,    KC_P5,    KC_P6,    KC_ENT,   UG_HUED, _______,      KC_HOME,
        _______,              _______,    _______,    _______,    _______,  BAT_LVL,           KC_COMM,           KC_P1,    KC_P2,    KC_P3,    KC_PDOT,  LGUI(KC_DOT),     _______,
        _______,  _______,  _______,    KC_P0,         _______,       _______,       KC_RCTL,  KC_HOME,  KC_PGUP,  KC_PGDN),

   
  /* ==================== MAC_SYM ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [MAC_SYM] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        TO(MAC_BASE),         KC_F13,     KC_F14,     KC_F15,     KC_F16,   KC_F17,            KC_F18,            KC_F19,   KC_F20,   KC_F21,   KC_F22,   KC_F23,  KC_F24,  _______,              UG_TOGG,
        KC_GRV,               KC_EXLM,    KC_AT,      KC_HASH,    KC_DLR,   KC_PERC,           KC_CIRC,           KC_AMPR,  KC_ASTR,  TD(TD_MINS),TD(TD_EQL),LGUI(KC_0),LGUI(KC_MINS),LGUI(KC_EQL),         _______,
        KC_TILD,              KC_LCTL,    KC_LGUI,    KC_LALT,    KC_LSFT,  _______,           _______,           TD(TD_PAREN),TD(TD_BRACE),TD(TD_BRACK),_______,  _______, _______, _______,              _______,
        _______,              _______,    _______,    _______,    _______,  _______,           _______,           TD(TD_ANGLE),_______,  _______,  _______,  _______, _______,      _______,
        _______,              _______,    _______,    _______,    _______,  _______,           _______,           _______,  _______,  _______,  _______,  LGUI(KC_SPC),     _______,
        _______,  _______,  _______,    _______,       _______,       _______,       _______,  _______,  _______,  _______),

   
  /* ==================== MAC_NAV ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [MAC_NAV] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        TO(MAC_BASE),         KC_F13,     KC_F14,     KC_F15,     KC_F16,   KC_F17,            KC_F18,            KC_F19,   KC_F20,   KC_F21,   KC_F22,   KC_F23,  KC_F24,  _______,              UG_TOGG,
        _______,              LGUI(KC_Q), LGUI(KC_W), LGUI(KC_E), LGUI(KC_R),_______,           _______,           MS_WHLL,  MS_WHLU,  MS_WHLR,  _______,  _______, _______, _______,              KC_PGUP,
        _______,              _______,    _______,    _______,    _______,  _______,           _______,           MS_LEFT,  MS_UP,    MS_RGHT,  _______,  _______, _______, _______,              KC_PGDN,
        _______,              _______,    _______,    _______,    _______,  _______,           _______,           MS_BTN1,  MS_DOWN,  MS_BTN2,  MS_BTN3,  _______, _______,      KC_HOME,
        _______,              _______,    _______,    LGUI(KC_C), LGUI(KC_V),_______,           _______,           MS_ACL0,  MS_ACL1,  MS_ACL2,  _______,  _______,          KC_UP,
        _______,  _______,  _______,    _______,       _______,       _______,       _______,  KC_LEFT,  KC_DOWN,  KC_RGHT),

   
  /* ==================== MAC_UAT ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [MAC_UAT] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        TO(MAC_BASE),         XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,          XXXXXXX,
        XXXXXXX,  XXXXXXX,  XXXXXXX,    XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX),

   
  /* ==================== MAC_MCRO ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [MAC_MCRO] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        TO(MAC_BASE),         XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              TO(MAC_FUNC),TO(MAC_SYM),TO(MAC_NAV),TO(MAC_UAT),XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,          XXXXXXX,
        XXXXXXX,  XXXXXXX,  XXXXXXX,    XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX),

   
  /* ==================== WIN_SYM ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [WIN_SYM] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        TO(WIN_BASE),         KC_F13,     KC_F14,     KC_F15,     KC_F16,   KC_F17,            KC_F18,            KC_F19,   KC_F20,   KC_F21,   KC_F22,   KC_F23,  KC_F24,  _______,              UG_TOGG,
        KC_GRV,               KC_EXLM,    KC_AT,      KC_HASH,    KC_DLR,   KC_PERC,           KC_CIRC,           KC_AMPR,  KC_ASTR,  TD(TD_MINS),TD(TD_EQL),LCTL(KC_0),LCTL(KC_MINS),LCTL(KC_EQL),         _______,
        KC_TILD,              KC_LCTL,    KC_LGUI,    KC_LALT,    KC_LSFT,  _______,           _______,           TD(TD_PAREN),TD(TD_BRACE),TD(TD_BRACK),_______,  _______, _______, _______,              _______,
        _______,              _______,    _______,    _______,    _______,  _______,           _______,           TD(TD_ANGLE),_______,  _______,  _______,  _______, _______,      _______,
        _______,              _______,    _______,    _______,    _______,  _______,           _______,           _______,  _______,  _______,  _______,  LGUI(KC_DOT),     _______,
        _______,  _______,  _______,    _______,       _______,       _______,       _______,  _______,  _______,  _______),

   
  /* ==================== WIN_NAV ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [WIN_NAV] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        TO(WIN_BASE),         KC_F13,     KC_F14,     KC_F15,     KC_F16,   KC_F17,            KC_F18,            KC_F19,   KC_F20,   KC_F21,   KC_F22,   KC_F23,  KC_F24,  _______,              UG_TOGG,
        _______,              LALT(KC_F4),LCTL(KC_W), LGUI(KC_E), LGUI(KC_R),_______,           _______,           MS_WHLL,  MS_WHLU,  MS_WHLR,  _______,  _______, _______, _______,              KC_PGUP,
        _______,              _______,    _______,    _______,    _______,  _______,           _______,           MS_LEFT,  MS_UP,    MS_RGHT,  _______,  _______, _______, _______,              KC_PGDN,
        _______,              _______,    _______,    _______,    _______,  _______,           _______,           MS_BTN1,  MS_DOWN,  MS_BTN2,  MS_BTN3,  _______, _______,      KC_HOME,
        _______,              _______,    _______,    LCTL(KC_C), LCTL(KC_V),_______,           _______,           MS_ACL0,  MS_ACL1,  MS_ACL2,  _______,  _______,          KC_UP,
        _______,  _______,  _______,    _______,       _______,       _______,       _______,  KC_LEFT,  KC_DOWN,  KC_RGHT),

   
  /* ==================== WIN_UAT ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [WIN_UAT] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        TO(WIN_BASE),         XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,          XXXXXXX,
        XXXXXXX,  XXXXXXX,  XXXXXXX,    XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX),

   
  /* ==================== WIN_MCRO ==================== */
/* ┏━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━┓ ┏━━━━━━━━┓
    ┃           ┃         ┃         ┃ MISSION ┃   MAC   ┃  SEARCH ┃ CTRL+ALT┃ PREVIOUS┃ PLAY/   ┃   NEXT  ┃   Vol   ┃         ┃         ┃        ┃ ┃  Knob  ┃
    ┃TAP: ESC   ┃ Bright- ┃ Bright+ ┃ CONTROL ┃ LAUNCH  ┃         ┃    + M  ┃  TRACK  ┃  PAUSE  ┃  TRACK  ┃   Mute  ┃   Vol-  ┃  Vol+   ┃   DEL  ┃ ┃  Mute  ┃
    ┃HOLD: CAPS ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃        ┃ ┗━━━━━━━━┛
    ┣━━━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━┳━━━┻━━━━━━━━┫ ┏━━━━━━━━┓
    ┃   ~   ┃    !    ┃    @    ┃    #    ┃    $    ┃    %    ┃    ^    ┃    *    ┃    -    ┃    (    ┃    )    ┃    _    ┃    +    ┃            ┃ ┃        ┃
    ┃   `   ┃    1    ┃    2    ┃    3    ┃    4    ┃    5    ┃    6    ┃    7    ┃    8    ┃    9    ┃    0    ┃    -    ┃    =    ┃    BKSP    ┃ ┃  PgUp  ┃
    ┃       ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃            ┃ ┃        ┃
    ┣━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP: TAB  ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    {    ┃    }    ┃    |    ┃ ┃        ┃
    ┃HOLD: HYPR┃    Q    ┃    W    ┃    E    ┃    R    ┃    T    ┃    Y    ┃    U    ┃    I    ┃    O    ┃    P    ┃    [    ┃    ]    ┃    \    ┃ ┃  PgDn  ┃
    ┃          ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃ ┃        ┃
    ┣━━━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻┳━━━━━━━━┻━━━━━━━━━┫ ┣━━━━━━━━┫
    ┃TAP=ESC    ┃ TAP=A   ┃ TAP=S   ┃ TAP=D   ┃ TAP=F   ┃ TAP=G   ┃ TAP=H   ┃ TAP=J   ┃ TAP=K   ┃ TAP=L   ┃ TAP=;/: ┃    "    ┃                  ┃ ┃        ┃
    ┃HOLD:MO(1) ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃ HLD=    ┃    '    ┃      RETURN      ┃ ┃  Home  ┃
    ┃           ┃  L_CTRL ┃  L_GUI  ┃  L_ALT  ┃ L_SHIFT ┃   MO(2) ┃   MO(2) ┃ R_SHIFT ┃  R_ALT  ┃  R_GUI  ┃  R_CTRL ┃         ┃                  ┃ ┃        ┃
    ┣━━━━━━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━┳━━━━━━┻━━━━━━━━━┳━━━━━━━━┛ ┗━━━━━━━━┛
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃    <    ┃    >    ┃    ?    ┃                ┃
    ┃   L_SHIFT    ┃    Z    ┃    X    ┃    C    ┃    V    ┃    B    ┃   N     ┃    M    ┃    ,    ┃    .    ┃    /    ┃      R_SHIFT   ┃ ┏━━━━━━━━━┓
    ┃              ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃         ┃                ┃ ┃    ↑    ┃
    ┣━━━━━━━━━━━┳━━┻━━━━━━━┳━┻━━━━━━━━━╋━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━━╋━━━━━━━━━┻━━━━┳━━━━┻━━━━━┳━━━┻━━━━━━┳━━━━━━━━━┛ ┃         ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃TAP:OSL(5)┃TAP:OSL(5)┃ ┏━━━━━━━━━╋━━━━━━━━━╋━━━━━━━━┓
    ┃  L_CTRL   ┃  L_ALT   ┃   L_CMD   ┃             * < LYR_M_NAV1 SPACE *              ┃    R_CMD     ┃HOLD:MO(1)┃    ←     ┃ ┃    ←    ┃         ┃        ┃
    ┃           ┃          ┃           ┃                                                 ┃              ┃          ┃          ┃ ┃         ┃    ↓    ┃   →    ┃
    ┗━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━┛ ┗━━━━━━━━━┻━━━━━━━━━┻━━━━━━━━┛
*/
    [WIN_MCRO] = LAYOUT_ansi_82(
        //ESC                 //f1        //f2        //f3        //f4      //f5               //f6               //f7      //f8      //f9      //f10     //f11    //f12    //DEL                 //BUTTON
        TO(WIN_BASE),         XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              TO(WIN_FUNC),TO(WIN_SYM),TO(WIN_NAV),TO(WIN_UAT),XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX, XXXXXXX,      XXXXXXX,
        XXXXXXX,              XXXXXXX,    XXXXXXX,    XXXXXXX,    XXXXXXX,  XXXXXXX,           XXXXXXX,           XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,          XXXXXXX,
        XXXXXXX,  XXXXXXX,  XXXXXXX,    XXXXXXX,       XXXXXXX,       XXXXXXX,       XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX)
};
// clang-format on

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [MAC_BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [MAC_FUNC] = {ENCODER_CCW_CW(UG_VALD, UG_VALU)},
    [WIN_BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [WIN_FUNC] = {ENCODER_CCW_CW(UG_VALD, UG_VALU)},
    [MAC_SYM]  = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [MAC_NAV]  = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [MAC_UAT]  = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [MAC_MCRO] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [WIN_SYM]  = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [WIN_NAV]  = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [WIN_UAT]  = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [WIN_MCRO] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
};
#endif
