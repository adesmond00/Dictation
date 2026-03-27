#pragma once

#include <stdint.h>
#include <stdbool.h>

// USB HID Keyboard Report Descriptor constants
#define HID_REPORT_ID_KEYBOARD 1

// HID keycodes for USB Keyboard
#define HID_KEY_RESERVED 0x00
#define HID_KEY_ERROR_ROLLOVER 0x01
#define HID_KEY_POST_FAIL 0x02
#define HID_KEY_ERROR_UNDEFINED 0x03

// Letters A-Z (0x04-0x1D)
#define HID_KEY_A 0x04
#define HID_KEY_B 0x05
#define HID_KEY_C 0x06
#define HID_KEY_D 0x07
#define HID_KEY_E 0x08
#define HID_KEY_F 0x09
#define HID_KEY_G 0x0A
#define HID_KEY_H 0x0B
#define HID_KEY_I 0x0C
#define HID_KEY_J 0x0D
#define HID_KEY_K 0x0E
#define HID_KEY_L 0x0F
#define HID_KEY_M 0x10
#define HID_KEY_N 0x11
#define HID_KEY_O 0x12
#define HID_KEY_P 0x13
#define HID_KEY_Q 0x14
#define HID_KEY_R 0x15
#define HID_KEY_S 0x16
#define HID_KEY_T 0x17
#define HID_KEY_U 0x18
#define HID_KEY_V 0x19
#define HID_KEY_W 0x1A
#define HID_KEY_X 0x1B
#define HID_KEY_Y 0x1C
#define HID_KEY_Z 0x1D

// Numbers 1-0 (0x1E-0x27)
#define HID_KEY_1 0x1E
#define HID_KEY_2 0x1F
#define HID_KEY_3 0x20
#define HID_KEY_4 0x21
#define HID_KEY_5 0x22
#define HID_KEY_6 0x23
#define HID_KEY_7 0x24
#define HID_KEY_8 0x25
#define HID_KEY_9 0x26
#define HID_KEY_0 0x27

// Special keys
#define HID_KEY_ENTER 0x28
#define HID_KEY_ESC 0x29
#define HID_KEY_BACKSPACE 0x2A
#define HID_KEY_TAB 0x2B
#define HID_KEY_SPACE 0x2C
#define HID_KEY_MINUS 0x2D
#define HID_KEY_EQUAL 0x2E
#define HID_KEY_BRACKET_LEFT 0x2F
#define HID_KEY_BRACKET_RIGHT 0x30
#define HID_KEY_BACKSLASH 0x31
#define HID_KEY_SEMICOLON 0x33
#define HID_KEY_APOSTROPHE 0x34
#define HID_KEY_GRAVE 0x35
#define HID_KEY_COMMA 0x36
#define HID_KEY_PERIOD 0x37
#define HID_KEY_SLASH 0x38
#define HID_KEY_CAPS_LOCK 0x39

// Function keys F1-F12 (0x3A-0x45)
#define HID_KEY_F1 0x3A
#define HID_KEY_F2 0x3B
#define HID_KEY_F3 0x3C
#define HID_KEY_F4 0x3D
#define HID_KEY_F5 0x3E
#define HID_KEY_F6 0x3F
#define HID_KEY_F7 0x40
#define HID_KEY_F8 0x41
#define HID_KEY_F9 0x42
#define HID_KEY_F10 0x43
#define HID_KEY_F11 0x44
#define HID_KEY_F12 0x45

// Modifier keys
#define HID_MODIFIER_LEFT_CTRL 0x01
#define HID_MODIFIER_LEFT_SHIFT 0x02
#define HID_MODIFIER_LEFT_ALT 0x04
#define HID_MODIFIER_LEFT_GUI 0x08
#define HID_MODIFIER_RIGHT_CTRL 0x10
#define HID_MODIFIER_RIGHT_SHIFT 0x20
#define HID_MODIFIER_RIGHT_ALT 0x40
#define HID_MODIFIER_RIGHT_GUI 0x80

// HID Keyboard report structure (8 bytes)
typedef struct {
    uint8_t modifier;      // Modifier keys
    uint8_t reserved;      // Reserved (always 0)
    uint8_t keycode[6];    // Up to 6 simultaneous keycodes
} HID_KeyboardReport;

// HID Handler state
typedef struct {
    bool is_initialized;
    bool is_connected;
    HID_KeyboardReport last_report;
} HIDHandlerState;

// Global state instance
extern HIDHandlerState hid_state;

// Initialize USB HID keyboard
void hid_keyboard_init(void);

// Deinitialize USB HID keyboard
void hid_keyboard_deinit(void);

// Send a single key press and release
void hid_keyboard_type_key(uint8_t keycode);

// Send a key press with modifiers (e.g., Shift+A)
void hid_keyboard_type_key_with_modifier(uint8_t modifier, uint8_t keycode);

// Send raw keyboard report
void hid_keyboard_send_report(const HID_KeyboardReport* report);

// Release all keys (send empty report)
void hid_keyboard_release_all(void);

// Check if USB is connected
bool hid_keyboard_is_connected(void);

// Convert ASCII character to HID keycode
// Returns true if conversion successful, false if character not supported
bool hid_ascii_to_keycode(uint8_t ascii, uint8_t* keycode, uint8_t* modifier);