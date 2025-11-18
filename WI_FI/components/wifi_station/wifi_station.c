#include <stdio.h>
#include "wifi_station.h"
#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

// WiFi credentials
#define WIFI_SSID      "WHIRLYBIRD 4GHZ"
#define WIFI_PASS      "*89c51ed2#"
#define MAX_RETRY      5

static const char *TAG = "WiFi_Station";

bool wifi_check_connected(void)
{
    wifi_ap_record_t ap_info;
    return (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK);
}

esp_err_t wifi_init_sta(void)
{
    esp_err_t ret;

    // Initialize network interface
    ret = esp_netif_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init netif: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_netif_create_default_wifi_sta();

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init WiFi: %s", esp_err_to_name(ret));
        return ret;
    }

    // Configure WiFi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi mode: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi config: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start WiFi: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "WiFi initialized, starting connection...");
    vTaskDelay(pdMS_TO_TICKS(1000));
    // Attempt to connect with retries
    for (int retry = 0; retry < MAX_RETRY; retry++) {
        ret = esp_wifi_connect();
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "WiFi connect failed: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        ESP_LOGI(TAG, "Connecting to AP... (attempt %d/%d)", retry + 1, MAX_RETRY);

        // Wait for connection with timeout
        int wait_time = 0;
        while (wait_time < 10000) {  // 10 second timeout
            if (wifi_check_connected()) {
                // Get IP address
                esp_netif_ip_info_t ip_info;
                esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
                if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
                    ESP_LOGI(TAG, "Connected! IP: " IPSTR, IP2STR(&ip_info.ip));
                    return ESP_OK;
                }
            }
            vTaskDelay(pdMS_TO_TICKS(500));
            wait_time += 500;
        }

        ESP_LOGW(TAG, "Connection timeout, retrying...");
    }

    ESP_LOGE(TAG, "Failed to connect after %d attempts", MAX_RETRY);
    return ESP_FAIL;
}

void connect_to_wifi_access_point(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Starting WiFi Station Mode");
    
    ret = wifi_init_sta();
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "WiFi connection successful!");
        // Your application code here
    } else {
        ESP_LOGE(TAG, "WiFi connection failed!");
        // Handle failure
    }
}