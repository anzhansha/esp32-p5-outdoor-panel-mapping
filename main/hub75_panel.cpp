#include "hub75_panel.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"

static const char *TAG = "HUB75_PANEL";
static MatrixPanel_I2S_DMA *dma = nullptr;

static inline void px_tilemap(int x, int y, uint16_t color)
{
    if (!dma) return;
    if ((unsigned)x >= PANEL_WIDTH || (unsigned)y >= PANEL_HEIGHT) return;

    int tx = x / PANEL_TILE_SIZE;
    int ty = y / PANEL_TILE_SIZE;

    int lx = x % PANEL_TILE_SIZE;
    int ly = y % PANEL_TILE_SIZE;

    PanelTileMapEntry m = PANEL_TILE_MAP[ty][tx];

    if (m.flip_x) lx = (PANEL_TILE_SIZE - 1) - lx;
    if (m.flip_y) ly = (PANEL_TILE_SIZE - 1) - ly;

    int dx = m.ptx * PANEL_TILE_SIZE + lx;
    int dy = m.pty * PANEL_TILE_SIZE + ly;

    dma->drawPixel(dx, dy, color);
}

// ============================================================
// Minimal 5x7 font
// ============================================================
static const uint8_t font5x7[][5] = {
    {0x7E,0x11,0x11,0x11,0x7E}, {0x7F,0x49,0x49,0x49,0x36},
    {0x3E,0x41,0x41,0x41,0x22}, {0x7F,0x41,0x41,0x22,0x1C},
    {0x7F,0x49,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x09,0x01},
    {0x3E,0x41,0x49,0x49,0x7A}, {0x7F,0x08,0x08,0x08,0x7F},
    {0x00,0x41,0x7F,0x41,0x00}, {0x20,0x40,0x41,0x3F,0x01},
    {0x7F,0x08,0x14,0x22,0x41}, {0x7F,0x40,0x40,0x40,0x40},
    {0x7F,0x02,0x0C,0x02,0x7F}, {0x7F,0x04,0x08,0x10,0x7F},
    {0x3E,0x41,0x41,0x41,0x3E}, {0x7F,0x09,0x09,0x09,0x06},
    {0x3E,0x41,0x51,0x21,0x5E}, {0x7F,0x09,0x19,0x29,0x46},
    {0x46,0x49,0x49,0x49,0x31}, {0x01,0x01,0x7F,0x01,0x01},
    {0x3F,0x40,0x40,0x40,0x3F}, {0x1F,0x20,0x40,0x20,0x1F},
    {0x3F,0x40,0x38,0x40,0x3F}, {0x63,0x14,0x08,0x14,0x63},
    {0x07,0x08,0x70,0x08,0x07}, {0x61,0x51,0x49,0x45,0x43},
    {0x3E,0x51,0x49,0x45,0x3E}, {0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10}, {0x27,0x45,0x45,0x45,0x39},
    {0x3C,0x4A,0x49,0x49,0x30}, {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36}, {0x06,0x49,0x49,0x29,0x1E},
    {0x00,0x00,0x00,0x00,0x00},
};

static void draw_char_internal(int cx, int cy, char ch, uint16_t color)
{
    int idx;
    if      (ch >= 'A' && ch <= 'Z') idx = ch - 'A';
    else if (ch >= 'a' && ch <= 'z') idx = ch - 'a';
    else if (ch >= '0' && ch <= '9') idx = 26 + (ch - '0');
    else                              idx = 36;

    for (int col = 0; col < 5; col++) {
        uint8_t line = font5x7[idx][col];
        for (int row = 0; row < 7; row++) {
            if (line & (1 << row)) {
                px_tilemap(cx + col, cy + row, color);
            }
        }
    }
}

