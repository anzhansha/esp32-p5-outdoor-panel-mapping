#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hub75_panel.h"

static void draw_mapping_demo(void)
{
    panel_clear();

    panel_draw_border(panel_color(64, 0, 0));
    panel_draw_tile_grid(panel_color(0, 32, 0));

    panel_draw_text(2, 2, "P5", panel_color(255, 255, 0));
    panel_draw_text(26, 2, "64X32", panel_color(0, 255, 255));
    panel_draw_text(2, 12, "1/8", panel_color(255, 255, 255));
    panel_draw_text(26, 12, "ICN", panel_color(255, 0, 0));
    panel_draw_text(2, 24, "MAP", panel_color(0, 255, 0));
}

extern "C" void app_main(void)
{
    if (!panel_init()) {
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    panel_set_brightness(20);
    draw_mapping_demo();

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
