//==============================================================================
//header:

#ifndef _SFC_SPI_STM32H7XX_ADAPTER_H_
#define _SFC_SPI_STM32H7XX_ADAPTER_H_
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "sfc_spi/controls/sfc_spi.h"
#include "sfc_spi_numbers.h"
#include "driver/spi_master.h"
//==============================================================================
//types:

typedef struct
{
	uint32_t transfer_start_time_stamp;
	spi_device_handle_t handle;
	spi_device_interface_config_t config;

} sfc_spi_adapter_data_t;
//------------------------------------------------------------------------------

typedef struct
{
	//the basic representation initialized in the adapter
	sfc_spi_adapter_base_t base;

	//additional variables required for the operation of the adapter logic, does not require initialization
	sfc_spi_adapter_data_t data;

	sfc_spi_number_t spi_number;
	
	uint32_t cs_pin;
	//sfc_spi_pin_mux_t pin_mux;

} sfc_spi_adapter_t;
//==============================================================================
//functions:

sfc_spi_result_t sfc_spi_adapter_init(sfc_spi_t* spi, sfc_spi_adapter_t* adapter);

void sfc_spi_core_init();
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_SFC_SPI_STM32H7XX_ADAPTER_H_
