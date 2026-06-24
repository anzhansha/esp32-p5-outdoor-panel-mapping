#pragma once

#include <stdint.h>

struct PanelTileMapEntry
{
    uint8_t ptx;
    uint8_t pty;
    bool flip_x;
    bool flip_y;
};

// Public logical display size.
#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32

// DMA surface size used by the HUB75 driver.
static constexpr int PANEL_DMA_WIDTH = 128;
static constexpr int PANEL_DMA_HEIGHT = 16;

// Tile size used by the custom remapper.
static constexpr int PANEL_TILE_SIZE = 8;
static constexpr int PANEL_TILE_COLS = PANEL_WIDTH / PANEL_TILE_SIZE;
static constexpr int PANEL_TILE_ROWS = PANEL_HEIGHT / PANEL_TILE_SIZE;

// HUB75 pin assignment for the tested ESP32-S3 wiring.
static constexpr int PANEL_R1_PIN = 4;
static constexpr int PANEL_G1_PIN = 5;
static constexpr int PANEL_B1_PIN = 6;
static constexpr int PANEL_R2_PIN = 7;
static constexpr int PANEL_G2_PIN = 15;
static constexpr int PANEL_B2_PIN = 16;

static constexpr int PANEL_A_PIN = 18;
static constexpr int PANEL_B_PIN = 8;
static constexpr int PANEL_C_PIN = 3;
static constexpr int PANEL_D_PIN = -1;
static constexpr int PANEL_E_PIN = -1;

static constexpr int PANEL_LAT_PIN = 40;
static constexpr int PANEL_OE_PIN = 2;
static constexpr int PANEL_CLK_PIN = 41;

// Final working map for a 64x32 1/8 scan outdoor panel.
static constexpr PanelTileMapEntry PANEL_TILE_MAP[PANEL_TILE_ROWS][PANEL_TILE_COLS] = {
    {
        {8, 0, false, false}, {9, 0, false, false}, {10, 0, false, false}, {11, 0, false, false},
        {12, 0, false, false}, {13, 0, false, false}, {14, 0, false, false}, {15, 0, false, false}
    },
    {
        {0, 0, false, false}, {1, 0, false, false}, {2, 0, false, false}, {3, 0, false, false},
        {4, 0, false, false}, {5, 0, false, false}, {6, 0, false, false}, {7, 0, false, false}
    },
    {
        {8, 1, false, false}, {9, 1, false, false}, {10, 1, false, false}, {11, 1, false, false},
        {12, 1, false, false}, {13, 1, false, false}, {14, 1, false, false}, {15, 1, false, false}
    },
    {
        {0, 1, false, false}, {1, 1, false, false}, {2, 1, false, false}, {3, 1, false, false},
        {4, 1, false, false}, {5, 1, false, false}, {6, 1, false, false}, {7, 1, false, false}
    }
};
