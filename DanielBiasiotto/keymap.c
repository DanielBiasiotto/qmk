/* Copyright 2015-2017 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "muse.h"

typedef struct {
    bool    is_press_action;
    uint8_t state;
} tap;

enum { SINGLE_TAP = 1, SINGLE_HOLD, DOUBLE_TAP, DOUBLE_HOLD, DOUBLE_SINGLE_TAP, TRIPLE_TAP, TRIPLE_HOLD };
enum { CTL_ESC, ENT_SFT, MIN_CTL };
uint8_t cur_dance(qk_tap_dance_state_t *state);
void    ctl_finished(qk_tap_dance_state_t *state, void *user_data);
void    ctl_reset(qk_tap_dance_state_t *state, void *user_data);
void    ent_finished(qk_tap_dance_state_t *state, void *user_data);
void    ent_reset(qk_tap_dance_state_t *state, void *user_data);
void    min_finished(qk_tap_dance_state_t *state, void *user_data);
void    min_reset(qk_tap_dance_state_t *state, void *user_data);

enum preonic_layers { _QWERTY, _COLEMAK, _DVORAK, _LOWER, _RAISE, _ADJUST };

enum preonic_keycodes { QWERTY = SAFE_RANGE, COLEMAK, DVORAK, LOWER, RAISE, BACKLIT };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* Qwerty
     * ,-----------------------------------------------------------------------------------.
     * |   `  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Bksp |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Tab  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  | Del  |
     * |------+------+------+------+------+-------------+------+------+------+------+------|
     * | Esc  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  "   |
     * |------+------+------+------+------+------|------+------+------+------+------+------|
     * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |Enter |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Brite| Ctrl | Alt  | GUI  |Lower |    Space    |Raise | Left | Down |  Up  |Right |
     * `-----------------------------------------------------------------------------------'
     */
    [_QWERTY] = LAYOUT_preonic_grid(KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_DEL, KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_ENT, KC_LCTL, KC_ESC, KC_LALT, KC_LGUI, LOWER, KC_SPC, KC_SPC, RAISE, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT),

    /* Colemak
     * ,-----------------------------------------------------------------------------------.
     * |   `  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Bksp |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Tab  |   Q  |   W  |   F  |   P  |   G  |   J  |   L  |   U  |   Y  |   ;  | Del  |
     * |------+------+------+------+------+-------------+------+------+------+------+------|
     * | Esc  |   A  |   R  |   S  |   T  |   D  |   H  |   N  |   E  |   I  |   O  |  "   |
     * |------+------+------+------+------+------|------+------+------+------+------+------|
     * | Shift|   Z  |   X  |   C  |   V  |   B  |   K  |   M  |   ,  |   .  |   /  |Enter |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Brite| Ctrl | Alt  | GUI  |Lower |    Space    |Raise | Left | Down |  Up  |Right |
     * `-----------------------------------------------------------------------------------'
     */
    [_COLEMAK] = LAYOUT_preonic_grid(KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, KC_TAB, KC_Q, KC_W, KC_F, KC_P, KC_G, KC_J, KC_L, KC_U, KC_Y, KC_SCLN, KC_DEL, KC_ESC, KC_A, KC_R, KC_S, KC_T, KC_D, KC_H, KC_N, KC_E, KC_I, KC_O, KC_QUOT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_K, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_ENT, BACKLIT, KC_LCTL, KC_LALT, KC_LGUI, LOWER, KC_SPC, KC_SPC, RAISE, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT),

    /* Dvorak
     * ,-----------------------------------------------------------------------------------.
     * |   `  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Bksp |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | Tab  |   ;  |   ,  |   .  |   P  |   Y  |   F  |   G  |   C  |   R  |   L  | Del  |
     * |------+------+------+------+------+-------------+------+------+------+------+------|
     * | Ctrl |   A  |   O  |   E  |   U  |   I  |   D  |   H  |   T  |   N  |   S  |  -   |
     * |------+------+------+------+------+------|------+------+------+------+------+------|
     * | Shift|   "  |   Q  |   J  |   K  |   X  |   B  |   M  |   W  |   V  |   Z  |Enter |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * | RGB  | Esc  | Alt  | GUI  |Lower |    Space    |Raise | Left | Down |  Up  |Right |
     * `-----------------------------------------------------------------------------------'
     */
    [_DVORAK] = LAYOUT_preonic_grid(
        // |---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------|
        KC_GESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC,
        // |---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------|
        KC_TAB, KC_SCLN, KC_COMM, KC_DOT, KC_P, KC_Y, KC_F, KC_G, KC_C, KC_R, KC_L, KC_DEL,
        // |---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------|
        TD(CTL_ESC), KC_A, KC_O, KC_E, KC_U, KC_I, KC_D, KC_H, KC_T, KC_N, KC_S, TD(MIN_CTL),
        // |---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------|
        KC_LSFT, KC_QUOT, KC_Q, KC_J, KC_K, KC_X, KC_B, KC_M, KC_W, KC_V, KC_Z, TD(ENT_SFT),
        // |---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------|
        KC_LCTL, KC_ESC, KC_LALT, KC_LGUI, LOWER, KC_SPC, KC_SPC, RAISE, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT
        // |---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------|
        ),

    /* Lower
     * ,-----------------------------------------------------------------------------------.
     * |      |      |      |      |      |      |      |  7   |  8   |  9   |  /   |   =  |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      |   (  |   )  |      |      |      |  4   |  5   |  6   |  *   |   \  |
     * |------+------+------+------+------+-------------+------+------+------+------+------|
     * |      |      |   [  |   ]  |      |      |      |  1   |  2   |  3   |  -   |   |  |
     * |------+------+------+------+------+------|------+------+------+------+------+------|
     * |      |      |   {  |   }  |      |      |      |  0   |  ,   |  .   |  +   |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |             |      | Next | Vol- | Vol+ | Play |
     * `-----------------------------------------------------------------------------------'
     */
    [_LOWER] = LAYOUT_preonic_grid(_______, _______, _______, _______, _______, _______, _______, KC_7, KC_8, KC_9, KC_SLSH, KC_EQUAL, _______, _______, KC_LPRN, KC_RPRN, _______, _______, _______, KC_4, KC_5, KC_6, KC_ASTR, KC_BSLASH, _______, _______, KC_LBRC, KC_RBRC, _______, _______, _______, KC_1, KC_2, KC_3, KC_MINUS, KC_PIPE, _______, _______, KC_LCBR, KC_RCBR, _______, _______, _______, _______, KC_COMM, KC_DOT, KC_PLUS, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY),

    /* Raise
     * ,-----------------------------------------------------------------------------------.
     * |  F12 |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F7  |  F8  |  F9  |  F10 |  F11 |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |      |      |      |      |      |      |      |
     * |------+------+------+------+------+-------------+------+------+------+------+------|
     * |      |      |      |      |      |      |      |      |      |      |      |      |
     * |------+------+------+------+------+------|------+------+------+------+------+------|
     * |      |      |      |      |      |      |      |      |      |      |      |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |             |      | Next | Vol- | Vol+ | Play |
     * `-----------------------------------------------------------------------------------'
     */
    [_RAISE] = LAYOUT_preonic_grid(KC_F12, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY),

    /* Adjust (Lower + Raise)
     * ,-----------------------------------------------------------------------------------.
     * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      | Reset| Debug|      |      |      |      |TermOf|TermOn|      |      |  Del |
     * |------+------+------+------+------+-------------+------+------+------+------+------|
     * |      |      |Aud cy|Aud on|AudOff|AGnorm|AGswap|Qwerty|Colemk|Dvorak|      |      |
     * |------+------+------+------+------+------|------+------+------+------+------+------|
     * |      |Voice-|Voice+|Mus on|MusOff|MidiOn|MidOff|      |      |      |      |      |
     * |------+------+------+------+------+------+------+------+------+------+------+------|
     * |      |      |      |      |      |             |      |      |      |      |      |
     * `-----------------------------------------------------------------------------------'
     */
    [_ADJUST] = LAYOUT_preonic_grid(KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, _______, RESET, DEBUG, _______, _______, _______, _______, TERM_ON, TERM_OFF, _______, _______, KC_DEL, _______, _______, MU_MOD, AU_ON, AU_OFF, AG_NORM, AG_SWAP, QWERTY, COLEMAK, DVORAK, _______, _______, _______, MUV_DE, MUV_IN, MU_ON, MU_OFF, MI_ON, MI_OFF, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______)

};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QWERTY:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_QWERTY);
            }
            return false;
            break;
        case COLEMAK:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_COLEMAK);
            }
            return false;
            break;
        case DVORAK:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_DVORAK);
            }
            return false;
            break;
        case LOWER:
            if (record->event.pressed) {
                layer_on(_LOWER);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
            } else {
                layer_off(_LOWER);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
            }
            return false;
            break;
        case RAISE:
            if (record->event.pressed) {
                layer_on(_RAISE);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
            } else {
                layer_off(_RAISE);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
            }
            return false;
            break;
        case BACKLIT:
            if (record->event.pressed) {
                register_code(KC_RSFT);
#ifdef BACKLIGHT_ENABLE
                backlight_step();
#endif
#ifdef __AVR__
                writePinLow(E6);
#endif
            } else {
                unregister_code(KC_RSFT);
#ifdef __AVR__
                writePinHigh(E6);
#endif
            }
            return false;
            break;
    }
    return true;
};

