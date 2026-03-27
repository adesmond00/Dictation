#pragma once

#include <stdint.h>
#include <stdbool.h>

// Device UUID (Peripheral)
#define BLE_DEVICE_UUID "E1C27A89-BF45-7A7F-0AE8-07E4B4F9D289"

// Custom Service UUID
#define BLE_SERVICE_UUID "8FE5B3D5-2E7F-4A98-2A48-7ACC60FE0000"

// Characteristic UUIDs
#define BLE_CHAR_TX_UUID "19ED82AE-ED21-4C9D-4145-228E62FE0000"   // Write, Write Without Response, Read
#define BLE_CHAR_RX_UUID "19ED82AE-ED21-4C9D-4145-228E61FE0000"   // Indicate, Read
#define BLE_CHAR_STATUS_UUID "19ED82AE-ED21-4C9D-4145-228E63FE0000" // Notify, Read
#define BLE_CHAR_CONFIG_UUID "19ED82AE-ED21-4C9D-4145-228E64FE0000" // Notify, Read, Write

// Command definitions (App -> Flipper)
#define CMD_START_TYPING 0x01
#define CMD_STOP_TYPING 0x02
#define CMD_CHANGE_SPEED 0x03
#define CMD_GET_STATUS 0x04

// Status definitions (Flipper -> App)
#define STATUS_IDLE 0x00
#define STATUS_TYPING 0x01
#define STATUS_COMPLETED 0x02
#define STATUS_STOPPED 0x03
#define STATUS_ERROR 0xFF

// Speed presets (delay per character in ms)
#define SPEED_SLOW 150
#define SPEED_NORMAL 75
#define SPEED_FAST 30
#define SPEED_TURBO 10

// Maximum text buffer size
#define MAX_TEXT_BUFFER 1024

// BLE Handler state
typedef struct {
    bool is_connected;
    uint8_t current_speed;
    uint8_t typing_status;
    uint16_t bytes_received;
} BLEHandlerState;

// Global state instance
extern BLEHandlerState ble_state;

// Initialize BLE peripheral
void ble_handler_init(void);

// Deinitialize BLE peripheral
void ble_handler_deinit(void);

// Send status notification to central
void ble_send_status(uint8_t status);

// Send indication to central
void ble_send_indication(const uint8_t* data, uint16_t length);

// Handle write command from central
void ble_handle_write(const uint8_t* data, uint16_t length);

// Handle write without response from central
void ble_handle_write_no_response(const uint8_t* data, uint16_t length);

// Get current connection state
bool ble_is_connected(void);

// Get current typing status
uint8_t ble_get_typing_status(void);

// Set typing status
void ble_set_typing_status(uint8_t status);

// Get current speed setting
uint8_t ble_get_speed(void);

// Set speed setting
void ble_set_speed(uint8_t speed);