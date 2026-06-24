idf_component_register(
        SRCS
            "main.cpp"
            "hub75_panel.cpp"
        INCLUDE_DIRS "."
        REQUIRES espressif__arduino-esp32 ESP32-HUB75-MatrixPanel-I2S-DMA
)
