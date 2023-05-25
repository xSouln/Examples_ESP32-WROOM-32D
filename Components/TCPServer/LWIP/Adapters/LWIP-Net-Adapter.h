//==============================================================================
//header:

#ifndef _LWIP_NET_ADAPTER_H_
#define _LWIP_NET_ADAPTER_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Common/xNet/xNet.h"
//==============================================================================
//types:

typedef struct
{
	uint8_t idle;

} LWIP_NetAdapterInternalT;
//------------------------------------------------------------------------------
typedef struct
{
	xNetAdapterBaseT Base;

	LWIP_NetAdapterInternalT Internal;

} LWIP_NetAdapterT;
//------------------------------------------------------------------------------
typedef struct
{
	LWIP_NetAdapterT* Adapter;
	
} LWIP_NetAdapterInitT;
//==============================================================================
//functions:

xResult LWIP_NetAdapterInit(xNetT* net, LWIP_NetAdapterInitT* init);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_LWIP_NET_ADAPTER_H_
