#include <stdio.h>
#include "web_page.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "lwip/err.h"
#include "lwip/sys.h"

// AP Configuration
#define AP_SSID "ESP32_AP"
#define AP_PASS "12345678"
#define AP_CHANNEL 1
#define MAX_CLIENTS 4

static const char *TAG = "WiFi_AP_Server";
static httpd_handle_t server = NULL;

// HTML webpage content
static const char html_page[] = "<!DOCTYPE html>\
<html>\
<head>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <title>ESP32 Web Server</title>\
    <style>\
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); }\
        .container { max-width: 600px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }\
        h1 { color: #333; text-align: center; margin-bottom: 30px; }\
        .info-box { background: #f5f5f5; padding: 15px; border-radius: 5px; margin: 15px 0; }\
        .info-label { font-weight: bold; color: #667eea; }\
        .button { background: #667eea; color: white; padding: 12px 24px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; margin: 5px; }\
        .button:hover { background: #5568d3; }\
        .led-status { display: inline-block; width: 20px; height: 20px; border-radius: 50%; margin-left: 10px; }\
        .led-on { background: #4caf50; box-shadow: 0 0 10px #4caf50; }\
        .led-off { background: #ccc; }\
    </style>\
</head>\
<body>\
    <div class='container'>\
        <h1>ESP32 Web Server</h1>\
        <div class='info-box'>\
            <div class='info-label'>Device:</div>\
            <div>ESP32 Access Point</div>\
        </div>\
        <div class='info-box'>\
            <div class='info-label'>IP Address:</div>\
            <div>192.168.4.1</div>\
        </div>\
        <div class='info-box'>\
            <div class='info-label'>Status:</div>\
            <div>Online <span class='led-status led-on'></span></div>\
        </div>\
        <div style='text-align: center; margin-top: 30px;'>\
            <button class='button' onclick='location.href=\"/led/on\"'>LED ON</button>\
            <button class='button' onclick='location.href=\"/led/off\"'>LED OFF</button>\
        </div>\
        <div style='text-align: center; margin-top: 20px;'>\
            <button class='button' onclick='location.reload()'>Refresh</button>\
        </div>\
    </div>\
</body>\
</html>";

// Handler for root path
static esp_err_t root_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    ESP_LOGI(TAG, "Served main page to client");
    return ESP_OK;
}

// Handler for LED ON
static esp_err_t led_on_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "LED ON command received");
    // Add your LED control code here
    // gpio_set_level(LED_PIN, 1);

    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

// Handler for LED OFF
static esp_err_t led_off_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "LED OFF command received");
    // Add your LED control code here
    // gpio_set_level(LED_PIN, 0);

    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

// Start HTTP server
static esp_err_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting HTTP server on port: %d", config.server_port);

    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Register URI handlers
        httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &root);

        httpd_uri_t led_on = {
            .uri = "/led/on",
            .method = HTTP_GET,
            .handler = led_on_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &led_on);

        httpd_uri_t led_off = {
            .uri = "/led/off",
            .method = HTTP_GET,
            .handler = led_off_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &led_off);

        ESP_LOGI(TAG, "HTTP server started successfully");
        return ESP_OK;
    }

    ESP_LOGE(TAG, "Error starting HTTP server!");
    return ESP_FAIL;
}

void host_web_page(void)
{
    // Start web server
    ESP_LOGI(TAG, "Starting Web Server \r\n");
    esp_err_t ret = start_webserver();
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Web server is running!");
        ESP_LOGI(TAG, "Open http://192.168.4.1 in your browser");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to start web server!");
    }
}