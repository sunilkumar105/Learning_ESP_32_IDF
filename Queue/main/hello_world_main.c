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

void receiverTask(void *pvParameters);
QueueHandle_t xQueue1 = {0};

void app_main(void)
{
    /*Create Queue */
    /*It will hold 10 integer */
    xQueue1 = xQueueCreate(10, sizeof(int));
    if (xQueue1 == NULL)
    {
        printf("[ ERROR ] XQueue1 creation failed \r\n");
    }
    /*Create Task */
    BaseType_t taskCreationStatus = xTaskCreate(&receiverTask, "Receiver Task", 2048, NULL, 2, NULL);
    if (taskCreationStatus == pdPASS)
    {
        printf("[ INFO ] Receiver Task has been successfully created \r\n");
    }
    else
    {
        printf("[ ERROR ] Receiver Task creation failed \r\n");
    }
    while (1)
    {
        printf("Sender Task \r\n");
        int randomNumber = rand();
        BaseType_t XQueueSentStatus = xQueueSend(xQueue1, (void *)&randomNumber, 1000);
        if (XQueueSentStatus == pdPASS)
        {
            printf("[ INFO XQueue1 ] Random number sent successfully \r\n");
        }
        else
        {
            printf("[ ERROR XQueue1 ] Failed to add random number in XQueue1 \r\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void receiverTask(void *pvParameters)
{
    while (1)
    {
        if (xQueue1 == NULL)
        {
            continue;
        }
        printf("Receiver Task \r\n");
        int receivedNumber = 0;
        BaseType_t XQueueReceiveStatus = xQueueReceive(
            xQueue1,
            (void *)&receivedNumber,
            1000);
        if (XQueueReceiveStatus == pdPASS)
        {
            printf("[ INFO XQueue1 ] Number received successfully, number is: %d \r\n", receivedNumber);
        }
        else
        {
            printf("[ ERROR XQueue1 ] Failed to receive number in XQueue1 \r\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}