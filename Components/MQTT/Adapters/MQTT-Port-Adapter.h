//==============================================================================
//header:

#ifndef _MQTT_PORT_ADAPTER_H_
#define _MQTT_PORT_ADAPTER_H_
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

	xRxReceiverT RxReceiver;
	xDataBufferT TxBuffer;

	SemaphoreHandle_t TransactionMutex;

} MqttPortAdapterT;
//------------------------------------------------------------------------------
typedef struct
{
	MqttPortAdapterT* Adapter;

	uint8_t* TxBuffer;
	int TxBufferSize;

	uint8_t* RxBuffer;
	int RxBufferSize;

} MqttPortAdapterInitT;
//==============================================================================
//functions:

xResult MqttPortAdapterInit(xPortT* port, MqttPortAdapterInitT* adapter);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_MQTT_PORT_ADAPTER_H_
