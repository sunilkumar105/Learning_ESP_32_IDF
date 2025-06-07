idf_component_register(SRCS "hello_world_main.cpp"
                    REQUIRES esp_driver_gpio spi_flash
                    INCLUDE_DIRS "." 
                    )
