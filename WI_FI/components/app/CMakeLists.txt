idf_component_register(SRCS "app.c"
                    INCLUDE_DIRS "include"
                    REQUIRES wifi_station nvs_flash wifi_access_point web_page)
