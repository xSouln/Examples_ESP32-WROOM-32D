//==============================================================================
//includes:

#include "Components.h"

#include "LWIP-NetTcpServer-Component.h"
#include "Adapters/LWIP-Net-Adapter.h"
#include "Adapters/LWIP-NetPort-Adapter.h"

#include "driver/gpio.h"
//==============================================================================
//defines:

#define TASK_STACK_SIZE 0x1000
#define RX_OPERATION_BUFFER_SIZE 0x800
#define RX_BUFFER_SIZE 0x800
#define TX_BUFFER_SIZE 0x800
//==============================================================================
//variables:

static uint8_t private_rx_operation_buffer[RX_OPERATION_BUFFER_SIZE];
static uint8_t private_rx_buffer[RX_BUFFER_SIZE];
static uint8_t private_tx_buffer[RX_BUFFER_SIZE];

xNetT LWIP_Net;

static TaskHandle_t task_handle;
static StaticTask_t task_buffer;
static StackType_t task_stack[TASK_STACK_SIZE];

xNetSocketT ListenSocket =
{
	.Port = 5000,
	.Address.Value = 0
};

xNetSocketT Socket =
{
	.Number = -1
};

xPortT ServerPort;
//==============================================================================
//functions:


//------------------------------------------------------------------------------
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
		default : return xResultRequestIsNotFound;
	}
	
	return xResultAccept;
}
//------------------------------------------------------------------------------
static void Task(void* arg)
{
	xWiFi_States last_state = xWiFi_StateIdle;
	static char data[512];
	int data_size = 0;
	char* result;

	static uint32_t time_stamp = 0;

	while (true)
	{
		vTaskDelay(pdMS_TO_TICKS(1));

		if (mWifi.Status.State == xWiFi_StateConnected)
		{
			if (last_state != mWifi.Status.State)
			{
				if (xNetInitTcpSocket(&LWIP_Net, &ListenSocket) == xResultAccept)
				{
					result = "xResultAccept";
				}
				else
				{
					result = "xResultError";
				}
				data_size = sprintf(data, "xNetInitTcpSocket: %s\r", result);
				
				if (xNetListen(&ListenSocket, 1) == xResultAccept)
				{
					result = "xResultAccept";
				}
				else
				{
					result = "xResultError";
				}

				data_size += sprintf(data + data_size, "xNetListen: %s\r", result);

				xPortStartTransmission(&UsartPort);
				xPortTransmitData(&UsartPort, data, data_size);
				xPortEndTransmission(&UsartPort);
				
			}

			if (ListenSocket.Number != -1 && Socket.Number == -1)
			{
				if (xNetAccept(&ListenSocket, &Socket) == xResultAccept)
				{
					data_size = sprintf(data, "xNetAccept: %s\r", "xResultAccept");

					xPortStartTransmission(&UsartPort);
					xPortTransmitData(&UsartPort, data, data_size);
					xPortEndTransmission(&UsartPort);
				}
			}
/*
			if (xSystemGetTime(Task) - time_stamp > 1000)
			{
				time_stamp = xSystemGetTime(Task);
				xPortTransmitString(&ServerPort, "qwerty\r");
			}
*/
			xPortHandler(&ServerPort);
		}

		last_state = mWifi.Status.State;
	}
}
//==============================================================================
//initializations:

static xNetObjectInterfaceT objectInterface =
{
	.RequestListener = (xObjectRequestListenerT)PrivateRequestListener,
	.EventListener = (xObjectEventListenerT)PrivateEventListener
};

LWIP_NetAdapterT LWIP_NetAdapter = { 0 };
LWIP_NetPortAdapterT LWIP_NetPortAdapter = { 0 };
//==============================================================================
//initialization:

xResult LWIP_NetTcpServerComponentInit(void* parent)
{
	LWIP_NetAdapterInitT adapter_init =
	{
		.Adapter = &LWIP_NetAdapter
	};
	LWIP_NetAdapterInit(&LWIP_Net, &adapter_init);

	xNetInitT init =
	{
		.Parent = parent,
		.Interface = &objectInterface
	};
	xNetInit(&LWIP_Net, &init);

	LWIP_NetPortAdapterInitT net_port_init =
	{
		.Adapter = &LWIP_NetPortAdapter,

		.RxOperationBuffer = private_rx_operation_buffer,
		.RxOperationBufferSize = sizeof(private_rx_operation_buffer),

		.RxBuffer = private_rx_buffer,
		.RxBufferSize = sizeof(private_rx_buffer),

		.TxBuffer = private_tx_buffer,
		.TxBufferSize = sizeof(private_tx_buffer)
	};

	LWIP_NetPortAdapterInit(&ServerPort, &net_port_init);

	xPortInitT port_init =
	{
		.Parent = parent,
		.Interface = &objectInterface
	};
	xPortInit(&ServerPort, &port_init);

	xPortSetBinding(&ServerPort, &Socket);

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
