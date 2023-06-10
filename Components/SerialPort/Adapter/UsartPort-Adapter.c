//==============================================================================
//includes:

#include "Components.h"
#include "UsartPort-Adapter.h"
#include "driver/uart.h"
#include "esp_intr_alloc.h"
//==============================================================================
//functions:

static void PrivateHandler(xPortT* port)
{
	register UsartPortAdapterT* adapter = (UsartPortAdapterT*)port->Adapter;

	int tx_data_size = 0;
	uint8_t* tx_data_ptr;

	//uart_get_buffered_data_len(adapter->UART_NUM, &rx_data_len);
	xRxReceiverRead(&adapter->RxReceiver, &adapter->Data.RxCircleBuffer);

	tx_data_size = xCircleBufferGetLine(&adapter->Data.TxCircleBuffer, &tx_data_ptr);

	if (tx_data_size)
	{
		tx_data_size = uart_write_bytes(adapter->UART_NUM, tx_data_ptr, tx_data_size);

		if (tx_data_size > 0)
		{
			xCircleBufferOffsetHandlerIndex(&adapter->Data.TxCircleBuffer, tx_data_size);
		}
	}
}
//------------------------------------------------------------------------------
static void PrivateIRQ(xPortT* port, void* arg)
{

}
//------------------------------------------------------------------------------
static xResult PrivateRequestListener(xPortT* port, xPortRequestSelector selector, void* arg)
{
	register UsartPortAdapterT* adapter = (UsartPortAdapterT*)port->Adapter;

	switch ((uint32_t)selector)
	{
		case xPortRequestUpdateTxStatus:
			port->Tx.IsEnable = true;
			//port->Tx.IsTransmitting = adapter->Usart->Control1.TxEmptyInterruptEnable;
			break;

		case xPortRequestUpdateRxStatus:
			//port->Rx.IsEnable = adapter->Usart->Control1.ReceiverEnable;
			break;

		case xPortRequestGetRxBuffer:
			*(uint8_t**)arg = adapter->RxReceiver.Buffer;
			break;

		case xPortRequestGetRxBufferSize:
			*(uint32_t*)arg = adapter->RxReceiver.BufferSize;
			break;

		case xPortRequestGetRxBufferFreeSize:
			*(uint32_t*)arg = adapter->RxReceiver.BufferSize - adapter->RxReceiver.BytesReceived;
			break;

		case xPortRequestClearRxBuffer:
			adapter->RxReceiver.BytesReceived = 0;
			break;

		case xPortRequestGetTxBufferSize:
			*(uint32_t*)arg = (uint32_t)-1;
			break;

		case xPortRequestGetTxBufferFreeSize:
			*(uint32_t*)arg = (uint32_t)-1;
			break;

		case xPortRequestClearTxBuffer:
			break;

		case xPortRequestStartTransmission:
			xSemaphoreTake(adapter->Data.TransactionMutex, portMAX_DELAY);
			break;

		case xPortRequestEndTransmission:
			xSemaphoreGive(adapter->Data.TransactionMutex);
			break;

		default : return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void PrivateEventListener(xPortT* port, xPortEventSelector selector, void* arg)
{
	//register UsartPortAdapterT* adapter = (UsartPortAdapterT*)port->Adapter;

	switch((int)selector)
	{
		default: return;
	}
}
//------------------------------------------------------------------------------
static int PrivateTransmit(xPortT* port, void* data, uint32_t size)
{
	register UsartPortAdapterT* adapter = (UsartPortAdapterT*)port->Adapter;
	
	xCircleBufferAdd(&adapter->Data.TxCircleBuffer, data, size);

	return xResultAccept;
}
//------------------------------------------------------------------------------
static int PrivateReceive(xPortT* port, void* data, uint32_t size)
{
	return -xResultNotSupported;
}
//------------------------------------------------------------------------------
static void PrivateRxReceiverEventListener(xRxReceiverT* receiver, xRxReceiverEventSelector event, void* arg)
{
	register xPortT* port = receiver->Base.Parent;

	switch ((uint8_t)event)
	{
		case xRxReceiverEventEndLine:
			xPortEventListener(port, xPortObjectEventRxFoundEndLine, arg);
			break;

		case xRxReceiverEventBufferIsFull:
			xPortEventListener(port, xPortObjectEventRxBufferIsFull, arg);
			break;

		default: return;
	}
}
//------------------------------------------------------------------------------
static void uart_event_task(void *pvParameters)
{
    uart_event_t event;

	xPortT* port = pvParameters;
	UsartPortAdapterT* adapter = (UsartPortAdapterT*)port->Adapter;

    while (true)	
	{
        //Waiting for UART event.
        if(xQueueReceive(adapter->Data.handle_queue, (void*)&event, (TickType_t)portMAX_DELAY))
		{
            switch(event.type)
			{
                //Event of UART receving data
                //We'd better handler data event fast, there would be much more data events than
                //other types of events. If we take too much time on data event, the queue might
                //be full.
                case UART_DATA:
					{
						int size = uart_read_bytes(adapter->UART_NUM, adapter->RxOperationBuffer, event.size, portMAX_DELAY);
						if (size > 0)
						{
							xCircleBufferAdd(&adapter->Data.RxCircleBuffer, adapter->RxOperationBuffer, size);
						}
					}
                    break;

                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(adapter->UART_NUM);
                    xQueueReset(adapter->Data.handle_queue);
                    break;

                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(adapter->UART_NUM);
                    xQueueReset(adapter->Data.handle_queue);
                    break;

                //Event of UART RX break detected
                case UART_BREAK:
                    break;

                //Event of UART parity check error
                case UART_PARITY_ERR:
                    break;

                //Event of UART frame error
                case UART_FRAME_ERR:
                    break;

                //UART_PATTERN_DET
                case UART_PATTERN_DET:                   
                    break;

                //Others
                default:
                    break;
            }
        }
    }
	
    vTaskDelete(NULL);
}
//==============================================================================
//initializations:

static xPortInterfaceT PrivatePortInterface =
{
	.Handler = (xPortHandlerT)PrivateHandler,
	.IRQ = (xPortIRQT)PrivateIRQ,

	.RequestListener = (xPortRequestListenerT)PrivateRequestListener,
	.EventListener = (xPortEventListenerT)PrivateEventListener,

	.Transmit = (xPortTransmitActionT)PrivateTransmit,
	.Receive = (xPortReceiveActionT)PrivateReceive
};
//------------------------------------------------------------------------------
static xRxReceiverInterfaceT PrivateRxReceiverInterface =
{
	.EventListener = (xRxReceiverEventListenerT)PrivateRxReceiverEventListener
};
//------------------------------------------------------------------------------
xResult UsartPortAdapterInit(xPortT* port, UsartPortAdapterT* adapter)
{
	if (port && adapter)
	{
		port->Adapter = (xPortAdapterBaseT*)adapter;

		port->Adapter->Base.Note = nameof(UsartPortAdapterT);
		port->Adapter->Base.Parent = port;
		
		port->Adapter->Interface = &PrivatePortInterface;

		adapter->RxReceiver.Interface = &PrivateRxReceiverInterface;

		//xDataBufferInit(&adapter->Data.TxBuffer, port, 0, adapter->TxOperationBuffer, adapter->TxOperationBufferSize);
		xCircleBufferInit(&adapter->Data.TxCircleBuffer, adapter->TxOperationBuffer, adapter->TxOperationBufferSize - 1, sizeof(uint8_t));

		//uart_driver_install(adapter->UART_NUM, adapter->RxOperationBufferSize * 2, 0, 0, 0, 0);
		uart_driver_install(adapter->UART_NUM, adapter->RxOperationBufferSize, 0, 20, &adapter->Data.handle_queue, 0);

		uart_param_config(adapter->UART_NUM, &adapter->Config);
		uart_set_pin(adapter->UART_NUM, adapter->TxPinNumber, adapter->RxPinNumber, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
		
		uart_pattern_queue_reset(adapter->UART_NUM, 20);

		xTaskCreate(uart_event_task, "uart_event_task", 1024, port, configMAX_PRIORITIES, NULL);
		//xTaskCreate(uart_rx_task, "uart_rx_task", 1024, port, configMAX_PRIORITIES, NULL);

		adapter->Data.TransactionMutex = xSemaphoreCreateMutex();
		
		return xResultAccept;
	}
  
  return xResultError;
}
//==============================================================================
