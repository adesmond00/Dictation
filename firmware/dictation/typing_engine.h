#pragma once

#include <stdint.h>
#include <stdbool.h>

// Maximum characters in typing buffer
#define TYPING_BUFFER_SIZE 1024

// Speed presets
typedef enum {
    SPEED_PRESET_SLOW = 1,
    SPEED_PRESET_NORMAL = 2,
    SPEED_PRESET_FAST = 3,
    SPEED_PRESET_TURBO = 4
} SpeedPreset;

// Typing engine states
typedef enum {
    TYPING_STATE_IDLE = 0,
    TYPING_STATE_RUNNING = 1,
    TYPING_STATE_PAUSED = 2,
    TYPING_STATE_STOPPING = 3
} TypingState;

// Typing engine configuration
typedef struct {
    uint16_t delay_ms;           // Delay between characters
    bool stop_requested;        // Stop flag
    TypingState state;          // Current state
    uint16_t current_index;     // Current position in buffer
    uint16_t total_length;      // Total characters to type
} TypingEngineConfig;

// Global typing engine state
extern TypingEngineConfig typing_config;

// Initialize typing engine
void typing_engine_init(void);

// Deinitialize typing engine
void typing_engine_deinit(void);

// Start typing text from buffer
// Returns true if started successfully
bool typing_engine_start(const uint8_t* text, uint16_t length);

// Stop typing immediately and clear buffer
void typing_engine_stop(void);

// Pause typing (not implemented yet - reserved for future)
void typing_engine_pause(void);

// Resume typing after pause
void typing_engine_resume(void);

// Set typing speed
void typing_engine_set_speed(uint8_t speed_preset);

// Get current progress (characters typed / total)
uint16_t typing_engine_get_progress(void);

// Get current state
TypingState typing_engine_get_state(void);

// Check if typing is in progress
bool typing_engine_is_running(void);

// Process one character (called by timer/fiber)
// Returns true if character was processed
bool typing_engine_process_next(void);

// Check if stop has been requested
bool typing_engine_is_stop_requested(void);

// Clear the stop request flag
void typing_engine_clear_stop_request(void);