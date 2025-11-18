idf_component_register(SRCS "main.c"
                      REQUIRES spi_flash nvs_flash esp_driver_i2c fatfs
                    INCLUDE_DIRS ".")
