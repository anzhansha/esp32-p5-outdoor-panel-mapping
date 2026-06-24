#pragma once
#include <stdint.h>
#include <stdbool.h>

#include "panel_config.h"

bool panel_init(void);
void panel_clear(void);
void panel_set_brightness(uint8_t value);
uint16_t panel_color(uint8_t r, uint8_t g, uint8_t b);

void panel_draw_pixel(int x, int y, uint16_t color);
void panel_fill_screen(uint16_t color);
void panel_draw_rect(int x, int y, int w, int h, uint16_t color);
void panel_draw_line(int x0, int y0, int x1, int y1, uint16_t color);
void panel_draw_border(uint16_t color);
void panel_draw_tile_grid(uint16_t color);

void panel_draw_char(int x, int y, char ch, uint16_t color);
void panel_draw_text(int x, int y, const char *text, uint16_t color);
