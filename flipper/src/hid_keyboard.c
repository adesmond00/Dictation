#include "hid_keyboard.h"
#include <furi_hal_usb_hid.h>
#include <ctype.h>

#define INTER_KEY_DELAY_MS 8
#define KEY_PRESS_DELAY_MS 2
#define PROGRESS_INTERVAL 32

/* Mapping entry: HID keycode + whether Shift is needed */
typedef struct {
    uint16_t hid_key;
    bool shift;
} AsciiHidEntry;

struct HidKeyboard {
    uint8_t _unused; /* Placeholder — struct exists for future config (typing speed, etc.) */
};

/* US QWERTY mapping table for ASCII 0x20 (space) through 0x7E (~).
 * Index = ascii_value - 0x20.
 * HID keycodes use the Flipper SDK constants from furi_hal_usb_hid.h. */
static const AsciiHidEntry ascii_hid_map[95] = {
    /* 0x20 ' ' */ {HID_KEYBOARD_SPACEBAR, false},
    /* 0x21 '!' */ {HID_KEYBOARD_1, true},
    /* 0x22 '"' */ {HID_KEYBOARD_APOSTROPHE, true},
    /* 0x23 '#' */ {HID_KEYBOARD_3, true},
    /* 0x24 '$' */ {HID_KEYBOARD_4, true},
    /* 0x25 '%' */ {HID_KEYBOARD_5, true},
    /* 0x26 '&' */ {HID_KEYBOARD_7, true},
    /* 0x27 '\''*/ {HID_KEYBOARD_APOSTROPHE, false},
    /* 0x28 '(' */ {HID_KEYBOARD_9, true},
    /* 0x29 ')' */ {HID_KEYBOARD_0, true},
    /* 0x2A '*' */ {HID_KEYBOARD_8, true},
    /* 0x2B '+' */ {HID_KEYBOARD_EQUAL_SIGN, true},
    /* 0x2C ',' */ {HID_KEYBOARD_COMMA, false},
    /* 0x2D '-' */ {HID_KEYBOARD_MINUS, false},
    /* 0x2E '.' */ {HID_KEYBOARD_DOT, false},
    /* 0x2F '/' */ {HID_KEYBOARD_SLASH, false},
    /* 0x30 '0' */ {HID_KEYBOARD_0, false},
    /* 0x31 '1' */ {HID_KEYBOARD_1, false},
    /* 0x32 '2' */ {HID_KEYBOARD_2, false},
    /* 0x33 '3' */ {HID_KEYBOARD_3, false},
    /* 0x34 '4' */ {HID_KEYBOARD_4, false},
    /* 0x35 '5' */ {HID_KEYBOARD_5, false},
    /* 0x36 '6' */ {HID_KEYBOARD_6, false},
    /* 0x37 '7' */ {HID_KEYBOARD_7, false},
    /* 0x38 '8' */ {HID_KEYBOARD_8, false},
    /* 0x39 '9' */ {HID_KEYBOARD_9, false},
    /* 0x3A ':' */ {HID_KEYBOARD_SEMICOLON, true},
    /* 0x3B ';' */ {HID_KEYBOARD_SEMICOLON, false},
    /* 0x3C '<' */ {HID_KEYBOARD_COMMA, true},
    /* 0x3D '=' */ {HID_KEYBOARD_EQUAL_SIGN, false},
    /* 0x3E '>' */ {HID_KEYBOARD_DOT, true},
    /* 0x3F '?' */ {HID_KEYBOARD_SLASH, true},
    /* 0x40 '@' */ {HID_KEYBOARD_2, true},
    /* 0x41-0x5A: A-Z (uppercase, shift required) */
    /* 0x41 'A' */ {HID_KEYBOARD_A, true},
    /* 0x42 'B' */ {HID_KEYBOARD_B, true},
    /* 0x43 'C' */ {HID_KEYBOARD_C, true},
    /* 0x44 'D' */ {HID_KEYBOARD_D, true},
    /* 0x45 'E' */ {HID_KEYBOARD_E, true},
    /* 0x46 'F' */ {HID_KEYBOARD_F, true},
    /* 0x47 'G' */ {HID_KEYBOARD_G, true},
    /* 0x48 'H' */ {HID_KEYBOARD_H, true},
    /* 0x49 'I' */ {HID_KEYBOARD_I, true},
    /* 0x4A 'J' */ {HID_KEYBOARD_J, true},
    /* 0x4B 'K' */ {HID_KEYBOARD_K, true},
    /* 0x4C 'L' */ {HID_KEYBOARD_L, true},
    /* 0x4D 'M' */ {HID_KEYBOARD_M, true},
    /* 0x4E 'N' */ {HID_KEYBOARD_N, true},
    /* 0x4F 'O' */ {HID_KEYBOARD_O, true},
    /* 0x50 'P' */ {HID_KEYBOARD_P, true},
    /* 0x51 'Q' */ {HID_KEYBOARD_Q, true},
    /* 0x52 'R' */ {HID_KEYBOARD_R, true},
    /* 0x53 'S' */ {HID_KEYBOARD_S, true},
    /* 0x54 'T' */ {HID_KEYBOARD_T, true},
    /* 0x55 'U' */ {HID_KEYBOARD_U, true},
    /* 0x56 'V' */ {HID_KEYBOARD_V, true},
    /* 0x57 'W' */ {HID_KEYBOARD_W, true},
    /* 0x58 'X' */ {HID_KEYBOARD_X, true},
    /* 0x59 'Y' */ {HID_KEYBOARD_Y, true},
    /* 0x5A 'Z' */ {HID_KEYBOARD_Z, true},
    /* 0x5B '[' */ {HID_KEYBOARD_OPEN_BRACKET, false},
    /* 0x5C '\\'*/ {HID_KEYBOARD_BACKSLASH, false},
    /* 0x5D ']' */ {HID_KEYBOARD_CLOSE_BRACKET, false},
    /* 0x5E '^' */ {HID_KEYBOARD_6, true},
    /* 0x5F '_' */ {HID_KEYBOARD_MINUS, true},
    /* 0x60 '`' */ {HID_KEYBOARD_GRAVE_ACCENT, false},
    /* 0x61-0x7A: a-z (lowercase, no shift) */
    /* 0x61 'a' */ {HID_KEYBOARD_A, false},
    /* 0x62 'b' */ {HID_KEYBOARD_B, false},
    /* 0x63 'c' */ {HID_KEYBOARD_C, false},
    /* 0x64 'd' */ {HID_KEYBOARD_D, false},
    /* 0x65 'e' */ {HID_KEYBOARD_E, false},
    /* 0x66 'f' */ {HID_KEYBOARD_F, false},
    /* 0x67 'g' */ {HID_KEYBOARD_G, false},
    /* 0x68 'h' */ {HID_KEYBOARD_H, false},
    /* 0x69 'i' */ {HID_KEYBOARD_I, false},
    /* 0x6A 'j' */ {HID_KEYBOARD_J, false},
    /* 0x6B 'k' */ {HID_KEYBOARD_K, false},
    /* 0x6C 'l' */ {HID_KEYBOARD_L, false},
    /* 0x6D 'm' */ {HID_KEYBOARD_M, false},
    /* 0x6E 'n' */ {HID_KEYBOARD_N, false},
    /* 0x6F 'o' */ {HID_KEYBOARD_O, false},
    /* 0x70 'p' */ {HID_KEYBOARD_P, false},
    /* 0x71 'q' */ {HID_KEYBOARD_Q, false},
    /* 0x72 'r' */ {HID_KEYBOARD_R, false},
    /* 0x73 's' */ {HID_KEYBOARD_S, false},
    /* 0x74 't' */ {HID_KEYBOARD_T, false},
    /* 0x75 'u' */ {HID_KEYBOARD_U, false},
    /* 0x76 'v' */ {HID_KEYBOARD_V, false},
    /* 0x77 'w' */ {HID_KEYBOARD_W, false},
    /* 0x78 'x' */ {HID_KEYBOARD_X, false},
    /* 0x79 'y' */ {HID_KEYBOARD_Y, false},
    /* 0x7A 'z' */ {HID_KEYBOARD_Z, false},
    /* 0x7B '{' */ {HID_KEYBOARD_OPEN_BRACKET, true},
    /* 0x7C '|' */ {HID_KEYBOARD_BACKSLASH, true},
    /* 0x7D '}' */ {HID_KEYBOARD_CLOSE_BRACKET, true},
    /* 0x7E '~' */ {HID_KEYBOARD_GRAVE_ACCENT, true},
};

