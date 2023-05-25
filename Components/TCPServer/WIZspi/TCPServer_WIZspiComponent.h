//==============================================================================
//header:

#ifndef _TCP_SERVER_WIZ_SPI_COMPONENT_H
#define _TCP_SERVER_WIZ_SPI_COMPONENT_H
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
//==============================================================================
//includes:

#include "TCPServer_WIZspiComponentConfig.h"
#include "TCPServer/Controls/TCPServer.h"
//==============================================================================
//defines:


//==============================================================================
//functions:

xResult TCPServerWIZspiComponentInit(void* parent);

void _TCPServerWIZspiComponentHandler();
void _TCPServerWIZspiComponentTimeSynchronization();
//==============================================================================
//import:


//==============================================================================
//override:

#define TCPServerWIZspiComponentHandler() TCPServerHandler(&TCPServerWIZspi)
#define TCPServerWIZspiComponentTimeSynchronization() //TCPServerTimeSynchronization(&TCPServerWIZspi)
//==============================================================================
//export:

extern TCPServerT TCPServerWIZspi;
//==============================================================================
#ifdef __cplusplus
}
#endif
//------------------------------------------------------------------------------
#endif //_TCP_SERVER_WIZ_SPI_COMPONENT_H