bool     muse_mode      = false;
uint8_t  last_muse_note = 0;
uint16_t muse_counter   = 0;
uint8_t  muse_offset    = 70;
uint16_t muse_tempo     = 50;

void encoder_update_user(uint8_t index, bool clockwise) {
    if (muse_mode) {
        if (IS_LAYER_ON(_RAISE)) {
            if (clockwise) {
                muse_offset++;
            } else {
                muse_offset--;
            }
        } else {
            if (clockwise) {
                muse_tempo += 1;
            } else {
                muse_tempo -= 1;
            }
        }
    } else {
        if (clockwise) {
            register_code(KC_PGDN);
            unregister_code(KC_PGDN);
        } else {
            register_code(KC_PGUP);
            unregister_code(KC_PGUP);
        }
    }
}

void dip_switch_update_user(uint8_t index, bool active) {
    switch (index) {
        case 0:
            if (active) {
                layer_on(_ADJUST);
            } else {
                layer_off(_ADJUST);
            }
            break;
        case 1:
            if (active) {
                muse_mode = true;
            } else {
                muse_mode = false;
            }
    }
}

void matrix_scan_user(void) {
#ifdef AUDIO_ENABLE
    if (muse_mode) {
        if (muse_counter == 0) {
            uint8_t muse_note = muse_offset + SCALE[muse_clock_pulse()];
            if (muse_note != last_muse_note) {
                stop_note(compute_freq_for_midi_note(last_muse_note));
                play_note(compute_freq_for_midi_note(muse_note), 0xF);
                last_muse_note = muse_note;
            }
        }
        muse_counter = (muse_counter + 1) % muse_tempo;
    } else {
        if (muse_counter) {
            stop_all_notes();
            muse_counter = 0;
        }
    }
#endif
}

