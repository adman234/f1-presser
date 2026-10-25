// Presses F1 once shortly after the host enumerates the keyboard, then every
// 5 minutes. A real boot-protocol HID keyboard, so it works in BIOS/UEFI.
#include <stdbool.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "tusb.h"

#define FIRST_PRESS_DELAY_US  (2ull * 1000 * 1000)         // after each enumeration
#define PERIOD_US             (5ull * 60 * 1000 * 1000)    // 5 minutes
#define HOLD_US               (40ull * 1000)               // key held for 40 ms

typedef enum { IDLE, HELD } press_state_t;

static press_state_t state = IDLE;
static bool was_mounted = false;
static uint64_t next_press_us = 0;
static uint64_t release_us = 0;

static void send_keys(uint8_t keycode) {
    uint8_t keys[6] = {keycode, 0, 0, 0, 0, 0};
    // report_id 0: this keyboard has no report ID
    tud_hid_keyboard_report(0, 0, keycode ? keys : NULL);
}

int main(void) {
    tusb_init();

    while (true) {
        tud_task();
        uint64_t now = time_us_64();

        // Restart the schedule every time the host (re)enumerates us. This
        // also covers the BIOS -> OS handoff, where the bus resets.
        bool mounted = tud_mounted();
        if (mounted && !was_mounted) {
            state = IDLE;
            next_press_us = now + FIRST_PRESS_DELAY_US;
        }
        was_mounted = mounted;
        if (!mounted) continue;

        if (state == IDLE && now >= next_press_us && tud_hid_ready()) {
            send_keys(HID_KEY_F1);
            release_us = now + HOLD_US;
            state = HELD;
        } else if (state == HELD && now >= release_us && tud_hid_ready()) {
            send_keys(0);                      // release; retried until it goes out
            state = IDLE;
            next_press_us += PERIOD_US;        // fixed schedule, no drift
            while (next_press_us <= now) {     // skip presses missed during suspend
                next_press_us += PERIOD_US;
            }
        }
    }
}

// Required HID callbacks. We have no use for host-to-device data (LED state).
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                               uint8_t *buffer, uint16_t reqlen) {
    (void)instance; (void)report_id; (void)report_type; (void)buffer; (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t bufsize) {
    (void)instance; (void)report_id; (void)report_type; (void)buffer; (void)bufsize;
}
