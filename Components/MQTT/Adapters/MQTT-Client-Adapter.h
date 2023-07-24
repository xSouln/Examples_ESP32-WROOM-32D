//==============================================================================
//header:

#ifndef _MQTT_CLIENT_ADAPTER_H_
#define _MQTT_CLIENT_ADAPTER_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Components_Types.h"
#include "Common/xPort/xPort.h"
#include "Common/xRxReceiver.h"
#include "Common/xDataBuffer.h"
#include "Common/xMQTT/xMQTT.h"
//==============================================================================
//types:

//------------------------------------------------------------------------------
typedef struct
{
	xPortAdapterBaseT Base;

} MqttClientAdapterT;
//------------------------------------------------------------------------------
typedef struct
{
	MqttPortAdapterT* Adapter;

} MqttClientAdapterInitT;
//==============================================================================
//functions:

xResult MqttClientAdapterInit(xMqttClientT* port, MqttClientAdapterInitT* adapter);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_MQTT_CLIENT_ADAPTER_H_
