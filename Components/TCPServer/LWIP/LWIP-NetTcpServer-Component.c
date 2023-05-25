//==============================================================================
//includes:

#include "Components.h"

#include "LWIP-NetTcpServer-Component.h"
#include "Adapters/LWIP-Net-Adapter.h"

#include "driver/gpio.h"
//==============================================================================
//defines:

#define TASK_STACK_SIZE 0x1000
//==============================================================================
//variables:

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
//==============================================================================
//functions:


//------------------------------------------------------------------------------
static void PrivateEventListener(xNetT* net, xNetSysEventSelector selector, void* arg)
{
	switch ((uint8_t)selector)
	{
		default : return;
	}
}
//------------------------------------------------------------------------------
static xResult PrivateRequestListener(xNetT* net, xNetSysRequestSelector selector, void* arg)
{
	switch ((uint8_t)selector)
	{		
		default : return xResultRequestIsNotFound;
	}
	
	return xResultAccept;
}
//------------------------------------------------------------------------------
static void Task(void* arg)
{
	xWiFi_StatusT last_status;
	static char data[512];
	int data_size = 0;
	char* result;

	while (true)
	{
		vTaskDelay(pdMS_TO_TICKS(1));

		if (mWifi.Status.AddressIsSet)
		{
			if (!last_status.AddressIsSet)
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

			xNetSocketHandler(&Socket);
		}

		last_status.Value = mWifi.Status.Value;
	}
}
//==============================================================================
//initializations:

xNetSysInterfaceT NetSysInterface =
{
	.RequestListener = (xNetSysRequestListenerT)PrivateRequestListener,
	.EventListener = (xNetSysEventListenerT)PrivateEventListener
};

LWIP_NetAdapterT LWIP_NetAdapter = { 0 };
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
		.Interface = &NetSysInterface
	};
	xNetInit(&LWIP_Net, &init);

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