HidKeyboard* hid_keyboard_alloc(void) {
    HidKeyboard* hid = malloc(sizeof(HidKeyboard));
    hid->_unused = 0;
    return hid;
}

void hid_keyboard_free(HidKeyboard* hid) {
    furi_assert(hid);
    free(hid);
}

bool hid_keyboard_type_char(HidKeyboard* hid, char c) {
    UNUSED(hid);

    uint16_t key = 0;
    bool shift = false;

    if(c == '\n' || c == '\r') {
        key = HID_KEYBOARD_RETURN;
    } else if(c == '\t') {
        key = HID_KEYBOARD_TAB;
    } else if(c >= 0x20 && c <= 0x7E) {
        const AsciiHidEntry* entry = &ascii_hid_map[c - 0x20];
        key = entry->hid_key;
        shift = entry->shift;
    } else {
        /* Unsupported character — skip */
        return false;
    }

    if(shift) {
        furi_hal_hid_kb_press(HID_KEYBOARD_L_SHIFT);
    }
    furi_hal_hid_kb_press(key);
    furi_delay_ms(KEY_PRESS_DELAY_MS);
    furi_hal_hid_kb_release(key);
    if(shift) {
        furi_hal_hid_kb_release(HID_KEYBOARD_L_SHIFT);
    }
    furi_delay_ms(INTER_KEY_DELAY_MS);

    return true;
}

uint32_t hid_keyboard_type_string(
    HidKeyboard* hid,
    const char* text,
    size_t len,
    HidProgressCb progress_cb,
    void* context) {
    uint32_t typed = 0;
    for(size_t i = 0; i < len; i++) {
        hid_keyboard_type_char(hid, text[i]);
        typed++;

        if(progress_cb && (typed % PROGRESS_INTERVAL == 0)) {
            progress_cb(typed, (uint32_t)len, context);
        }
    }

    /* Final progress callback */
    if(progress_cb) {
        progress_cb(typed, (uint32_t)len, context);
    }

    return typed;
}

void hid_keyboard_release_all(void) {
    furi_hal_hid_kb_release_all();
}
