//==============================================================================
//includes:

#include "Components.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//==============================================================================
//import:

extern void blink_led(void);
//==============================================================================
//variables:

static uint8_t time1_ms;
static uint8_t time5_ms;

static uint32_t led_toggle_time_stamp;
static uint32_t sntp_update_time_stamp;

static uint32_t sys_time = 0;
//==============================================================================
//functions:

void ComponentsEventListener(ObjectBaseT* object, int selector, void* arg, ...)
{
	if (object->Description->Key == OBJECT_DESCRIPTION_KEY)
	{
		if (object->Description->ObjectId == TERMINAL_OBJECT_ID)
		{
			blink_led();
		}
	}
}
//------------------------------------------------------------------------------

xResult ComponentsRequestListener(ObjectBaseT* object, int selector, void* arg, ...)
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
	//SerialPortUARTComponentHandler();
	//SerialPortUSBComponentHandler();
	TCPServerWIZspiComponentHandler();
	TerminalComponentHandler();
	//TransferLayerComponentHandler();

	if (ComponentsSysGetTime() - led_toggle_time_stamp > 999)
	{
		led_toggle_time_stamp = ComponentsSysGetTime();
		//blink_led();
	}

	if (ComponentsSysGetTime() - sntp_update_time_stamp > 10000)
	{
		sntp_update_time_stamp = ComponentsSysGetTime();
	}
}
//------------------------------------------------------------------------------
/**
 * @brief time synchronization of time-dependent processes
 */
void ComponentsTimeSynchronization()
{
	sys_time++;

	TerminalComponentTimeSynchronization();
	//SerialPortUARTComponentTimeSynchronization();
	//SerialPortUSBComponentTimeSynchronization();
	TCPServerWIZspiComponentTimeSynchronization();
	//TransferLayerComponentTimeSynchronization();

	if (time5_ms)
	{
		time5_ms--;
	}
}
//------------------------------------------------------------------------------

void ComponentsSysDelay(uint32_t time)
{
	vTaskDelay(pdMS_TO_TICKS(time));
}
//------------------------------------------------------------------------------

uint32_t ComponentsSysGetTime()
{
	return sys_time;
}
//------------------------------------------------------------------------------

void ComponentsTrace(char* text)
{

}
//------------------------------------------------------------------------------

void ComponentsSysEnableIRQ()
{

}
//------------------------------------------------------------------------------

void ComponentsSysDisableIRQ()
{

}
//------------------------------------------------------------------------------

void ComponentsSysReset()
{

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
	//SerialPortUARTComponentInit(parent);
	//SerialPortUSBComponentInit(parent);
	TCPServerWIZspiComponentInit(parent);
	//TransferLayerComponentInit(parent);

	//TerminalTxBind(&SerialPortUART.Tx);

	return xResultAccept;
}
//==============================================================================
