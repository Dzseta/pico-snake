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
// Bitmaps
#include "bitmap_icons.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// ############################# QUEUE ###############################
// Defining the Queue structure
typedef struct  {
     uint8_t x[400];
     uint8_t y[400];
     int size;
     int front;
     int rear;
} Queue;
// Function to initialize the queue
void initializeQueue(Queue* q) {
    q->front = 0;
    q->rear = 0;
    q->size = 0;
}
// Function to add an element to the queue
void enqueue(Queue* q, uint8_t x, uint8_t y) {
    q->rear = (q->front + q->size) % 400;
    q->x[q->rear] = x;
    q->y[q->rear] = y;
    q->size++;
}
// Function to remove an element from the queue
void dequeue(Queue* q)
{
    q->front = (q->front + 1) % 400;
    q->size--;
}
// ########################## VARIABLES ###############################

// fps counter
volatile bool fps_flag = false;
static fps_instance_t fps;
static const uint64_t US_PER_FRAME_60_FPS = 1000000 / 60;
static const uint64_t US_PER_FRAME_1_FPS = 1000000;

// message
wchar_t message[32];
// text
wchar_t text[20];

// display
static hagl_backend_t *display;

// board
int board[20][20];

// bitmap
hagl_bitmap_t icon_bitmap;

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
void static place_fruit(uint8_t* x, uint8_t* y){    
    do {
        *x = rand() % 20;
        *y = rand() % 20;
    } while (board[*x][*y] == 1);
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

    // Init bitmap
    icon_bitmap.buffer = (uint8_t *) malloc(ICONS_WIDTH * ICONS_HEIGHT * sizeof(hagl_color_t));

    // Variables
    // direction
    u_int8_t direction; // up-right-down-left (0-3)
    u_int8_t new_direction;
    // ending
    bool end;

    while(true) {
        // ################################################# Game logic ##############################################################
        uint64_t start = time_us_64();
        hagl_flush(display);
        busy_wait_until(start + 1000000);
        
        // initialise variables
        end = false;
        srand(start);
        Queue snake;
        initializeQueue(&snake);
        uint8_t current_x = 4;
        uint8_t current_y = 10;
        enqueue(&snake, current_x, current_y);
        current_x = 5;
        enqueue(&snake, current_x, current_y);
        current_x = 6;
        enqueue(&snake, current_x, current_y);
        direction = RIGHT;
        new_direction = RIGHT;
        uint8_t fruit_x = 0;
        uint8_t fruit_y = 0;
        place_fruit(&fruit_x, &fruit_y);
        for(int i=0; i<20; i++) {
            for(int j=0; j<20; j++) {
                board[i][j] = 0;
            }
        }
        board[4][10] = 1;
        board[5][10] = 1;
        board[6][10] = 1;
        board[fruit_x][fruit_y] = 2;
        uint8_t last_x = 0;
        uint8_t last_y = 0;

        // draw the field
        for(int i=0; i<20; i++) {
            for(int j=0; j<20; j++) {
                if(board[i][j] == 0) hagl_fill_rectangle_xywh(display, i*12, j*12, 12, 12, color_green);
                if(board[i][j] == 1) {
                    hagl_draw_rectangle_xywh(display, i*12, j*12, 20, 20, color_black);
                    hagl_fill_rectangle_xywh(display, i*12, j*12, 12, 12, color_lightblue);
                }
                if(board[i][j] == 2) {
                    hagl_bitmap_init(&icon_bitmap, ICONS_WIDTH, ICONS_HEIGHT, sizeof(hagl_color_t), &apple_icon);
                    hagl_blit(display, i*12, j*12, &icon_bitmap);
                }
            }
        }
        if(direction == LEFT) {
            hagl_bitmap_init(&icon_bitmap, ICONS_WIDTH, ICONS_HEIGHT, sizeof(hagl_color_t), &head_icon_l);
            hagl_blit(display, snake.x[snake.rear]*12, snake.y[snake.rear]*12, &icon_bitmap);
        } else if(direction == UP) {
            hagl_bitmap_init(&icon_bitmap, ICONS_WIDTH, ICONS_HEIGHT, sizeof(hagl_color_t), &head_icon_u);
            hagl_blit(display, snake.x[snake.rear]*12, snake.y[snake.rear]*12, &icon_bitmap);
        } else if(direction == RIGHT) {
            hagl_bitmap_init(&icon_bitmap, ICONS_WIDTH, ICONS_HEIGHT, sizeof(hagl_color_t), &head_icon_r);
            hagl_blit(display, snake.x[snake.rear]*12, snake.y[snake.rear]*12, &icon_bitmap);
        } else {
            hagl_bitmap_init(&icon_bitmap, ICONS_WIDTH, ICONS_HEIGHT, sizeof(hagl_color_t), &head_icon_d);
            hagl_blit(display, snake.x[snake.rear]*12, snake.y[snake.rear]*12, &icon_bitmap);
        }
        
        // timer
        uint16_t snake_timer = 0;

        while (true) {
            // Game logic
            // start frame timer
            uint64_t start = time_us_64();
            snake_timer++;
            // read input
            if(end) {
                if(!gpio_get(KEY_Y)) {
                    if(end) break;
                }
            } else {
                if(!gpio_get(JOY_UP)) {
                    if(direction != DOWN) new_direction = UP;
                } else if(!gpio_get(JOY_DOWN)) {
                    if(direction != UP) new_direction = DOWN;
                } else if(!gpio_get(JOY_LEFT)) {
                    if(direction != RIGHT) new_direction = LEFT;
                } else if(!gpio_get(JOY_RIGHT)) {
                    if(direction != LEFT) new_direction = RIGHT;
                }
            }

            // move
            if(snake_timer >= 20 && !end) {
                direction = new_direction;
                // get new direction
                if(direction == LEFT && current_x > 0) {
                    current_x--;
                } else if(direction == UP && current_y > 0) {
                    current_y--;
                } else if(direction == RIGHT && current_x < 19) {
                    current_x++;
                } else if(direction == DOWN && current_y < 19) {
                    current_y++;
                } else {
                    end = true;
                }
                // check for fruit
                if(board[current_x][current_y] == 2) {
                    place_fruit(&fruit_x, &fruit_y);
                    board[fruit_x][fruit_y] = 2;
                } else {
                    last_x = snake.x[snake.front];
                    last_y = snake.y[snake.front];
                    dequeue(&snake);
                    board[last_x][last_y] = 0;
                }
                // check for tail
                if(board[current_x][current_y] == 1) {
                    end = true;
                }
                enqueue(&snake, current_x, current_y);
                board[current_x][current_y] = 1;
                // check if board full
                if(snake.size == 400) {
                    end = true;
                }
                // draw the field
                for(int i=0; i<20; i++) {
                    for(int j=0; j<20; j++) {
                        if(board[i][j] == 0) hagl_fill_rectangle_xywh(display, i*12, j*12, 12, 12, color_green);
                        if(board[i][j] == 1) {
                            hagl_fill_rectangle_xywh(display, i*12, j*12, 12, 12, color_lightblue);
                            hagl_draw_rectangle_xywh(display, i*12, j*12, 12, 12, color_black);
                        }
                        if(board[i][j] == 2) {
                            hagl_bitmap_init(&icon_bitmap, ICONS_WIDTH, ICONS_HEIGHT, sizeof(hagl_color_t), &apple_icon);
                            hagl_blit(display, i*12, j*12, &icon_bitmap);
                        }
                    }
                }
                if(direction == LEFT) {
                    hagl_bitmap_init(&icon_bitmap, ICONS_WIDTH, ICONS_HEIGHT, sizeof(hagl_color_t), &head_icon_l);
                    hagl_blit(display, snake.x[snake.rear]*12, snake.y[snake.rear]*12, &icon_bitmap);
                } else if(direction == UP) {
                    hagl_bitmap_init(&icon_bitmap, ICONS_WIDTH, ICONS_HEIGHT, sizeof(hagl_color_t), &head_icon_u);
                    hagl_blit(display, snake.x[snake.rear]*12, snake.y[snake.rear]*12, &icon_bitmap);
                } else if(direction == RIGHT) {
                    hagl_bitmap_init(&icon_bitmap, ICONS_WIDTH, ICONS_HEIGHT, sizeof(hagl_color_t), &head_icon_r);
                    hagl_blit(display, snake.x[snake.rear]*12, snake.y[snake.rear]*12, &icon_bitmap);
                } else {
                    hagl_bitmap_init(&icon_bitmap, ICONS_WIDTH, ICONS_HEIGHT, sizeof(hagl_color_t), &head_icon_d);
                    hagl_blit(display, snake.x[snake.rear]*12, snake.y[snake.rear]*12, &icon_bitmap);
                }
                if(end) {
                    swprintf(text, sizeof(text), L"GAME OVER");
                    hagl_put_text(display, text, 93, 0, color_red, font6x9);
                    swprintf(text, sizeof(text), L"SCORE: %u", snake.size-3);
                    hagl_put_text(display, text, 93, 9, color_white, font6x9);
                }
                snake_timer = 0;
            }
        
            // Update the displayed fps if requested
            /*
            if (fps_flag) {
                show_fps();
            }*/

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