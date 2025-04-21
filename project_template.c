#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <sys/time.h>

// Display libraries
#include <hagl_hal.h>
#include <hagl.h>
#include <fps.h>
#include <font6x9.h>

// Buttons
#include "buttons.h"

// Colors
#include "colors.h"

// fps counter
volatile bool fps_flag = false;
static fps_instance_t fps;
static const uint64_t US_PER_FRAME_60_FPS = 1000000 / 60;
static const uint64_t US_PER_FRAME_30_FPS = 1000000 / 30;

// message
wchar_t message[32];

// display
static hagl_backend_t *display;

bool show_timer_callback(struct repeating_timer *t)
{
    fps_flag = true;
    return true;
}

void static inline show_fps()
{
    hagl_color_t green = hagl_color(display, 0, 255, 0);

    fps_flag = 0;

    /* Set clip window to full screen so we can display the messages. */
    hagl_set_clip(display, 0, 0, display->width - 1, display->height - 1);

    /* Print the message on lower left corner. */
    swprintf(message, sizeof(message), L"%.*f FPS  ", 0, fps.current);
    hagl_put_text(display, message, 4, display->height - 14, green, font6x9);

    /* Set clip window back to smaller so effects do not mess the messages. */
    hagl_set_clip(display, 0, 20, display->width - 1, display->height - 21);
}

int main() {
    // Timer for the FPS counter
    struct repeating_timer show_timer;
    // Init I/O
    stdio_init_all();
    // Init FPS
    fps_init(&fps);
    // Init display
    display = hagl_init();
    // Clear the display
    hagl_clear(display);
    /* Update displayed FPS counter every 250 ms. */
    add_repeating_timer_ms(250, show_timer_callback, NULL, &show_timer);
    // Init buttons
    buttons_init();

    while(true) {

        // ################################################# Game logic ##############################################################
        uint64_t start = time_us_64();
        hagl_flush(display);
        busy_wait_until(start + 1000000);
        
        while (true) {
            // Game logic
            /*
                TODO
            */
        
            // Update the displayed fps if requested
            if (fps_flag) {
                show_fps();
            }

            // Flush back buffer contents to display
            hagl_flush(display);

            // Update FPS
            fps_update(&fps);

            // Cap to 60 fps
            busy_wait_until(start + US_PER_FRAME_60_FPS);
        }

        start = time_us_64();
        busy_wait_until(start + 500000);
    }

    return 0;
}