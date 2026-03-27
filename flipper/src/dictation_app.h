#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_port.h>

/* Forward declarations */
typedef struct BleSerial BleSerial;
typedef struct HidKeyboard HidKeyboard;
typedef struct TextBuffer TextBuffer;

/* Protocol constants */
#define PROTO_SOH 0x01
#define PROTO_EOT 0x04
#define PROTO_ACK 0x06
#define PROTO_NAK 0x15

/* Application states */
typedef enum {
    DictationStateIdle,
    DictationStateConnected,
    DictationStateTyping,
    DictationStateError,
} DictationState;

/* Custom event types for the message queue */
typedef enum {
    EventTypeInput,
    EventTypeBleData,
    EventTypeBleConnected,
    EventTypeBleDisconnected,
    EventTypeTypingDone,
} EventType;

/* Event structure passed through the message queue */
typedef struct {
    EventType type;
    InputEvent input;
} DictationEvent;

/* Main application state */
typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    FuriMutex* mutex;

    BleSerial* ble_serial;
    HidKeyboard* hid_keyboard;
    TextBuffer* text_buffer;

    DictationState state;
    uint32_t chars_typed;
    uint32_t chars_total;
    bool usb_connected;
    bool ble_connected;
} DictationApp;

/* Entry point called by the Flipper firmware */
int32_t dictation_app_entry(void* p);
