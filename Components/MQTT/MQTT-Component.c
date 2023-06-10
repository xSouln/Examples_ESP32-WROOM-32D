//==============================================================================
//includes:

#include "Components.h"

#include "esp_netif.h"
#include "mqtt_client.h"
//==============================================================================
//defines:

#define TASK_STACK_SIZE 0x1400
//==============================================================================
//variables:

static TaskHandle_t task_handle;
static StaticTask_t task_buffer;
static StackType_t task_stack[TASK_STACK_SIZE];

static struct
{
	uint32_t IsInit : 1;
	uint32_t IsSubscribed : 1;

} State;

esp_mqtt_client_handle_t MqttClient;
//==============================================================================
//import:


//==============================================================================
//functions:

static void EventListener(ObjectBaseT* object, int selector, void* arg)
{
	switch(selector)
	{
		default: break;
	}
}
//------------------------------------------------------------------------------

static xResult RequestListener(ObjectBaseT* object, int selector, void* arg)
{
	switch (selector)
	{
		default: return xResultRequestIsNotFound;
	}

	return xResultAccept;
}

//------------------------------------------------------------------------------
//component functions:
/**
 * @brief main handler
 */
void MQTT_ComponentHandler()
{
	static uint32_t time_stamp = 0;

	if (xSystemGetTime(MQTT_ComponentHandler) - time_stamp > 1000)
	{
		time_stamp = xSystemGetTime(MQTT_ComponentHandler);

		if (MqttClient && State.IsSubscribed)
		{
			esp_mqtt_client_publish(MqttClient, "/topic/adc", "update adc data", 0, 0, 0);
		}
	}
}

static void mqtt_event_handler_cb(void* handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
	char* note = "mqtt_event_handler_cb\r";

    switch (event_id)
	{
        case MQTT_EVENT_CONNECTED:
            // Подписываемся на нужные топики
            msg_id = esp_mqtt_client_subscribe(client, "/topic/adc", 0);
			note = "MQTT_EVENT_CONNECTED\r";
            break;

        case MQTT_EVENT_DISCONNECTED:
			note = "MQTT_EVENT_DISCONNECTED\r";
			State.IsSubscribed = false;
            break;

        case MQTT_EVENT_SUBSCRIBED:
            // Отправляем сообщение после подписки
            //msg_id = esp_mqtt_client_publish(client, "/topic/pub", "Hello MQTT", 0, 0, 0);
			State.IsSubscribed = true;
			note = "MQTT_EVENT_SUBSCRIBED\r";
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
			note = "MQTT_EVENT_UNSUBSCRIBED\r";
			State.IsSubscribed = false;
            break;

        case MQTT_EVENT_PUBLISHED:
			note = "MQTT_EVENT_PUBLISHED\r";
            break;

        case MQTT_EVENT_DATA:
            // Обработка полученных данных
            //printf("Received data: %.*s\n", event->data_len, event->data);
			note = "MQTT_EVENT_DATA\r";
            break;

        case MQTT_EVENT_ERROR:
			note = "MQTT_EVENT_ERROR\r";
            break;

        default:
			note = "MQTT_EVENT_ANY\r";
            break;
    }

	xPortStartTransmission(&UsartPort);
	xPortTransmitString(&UsartPort, note);
	xPortEndTransmission(&UsartPort);
}
//------------------------------------------------------------------------------
static void Task(void* arg)
{
	static char data[512];
	int data_size = 0;
	char* result;

	while (true)
	{
		vTaskDelay(pdMS_TO_TICKS(1));

		if (mWifi.Status.State == xWiFi_StateConnected && !State.IsInit)
		{
			esp_mqtt_client_config_t mqtt_cfg =
			{
				.broker.address.uri = "mqtt://192.168.0.110:1883",
				.broker.address.port = 1883,
				//.broker.address.hostname = "127.0.0.10",
				//.broker.address.transport = MQTT_TRANSPORT_OVER_TCP,
				.credentials.authentication.use_secure_element = false
			};

			MqttClient = esp_mqtt_client_init(&mqtt_cfg);

			if (esp_mqtt_client_register_event(MqttClient, ESP_EVENT_ANY_ID, mqtt_event_handler_cb, MqttClient) != ESP_OK)
			{
				result = "xResultError";
			}
			else
			{
				result = "xResultAccept";
			}

			data_size = sprintf(data, "esp_mqtt_client_register_event: %s\r", result);

			if (esp_mqtt_client_start(MqttClient) != ESP_OK)
			{
				result = "xResultError";
			}
			else
			{
				result = "xResultAccept";
			}

			data_size += sprintf(data + data_size, "esp_mqtt_client_start: %s\r", result);

			xPortStartTransmission(&UsartPort);
			xPortTransmitData(&UsartPort, data, data_size);
			xPortEndTransmission(&UsartPort);

			State.IsInit = true;
		}
		else if (mWifi.Status.State == xWiFi_StateIdle && State.IsInit)
		{
			esp_mqtt_client_destroy(MqttClient);
			MqttClient = NULL;
			State.IsInit = false;
		}
	}
}
//==============================================================================
//initialization:


//==============================================================================
//component initialization:

xResult MQTT_ComponentInit(void* parent)
{
	task_handle =
			xTaskCreateStatic(Task, // Function that implements the task.
								"Tcp server task", // Text name for the task.
								TASK_STACK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								5, // Priority at which the task is created.
								task_stack, // Array to use as the task's stack.
								&task_buffer);
  
	return xResultAccept;
}
//==============================================================================

