#include "ble_handler.h"
#include "typing_engine.h"
#include <furi.h>
#include <furi_hal_bt.h>
#include <furi_hal_usb.h>
#include <bt.h>
#include <bt_service.h>
#include <rpc_util.h>

// Global state instance
BLEHandlerState ble_state = {
    .is_connected = false,
    .current_speed = SPEED_NORMAL,
    .typing_status = STATUS_IDLE,
    .bytes_received = 0
};

// Text buffer for incoming data
static uint8_t text_buffer[MAX_TEXT_BUFFER];
static uint16_t text_buffer_length = 0;

// Service and characteristic handles (GAP/GATT)
// These would be set up during initialization with actual handles
static uint16_t service_handle = 0;
static uint16_t char_tx_handle = 0;
static uint16_t char_rx_handle = 0;
static uint16_t char_status_handle = 0;
static uint16_t char_config_handle = 0;

// Callback for BLE connection state changes
static void ble_connection_callback(BtStatus status) {
    if(status == BtStatusConnected) {
        ble_state.is_connected = true;
    } else if(status == BtStatusDisconnected) {
        ble_state.is_connected = false;
        ble_state.typing_status = STATUS_IDLE;
        // Stop any ongoing typing when disconnected
        typing_engine_stop();
    }
}

// Initialize BLE peripheral
void ble_handler_init(void) {
    // Initialize text buffer
    text_buffer_length = 0;
    
    // Reset state
    ble_state.is_connected = false;
    ble_state.current_speed = SPEED_NORMAL;
    ble_state.typing_status = STATUS_IDLE;
    ble_state.bytes_received = 0;
    
    // Register connection callback
    // Note: In actual Unleashed firmware, this would register with the BT stack
    // The exact API depends on the Unleashed firmware version
    
    FURI_LOG_I("Dictation", "BLE Handler initialized");
}

// Deinitialize BLE peripheral
void ble_handler_deinit(void) {
    // Stop any ongoing operations
    typing_engine_stop();
    ble_state.is_connected = false;
    ble_state.typing_status = STATUS_IDLE;
    
    FURI_LOG_I("Dictation", "BLE Handler deinitialized");
}

// Send status notification to central
void ble_send_status(uint8_t status) {
    ble_state.typing_status = status;
    
    // Prepare status data
    uint8_t status_data[2];
    status_data[0] = status;
    status_data[1] = (uint8_t)(typing_engine_get_progress() & 0xFF); // Progress byte
    
    // In actual implementation, this would send via GATT notification
    // through the status characteristic
    FURI_LOG_D("Dictation", "Sent status: %02X, progress: %d", status, status_data[1]);
}

// Send indication to central
void ble_send_indication(const uint8_t* data, uint16_t length) {
    if(!ble_state.is_connected) {
        return;
    }
    
    // In actual implementation, this would send via GATT indication
    // through the RX characteristic
    FURI_LOG_D("Dictation", "Sent indication, length: %d", length);
}

// Process received command from central
static void process_command(const uint8_t* data, uint16_t length) {
    if(length < 1) {
        return;
    }
    
    uint8_t command = data[0];
    
    switch(command) {
        case CMD_START_TYPING:
            if(length >= 2) {
                uint8_t speed = data[1];
                ble_set_speed(speed);
                
                // Get text data (everything after command and speed byte)
                uint16_t text_len = length - 2;
                if(text_len > 0 && text_len <= MAX_TEXT_BUFFER) {
                    // Copy text to buffer
                    memcpy(text_buffer, &data[2], text_len);
                    text_buffer_length = text_len;
                    
                    // Start typing
                    typing_engine_start(text_buffer, text_buffer_length);
                    ble_send_status(STATUS_TYPING);
                }
            }
            FURI_LOG_I("Dictation", "Start typing command received");
            break;
            
        case CMD_STOP_TYPING:
            typing_engine_stop();
            text_buffer_length = 0;
            ble_send_status(STATUS_STOPPED);
            FURI_LOG_I("Dictation", "Stop typing command received");
            break;
            
        case CMD_CHANGE_SPEED:
            if(length >= 2) {
                uint8_t speed = data[1];
                ble_set_speed(speed);
                typing_engine_set_speed(ble_state.current_speed);
                
                // Acknowledge with indication
                uint8_t ack[2] = {CMD_CHANGE_SPEED, speed};
                ble_send_indication(ack, 2);
            }
            FURI_LOG_I("Dictation", "Change speed command received: %d", data[1]);
            break;
            
        case CMD_GET_STATUS:
            ble_send_status(ble_state.typing_status);
            FURI_LOG_I("Dictation", "Get status command received");
            break;
            
        default:
            FURI_LOG_W("Dictation", "Unknown command: %02X", command);
            ble_send_status(STATUS_ERROR);
            break;
    }
}

// Handle write command from central
void ble_handle_write(const uint8_t* data, uint16_t length) {
    if(length == 0 || data == NULL) {
        return;
    }
    
    ble_state.bytes_received += length;
    process_command(data, length);
}

// Handle write without response from central
void ble_handle_write_no_response(const uint8_t* data, uint16_t length) {
    if(length == 0 || data == NULL) {
        return;
    }
    
    ble_state.bytes_received += length;
    process_command(data, length);
}

// Get current connection state
bool ble_is_connected(void) {
    return ble_state.is_connected;
}

// Get current typing status
uint8_t ble_get_typing_status(void) {
    return ble_state.typing_status;
}

// Set typing status
void ble_set_typing_status(uint8_t status) {
    ble_state.typing_status = status;
}

// Get current speed setting
uint8_t ble_get_speed(void) {
    return ble_state.current_speed;
}

// Set speed setting
void ble_set_speed(uint8_t speed) {
    // Validate speed value (1-4 for presets)
    if(speed >= 1 && speed <= 4) {
        ble_state.current_speed = speed;
        FURI_LOG_I("Dictation", "Speed set to: %d", speed);
    }
}

// Get delay in milliseconds for current speed
uint16_t ble_get_speed_delay_ms(void) {
    switch(ble_state.current_speed) {
        case 1: return SPEED_SLOW;
        case 2: return SPEED_NORMAL;
        case 3: return SPEED_FAST;
        case 4: return SPEED_TURBO;
        default: return SPEED_NORMAL;
    }
}