//==============================================================================
//includes:

#include "Components.h"
#include "Adapter/UsartPort-Adapter.h"
#include "driver/gpio.h"
//==============================================================================
//defines:

#define RX_OPERATION_BUF_SIZE 0x200
#define RX_BUF_SIZE 0x400
#define TX_BUF_SIZE 04000
//==============================================================================
//variables:

static uint8_t private_rx_operation_buf[RX_OPERATION_BUF_SIZE];
static uint8_t private_rx_buf[RX_BUF_SIZE];
static uint8_t private_tx_buf[TX_BUF_SIZE];

static uint8_t private_rx_circle_buf[RX_BUF_SIZE];

xPortT UsartPort;
//==============================================================================
//import:


//==============================================================================
//functions:

static void EventListener(xPortT* port, xPortObjectEventSelector selector, void* arg)
{
	switch((int)selector)
	{
		case xPortObjectEventRxFoundEndLine:
			TerminalReceiveData(port,
								((xPortEventDataPacketArgT*)arg)->Data,
								((xPortEventDataPacketArgT*)arg)->Size);
			break;

		case xPortObjectEventRxBufferIsFull:
			TerminalReceiveData(port,
								((xPortEventDataPacketArgT*)arg)->Data,
								((xPortEventDataPacketArgT*)arg)->Size);
			break;

		default: break;
	}
}
//------------------------------------------------------------------------------

static xResult RequestListener(xPortT* port, xPortObjectRequestSelector selector, void* arg)
{
	switch ((int)selector)
	{
		default: return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------

void UsartPortComponentIRQ()
{
	xPortIRQ(&UsartPort, 0);
}
//------------------------------------------------------------------------------
//component functions:
/**
 * @brief main handler
 */
void UsartPortComponentHandler()
{
	xPortHandler(&UsartPort);
}
//------------------------------------------------------------------------------
/**
 * @brief time synchronization of time-dependent processes
 */
void UsartPortComponentTimeSynchronization()
{

}

//==============================================================================
//initialization:

static UsartPortAdapterT PrivateUsartPortAdapter =
{
	.Config =
	{
		.baud_rate = 900000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
	},

	.TxPinNumber = GPIO_NUM_18,//GPIO_NUM_35,
	.RxPinNumber = GPIO_NUM_17,//GPIO_NUM_34,

	.RxOperationBuffer = private_rx_operation_buf,
	.RxOperationBufferSize = sizeof(private_rx_operation_buf),

	.TxOperationBuffer = private_tx_buf,
	.TxOperationBufferSize = sizeof(private_tx_buf),

	.UART_NUM = UART_NUM_1
};
//------------------------------------------------------------------------------

static xPortObjectInterfaceT objectInterface =
{
	.RequestListener = (xObjectRequestListenerT)RequestListener,
	.EventListener = (xObjectEventListenerT)EventListener
};
//==============================================================================
//component initialization:

xResult UsartPortComponentInit(void* parent)
{
	xRxReceiverInit(&PrivateUsartPortAdapter.RxReceiver, &UsartPort, 0, private_rx_buf, sizeof(private_rx_buf));
	xCircleBufferInit(&PrivateUsartPortAdapter.Data.RxCircleBuffer, private_rx_circle_buf, sizeof(private_rx_circle_buf) - 1, sizeof(uint8_t));

	UsartPortAdapterInit(&UsartPort, &PrivateUsartPortAdapter);
	
	xPortInitT port_init =
	{
		.Parent = parent,
		.Interface = &objectInterface
	};
	xPortInit(&UsartPort, &port_init);
  
	return xResultAccept;
}
//==============================================================================