// ============================================================
// Public API
// ============================================================
bool panel_init(void)
{
    HUB75_I2S_CFG mxconfig(PANEL_DMA_WIDTH, PANEL_DMA_HEIGHT, 1);

    mxconfig.gpio.r1  = PANEL_R1_PIN;
    mxconfig.gpio.g1  = PANEL_G1_PIN;
    mxconfig.gpio.b1  = PANEL_B1_PIN;
    mxconfig.gpio.r2  = PANEL_R2_PIN;
    mxconfig.gpio.g2  = PANEL_G2_PIN;
    mxconfig.gpio.b2  = PANEL_B2_PIN;

    mxconfig.gpio.a   = PANEL_A_PIN;
    mxconfig.gpio.b   = PANEL_B_PIN;
    mxconfig.gpio.c   = PANEL_C_PIN;
    mxconfig.gpio.d   = PANEL_D_PIN;
    mxconfig.gpio.e   = PANEL_E_PIN;

    mxconfig.gpio.lat = PANEL_LAT_PIN;
    mxconfig.gpio.oe  = PANEL_OE_PIN;
    mxconfig.gpio.clk = PANEL_CLK_PIN;

    mxconfig.driver         = HUB75_I2S_CFG::SHIFTREG;
    mxconfig.i2sspeed       = HUB75_I2S_CFG::HZ_8M;
    mxconfig.clkphase       = false;
    mxconfig.latch_blanking = 3;

    dma = new MatrixPanel_I2S_DMA(mxconfig);
    if (!dma->begin()) {
        ESP_LOGE(TAG, "DMA begin() failed");
        dma = nullptr;
        return false;
    }

    dma->setBrightness8(20);
    dma->clearScreen();

    ESP_LOGI(TAG, "panel_init OK");
    return true;
}

void panel_clear(void)
{
    if (dma) dma->clearScreen();
}

void panel_set_brightness(uint8_t value)
{
    if (dma) dma->setBrightness8(value);
}

uint16_t panel_color(uint8_t r, uint8_t g, uint8_t b)
{
    if (!dma) return 0;
    return dma->color565(r, g, b);
}

void panel_draw_pixel(int x, int y, uint16_t color)
{
    px_tilemap(x, y, color);
}

void panel_fill_screen(uint16_t color)
{
    for (int y = 0; y < PANEL_HEIGHT; y++) {
        for (int x = 0; x < PANEL_WIDTH; x++) {
            px_tilemap(x, y, color);
        }
    }
}

void panel_draw_rect(int x, int y, int w, int h, uint16_t color)
{
    for (int yy = y; yy < y + h; yy++) {
        for (int xx = x; xx < x + w; xx++) {
            px_tilemap(xx, yy, color);
        }
    }
}

void panel_draw_line(int x0, int y0, int x1, int y1, uint16_t color)
{
    int dx = abs(x1 - x0), dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (true) {
        px_tilemap(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void panel_draw_border(uint16_t color)
{
    panel_draw_line(0, 0, PANEL_WIDTH - 1, 0, color);
    panel_draw_line(0, PANEL_HEIGHT - 1, PANEL_WIDTH - 1, PANEL_HEIGHT - 1, color);
    panel_draw_line(0, 0, 0, PANEL_HEIGHT - 1, color);
    panel_draw_line(PANEL_WIDTH - 1, 0, PANEL_WIDTH - 1, PANEL_HEIGHT - 1, color);
}

void panel_draw_tile_grid(uint16_t color)
{
    for (int x = PANEL_TILE_SIZE; x < PANEL_WIDTH; x += PANEL_TILE_SIZE) {
        panel_draw_line(x, 0, x, PANEL_HEIGHT - 1, color);
    }

    for (int y = PANEL_TILE_SIZE; y < PANEL_HEIGHT; y += PANEL_TILE_SIZE) {
        panel_draw_line(0, y, PANEL_WIDTH - 1, y, color);
    }
}

void panel_draw_char(int x, int y, char ch, uint16_t color)
{
    draw_char_internal(x, y, ch, color);
}

void panel_draw_text(int x, int y, const char *text, uint16_t color)
{
    if (!text) return;
    while (*text) {
        draw_char_internal(x, y, *text++, color);
        x += 6;
    }
}
