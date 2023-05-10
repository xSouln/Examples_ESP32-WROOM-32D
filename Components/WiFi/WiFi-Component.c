//==============================================================================
//includes:

#include "WiFi-Component.h"
#include "Adapters/WiFi-Adapter.h"
//==============================================================================
//defines:


//==============================================================================
//variables:

xWiFi_T mWifi;
//==============================================================================
//functions:

//------------------------------------------------------------------------------
/**
 * @brief main handler
 */
void Internal_WiFi_ComponentHandler()
{

}
//------------------------------------------------------------------------------
/**
 * @brief time synchronization of time-dependent processes
 */
void Internal_WiFi_ComponentTimeSynchronization()
{

}
//==============================================================================
//initializations:

WiFi_AdapterT WiFi_Adapter;
//------------------------------------------------------------------------------

xResult WiFi_ComponentInit(void* parent)
{
	WiFi_AdapterInitializationT adapter_init;
	WiFi_AdapterInit(&mWifi, &WiFi_Adapter, &adapter_init);

	xWiFi_InitializationT init =
	{
		.Parent = parent,

		.SSID = "TP-LINK_F018",
		.Password = "94317749",
	}
	xWiFi_Init(&mWifi, &init);

  return xResultAccept;
}
//==============================================================================
