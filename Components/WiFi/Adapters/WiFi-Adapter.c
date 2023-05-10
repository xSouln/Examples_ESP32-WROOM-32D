//==============================================================================
//includes:

#include "WiFi-Adapter.h"

#include "esp_mac.h"
#include "lwip/err.h"
#include "lwip/sys.h"
//==============================================================================
//variables:

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
	//register ADC_AdapterT* adapter = (ADC_AdapterT*)adc->Adapter;

	switch ((uint32_t)selector)
	{
		default : return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void PrivateEventListener(xWiFi_T* wifi, xWiFi_EventSelector selector, void* arg)
{
	//register ADC_AdapterT* adapter = (ADC_AdapterT*)port->Adapter;

	switch((int)selector)
	{
		default: return;
	}
}
//------------------------------------------------------------------------------
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id)
	{
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "Wi-Fi connected");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Got IP address: %s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "Wi-Fi disconnected");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
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
xResult WiFi_AdapterInit(xWiFi_T* wifi, WiFi_AdapterT* adapter, WiFi_AdapterInitializationT* initialization)
{
	if (!wifi || !adapter || !initialization)
	{
		return xResultLinkError;
	}

	wifi->Adapter = adapter;

	wifi->Adapter->Base.Parent = wifi;
	wifi->Adapter->Base.Note = nameof(WiFi_AdapterT);

	ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  
  return xResultError;
}
//==============================================================================
