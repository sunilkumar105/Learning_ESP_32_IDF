/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "driver/gpio.h"
#include "esp_system.h"

#define LED_PIN_GPIO_NUM GPIO_NUM_33

extern "C" void app_main(void)
{
    gpio_set_direction(LED_PIN_GPIO_NUM, GPIO_MODE_OUTPUT);
    while (1)
    {
        printf("Hello world!\n");
        gpio_set_level(LED_PIN_GPIO_NUM, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN_GPIO_NUM, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
