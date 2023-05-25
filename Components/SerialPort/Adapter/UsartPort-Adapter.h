//==============================================================================
//header:

#ifndef _USART_PORT_ADAPTER_H_
#define _USART_PORT_ADAPTER_H_
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
//==============================================================================
//types:

typedef struct
{
	//xDataBufferT TxBuffer;
	xCircleBufferT RxCircleBuffer;
	xCircleBufferT TxCircleBuffer;
	
	QueueHandle_t handle_queue;
	SemaphoreHandle_t TransactionMutex;

} UsartPortAdapterDataT;
//------------------------------------------------------------------------------
typedef struct
{
	xPortAdapterBaseT Base;

	UsartPortAdapterDataT Data;

	const uart_config_t Config;

	uint8_t* RxOperationBuffer;
	uint16_t RxOperationBufferSize;

	uint8_t* TxOperationBuffer;
	uint16_t TxOperationBufferSize;

	xRxReceiverT RxReceiver;

	const uint8_t UART_NUM;

	const int RxPinNumber;
	const int TxPinNumber;

} UsartPortAdapterT;
//==============================================================================
//functions:

xResult UsartPortAdapterInit(xPortT* port, UsartPortAdapterT* adapter);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_USART_PORT_ADAPTER_H_
