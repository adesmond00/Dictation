#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct HidKeyboard HidKeyboard;

/* Allocate and free */
HidKeyboard* hid_keyboard_alloc(void);
void hid_keyboard_free(HidKeyboard* hid);

/* Type a single ASCII character as a HID keystroke */
bool hid_keyboard_type_char(HidKeyboard* hid, char c);

/* Type a string of characters with inter-key delay.
 * progress_cb is called every 32 chars with (typed, total).
 * Returns number of characters successfully typed. */
typedef void (*HidProgressCb)(uint32_t typed, uint32_t total, void* context);
uint32_t hid_keyboard_type_string(
    HidKeyboard* hid,
    const char* text,
    size_t len,
    HidProgressCb progress_cb,
    void* context);

/* Release all keys (safety — call on exit/error) */
void hid_keyboard_release_all(void);
