//==============================================================================
//includes:

#include "MQTT-Port-Adapter.h"
#include "mqtt_client.h"
//==============================================================================
//functions:

static void PrivateHandler(xPortT* port)
{
	register MqttPortAdapterT* adapter = (MqttPortAdapterT*)port->Adapter;
	xMqttTopicT* topic = port->Binding;

	if (!topic)
	{
		return;
	}

	if (topic->Client->State == xMqttClientConnected && adapter->TxBuffer.DataSize)
	{
		xSemaphoreTake(adapter->TransactionMutex, portMAX_DELAY);

		if (adapter->TxBuffer.DataSize)
		{
			esp_mqtt_client_publish(topic->Client->Handle, topic->TxTopic, (char*)adapter->TxBuffer.Data, adapter->TxBuffer.DataSize, 0, 0);
			adapter->TxBuffer.DataSize = 0;
		}
		xSemaphoreGive(adapter->TransactionMutex);
	}
}
//------------------------------------------------------------------------------
static void PrivateIRQ(xPortT* port, void* arg)
{

}
//------------------------------------------------------------------------------
static xResult PrivateRequestListener(xPortT* port, xPortRequestSelector selector, void* arg)
{
	register MqttPortAdapterT* adapter = (MqttPortAdapterT*)port->Adapter;
	xMqttTopicT* topic = port->Binding;

	switch ((uint32_t)selector)
	{
		case xPortRequestUpdateTxStatus:
			port->Tx.IsEnable = topic && topic->State == xMqttTopicSubscribed && topic->Client->State == xMqttClientConnected;
			break;

		case xPortRequestUpdateRxStatus:
			port->Rx.IsEnable = topic && topic->State == xMqttTopicSubscribed && topic->Client->State == xMqttClientConnected;
			break;

		case xPortRequestGetRxBuffer:
			*(uint8_t**)arg = adapter->RxReceiver.Buffer;
			break;

		case xPortRequestGetRxBufferSize:
			*(uint32_t*)arg = adapter->RxReceiver.BufferSize;
			break;

		case xPortRequestGetRxBufferFreeSize:
			*(uint32_t*)arg = adapter->RxReceiver.BufferSize - adapter->RxReceiver.BytesReceived;
			break;

		case xPortRequestClearRxBuffer:
			adapter->RxReceiver.BytesReceived = 0;
			break;

		case xPortRequestGetTxBufferSize:
			*(uint32_t*)arg = 1000;
			break;

		case xPortRequestGetTxBufferFreeSize:
			*(uint32_t*)arg = 1000;
			break;

		case xPortRequestSetBinding:
		{
			xMqttTopicT* topic = arg;

			if (port->Binding)
			{
				((xMqttTopicT*)port->Binding)->Handle = 0;
			}

			port->Binding = topic;

			if (topic)
			{
				topic->Handle = port;
			}
			break;
		}

		case xPortRequestStartTransmission:
			xSemaphoreTake(adapter->TransactionMutex, portMAX_DELAY);
			break;

		case xPortRequestEndTransmission:
		{
			if (topic && topic->Client->State == xMqttClientConnected && adapter->TxBuffer.DataSize)
			{
				esp_mqtt_client_publish(topic->Client->Handle, topic->TxTopic, (char*)adapter->TxBuffer.Data, adapter->TxBuffer.DataSize, 0, 0);
				adapter->TxBuffer.DataSize = 0;
			}
			xSemaphoreGive(adapter->TransactionMutex);
			break;
		}

		default : return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void PrivateEventListener(xPortT* port, xPortEventSelector selector, void* arg)
{
	//register UsartPortAdapterT* adapter = (UsartPortAdapterT*)port->Adapter;

	switch((int)selector)
	{
		case xPortEventReceiveData:
		{
			xPortEventListener(port, xPortObjectEventRxFoundEndLine, arg);
			//xPortEventDataPacketArgT* packet = arg;
			//TerminalReceiveData(port, packet->Data, packet->Size);
			break;
		}

		default: return;
	}
}
//------------------------------------------------------------------------------
static int PrivateTransmit(xPortT* port, void* data, uint32_t size)
{
	MqttPortAdapterT* adapter = (MqttPortAdapterT*)port->Adapter;

	xDataBufferAdd(&adapter->TxBuffer, data, size);
	return size;
}
//------------------------------------------------------------------------------
static int PrivateReceive(xPortT* port, void* data, uint32_t size)
{
	return 0;
}
//------------------------------------------------------------------------------
static void PrivateRxReceiverEventListener(xRxReceiverT* receiver, xRxReceiverEventSelector event, void* arg)
{
	register xPortT* port = receiver->Base.Parent;

	switch ((uint8_t)event)
	{
		case xRxReceiverEventEndLine:
			xPortEventListener(port, xPortObjectEventRxFoundEndLine, arg);
			break;

		case xRxReceiverEventBufferIsFull:
			xPortEventListener(port, xPortObjectEventRxBufferIsFull, arg);
			break;

		default: return;
	}
}
//==============================================================================
//initializations:

static xPortInterfaceT PrivatePortInterface =
{
	.Handler = (xPortHandlerT)PrivateHandler,
	.IRQ = (xPortIRQT)PrivateIRQ,

	.RequestListener = (xPortRequestListenerT)PrivateRequestListener,
	.EventListener = (xPortEventListenerT)PrivateEventListener,

	.Transmit = (xPortTransmitActionT)PrivateTransmit,
	.Receive = (xPortReceiveActionT)PrivateReceive
};
//------------------------------------------------------------------------------
static xRxReceiverInterfaceT PrivateRxReceiverInterface =
{
	.EventListener = (xRxReceiverEventListenerT)PrivateRxReceiverEventListener
};
//------------------------------------------------------------------------------
xResult MqttPortAdapterInit(xPortT* port, MqttPortAdapterInitT* init)
{
	if (port && init)
	{
		MqttPortAdapterT* adapter = init->Adapter;

		port->Adapter = (xPortAdapterBaseT*)adapter;

		port->Adapter->Base.Note = nameof(MqttPortAdapterT);
		port->Adapter->Base.Parent = port;
		
		port->Adapter->Interface = &PrivatePortInterface;

		xRxReceiverInit(&adapter->RxReceiver, 
						port,
						&PrivateRxReceiverInterface,
						init->RxBuffer,
						init->RxBufferSize);

		xDataBufferInit(&adapter->TxBuffer,
						port,
						0,
						init->TxBuffer,
						init->TxBufferSize);

		adapter->TransactionMutex = xSemaphoreCreateMutex();
		
		return xResultAccept;
	}
  
  return xResultError;
}
//==============================================================================
