#include "typing_engine.h"
#include "hid_keyboard.h"
#include "ble_handler.h"
#include <furi.h>
#include <string.h>

// Text buffer for typing
static uint8_t text_buffer[TYPING_BUFFER_SIZE];
static uint16_t text_buffer_length = 0;

// Global typing engine state
TypingEngineConfig typing_config = {
    .delay_ms = 75,      // Default to normal speed
    .stop_requested = false,
    .state = TYPING_STATE_IDLE,
    .current_index = 0,
    .total_length = 0
};

// Timer handle for typing tick
// static FuriTimer* typing_timer = NULL;

// Initialize typing engine
void typing_engine_init(void) {
    text_buffer_length = 0;
    typing_config.delay_ms = SPEED_NORMAL;
    typing_config.stop_requested = false;
    typing_config.state = TYPING_STATE_IDLE;
    typing_config.current_index = 0;
    typing_config.total_length = 0;
    
    FURI_LOG_I("Dictation", "Typing Engine initialized");
}

// Deinitialize typing engine
void typing_engine_deinit(void) {
    typing_engine_stop();
    FURI_LOG_I("Dictation", "Typing Engine deinitialized");
}

// Start typing text from buffer
bool typing_engine_start(const uint8_t* text, uint16_t length) {
    if(text == NULL || length == 0 || length > TYPING_BUFFER_SIZE) {
        FURI_LOG_E("Dictation", "Invalid text buffer for typing");
        return false;
    }
    
    if(typing_config.state == TYPING_STATE_RUNNING) {
        FURI_LOG_W("Dictation", "Typing already in progress");
        return false;
    }
    
    // Copy text to buffer
    memcpy(text_buffer, text, length);
    text_buffer_length = length;
    
    // Reset state
    typing_config.current_index = 0;
    typing_config.total_length = length;
    typing_config.stop_requested = false;
    typing_config.state = TYPING_STATE_RUNNING;
    
    FURI_LOG_I("Dictation", "Typing started: %d characters", length);
    
    // Start the typing loop
    // In actual implementation, this would start a timer or fiber
    // that calls typing_engine_process_next() at regular intervals
    
    return true;
}

// Stop typing immediately and clear buffer
void typing_engine_stop(void) {
    if(typing_config.state != TYPING_STATE_IDLE) {
        // Request stop
        typing_config.stop_requested = true;
        
        // Wait for state machine to process stop
        // (in interrupt context this would be handled differently)
        
        FURI_LOG_I("Dictation", "Typing stop requested");
    }
    
    // Clear buffer
    text_buffer_length = 0;
    typing_config.current_index = 0;
    typing_config.total_length = 0;
    typing_config.state = TYPING_STATE_IDLE;
    
    // Release any held keys
    hid_keyboard_release_all();
    
    // Update BLE status
    ble_set_typing_status(STATUS_STOPPED);
}

// Pause typing
void typing_engine_pause(void) {
    if(typing_config.state == TYPING_STATE_RUNNING) {
        typing_config.state = TYPING_STATE_PAUSED;
        FURI_LOG_I("Dictation", "Typing paused");
    }
}

// Resume typing
void typing_engine_resume(void) {
    if(typing_config.state == TYPING_STATE_PAUSED) {
        typing_config.state = TYPING_STATE_RUNNING;
        FURI_LOG_I("Dictation", "Typing resumed");
    }
}

// Set typing speed
void typing_engine_set_speed(uint8_t speed_preset) {
    switch(speed_preset) {
        case SPEED_PRESET_SLOW:
            typing_config.delay_ms = SPEED_SLOW;
            break;
        case SPEED_PRESET_NORMAL:
            typing_config.delay_ms = SPEED_NORMAL;
            break;
        case SPEED_PRESET_FAST:
            typing_config.delay_ms = SPEED_FAST;
            break;
        case SPEED_PRESET_TURBO:
            typing_config.delay_ms = SPEED_TURBO;
            break;
        default:
            typing_config.delay_ms = SPEED_NORMAL;
            break;
    }
    
    FURI_LOG_I("Dictation", "Speed set to %d ms/char", typing_config.delay_ms);
}

// Get current progress
uint16_t typing_engine_get_progress(void) {
    if(typing_config.total_length == 0) {
        return 0;
    }
    return (typing_config.current_index * 100) / typing_config.total_length;
}

// Get current state
TypingState typing_engine_get_state(void) {
    return typing_config.state;
}

// Check if typing is in progress
bool typing_engine_is_running(void) {
    return (typing_config.state == TYPING_STATE_RUNNING);
}

// Check if stop has been requested
bool typing_engine_is_stop_requested(void) {
    return typing_config.stop_requested;
}

// Clear the stop request flag
void typing_engine_clear_stop_request(void) {
    typing_config.stop_requested = false;
}

// Process one character (called by timer/fiber)
bool typing_engine_process_next(void) {
    uint8_t keycode;
    uint8_t modifier;
    
    // Check for stop request
    if(typing_config.stop_requested) {
        typing_engine_stop();
        return false;
    }
    
    // Check if we're done
    if(typing_config.current_index >= typing_config.total_length) {
        typing_config.state = TYPING_STATE_IDLE;
        hid_keyboard_release_all();
        
        // Send completion status
        ble_set_typing_status(STATUS_COMPLETED);
        
        FURI_LOG_I("Dictation", "Typing completed");
        return false;
    }
    
    // Get current character
    uint8_t ch = text_buffer[typing_config.current_index];
    
    // Convert to HID keycode
    if(hid_ascii_to_keycode(ch, &keycode, &modifier)) {
        // Send the key
        if(modifier != 0) {
            hid_keyboard_type_key_with_modifier(modifier, keycode);
        } else {
            hid_keyboard_type_key(keycode);
        }
        
        typing_config.current_index++;
        
        // Send progress update via BLE (every 10 characters or at end)
        if((typing_config.current_index % 10 == 0) || 
           (typing_config.current_index >= typing_config.total_length)) {
            ble_send_status(STATUS_TYPING);
        }
    } else {
        // Unsupported character - skip it but log
        FURI_LOG_W("Dictation", "Unsupported character: 0x%02X at index %d", 
                   ch, typing_config.current_index);
        typing_config.current_index++;
    }
    
    return true;
}

// Get the configured delay in milliseconds
uint16_t typing_engine_get_delay_ms(void) {
    return typing_config.delay_ms;
}