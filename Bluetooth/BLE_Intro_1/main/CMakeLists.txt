idf_component_register(SRCS "main.c" "heart_rate_mock.c" "hello_world_main.c"
                      REQUIRES driver spi_flash bt nvs_flash
                      INCLUDE_DIRS ".")

