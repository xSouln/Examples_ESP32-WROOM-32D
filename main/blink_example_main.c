/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "driver/spi_master.h"

#include "Components.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO
#define SFC_SPI_ENABLE 1

#define APP_TASK_STACK_SIZE (8192 / 4)

static uint8_t s_led_state = 0;

#ifdef CONFIG_BLINK_LED_RMT

#elif CONFIG_BLINK_LED_GPIO

char out_data[0xfff];
uint16_t out_data_size;

StaticTask_t app_task_buffer;
StackType_t app_task_stack[APP_TASK_STACK_SIZE];

void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
    ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
    /* Toggle the LED state */
    s_led_state = !s_led_state;

    out_data_size = sprintf(out_data, "sys_tim = %d", (int)ComponentsSysGetTime());
    out_data[out_data_size] = 0;

    //spi_device_handle_t
    //spi_bus_add_device

    //spi_bus_add_device(handle->instance, &config, &ctx->spi);
    //spi_device_polling_transmit
    //spi_device_acquire_bus(&adapter->data.handle, portMAX_DELAY);
    //spi_device_transmit
    //spi_device_interface_config_t

    ESP_LOGI(TAG, "%s", out_data);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

#endif

void vApplicationTickHook(void)
{
	ComponentsTimeSynchronization();
}

void app_task(void* arg)
{
    while (true)
    {
        ComponentsHandler();

        vTaskDelay(pdMS_TO_TICKS(10));
    }    
}

void app_main(void)
{

    /* Configure the peripheral according to the LED type */
    configure_led();

    ComponentsInit(app_main);
/*
    TCPServerWIZspiComponentInit(app_main);

    #ifdef SFC_SPI_ENABLE
    sfc_spi_init(&sfc_spi, app_main, (sfc_spi_sys_interface_t*)&spi_sys_interface);
    #endif
*/
    xTaskCreateStatic(app_task, // Function that implements the task.
                        "app_task", // Text name for the task.
                        APP_TASK_STACK_SIZE, // Number of indexes in the xStack array.
                        NULL, // Parameter passed into the task.
                        24, // Priority at which the task is created.
                        app_task_stack, // Array to use as the task's stack.
                        &app_task_buffer);

    while (1) {
        //ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        //blink_led();

        //ComponentsHandler();

        vTaskDelay(pdMS_TO_TICKS(1000));

        //xTxTransmitString(&TCPServerWIZspi.Tx, "qwerty");
    }
}
