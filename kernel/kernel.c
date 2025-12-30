#include <stdint.h>
#include "mouse.h"
#include "keyboard.h"

char* sysver = "noreldev7.0";

typedef struct {
    int x, y;
    int w;
    char buffer[128];
    int len;
    int focused;
} InputField;

void mainapp();

#define MAX_INPUTS 10
static InputField inputs[MAX_INPUTS];
static int input_count = 0;

typedef struct {
    int x, y;
    int w;
    const char *label;
    void (*on_click)();
} Button;

#define MAX_BUTTONS 10
static Button buttons[MAX_BUTTONS];
static int button_count = 0;

static volatile uint16_t *vga = (uint16_t*)0xB8000;

static int mouse_x = 40;
static int mouse_y = 12;

static uint16_t saved_cell = 0;
static int saved_x = 40;
static int saved_y = 12;

#define MOUSE_CHAR '+'
#define MOUSE_COLOR 0x16

void vga_clear(uint8_t color) {
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = ((uint16_t)color << 8) | ' ';
    }
}

void vga_write_at(int x, int y, const char *s, uint8_t color) {
    int i = 0;
    while (s[i]) {
        vga[y * 80 + x + i] = ((uint16_t)color << 8) | s[i];
        i++;
    }
}

void draw_button(int x, int y, const char *label, uint8_t color) {
    int len = 0;
    while (label[len]) len++;
    vga[y * 80 + x] = ((uint16_t)color << 8) | '[';
    for (int i = 0; i < len; i++) {
        vga[y * 80 + x + 1 + i] = ((uint16_t)color << 8) | label[i];
    }
    vga[y * 80 + x + len + 1] = ((uint16_t)color << 8) | ']';
}

void add_button(int x, int y, const char *label, void (*on_click)()) {
    int len = 0;
    while (label[len]) len++;
    buttons[button_count].x = x;
    buttons[button_count].y = y;
    buttons[button_count].w = len + 2;
    buttons[button_count].label = label;
    buttons[button_count].on_click = on_click;
    button_count++;
}

void check_button_click(int mx, int my) {
    for (int i = 0; i < button_count; i++) {
        Button *b = &buttons[i];
        if (my == b->y && mx >= b->x && mx < b->x + b->w) {
            b->on_click();
        }
    }
}

void draw_input(InputField *in) {
    vga_write_at(in->x, in->y, "[", 0x1F);
    for (int i = 0; i < in->w; i++) {
        char c = (i < in->len) ? in->buffer[i] : ' ';
        char s[2] = {c, 0};
        vga_write_at(in->x + 1 + i, in->y, s, 0x1F);
    }
    vga_write_at(in->x + 1 + in->w, in->y, "]", 0x1F);
}

void draw_all_inputs() {
    for (int i = 0; i < input_count; i++) {
        draw_input(&inputs[i]);
    }
}

void add_input(int x, int y, int w) {
    InputField *in = &inputs[input_count++];
    in->x = x;
    in->y = y;
    in->w = w;
    in->len = 0;
    in->focused = 0;
}

void focus_input_at(int mx, int my) {
    for (int i = 0; i < input_count; i++) {
        InputField *in = &inputs[i];
        if (my == in->y && mx >= in->x && mx <= in->x + in->w + 1) {
            for (int j = 0; j < input_count; j++) inputs[j].focused = 0;
            in->focused = 1;
        }
    }
}

void handle_keyboard() {
    char c = keyboard_read();
    if (!c) return;

    for (int i = 0; i < input_count; i++) {
        InputField *in = &inputs[i];
        if (!in->focused) continue;

        if (c == '\b') {
            if (in->len > 0) in->len--;
        } else if (c >= 32 && c <= 126) {
            if (in->len < in->w && in->len < 127) {
                in->buffer[in->len++] = c;
            }
        }
        draw_input(in);
    }
}

void draw_mouse(uint8_t color) {
    saved_x = mouse_x;
    saved_y = mouse_y;
    saved_cell = vga[mouse_y * 80 + mouse_x];
    vga[mouse_y * 80 + mouse_x] = ((uint16_t)color << 8) | MOUSE_CHAR;
}

void clear_mouse() {
    vga[saved_y * 80 + saved_x] = saved_cell;
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void mouse_poll() {
    uint8_t status = inb(0x64);
    if (!(status & 1)) return;
    if (!(status & 0x20)) return;

    uint8_t packet[3];
    packet[0] = mouse_read();
    packet[1] = mouse_read();
    packet[2] = mouse_read();

    int dx = (int8_t)packet[1];
    int dy = (int8_t)packet[2];
    uint8_t left = packet[0] & 1;

    clear_mouse();

    mouse_x += dx / 4;
    mouse_y -= dy / 4;

    if (mouse_x < 0) mouse_x = 0;
    if (mouse_x > 79) mouse_x = 79;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_y > 24) mouse_y = 24;

    if (left) {
        focus_input_at(mouse_x, mouse_y);
        check_button_click(mouse_x, mouse_y);
    }

    draw_mouse(MOUSE_COLOR);
}


void btn1Click() {
    vga_write_at(2, 10, "     ", 0x1F);
    vga_write_at(6, 10, "                                        ", 0x1F);
    vga_write_at(2, 10, "=^-^=", 0x1F);
    vga_write_at(8, 10, inputs[0].buffer, 0x1F);
}

void prgmHelloChi() {

    vga_clear(0x1F);

    vga_write_at(2, 2, "ChiOS System App - Hello Chi", 0x1B);
    vga_write_at(2, 4, "=^-^= Chi: Hallo!", 0x1A);

    draw_button(64, 22, "<- main menu", 0x1E);
    add_button(64, 22, "<- main menu", mainapp);
}

void prgmAbout() {

    vga_clear(0x1F);

    vga_write_at(2, 2, "ChiOS System App - Info", 0x1B);

    vga_write_at(4, 6, "Systemversion: noreldev7.0", 0x1F);

    draw_button(64, 22, "<- main menu", 0x1E);
    add_button(64, 22, "<- main menu", mainapp);
}

void mainapp() {
    vga_clear(0x1F);
    vga_write_at(2, 2, "ChiOS - mainapp - Main Menu", 0x1B);

    vga_write_at(2, 5, "System Apps:", 0x1F);

    draw_button(4, 7, "Hello Chi", 0x1E);
    add_button(4, 7, "Hello Chi", prgmHelloChi);

    draw_button(16, 7, "Info", 0x1E);
    add_button(16, 7, "Info", prgmAbout);
}

void kernel_main(uint32_t magic, void *mb_info) {
    (void)magic;
    (void)mb_info;

    vga_clear(0x1F);
    vga_write_at(1, 1, "Loading mainapp...", 0x1F);

    mainapp();

    mouse_init();
    draw_mouse(MOUSE_COLOR);

    while (1) {
        mouse_poll();
        handle_keyboard();
    }
}
