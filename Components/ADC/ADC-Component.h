//==============================================================================
//header:

#ifndef _ADC_COMPONENT_H_
#define _ADC_COMPONENT_H_
//==============================================================================
#ifdef __cplusplus
extern "C" {
#endif
//==============================================================================
//includes:

#include "UsartPort-ComponentConfig.h"
#include "Common/xADC/xADC.h"
//==============================================================================
//defines:


//==============================================================================
//macros:


//==============================================================================
//functions:

xResult ADC_ComponentInit(void* parent);

void ADC_ComponentHandler();
void ADC_ComponentTimeSynchronization();
void ADC_ComponentIRQ();
//==============================================================================
//import:


//==============================================================================
//export:

extern xADC_T mADC;
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_ADC_COMPONENT_H_
