//==============================================================================
//includes:

#include "sfc_spi_core.h"
#include "sfc_spi/controls/sfc_spi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
//==============================================================================
//import:

//==============================================================================
//variables:

extern sfc_spi_handle_t sfc_spi_handles[];

static SemaphoreHandle_t sfc_spi_core_mutex;
//==============================================================================
//functions:

//------------------------------------------------------------------------------
void sfc_spi_core_init()
{
	sfc_spi_core_mutex = xSemaphoreCreateMutex();

#if	defined(SFC_SPI1_ENABLE)

	spi_bus_config_t buscfg =
	{
        .miso_io_num = sfc_spi_handles[SFC_SPI1].total_pin_mux.miso_pin,
        .mosi_io_num = sfc_spi_handles[SFC_SPI1].total_pin_mux.mosi_pin,
        .sclk_io_num = sfc_spi_handles[SFC_SPI1].total_pin_mux.sck_pin,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = sfc_spi_handles[SFC_SPI1].max_transfer_size,
    };

    //initialize the spi bus
    spi_bus_initialize(sfc_spi_handles[SFC_SPI1].instance, &buscfg, SPI_DMA_CH_AUTO);
#endif
}
//------------------------------------------------------------------------------
void sfc_spi_core_lock()
{
	xSemaphoreTake(sfc_spi_core_mutex, portMAX_DELAY);
}
//------------------------------------------------------------------------------
void sfc_spi_core_unlock()
{
	xSemaphoreGive(sfc_spi_core_mutex);
}
//------------------------------------------------------------------------------
sfc_spi_result_t sfc_spi_core_clock_enable(sfc_spi_t* spi)
{
	return SFC_SPI_RESULT_NOT_SUPORTED;
}
//------------------------------------------------------------------------------
sfc_spi_result_t sfc_spi_core_clock_disable(sfc_spi_t* spi)
{
	return SFC_SPI_RESULT_NOT_SUPORTED;
}
//------------------------------------------------------------------------------
sfc_spi_result_t sfc_spi_core_interrupt_enable(sfc_spi_t* spi, uint8_t priority, uint8_t sub_priority)
{
	return SFC_SPI_RESULT_NOT_SUPORTED;
}
//------------------------------------------------------------------------------
sfc_spi_result_t sfc_spi_core_interrupt_disable(sfc_spi_t* spi)
{
	return SFC_SPI_RESULT_NOT_SUPORTED;
}
//------------------------------------------------------------------------------
void sfc_spi_core_lock_bus(sfc_spi_t* spi)
{
	sfc_spi_handle_t* handle = spi->handle;

	sfc_spi_core_lock();

	if (handle->spi)
	{
		spi->state.bus_is_locked = true;
	}

	sfc_spi_core_unlock();
}
//------------------------------------------------------------------------------
void sfc_spi_core_unlock_bus(sfc_spi_t* spi)
{
	spi->state.bus_is_locked = true;
}
//------------------------------------------------------------------------------
//called after the transfer is completed
sfc_spi_result_t sfc_spi_core_give_bus(sfc_spi_t* spi)
{
	sfc_spi_handle_t* handle = spi->handle;

	//spi_device_acquire_bus(ctx->spi, portMAX_DELAY);

	sfc_spi_core_lock();

	if (!spi->state.bus_is_locked)
	{
		handle->spi = 0;
		spi->state.bus_is_own = false;
	}

	spi->state.is_transmitting = false;

	sfc_spi_core_unlock();

	return SFC_SPI_RESULT_OK;
}
//------------------------------------------------------------------------------
//binding "spi" to a handler for an asynchronous request
//upon completion of the asynchronous request, the sfc_spi_core_give_bus command should be called
sfc_spi_result_t sfc_spi_core_take_bus(sfc_spi_t* spi, uint32_t time_out)
{
	sfc_spi_handle_t* handle = spi->handle;

	if (xSemaphoreTake(sfc_spi_core_mutex, time_out) != pdTRUE)
	{
		return SFC_SPI_RESULT_TIMEOUT;
	}

	handle->spi = spi;
	handle->irq = spi->adapter->interface->irq;
	spi->state.bus_is_own = true;

	xSemaphoreGive(sfc_spi_core_mutex);

	return SFC_SPI_RESULT_OK;
}
//------------------------------------------------------------------------------
sfc_spi_result_t sfc_spi_core_bind_handle(sfc_spi_t* spi, sfc_spi_number_t number)
{
	spi->handle = sfc_spi_handles + number;

	return SFC_SPI_RESULT_OK;
}
//------------------------------------------------------------------------------
sfc_spi_handle_t sfc_spi_handles[] =
{
#if	defined(SFC_SPI1_ENABLE)
	[SFC_SPI1] =
	{
		.instance = VSPI_HOST,
		.max_transfer_size = SFC_SPI1_MAX_TRANSFER_SIZE,
		.total_pin_mux =
		{
			.sck_port = SFC_SPI1_SCK_PORT,
			.sck_pin = SFC_SPI1_SCK_PIN,

			.mosi_port = SFC_SPI1_MOSI_PORT,
			.mosi_pin = SFC_SPI1_MOSI_PIN,

			.miso_port = SFC_SPI1_MISO_PORT,
			.miso_pin = SFC_SPI1_MISO_PIN,

			.cs_port = SFC_SPI1_CS_PORT,
			.cs_pin = SFC_SPI1_CS_PIN,
		}
	},
#endif

#if	defined(SFC_SPI2_ENABLE)
	[SFC_SPI2] =
	{
		.instance = SPI2_HOST,
		.total_pin_mux =
		{
			.sck_port = SFC_SPI2_SCK_PORT,
			.sck_pin = SFC_SPI2_SCK_PIN,

			.mosi_port = SFC_SPI2_MOSI_PORT,
			.mosi_pin = SFC_SPI2_MOSI_PIN,

			.miso_port = SFC_SPI2_MISO_PORT,
			.miso_pin = SFC_SPI2_MISO_PIN,

			.cs_port = SFC_SPI2_CS_PORT,
			.cs_pin = SFC_SPI2_CS_PIN,
		}
	},
#endif

#if	defined(SFC_SPI3_ENABLE)
	[SFC_SPI3] =
	{
		.instance = SPI3_HOST,
		.total_pin_mux =
		{
			.sck_port = SFC_SPI3_SCK_PORT,
			.sck_pin = SFC_SPI3_SCK_PIN,

			.mosi_port = SFC_SPI3_MOSI_PORT,
			.mosi_pin = SFC_SPI3_MOSI_PIN,

			.miso_port = SFC_SPI3_MISO_PORT,
			.miso_pin = SFC_SPI3_MISO_PIN,

			.cs_port = SFC_SPI3_CS_PORT,
			.cs_pin = SFC_SPI3_CS_PIN,
		}
	},
#endif

#if	defined(SFC_SPI4_ENABLE)
	[SFC_SPI4] =
	{
		.instance = SPI4_HOST,
		.total_pin_mux =
		{
			.sck_port = SFC_SPI4_SCK_PORT,
			.sck_pin = SFC_SPI4_SCK_PIN,

			.mosi_port = SFC_SPI4_MOSI_PORT,
			.mosi_pin = SFC_SPI4_MOSI_PIN,

			.miso_port = SFC_SPI4_MISO_PORT,
			.miso_pin = SFC_SPI4_MISO_PIN,

			.cs_port = SFC_SPI4_CS_PORT,
			.cs_pin = SFC_SPI4_CS_PIN,
		}
	},
#endif
};
//==============================================================================
