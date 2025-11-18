#include <stdio.h>
#include "wifi_station.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "wifi_access_point.h"
#include "web_page.h"
#include "app.h"
#include "esp_log.h"


static const char *TAG = "MAIN_APP";

void initializeApplication(void)
{
    printf("Initializing application \r\n");

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    if (!create_wifi_access_point())
    {
        return;
    }
    host_web_page();
}
