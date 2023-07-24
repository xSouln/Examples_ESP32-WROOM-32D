//==============================================================================
//includes:

#include "Components.h"

#include "esp_netif.h"
#include "mqtt_client.h"

#include "Common/xMQTT/xMQTT.h"
#include "Adapters/MQTT-Port-Adapter.h"
//==============================================================================
//defines:

#define TASK_STACK_SIZE 0x1400

#define RX_BUFFER_SIZE 0x800
#define TX_BUFFER_SIZE 0x1000
//==============================================================================
//variables:

static uint8_t private_rx_buffer[RX_BUFFER_SIZE];
static uint8_t private_tx_buffer[RX_BUFFER_SIZE];

static TaskHandle_t task_handle;
static StaticTask_t task_buffer;
static StackType_t task_stack[TASK_STACK_SIZE];

static const char MqttAdcTxTopic[] = "/topic/adc";
static const char MqttRxTopic[] = "/topic/rx";

static struct
{
	uint32_t IsInit : 1;

} State;

esp_mqtt_client_handle_t MqttClient;
xMqttClientT mMqttClient;
xPortT MqttPort;

xMqttTopicT MqttAdcTopic =
{
	.Client = &mMqttClient,

	.TxTopic = (char*)MqttAdcTxTopic,
	.RxTopic = (char*)MqttRxTopic,
};
//==============================================================================
//import:


//==============================================================================
//functions:

static void PrivateEventListener(ObjectBaseT* object, int selector, void* arg)
{
	if (object->Description->ObjectId == xPORT_OBJECT_ID)
	{
		xPortT* port = object;

		switch (selector)
		{
			case xPortObjectEventRxFoundEndLine:
			{
				xPortEventDataPacketArgT* packet = arg;

				TerminalReceiveData(port, packet->Data, packet->Size);
			}
			break;

			case xPortObjectEventRxBufferIsFull:
			{
				xPortEventDataPacketArgT* packet = arg;

				TerminalReceiveData(port, packet->Data, packet->Size);
			}
			break;

			default : return;
		}
	}
}
//------------------------------------------------------------------------------

static xResult PrivateRequestListener(ObjectBaseT* object, int selector, void* arg)
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
	static uint16_t points[100][2];
	static char json[4096];
	static int jsonSize;

	if (xSystemGetTime(MQTT_ComponentHandler) - time_stamp > 1000)
	{
		time_stamp = xSystemGetTime(MQTT_ComponentHandler);
	}
/*
	if (mADC.NotifiedChannels && mMqttClient.State == xMqttClientConnected)
	{
		int number_of_points = xADC_GetNumberOfNewPoints(&mADC);
		
		if (number_of_points > 100)
		{
			number_of_points = xCircleBufferReadObject(&mADC.Points->Buffer, points, 100, 0, 0);

			json[0] = '[';
			jsonSize = 1;

			for (int i = 0; i < 100; i++)
			{
				jsonSize += sprintf(json + jsonSize, "%u, ", points[i][0]);
			}
			
			json[jsonSize - 1] = ']';
			json[jsonSize] = 0;

			//esp_mqtt_client_publish(MqttClient, MqttAdcTopic.TxTopic, json, 0, 0, 0);
			xPortTransmitData(&MqttPort, json, jsonSize);
		}
	}
	*/
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
            //msg_id = esp_mqtt_client_subscribe(client, "/topic/adc", 0);
			MqttAdcTopic.Id = esp_mqtt_client_subscribe(client, MqttAdcTopic.RxTopic, 0);
			note = "MQTT_EVENT_CONNECTED\r";
			mMqttClient.State = xMqttClientConnected;
            break;

        case MQTT_EVENT_DISCONNECTED:
			note = "MQTT_EVENT_DISCONNECTED\r";
			mMqttClient.State = xMqttClientIdle;
			MqttAdcTopic.State = xMqttTopicIdle;
            break;

        case MQTT_EVENT_SUBSCRIBED:
            // Отправляем сообщение после подписки
            //msg_id = esp_mqtt_client_publish(client, "/topic/pub", "Hello MQTT", 0, 0, 0);
			
			if (MqttAdcTopic.Id == event->msg_id)
			{
				MqttAdcTopic.State = xMqttTopicSubscribed;
				note = "MQTT_EVENT_SUBSCRIBED ACCEPT\r";
			}
			else
			{
				note = "MQTT_EVENT_SUBSCRIBED\r";
			}
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
			note = "MQTT_EVENT_UNSUBSCRIBED\r";
			MqttAdcTopic.State = xMqttTopicIdle;
            break;

        case MQTT_EVENT_PUBLISHED:
			note = "MQTT_EVENT_PUBLISHED\r";
            break;

        case MQTT_EVENT_DATA:
            // Обработка полученных данных
            //printf("Received data: %.*s\n", event->data_len, event->data);

			xMqttTopicDataReceiverArgT arg =
			{
				.Base =
				{
					.Data = (uint8_t*)event->data,
					.Size = event->data_len - 1,
					.Content = event,
				},
				.Topic = &MqttAdcTopic
			};
			xPortAdapterEventListener(MqttAdcTopic.Handle, xPortEventReceiveData, &arg);

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
				//.broker.address.uri = "mqtt://86.57.154.232",
				.broker.address.port = 1883,
				//.credentials.username = "sample",
				//.credentials.authentication.password = "69greystones",
				//.broker.address.hostname = "127.0.0.10",
				//.broker.address.transport = MQTT_TRANSPORT_OVER_TCP,
				.credentials.authentication.use_secure_element = false
			};

			MqttClient = esp_mqtt_client_init(&mqtt_cfg);
			mMqttClient.Handle = MqttClient;

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
			mMqttClient.Handle = NULL;
		}

		xPortHandler(&MqttPort);
	}
}
//==============================================================================
//initialization:

MqttPortAdapterT MqttPortAdapter;

static xPortObjectInterfaceT objectInterface =
{
	.RequestListener = (xObjectRequestListenerT)PrivateRequestListener,
	.EventListener = (xObjectEventListenerT)PrivateEventListener
};
//==============================================================================
//component initialization:

xResult MQTT_ComponentInit(void* parent)
{
	MqttPortAdapterInitT adapter_init =
	{
		.Adapter = &MqttPortAdapter,
		.RxBuffer = private_rx_buffer,
		.RxBufferSize = sizeof(private_rx_buffer),
		.TxBuffer = private_tx_buffer,
		.TxBufferSize = sizeof(private_tx_buffer),
	};
	MqttPortAdapterInit(&MqttPort, &adapter_init);

	xPortInitT port_init =
	{
		.Parent = parent,
		.Interface = &objectInterface
	};
	xPortInit(&MqttPort, &port_init);

	xPortSetBinding(&MqttPort, &MqttAdcTopic);
	
	MqttAdcTopic.Client = &mMqttClient;

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

