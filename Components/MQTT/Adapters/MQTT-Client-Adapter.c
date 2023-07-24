//==============================================================================
//includes:

#include "MQTT-Port-Adapter.h"
#include "mqtt_client.h"
//==============================================================================
//functions:

static void PrivateHandler(xMqttClientT* client)
{
	
}
//------------------------------------------------------------------------------
static void PrivateIRQ(xMqttClientT* client, void* arg)
{

}
//------------------------------------------------------------------------------
static xResult PrivateRequestListener(xMqttClientT* client, xPortRequestSelector selector, void* arg)
{
	//register MqttPortAdapterT* adapter = (MqttPortAdapterT*)port->Adapter;

	switch ((int)selector)
	{
		default : return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void PrivateEventListener(xMqttClientT* client, xPortEventSelector selector, void* arg)
{
	//register UsartPortAdapterT* adapter = (UsartPortAdapterT*)port->Adapter;

	switch((int)selector)
	{
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
};
//------------------------------------------------------------------------------
xResult MqttClientAdapterInit(xMqttClientT* port, MqttClientAdapterInitT* adapter)
{
	if (port && init)
	{
		MqttPortAdapterT* adapter = init->Adapter;

		port->Adapter = (xPortAdapterBaseT*)adapter;

		port->Adapter->Base.Note = nameof(MqttPortAdapterT);
		port->Adapter->Base.Parent = port;
		
		port->Adapter->Interface = &PrivatePortInterface;
		
		return xResultAccept;
	}
  
  return xResultError;
}
//==============================================================================
