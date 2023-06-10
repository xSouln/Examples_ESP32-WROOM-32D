//==============================================================================
//header:

#ifndef _ADC_ADAPTER_H_
#define _ADC_ADAPTER_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Common/xADC/xADC.h"
#include "Common/xRxReceiver.h"
#include "Common/xDataBuffer.h"
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_oneshot.h"
//==============================================================================
//types:

typedef struct
{
	struct adc_continuous_ctx_t* Handle;
	adc_digi_output_data_t RxBuffer[128];

} ADC_AdapterInternalT;
//------------------------------------------------------------------------------
typedef struct
{
	xADC_AdapterBaseT Base;

	ADC_AdapterInternalT Internal;

	uint8_t ADC_Unit;

} ADC_AdapterT;
//------------------------------------------------------------------------------
typedef struct
{
	ADC_AdapterT* Adapter;

	xADC_PointsT* PointsBuffer;

	uint16_t* PointsMemmory;
	uint32_t SizeOfPointsMemmory;

	uint8_t ChannelsCount;
	uint8_t ADC_Unit;

} ADC_AdapterInitializationT;
//==============================================================================
//functions:

xResult ADC_AdapterInit(xADC_T* object, ADC_AdapterInitializationT* initialization);
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_ADC_ADAPTER_H_
