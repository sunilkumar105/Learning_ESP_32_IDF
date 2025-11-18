idf_component_register(SRCS "hello_world_main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi spi_flash app wifi_access_point)

