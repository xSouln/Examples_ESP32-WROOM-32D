//==============================================================================
#ifndef _USART_PORT_COMPONENT_CONFIG_H_
#define _USART_PORT_COMPONENT_CONFIG_H_
//==============================================================================
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "Components_Types.h"

//==============================================================================
//import:

//==============================================================================
//defines:

#define UART_PORT_RX_CIRCLE_BUF_SIZE_MASK 0x0ff
#define UART_PORT_RX_OBJECT_BUF_SIZE 0x1ff
#define UART_PORT_TX_CIRCLE_BUF_SIZE_MASK 0x3fff

#define UART_PORT_NUMBER UART_NUM_1
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_USART_PORT_COMPONENT_CONFIG_H_
