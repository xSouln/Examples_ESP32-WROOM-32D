//==============================================================================
//includes:

#include "WiFi-Component.h"
#include "Adapters/WiFi-Adapter.h"
//==============================================================================
//defines:

#define WIFI_SSID "TP-LINK_F018"
#define WIFI_Password "94317749"

#define WIFI_TASK_STACK_SIZE 0x1000
//==============================================================================
//variables:

xWiFi_T mWifi;

static TaskHandle_t wifi_task_handle;
static StaticTask_t wifi_task_buffer;
static StackType_t wifi_task_stack[WIFI_TASK_STACK_SIZE];
//==============================================================================
//functions:

static void EventListener(xWiFi_T* wifi, xWiFi_ObjectEventSelector selector, void* arg)
{
	switch((int)selector)
	{
		default: break;
	}
}
//------------------------------------------------------------------------------

static xResult RequestListener(xWiFi_T* wifi, xWiFi_ObjectRequestSelector selector, void* arg)
{
	switch ((int)selector)
	{
		default: return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
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
static xWiFi_ObjectInterfaceT PrivateSysInterface =
{
	.RequestListener = (xObjectRequestListenerT)RequestListener,
	.EventListener = (xObjectEventListenerT)EventListener
};

//==============================================================================
static void WiFi_Task(void* arg)
{
	WiFi_AdapterInitializationT adapter_init;
	WiFi_AdapterInit(&mWifi, &WiFi_Adapter, &adapter_init);

	xWiFi_InitializationT init =
	{
		.Parent = WiFi_Task,
		.Number = 0,
		.Interface = &PrivateSysInterface
	};

	xWiFi_Init(&mWifi, &init);

	xWiFi_ConfigT config =
	{ 
		.SSID = WIFI_SSID,
		.Password = WIFI_Password
	};

	xWiFi_SetConfig(&mWifi, &config);

	while (true)
	{
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
//------------------------------------------------------------------------------

xResult WiFi_ComponentInit(void* parent)
{
	/*
	wifi_task_handle =
			xTaskCreateStatic(WiFi_Task, // Function that implements the task.
								"WiFi Task", // Text name for the task.
								WIFI_TASK_STACK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								5, // Priority at which the task is created.
								wifi_task_stack, // Array to use as the task's stack.
								&wifi_task_buffer);
								*/

	WiFi_AdapterInitializationT adapter_init;
	WiFi_AdapterInit(&mWifi, &WiFi_Adapter, &adapter_init);

	xWiFi_InitializationT init =
	{
		.Parent = WiFi_Task,
		.Number = 0,
		.Interface = &PrivateSysInterface
	};

	xWiFi_Init(&mWifi, &init);

	xWiFi_ConfigT config =
	{ 
		.SSID = WIFI_SSID,
		.Password = WIFI_Password
	};

	xWiFi_SetConfig(&mWifi, &config);

  return xResultAccept;
}
//==============================================================================
