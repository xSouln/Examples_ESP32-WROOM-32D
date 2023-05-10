//==============================================================================
//header:

#ifndef _WIFI_ADAPTER_H_
#define _WIFI_ADAPTER_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Common/xWiFi/xWiFi.h"
#include "esp_wifi.h"
//==============================================================================
//types:

typedef struct
{
	EventGroupHandle_t wifi_event_group;

} WiFi_AdapterInternalT;
//------------------------------------------------------------------------------
typedef struct
{
	xWiFi_AdapterBaseT Base;

	WiFi_AdapterInternalT Internal;

} WiFi_AdapterT;
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t Idle;
	
} WiFi_AdapterInitializationT;
//==============================================================================
//functions:

xResult WiFi_AdapterInit(xADC_T* wifi, WiFi_AdapterT* adapter, WiFi_AdapterInitializationT* initialization);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_WIFI_ADAPTER_H_