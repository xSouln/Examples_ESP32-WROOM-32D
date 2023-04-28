//==============================================================================
//header:

#ifndef _SFC_SPI_NUMBERS_H_
#define _SFC_SPI_NUMBERS_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "sfc_spi_config.h"
#include "driver/spi_master.h"
//==============================================================================
//types:

typedef enum
{

#if	defined(SFC_SPI1_ENABLE)
	SFC_SPI1,
#endif

#if	defined(SFC_SPI2_ENABLE)
	SFC_SPI2,
#endif

#if	defined(SPI3_HOST) && defined(SFC_SPI3_ENABLE)
	SFC_SPI3,
#endif

#if	defined(SPI4_HOST) && defined(SFC_SPI4_ENABLE)
	SFC_SPI4,
#endif

#if	defined(SPI5_HOST) && defined(SFC_SPI5_ENABLE)
	SFC_SPI5,
#endif

#if	defined(SPI6_HOST) && defined(SFC_SPI6_ENABLE)
	SFC_SPI6,
#endif

#if	defined(SPI7_HOST) && defined(SFC_SPI7_ENABLE)
	SFC_SPI7,
#endif

#if	defined(SPI8_HOST) && defined(SFC_SPI8_ENABLE)
	SFC_SPI8,
#endif

#if	defined(SPI9_HOST) && defined(SFC_SPI9_ENABLE)
	SFC_SPI9,
#endif

#if	defined(SPI10_HOST) && defined(SFC_SPI10_ENABLE)
	SFC_SPI10,
#endif

#if	defined(SPI11_HOST) && defined(SFC_SPI11_ENABLE)
	SFC_SPI11,
#endif

#if	defined(SPI12_HOST) && defined(SFC_SPI12_ENABLE)
	SFC_SPI12,
#endif

	SFC_SPI_COUNT

} sfc_spi_number_t;
//------------------------------------------------------------------------------
#ifdef SFC_SPI1

#endif
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_SFC_SPI_CORE_H_
