//==============================================================================
//includes:
#include "Components.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/timer.h"

#include "nvs_flash.h"
//==============================================================================
//defines:

#define APP_TASK_STACK_SIZE (8192 / 4)
//==============================================================================
//variables:

char out_data[0xfff];
uint16_t out_data_size;

StaticTask_t app_task_buffer;
StackType_t app_task_stack[APP_TASK_STACK_SIZE];

static uint32_t time_stamp_1000ms = 0;

int timer_group;
int timer_id;
//==============================================================================
//functions:

void vApplicationTickHook(void)
{
	
}
//------------------------------------------------------------------------------
void app_task(void* arg)
{
    while (true)
    {
        //ComponentsHandler();

        vTaskDelay(pdMS_TO_TICKS(10));
    }    
}
//------------------------------------------------------------------------------
static bool IRAM_ATTR timer_group_isr_callback(void *args)
{
    ComponentsTimeSynchronization();
    xSystemTimeSynchronization();

    return pdFALSE; // return whether a task switch is needed
}
//------------------------------------------------------------------------------
void app_main(void)
{
    int out_data_size = 0;

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
    }

	ESP_ERROR_CHECK(ret);
    
    /* Configure the peripheral according to the LED type */
    timer_config_t timer_config =
    {
        .clk_src = TIMER_SRC_CLK_DEFAULT,
        .divider = APB_CLK_FREQ / 1000000,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true,
    };

    ESP_ERROR_CHECK(timer_init(timer_group, timer_id, &timer_config));
    // For the timer counter to a initial value
    ESP_ERROR_CHECK(timer_set_counter_value(timer_group, timer_id, 0));
    // Set alarm value and enable alarm interrupt
    ESP_ERROR_CHECK(timer_set_alarm_value(timer_group, timer_id, 1000));
    ESP_ERROR_CHECK(timer_enable_intr(timer_group, timer_id));
    // Hook interrupt callback
    ESP_ERROR_CHECK(timer_isr_callback_add(timer_group, timer_id, timer_group_isr_callback, 0, 0));
    // Start timer
    ESP_ERROR_CHECK(timer_start(timer_group, timer_id));

    /*
    xTaskCreateStatic(app_task, // Function that implements the task.
                        "app_task", // Text name for the task.
                        APP_TASK_STACK_SIZE, // Number of indexes in the xStack array.
                        NULL, // Parameter passed into the task.
                        24, // Priority at which the task is created.
                        app_task_stack, // Array to use as the task's stack.
                        &app_task_buffer);
*/
    ComponentsInit(app_main);
    
    while (1)
    {
        ComponentsHandler();

        if (xSystemGetTime(app_main) - time_stamp_1000ms > 1000)
        {
            out_data_size = 0;

            time_stamp_1000ms = xSystemGetTime(app_main);
            //ESP_LOGI(TAG, "%s", "tic\r");
/*
            out_data_size = sprintf(out_data,
                "is init: %d\r"
                "channel 1: %d\r"
                "channel 2: %d\r\r",
                mADC.Handle != 0,
                mADC.Channels[0].Points.InShort[0],
                mADC.Channels[1].Points.InShort[0]
            );
            
            xPortTransmitData(&UsartPort, out_data, out_data_size);
            */
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
