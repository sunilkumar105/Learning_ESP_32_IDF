#include <stdio.h>
#include "wifi_access_point.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

// AP Configuration
#define AP_SSID "ESP32_AP"
#define AP_PASS "12345678"
#define AP_CHANNEL 1
#define MAX_CLIENTS 4

static const char *TAG = "WiFi_AP";

esp_err_t wifi_init_ap(void)
{
    esp_err_t ret;

    // Initialize network interface
    ret = esp_netif_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init netif: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_netif_create_default_wifi_ap();

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init WiFi: %s", esp_err_to_name(ret));
        return ret;
    }

    // Configure AP
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .channel = AP_CHANNEL,
            .password = AP_PASS,
            .max_connection = MAX_CLIENTS,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = false},
        },
    };

    // If password is empty, use open mode
    if (strlen(AP_PASS) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ret = esp_wifi_set_mode(WIFI_MODE_AP);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set WiFi mode: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set WiFi config: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start WiFi: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "WiFi AP started successfully");
    ESP_LOGI(TAG, "SSID: %s", AP_SSID);
    ESP_LOGI(TAG, "Password: %s", AP_PASS);
    ESP_LOGI(TAG, "Channel: %d", AP_CHANNEL);
    ESP_LOGI(TAG, "Max Connections: %d", MAX_CLIENTS);

    // Get AP IP address
    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK)
    {
        ESP_LOGI(TAG, "AP IP Address: " IPSTR, IP2STR(&ip_info.ip));
        ESP_LOGI(TAG, "AP Gateway: " IPSTR, IP2STR(&ip_info.gw));
        ESP_LOGI(TAG, "AP Netmask: " IPSTR, IP2STR(&ip_info.netmask));
    }

    return ESP_OK;
}

bool wifi_ap_is_started(void)
{
    wifi_mode_t mode;
    if (esp_wifi_get_mode(&mode) == ESP_OK)
    {
        return (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA);
    }
    return false;
}

esp_err_t wifi_get_connected_clients(wifi_sta_list_t *sta_list)
{
    return esp_wifi_ap_get_sta_list(sta_list);
}

bool create_wifi_access_point(void)
{
    esp_err_t ret;
    ESP_LOGI(TAG, "Starting WiFi Access Point Mode");
    ret = wifi_init_ap();
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "WiFi AP initialized successfully!");
        return true;
    }
    else
    {
        ESP_LOGE(TAG, "WiFi AP initialization failed!");
        return false;
    }
    return false;
}

void manage_WiFi_network(void)
{
    vTaskDelay(pdMS_TO_TICKS(10000)); // Check every 10 seconds

    wifi_sta_list_t sta_list;
    if (wifi_get_connected_clients(&sta_list) == ESP_OK)
    {
        ESP_LOGI(TAG, "Connected clients: %d", sta_list.num);
        for (int i = 0; i < sta_list.num; i++)
        {
            ESP_LOGI(TAG, "Client %d MAC: %02x:%02x:%02x:%02x:%02x:%02x",
                     i,
                     sta_list.sta[i].mac[0],
                     sta_list.sta[i].mac[1],
                     sta_list.sta[i].mac[2],
                     sta_list.sta[i].mac[3],
                     sta_list.sta[i].mac[4],
                     sta_list.sta[i].mac[5]);
        }
    }
}