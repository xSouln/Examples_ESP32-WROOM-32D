//==============================================================================
//includes:

#include <string.h>
#include "sfc_spi_adapter.h"
#include "sfc_spi_core.h"
#include "driver/gpio.h"
//==============================================================================
//defines:

#define SFC_SPI_ADAPTER_TYPE 0x3C
#define SFC_SPI_ADAPTER_SUBTYPE 0x00
#define SFC_SPI_ADAPTER_ID 0x3468
//==============================================================================
//types:


//==============================================================================
//variables:

static uint32_t private_temp;
//==============================================================================
//functions:

static void private_lock()
{
	sfc_spi_core_lock();
}
//------------------------------------------------------------------------------

static void private_unlock()
{
	sfc_spi_core_unlock();
}
//------------------------------------------------------------------------------
//
static void private_handler(sfc_spi_t* spi)
{

}
//------------------------------------------------------------------------------
static void private_irq(sfc_spi_t* spi)
{

}
//------------------------------------------------------------------------------
static sfc_spi_result_t private_request_listener(sfc_spi_t* spi, sfc_spi_adapter_request_selector_t selector, void* arg)
{
	sfc_spi_adapter_t* adapter = (sfc_spi_adapter_t*)spi->adapter;

	switch((uint8_t)selector)
	{
		case SFC_SPI_ADAPTER_REQUEST_APDATE_STATE:

			break;

		case SFC_SPI_ADAPTER_REQUEST_CHIP_SELECT:
			gpio_set_level(adapter->cs_pin, 0);
			break;

		case SFC_SPI_ADAPTER_REQUEST_ABORT_TRANSFER:
			break;

		case SFC_SPI_ADAPTER_REQUEST_CHIP_DESELECT:
			gpio_set_level(adapter->cs_pin, 1);
			break;

		case SFC_SPI_ADAPTER_REQUEST_APPLY_OPTIONS:
			break;

		case SFC_SPI_ADAPTER_REQUEST_LOCK:
			private_lock();
			break;

		case SFC_SPI_ADAPTER_REQUEST_UNLOCK:
			private_unlock();
			break;

		default: return SFC_SPI_RESULT_NOT_SUPORTED;
	}

	return SFC_SPI_RESULT_OK;
}
//------------------------------------------------------------------------------
static void private_event_listener(sfc_spi_t* spi, sfc_spi_adapter_event_selector_t selector, void* arg)
{
	
}
//------------------------------------------------------------------------------
static int private_transfer(sfc_spi_t* spi, sfc_spi_transfer_t* request)
{
	sfc_spi_handle_t* handle = spi->handle;
	sfc_spi_adapter_t* adapter = (sfc_spi_adapter_t*)spi->adapter;

	//getting access to use the bus before the end of the transfer
	//spi_device_acquire_bus(&adapter->data.handle, portMAX_DELAY);
	uint8_t* tx_data = request->tx_data;
	uint8_t* rx_data = request->rx_data;
	uint32_t tx_data_size = request->data_size;
	uint32_t rx_data_size = 0;

	if (rx_data)
	{
		rx_data_size = tx_data_size;

		if (!tx_data)
		{
			tx_data = rx_data;
		}
	}

	spi_transaction_t transaction = { 0 };
	
	transaction.user = spi;
	transaction.tx_buffer = tx_data;
	transaction.rx_buffer = rx_data;
	transaction.length = tx_data_size * 8;
	transaction.rxlength = rx_data_size * 8;

	//spi_device_polling_transmit(&adapter->data.handle, &transaction);
	//spi_device_transmit(&adapter->data.handle, &transaction);
	spi_device_transmit(adapter->data.handle, &transaction);

	//spi_device_acquire_bus(spi_device_t *device, TickType_t wait);
	//spi_device_queue_trans(adapter->data.handle, &transaction, 1000);

	//spi_device_release_bus(&adapter->data.handle);

	return SFC_SPI_RESULT_OK;
}
//------------------------------------------------------------------------------
static sfc_spi_result_t private_transfer_async(sfc_spi_t* spi, sfc_spi_transfer_t* request)
{
	sfc_spi_handle_t* handle = spi->handle;

	//getting access to use the bus before the end of the transfer
	if (sfc_spi_core_take_bus(spi, 1000) != SFC_SPI_RESULT_OK)
	{
		return SFC_SPI_RESULT_TIMEOUT;
	}
	return SFC_SPI_RESULT_OK;
}
//------------------------------------------------------------------------------
static sfc_spi_result_t private_transfer_async_dma(sfc_spi_t* spi, sfc_spi_transfer_t* request)
{
	return SFC_SPI_RESULT_NOT_SUPORTED;
}
//------------------------------------------------------------------------------
static sfc_spi_result_t private_set_options(sfc_spi_t* spi, sfc_spi_options_t* request)
{
	//sfc_spi_adapter_t* adapter = (sfc_spi_adapter_t*)spi->adapter;

	return SFC_SPI_RESULT_NOT_SUPORTED;
}
//==============================================================================
static const sfc_spi_adapter_interface_t adapter_interface =
{
	.handler = (sfc_spi_handler_t)private_handler,
	.irq = (sfc_spi_irq_t)private_irq,

	.request_listener = (sfc_spi_request_listener_t)private_request_listener,
	.event_listener = (sfc_spi_event_listener_t)private_event_listener,

	.transfer = (sfc_spi_transfer_action_t)private_transfer,
	.transfer_async = (sfc_spi_transfer_action_async_t)private_transfer_async,
	.transfer_async_dma = (sfc_spi_transfer_action_async_t)private_transfer_async_dma,

	.set_options = (sfc_spi_set_options_t)private_set_options
};
//==============================================================================
//initialization:

sfc_spi_result_t sfc_spi_adapter_init(sfc_spi_t* spi, sfc_spi_adapter_t* adapter)
{
	//description of the object
	spi->adapter = (sfc_spi_adapter_base_t*)adapter;
	spi->adapter->object.type = SFC_SPI_ADAPTER_TYPE;
	spi->adapter->object.sub_type = SFC_SPI_ADAPTER_SUBTYPE;
	spi->adapter->object.identifier = SFC_SPI_ADAPTER_ID;

	//linking objects
	spi->adapter->object.parent = spi;
	spi->adapter->interface = (sfc_spi_adapter_interface_t*)&adapter_interface;

	//binding the spi bus to an instance
	sfc_spi_core_bind_handle(spi, adapter->spi_number);

	//enable spi clocking
	sfc_spi_core_clock_enable(spi);

	sfc_spi_handle_t* handle = spi->handle;

	adapter->data.config.command_bits = 0;
	adapter->data.config.address_bits = 0;
	adapter->data.config.clock_speed_hz = 10 * 1000 * 1000;
	adapter->data.config.mode = 0;
	adapter->data.config.spics_io_num = -1;
	adapter->data.config.queue_size = 1;
	adapter->data.config.flags = 0;

	esp_err_t ret = spi_bus_add_device(handle->instance, &adapter->data.config, &adapter->data.handle);

	ESP_ERROR_CHECK(ret);

	gpio_reset_pin(adapter->cs_pin);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(adapter->cs_pin, GPIO_MODE_OUTPUT);

	//initializing spi pins
	//sfc_spi_core_pin_mux_set(spi, &adapter->pin_mux);

	//enabling interrupts
	sfc_spi_core_interrupt_enable(spi, 5, 0);

	return SFC_SPI_RESULT_OK;
}
//==============================================================================
