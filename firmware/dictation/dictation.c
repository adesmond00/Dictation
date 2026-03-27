#include "dictation.h"
#include "ble_handler.h"
#include "hid_keyboard.h"
#include "typing_engine.h"
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

// Application instance
static DictationApp app = {
    .state = APP_STATE_IDLE,
    .ble_connected = false,
    .usb_connected = false
};

// Main application entry point
int32_t dictation_app(void* p) {
    UNUSED(p);
    
    // Initialize all subsystems
    if(dictation_app_init() != 0) {
        FURI_LOG_E("Dictation", "Failed to initialize application");
        return -1;
    }
    
    // Allocate viewport for UI
    ViewPort* vp = view_port_alloc();
    
    // Set up render callback (simple status display)
    view_port_draw_callback_set(vp, dictation_render_callback, &app);
    
    // Set up input callback
    view_port_input_callback_set(vp, dictation_input_callback, &app);
    
    // Register viewport with GUI
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, vp, GuiLayerFullscreen);
    
    // Main event loop
    FuriEventLoop* event_loop = furi_event_loop_get();
    
    // Start BLE advertising
    ble_handler_init();
    
    // Initialize HID keyboard
    hid_keyboard_init();
    
    // Initialize typing engine
    typing_engine_init();
    
    // Main loop
    while(app.state != APP_STATE_IDLE || !furi_event_loop_is_running(event_loop)) {
        // Process typing engine tick
        if(typing_engine_is_running()) {
            // Check if delay has elapsed and process next character
            // In actual implementation this would be timer-driven
            static uint32_t last_tick = 0;
            uint32_t now = furi_get_tick();
            
            if(now - last_tick >= typing_engine_get_delay_ms()) {
                typing_engine_process_next();
                last_tick = now;
            }
        }
        
        // Update display
        view_port_update(vp);
        
        // Small delay to prevent busy waiting
        furi_delay_ms(10);
    }
    
    // Cleanup
    gui_remove_view_port(gui, vp);
    view_port_free(vp);
    furi_record_close("gui");
    
    dictation_app_cleanup();
    
    return 0;
}

// Initialize the dictation application
int32_t dictation_app_init(void) {
    app.state = APP_STATE_IDLE;
    app.ble_connected = false;
    app.usb_connected = false;
    
    FURI_LOG_I("Dictation", "Dictation app initialized");
    return 0;
}

// Cleanup the application
void dictation_app_cleanup(void) {
    // Stop any ongoing typing
    typing_engine_stop();
    
    // Deinitialize subsystems
    typing_engine_deinit();
    hid_keyboard_deinit();
    ble_handler_deinit();
    
    app.state = APP_STATE_IDLE;
    
    FURI_LOG_I("Dictation", "Dictation app cleanup complete");
}

// Get current application state
AppState dictation_app_get_state(void) {
    return app.state;
}

// Render callback for status display
static void dictation_render_callback(Canvas* canvas, void* context) {
    DictationApp* app = (DictationApp*)context;
    
    // Clear canvas
    canvas_clear(canvas);
    
    // Draw header
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, "Dictation");
    
    // Draw status
    canvas_set_font(canvas, FontSecondary);
    
    switch(app->state) {
        case APP_STATE_IDLE:
            canvas_draw_str(canvas, 0, 30, "Status: Idle");
            break;
        case APP_STATE_CONNECTED:
            canvas_draw_str(canvas, 0, 30, "Status: Connected");
            break;
        case APP_STATE_TYPING:
            canvas_draw_str(canvas, 0, 30, "Status: Typing...");
            break;
    }
    
    // Draw BLE status
    if(app->ble_connected) {
        canvas_draw_str(canvas, 0, 50, "BLE: Connected");
    } else {
        canvas_draw_str(canvas, 0, 50, "BLE: Disconnected");
    }
    
    // Draw USB status
    if(app->usb_connected) {
        canvas_draw_str(canvas, 0, 62, "USB: Connected");
    } else {
        canvas_draw_str(canvas, 0, 62, "USB: Waiting...");
    }
    
    // Draw typing progress if typing
    if(app->state == APP_STATE_TYPING) {
        uint16_t progress = typing_engine_get_progress();
        canvas_draw_str(canvas, 0, 74, "Progress:");
        char progress_str[16];
        snprintf(progress_str, sizeof(progress_str), "%d%%", progress);
        canvas_draw_str(canvas, 60, 74, progress_str);
    }
    
    // Draw instructions
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 100, "Press BACK to exit");
}

// Input callback for handling button presses
static void dictation_input_callback(InputEvent* event, void* context) {
    DictationApp* app = (DictationApp*)context;
    
    if(event->type == InputTypeShort) {
        switch(event->key) {
            case InputKeyBack:
                app->state = APP_STATE_IDLE;  // Signal exit
                break;
                
            case InputKeyOk:
                // Stop typing if in progress
                if(typing_engine_is_running()) {
                    typing_engine_stop();
                    app->state = APP_STATE_CONNECTED;
                }
                break;
                
            default:
                break;
        }
    }
}