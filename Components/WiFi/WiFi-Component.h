//==============================================================================
//header:

#ifndef _WIFI_COMPONENT_H_
#define _WIFI_COMPONENT_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Components_Types.h"
#include "WiFi-ComponentConfig.h"
#include "Common/xWiFi/xWiFi.h"
//==============================================================================
//defines:


//==============================================================================
//internal:

void Internal_WiFi_ComponentHandler();
void Internal_WiFi_ComponentTimeSynchronization();
//==============================================================================
//functions:

xResult WiFi_ComponentInit(void* parent);
//==============================================================================
//overrides:

#define WiFi_ComponentHandler()
#define WiFi_ComponentTimeSynchronization()
//==============================================================================
//export:

extern xWiFi_T mWifi;
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_WIFI_COMPONENT_H_
