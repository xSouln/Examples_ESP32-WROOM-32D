//==============================================================================
//includes:

#include "Components.h"
//==============================================================================
//import:


//==============================================================================
//variables:

static uint32_t led_toggle_time_stamp;
//==============================================================================
//functions:

void ComponentsEventListener(ObjectBaseT* object, int selector, void* arg)
{
	if (object->Description->Key == OBJECT_DESCRIPTION_KEY)
	{
		if (object->Description->ObjectId == TERMINAL_OBJECT_ID)
		{
			
		}
	}
}
//------------------------------------------------------------------------------

xResult ComponentsRequestListener(ObjectBaseT* object, int selector, void* arg)
{
	switch((int)selector)
	{
		default: return xResultNotSupported;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
/**
 * @brief main handler
 */
void ComponentsHandler()
{
	TerminalComponentHandler();
	UsartPortComponentHandler();
	WiFi_ComponentHandler();
	MQTT_ComponentHandler();
	//ADC_ComponentHandler();
	//TransferLayerComponentHandler();

	if (xSystemGetTime(ComponentsHandler) - led_toggle_time_stamp > 999)
	{
		led_toggle_time_stamp = xSystemGetTime(ComponentsHandler);
	}
}
//------------------------------------------------------------------------------
/**
 * @brief time synchronization of time-dependent processes
 */
void ComponentsTimeSynchronization()
{
	TerminalComponentTimeSynchronization();
	UsartPortComponentTimeSynchronization();
	//ADC_ComponentTimeSynchronization();
	WiFi_ComponentTimeSynchronization();
}
//------------------------------------------------------------------------------
/**
 * @brief initializing the component
 * @param parent binding to the parent object
 * @return int
 */
xResult ComponentsInit(void* parent)
{
	TerminalComponentInit(parent);
	xSystemInit(parent);
	
	UsartPortComponentInit(parent);
	WiFi_ComponentInit(parent);

	LWIP_NetTcpServerComponentInit(parent);
	MQTT_ComponentInit(parent);
	//TransferLayerComponentInit(parent);

	ADC_ComponentInit(parent);

	return xResultAccept;
}
//==============================================================================
