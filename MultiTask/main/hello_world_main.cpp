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
#include "esp_system.h"

void task1(void *pvParameters);

extern "C" void app_main(void)
{
    BaseType_t taskCreationStatus = xTaskCreate(&task1, "Task 1", 2048, NULL, 2, NULL);
    if (taskCreationStatus == pdPASS)
    {
        printf("[ INFO ] Task1 has been successfully created \r\n");
    }
    else
    {
        printf("[ ERROR ] Task1 creation failed \r\n");
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS); /*Wait for 3 sec */

    while (1)
    {
        printf("Hello world! \r\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task1(void *pvParameters)
{

    while (1)
    {
        printf("I am task 1 \r\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}