//==============================================================================
//header:

#ifndef _LWIP_NET_PORT_ADAPTER_H_
#define _LWIP_NET_PORT_ADAPTER_H_
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
#include "Common/xNet/xNet.h"
//==============================================================================
//types:

//------------------------------------------------------------------------------
typedef struct
{
	xPortAdapterBaseT Base;

	xRxReceiverT RxReceiver;
	xDataBufferT TxBuffer;

	uint8_t* RxOperationBuffer;
	int RxOperationBufferSize;

	SemaphoreHandle_t TransactionMutex;

} LWIP_NetPortAdapterT;
//------------------------------------------------------------------------------
typedef struct
{
	LWIP_NetPortAdapterT* Adapter;

	uint8_t* RxOperationBuffer;
	int RxOperationBufferSize;

	uint8_t* TxBuffer;
	int TxBufferSize;

	uint8_t* RxBuffer;
	int RxBufferSize;

} LWIP_NetPortAdapterInitT;
//==============================================================================
//functions:

xResult LWIP_NetPortAdapterInit(xPortT* port, LWIP_NetPortAdapterInitT* adapter);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_LWIP_NET_PORT_ADAPTER_H_
