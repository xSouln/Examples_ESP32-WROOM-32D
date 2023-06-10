//==============================================================================
//includes:

#include "ADC-Adapter.h"
#include "hal/adc_hal_common.h"
#include "hal/adc_hal.h"

#include "esp_adc/adc_oneshot.h"
//==============================================================================
//variables:

//==============================================================================
//functions:

static void PrivateHandler(xADC_T* adc)
{
	register ADC_AdapterT* adapter = (ADC_AdapterT*)adc->Adapter;
	esp_err_t ret;
	uint32_t ret_num = 0;
	uint8_t index;
	uint8_t channel_index;

	static uint32_t time_stamp = 0;

	if (!adc->Handle || !adc->NotifiedChannels)
	{
		return;
	}

	/*
	ret = adc_continuous_read(adapter->Internal.Handle, (uint8_t*)adapter->Internal.RxBuffer, SIZE_ARRAY(adapter->Internal.RxBuffer), &ret_num, 0);

	if (ret == ESP_OK)
	{
		for (int i = 0; i < ret_num; i++)
		{
			switch (adapter->Internal.RxBuffer[i].type1.channel)
			{
				case ADC_CHANNEL_6:
					break;
				
				default:
					continue;
			}

			uint16_t point = adapter->Internal.RxBuffer[i].type1.data;
			uint32_t value = 0;
			value |= point;

			xCircleBufferAddObject(&adc->Points->Buffer, &value, 1, 0, 0);
		}
	}
	*/

	int value = 0;
	adc_oneshot_read(adc->Handle, ADC_CHANNEL_6, &value);

	xCircleBufferAddObject(&adc->Points->Buffer, &value, 1, 0, 0);

	xSystemDelay(PrivateHandler, 1);
}
//------------------------------------------------------------------------------
static void PrivateIRQ(xADC_T* object, void* arg)
{
	//register ADC_AdapterT* adapter = (UsartPortAdapterT*)port->Adapter;
}
//------------------------------------------------------------------------------
static xResult PrivateRequestInitChannels(xADC_T* adc, xADC_RequestInitChannelsT* request)
{
	register ADC_AdapterT* adapter = (ADC_AdapterT*)adc->Adapter;

	uint16_t channels = request->Channels;
	uint8_t i = 0;
	uint8_t channels_count = 0;
	/*
	adc_continuous_config_t dig_cfg =
	{
		.sample_freq_hz = SOC_ADC_SAMPLE_FREQ_THRES_LOW,
		.conv_mode = ADC_CONV_SINGLE_UNIT_1,
		.format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
	};
	
	adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = { 0 };

	while (i < adc->NumberOfChannels && channels)	
	{
		if (channels & 0x1)
		{
			adc_pattern[channels_count].atten = ADC_ATTEN_DB_0;
			adc_pattern[channels_count].channel = i;
			adc_pattern[channels_count].unit = adapter->ADC_Unit;
			adc_pattern[channels_count].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
			
			channels_count++;
		}

		i++;
		channels >>= 1;
	}

	dig_cfg.adc_pattern = adc_pattern;
	dig_cfg.pattern_num = channels_count;

	ESP_ERROR_CHECK(adc_continuous_config(adapter->Internal.Handle, &dig_cfg));
	*/

	adc_oneshot_chan_cfg_t config =
	{
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_0,
    };

	while (i < adc->NumberOfChannels && channels)	
	{
		if (channels & 0x1)
		{
			adc_oneshot_config_channel(adc->Handle, i, &config);
			
			channels_count++;
		}

		i++;
		channels >>= 1;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static xResult PrivateRequestListener(xADC_T* adc, xADC_RequestSelector selector, void* arg)
{
	register ADC_AdapterT* adapter = (ADC_AdapterT*)adc->Adapter;

	switch ((uint32_t)selector)
	{
		case xADC_RequestStart:
			//return adc_continuous_start(adapter->Internal.Handle) == ESP_OK ? xResultAccept : xResultError;
			break;

		case xADC_RequestStop:
			//adc_continuous_stop(adapter->Internal.Handle);
			break;

		case xADC_RequestInitChannels:
			return PrivateRequestInitChannels(adc, arg);

		case xADC_RequestGetNumberOfNewPoints:
			*((uint32_t*)arg) = xCircleBufferGetFilledSize(&adc->Points->Buffer);
			break;

		case xADC_RequestGetPoints:
			return xResultNotSupported;

		default : return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void PrivateEventListener(xADC_T* object, xADC_EventSelector selector, void* arg)
{
	//register ADC_AdapterT* adapter = (ADC_AdapterT*)port->Adapter;

	switch((int)selector)
	{
		default: return;
	}
}
//==============================================================================
//initializations:

static xADC_InterfaceT PrivatePortInterface =
{
	.Handler = (xADC_HandlerT)PrivateHandler,
	.IRQ = (xADC_IRQT)PrivateIRQ,

	.RequestListener = (xADC_RequestListenerT)PrivateRequestListener,
	.EventListener = (xADC_EventListenerT)PrivateEventListener,
};
//------------------------------------------------------------------------------
xResult ADC_AdapterInit(xADC_T* adc, ADC_AdapterInitializationT* initialization)
{
	if (adc && initialization)
	{
		ADC_AdapterT* adapter = initialization->Adapter;

		adc->Adapter = (xADC_AdapterBaseT*)adapter;

		adc->Adapter->Base.Note = nameof(ADC_AdapterT);
		adc->Adapter->Base.Parent = adc;
		
		adc->Adapter->Interface = &PrivatePortInterface;

		adc->Points = initialization->PointsBuffer;
		adc->NumberOfPointsBuffer = 1;

		adapter->ADC_Unit = initialization->ADC_Unit;
		/*
		adc_continuous_handle_cfg_t adc_config =
		{
			.max_store_buf_size = 10240,
			.conv_frame_size = 64,
    	};

		ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &adapter->Internal.Handle));
		adc->Handle = adapter->Internal.Handle;

		adc_continuous_register_event_callbacks(adc->Handle, &adc_continuous_evt, adc);
		*/

		adc_oneshot_unit_init_cfg_t init_config =
		{
			.unit_id = initialization->ADC_Unit,
			.ulp_mode = ADC_ULP_MODE_DISABLE,
		};

		adc_oneshot_new_unit(&init_config, &adc->Handle);

		xCircleBufferInit(&adc->Points->Buffer,
				initialization->PointsMemmory,
				initialization->SizeOfPointsMemmory / SIZE_ELEMENT(initialization->PointsMemmory) / initialization->ChannelsCount - 1,
				SIZE_ELEMENT(initialization->PointsMemmory) * initialization->ChannelsCount);
		
		return xResultAccept;
	}
  
  return xResultError;
}
//==============================================================================
