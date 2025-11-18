idf_component_register(SRCS "hello_world_main.c"
                      REQUIRES spi_flash nvs_flash esp_driver_cam esp_driver_i2c
                    INCLUDE_DIRS ".")
