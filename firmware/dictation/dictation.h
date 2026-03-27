#pragma once

#include <stdint.h>

// Application state
typedef enum {
    APP_STATE_IDLE = 0,
    APP_STATE_CONNECTED = 1,
    APP_STATE_TYPING = 2
} AppState;

// Application instance structure
typedef struct {
    AppState state;
    bool ble_connected;
    bool usb_connected;
} DictationApp;

// Initialize the dictation application
int32_t dictation_app_init(void);

// Main application entry point (called by flipper)
int32_t dictation_app(void* p);

// Cleanup and exit the application
void dictation_app_cleanup(void);

// Get current application state
AppState dictation_app_get_state(void);