bool music_mask_user(uint16_t keycode) {
    switch (keycode) {
        case RAISE:
        case LOWER:
            return false;
        default:
            return true;
    }
}

uint8_t cur_dance(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else
            return SINGLE_HOLD;
    } else if (state->count == 2) {
        // DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted)
            return DOUBLE_SINGLE_TAP;
        else if (state->pressed)
            return DOUBLE_HOLD;
        else
            return DOUBLE_TAP;
    }
    if (state->count == 3) {
        if (state->interrupted || !state->pressed)
            return TRIPLE_TAP;
        else
            return TRIPLE_HOLD;
    } else
        return 8;  // Magic number. At some point this method will expand to work for more presses
}

static tap ctltap_state = {.is_press_action = true, .state = 0};

void ctl_finished(qk_tap_dance_state_t *state, void *user_data) {
    ctltap_state.state = cur_dance(state);
    switch (ctltap_state.state) {
        case SINGLE_TAP:
            register_code(KC_ESC);
            break;
        case SINGLE_HOLD:
            register_code(KC_LCTRL);
            break;
        case DOUBLE_TAP:
            break;
        case DOUBLE_HOLD:
            break;
        case DOUBLE_SINGLE_TAP:
            break;
    }
}

void ctl_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (ctltap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_ESC);
            break;
        case SINGLE_HOLD:
            unregister_code(KC_LCTRL);
            break;
        case DOUBLE_TAP:
            break;
        case DOUBLE_HOLD:
            break;
        case DOUBLE_SINGLE_TAP:
            break;
    }
    ctltap_state.state = 0;
}

static tap enttap_state = {.is_press_action = true, .state = 0};

void ent_finished(qk_tap_dance_state_t *state, void *user_data) {
    enttap_state.state = cur_dance(state);
    switch (enttap_state.state) {
        case SINGLE_TAP:
            register_code(KC_ENTER);
            break;
        case SINGLE_HOLD:
            register_code(KC_RSHIFT);
            break;
        case DOUBLE_TAP:
            break;
        case DOUBLE_HOLD:
            break;
        case DOUBLE_SINGLE_TAP:
            break;
    }
}

void ent_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (enttap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_ENTER);
            break;
        case SINGLE_HOLD:
            unregister_code(KC_RSHIFT);
            break;
        case DOUBLE_TAP:
            break;
        case DOUBLE_HOLD:
            break;
        case DOUBLE_SINGLE_TAP:
            break;
    }
    enttap_state.state = 0;
}

static tap mintap_state = {.is_press_action = true, .state = 0};

void min_finished(qk_tap_dance_state_t *state, void *user_data) {
    mintap_state.state = cur_dance(state);
    switch (mintap_state.state) {
        case SINGLE_TAP:
            register_code(KC_MINS);
            break;
        case SINGLE_HOLD:
            register_code(KC_RCTRL);
            break;
        case DOUBLE_TAP:
            break;
        case DOUBLE_HOLD:
            break;
        case DOUBLE_SINGLE_TAP:
            break;
    }
}

void min_reset(qk_tap_dance_state_t *state, void *user_data) {
    switch (mintap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_MINS);
            break;
        case SINGLE_HOLD:
            unregister_code(KC_RCTRL);
            break;
        case DOUBLE_TAP:
            break;
        case DOUBLE_HOLD:
            break;
        case DOUBLE_SINGLE_TAP:
            break;
    }
    enttap_state.state = 0;
}

qk_tap_dance_action_t tap_dance_actions[] = {[CTL_ESC] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ctl_finished, ctl_reset), [ENT_SFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, ent_finished, ent_reset), [MIN_CTL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, min_finished, min_reset)};
