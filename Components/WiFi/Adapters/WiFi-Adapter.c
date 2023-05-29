//==============================================================================
//includes:

#include "WiFi-Adapter.h"
#include "Communications/xWiFi-RxEvents.h"

#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event_loop.h"

#include "Components.h"
//==============================================================================
//variables:

static const char *TAG = "wifi adapter";
//==============================================================================
//prototypes:

static xResult PrivateRequestSetConfig(xWiFi_T* wifi, xWiFi_ConfigT* request);
static xResult PrivateRequestEnable(xWiFi_T* wifi);
static xResult PrivateRequestDisable(xWiFi_T* wifi);
static xResult PrivateRequestInit(xWiFi_T* wifi);
//==============================================================================
//functions:

static void PrivateHandler(xWiFi_T* wifi)
{
	
}
//------------------------------------------------------------------------------
static void PrivateIRQ(xWiFi_T* wifi, void* arg)
{
	//register ADC_AdapterT* adapter = (UsartPortAdapterT*)port->Adapter;
}
//------------------------------------------------------------------------------
static xResult PrivateRequestListener(xWiFi_T* wifi, xWiFi_RequestSelector selector, void* arg)
{
	register WiFi_AdapterT* adapter = (WiFi_AdapterT*)wifi->Adapter;

	switch ((uint32_t)selector)
	{
		case xWiFi_RequestSetConfig:
			return PrivateRequestSetConfig(wifi, arg);

		case xWiFi_RequestEnable:
			return PrivateRequestEnable(wifi);

		case xWiFi_RequestDisable:
			return PrivateRequestDisable(wifi);

		case xWiFi_RequestInit:
			return PrivateRequestInit(wifi);

		case xWiFi_RequestGetAddress:
			memcpy(arg, &adapter->Internal.address, sizeof(adapter->Internal.address));
			break;

		default : return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void PrivateEventListener(xWiFi_T* wifi, xWiFi_EventSelector selector, void* arg)
{
	switch((int)selector)
	{
		default: return;
	}
}
//------------------------------------------------------------------------------
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	xWiFi_T* wifi = arg;
	WiFi_AdapterT* adapter = (WiFi_AdapterT*)wifi->Adapter;

	if (event_base == WIFI_EVENT)
	{
		switch (event_id)
		{
			case WIFI_EVENT_STA_START:
				esp_wifi_connect();
				wifi->Status.IsStarted = true;
				wifi->Status.State = xWiFi_StateConnecting;
				break;

			case WIFI_EVENT_STA_DISCONNECTED:
				esp_wifi_connect();
				wifi->Status.State = xWiFi_StateConnecting;
				break;

			case WIFI_EVENT_STA_STOP:
				wifi->Status.IsStarted = false;
				break;
			
			default:
				break;
		}
	}
	else if (event_base == IP_EVENT)
	{
		switch (event_id)
		{
			case IP_EVENT_STA_GOT_IP:
				ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
				
				adapter->Internal.address.Ip = event->ip_info.ip.addr;
				adapter->Internal.address.Netmask = event->ip_info.netmask.addr;
				adapter->Internal.address.Gateway = event->ip_info.gw.addr;

				wifi->Status.State = xWiFi_StateConnected;
				break;

			case IP_EVENT_STA_LOST_IP:
				memset(&adapter->Internal.address, 0, sizeof(adapter->Internal.address));

				wifi->Status.State = xWiFi_StateIdle;
				break;
			
			default:
				break;
		}
	}
}
//------------------------------------------------------------------------------
static xResult PrivateRequestSetConfig(xWiFi_T* wifi, xWiFi_ConfigT* request)
{
	wifi_config_t wifi_config =
	{
        .sta =
		{
            .threshold =
			{
				.authmode = WIFI_AUTH_WPA2_PSK
			},
			/*
            .pmf_cfg =
			{
                .capable = true,
                .required = false
            },
			*/
        },
    };

	memcpy(wifi_config.sta.ssid, request->SSID, sizeof(wifi_config.sta.ssid));
	memcpy(wifi_config.sta.password, request->Password, sizeof(wifi_config.sta.password));

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

	return xResultAccept;
}
//------------------------------------------------------------------------------
static xResult PrivateRequestEnable(xWiFi_T* wifi)
{
	esp_err_t err = esp_wifi_start();
	wifi->Status.IsEnable = err == ESP_OK;
	
	return err == ESP_OK ? xResultAccept : xResultError;
}
//------------------------------------------------------------------------------
static xResult PrivateRequestDisable(xWiFi_T* wifi)
{
	esp_err_t err = esp_wifi_stop();
	wifi->Status.IsEnable = false;

	return err == ESP_OK ? xResultAccept : xResultError;
}
//------------------------------------------------------------------------------
static xResult PrivateRequestInit(xWiFi_T* wifi)
{
	WiFi_AdapterT* adapter = (WiFi_AdapterT*)wifi->Adapter;

	esp_netif_init();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	
	adapter->Internal.sta_netif = esp_netif_create_default_wifi_sta();
	assert(adapter->Internal.sta_netif);

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, wifi));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, wifi));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_LOST_IP, &event_handler, wifi));

	wifi->Status.IsInit = true;

	return xResultAccept;
}
//==============================================================================
//initializations:

static xWiFi_InterfaceT PrivateInterface =
{
	.Handler = (xWiFi_HandlerT)PrivateHandler,
	.IRQ = (xWiFi_IRQT)PrivateIRQ,

	.RequestListener = (xWiFi_RequestListenerT)PrivateRequestListener,
	.EventListener = (xWiFi_EventListenerT)PrivateEventListener,
};
//------------------------------------------------------------------------------
xResult WiFi_AdapterInit(xWiFi_T* wifi, WiFi_AdapterInitT* init)
{
	if (!wifi || !init || !init->Adapter)
	{
		return xResultLinkError;
	}

	wifi->Adapter = init->Adapter;

	wifi->Adapter->Base.Parent = wifi;
	wifi->Adapter->Base.Note = nameof(WiFi_AdapterT);

	wifi->Adapter->Interface = &PrivateInterface;
  
  return xResultAccept;
}
//==============================================================================
