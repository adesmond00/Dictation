#include "hid_keyboard.h"
#include <furi.h>

// Global state instance
HIDHandlerState hid_state = {
    .is_initialized = false,
    .is_connected = false,
    .last_report = {0}
};

// Initialize USB HID keyboard
void hid_keyboard_init(void) {
    // Initialize state
    hid_state.is_initialized = false;
    hid_state.is_connected = false;
    memset(&hid_state.last_report, 0, sizeof(HID_KeyboardReport));
    
    // In actual implementation:
    // 1. Initialize USB HAL
    // 2. Register HID report descriptor
    // 3. Start USB device mode
    // 4. Wait for host enumeration
    
    hid_state.is_initialized = true;
    FURI_LOG_I("Dictation", "HID Keyboard initialized");
}

// Deinitialize USB HID keyboard
void hid_keyboard_deinit(void) {
    hid_keyboard_release_all();
    hid_state.is_initialized = false;
    hid_state.is_connected = false;
    FURI_LOG_I("Dictation", "HID Keyboard deinitialized");
}

// Send a single key press and release
void hid_keyboard_type_key(uint8_t keycode) {
    HID_KeyboardReport report = {
        .modifier = 0,
        .reserved = 0,
        .keycode = {keycode, 0, 0, 0, 0, 0}
    };
    
    // Send press report
    hid_keyboard_send_report(&report);
    
    // Small delay for host to register press
    delay_us(1000);
    
    // Send release report
    hid_keyboard_release_all();
}

// Send a key press with modifiers
void hid_keyboard_type_key_with_modifier(uint8_t modifier, uint8_t keycode) {
    HID_KeyboardReport report = {
        .modifier = modifier,
        .reserved = 0,
        .keycode = {keycode, 0, 0, 0, 0, 0}
    };
    
    // Send press report with modifier
    hid_keyboard_send_report(&report);
    
    // Small delay for host to register press
    delay_us(1000);
    
    // Send release report
    hid_keyboard_release_all();
}

// Send raw keyboard report
void hid_keyboard_send_report(const HID_KeyboardReport* report) {
    if(!hid_state.is_initialized) {
        return;
    }
    
    // Store last report
    memcpy(&hid_state.last_report, report, sizeof(HID_KeyboardReport));
    
    // In actual implementation, this would:
    // 1. Wait for USB ready state
    // 2. Send report via USB IN endpoint
    // 3. Handle any transmission errors
    
    // Placeholder for actual USB HID transfer
    // usb_hid_send_report(HID_REPORT_ID_KEYBOARD, (uint8_t*)report, sizeof(HID_KeyboardReport));
}

// Release all keys
void hid_keyboard_release_all(void) {
    HID_KeyboardReport report = {
        .modifier = 0,
        .reserved = 0,
        .keycode = {0, 0, 0, 0, 0, 0}
    };
    
    hid_keyboard_send_report(&report);
}

// Check if USB is connected
bool hid_keyboard_is_connected(void) {
    // In actual implementation, check USB connection state
    // return usb_hal_get_connection_state();
    return hid_state.is_connected;
}

// Convert ASCII character to HID keycode
bool hid_ascii_to_keycode(uint8_t ascii, uint8_t* keycode, uint8_t* modifier) {
    *modifier = 0;
    
    // Handle uppercase A-Z
    if(ascii >= 'A' && ascii <= 'Z') {
        *keycode = HID_KEY_A + (ascii - 'A');
        return true;
    }
    
    // Handle lowercase a-z (convert to uppercase)
    if(ascii >= 'a' && ascii <= 'z') {
        *keycode = HID_KEY_A + (ascii - 'a');
        return true;
    }
    
    // Handle numbers 1-9, 0
    if(ascii >= '1' && ascii <= '9') {
        *keycode = HID_KEY_1 + (ascii - '1');
        return true;
    }
    if(ascii == '0') {
        *keycode = HID_KEY_0;
        return true;
    }
    
    // Handle special characters
    switch(ascii) {
        case ' ':
            *keycode = HID_KEY_SPACE;
            return true;
            
        case '\n':
        case '\r':
            *keycode = HID_KEY_ENTER;
            return true;
            
        case '\t':
            *keycode = HID_KEY_TAB;
            return true;
            
        case '\b':
        case 0x7F: // DEL
            *keycode = HID_KEY_BACKSPACE;
            return true;
            
        case '!':
            *modifier = HID_MODIFIER_LEFT_SHIFT;
            *keycode = HID_KEY_1;
            return true;
            
        case '@':
            *modifier = HID_MODIFIER_LEFT_SHIFT;
            *keycode = HID_KEY_2;
            return true;
            
        case '#':
            *modifier = HID_MODIFIER_LEFT_SHIFT;
            *keycode = HID_KEY_3;
            return true;
            
        case '$':
            *modifier = HID_MODIFIER_LEFT_SHIFT;
            *keycode = HID_KEY_4;
            return true;
            
        case '%':
            *modifier = HID_MODIFIER_LEFT_SHIFT;
            *keycode = HID_KEY_5;
            return true;
            
        case '^':
            *modifier = HID_MODIFIER_LEFT_SHIFT;
            *keycode = HID_KEY_6;
            return true;
            
        case '&':
            *modifier = HID_MODIFIER_LEFT_SHIFT;
            *keycode = HID_KEY_7;
            return true;
            
        case '*':
            *modifier = HID_MODIFIER_LEFT_SHIFT;
            *keycode = HID_KEY_8;
            return true;
            
        case '(':
            *modifier = HID_MODIFIER_LEFT_SHIFT;
            *keycode = HID_KEY_9;
            return true;
            
        case ')':
            *modifier = HID_MODIFIER_LEFT_SHIFT;
            *keycode = HID_KEY_0;
            return true;
            
        case '-':
        case '_':
            *keycode = HID_KEY_MINUS;
            if(ascii == '_') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        case '=':
        case '+':
            *keycode = HID_KEY_EQUAL;
            if(ascii == '+') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        case '[':
        case '{':
            *keycode = HID_KEY_BRACKET_LEFT;
            if(ascii == '{') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        case ']':
        case '}':
            *keycode = HID_KEY_BRACKET_RIGHT;
            if(ascii == '}') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        case '\\':
        case '|':
            *keycode = HID_KEY_BACKSLASH;
            if(ascii == '|') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        case ';':
        case ':':
            *keycode = HID_KEY_SEMICOLON;
            if(ascii == ':') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        case '\'':
        case '"':
            *keycode = HID_KEY_APOSTROPHE;
            if(ascii == '"') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        case '`':
        case '~':
            *keycode = HID_KEY_GRAVE;
            if(ascii == '~') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        case ',':
        case '<':
            *keycode = HID_KEY_COMMA;
            if(ascii == '<') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        case '.':
        case '>':
            *keycode = HID_KEY_PERIOD;
            if(ascii == '>') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        case '/':
        case '?':
            *keycode = HID_KEY_SLASH;
            if(ascii == '?') *modifier = HID_MODIFIER_LEFT_SHIFT;
            return true;
            
        default:
            return false;
    }
}