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

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// ############################# QUEUE ###############################
typedef struct {
  u_int8_t x;
  u_int8_t y;
} coordinate;
// Defining the Queue structure
typedef struct  {
     coordinate items[400];
     int front;
     int rear;
} Queue;
// Function to initialize the queue
void initializeQueue(Queue* q)
{
    q->front = -1;
    q->rear = 0;
}
// Function to add an element to the queue
void enqueue(Queue* q, coordinate value)
{
    if (isFull(q)) {
        printf("Queue is full\n");
        return;
    }
    q->items[q->rear] = value;
    q->rear++;
}
// Function to remove an element from the queue
void dequeue(Queue* q)
{
    q->front++;
}
// ########################## VARIABLES ###############################

// fps counter
volatile bool fps_flag = false;
static fps_instance_t fps;
static const uint64_t US_PER_FRAME_2_FPS = 1000000 / 2;

// message
wchar_t message[32];
// text
wchar_t text[10];

// display
static hagl_backend_t *display;

// board
int board[20][20];

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

// place the fruit
coordinate static place_fruit(){
    int16_t fruit_x;
    int16_t fruit_y;
    do {
        fruit_x = rand() % 20;
        fruit_y = rand() % 20;
    } while (board[fruit_x][fruit_y] == 1);
    coordinate coords(fruit_x, fruit_y);
    return coords();
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

    // Variables
    // direction
    u_int8_t direction; // up-right-down-left (0-3)
    // buttons
    bool key_a, key_b, joy_up, joy_down, joy_left, joy_right; 
    // ending
    bool end;

    while(true) {
        // ################################################# Game logic ##############################################################
        uint64_t start = time_us_64();
        hagl_flush(display);
        busy_wait_until(start + 1000000);
        
        // initialise variables
        key_a = key_b = joy_up = joy_down = joy_left = joy_right = false;
        end = false;
        srand(start);
        Queue snake;
        initializeQueue(&snake);
        coordinate helper;
        helper.x = 5;
        helper.y = 10;
        enqueue(&snake, helper);
        helper.y = 11;
        enqueue(&snake, helper);
        helper.y = 12;
        enqueue(&snake, helper);
        direction = RIGHT;
        coordinate fruit = place_fruit();
        for(int i=0; i<20; i++) {
            for(int j=0; j<20; j++) {
                board[i][j] = 0;
            }
        }
        board[5][10] = 1;
        board[5][11] = 1;
        board[5][12] = 1;
        board[fruit.x][fruit.y] = 2;

        // draw the field
        for(int i=0; i<20; i++) {
            for(int j=0; j<20; j++) {
                if(board[i][j] == 0) hagl_fill_rectangle_xywh(display, i*20, j*20, 20, 20, color_green);
                if(board[i][j] == 1) hagl_fill_rectangle_xywh(display, i*20, j*20, 20, 20, color_lightblue);
                if(board[i][j] == 2) hagl_fill_rectangle_xywh(display, i*20, j*20, 20, 20, color_red);
                hagl_draw_rectangle_xywh(display, i*20, j*20, 20, 20, color_black);
            }
        }

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
            busy_wait_until(start + US_PER_FRAME_2_FPS);
        }

        start = time_us_64();
        busy_wait_until(start + 500000);
    }

    return 0;
}