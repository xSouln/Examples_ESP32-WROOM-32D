//==============================================================================
//includes:

#include "LWIP-Net-Adapter.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
//==============================================================================
//variables:

static int keepAlive = 1;
static int keepIdle = 30;
static int keepInterval = 1;
static int keepCount = 2;
//==============================================================================
//prototypes:


//==============================================================================
//functions:

static void PrivateHandler(xNetT* net)
{

}
//------------------------------------------------------------------------------
static void PrivateIRQ(xNetT* net, void* arg)
{
	
}
//------------------------------------------------------------------------------
static void PrivateSocketHandler(xNetSocketT* socket)
{
	if (socket->Number != -1)
	{
		int optval;
    	socklen_t optlen = sizeof(optval);

		if (getsockopt(socket->Number, SOL_SOCKET, SO_ERROR, &optval, &optlen) != 0 || optval != ERR_OK)
		{
			shutdown(socket->Number, 0);
			close(socket->Number);
			socket->Number = -1;
		}
	}
}
//------------------------------------------------------------------------------
static xResult PrivateRequestListener(void* object, xNetRequestSelector selector, void* arg)
{
	//LWIP_NetAdapterInitT* adapter = (LWIP_NetAdapterInitT*)net->Adapter;

	switch ((int)selector)
	{
		case xNetRequestInitTcpSocket:
		{
			xNetSocketT* sock = arg;

			sock->Number = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

			if (sock->Number < 0)
			{
				return xResultError;
			}

			sock->Net = object;
		}
		break;

		case xNetRequesBind:
		{
			xNetSocketT* socket = object;

			struct sockaddr_storage dest_addr;
			struct sockaddr_in* dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
			dest_addr_ip4->sin_addr.s_addr = socket->Address.Value;
			dest_addr_ip4->sin_family = AF_INET;
			dest_addr_ip4->sin_port = htons(socket->Port);

			if (bind(socket->Number, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) != 0)
			{
				close(socket->Number);
				return xResultError;
			}
		}
		break;

		case xNetRequesListen:
		{
			xNetSocketT* socket = object;
			int max_count = *(int*)arg;

			setsockopt(socket->Number, SOL_SOCKET, SO_REUSEADDR, &max_count, sizeof(max_count));

			struct sockaddr_storage dest_addr;
			struct sockaddr_in* dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
			dest_addr_ip4->sin_addr.s_addr = socket->Address.Value;
			dest_addr_ip4->sin_family = AF_INET;
			dest_addr_ip4->sin_port = htons(socket->Port);

			if (bind(socket->Number, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) != 0)
			{
				close(socket->Number);
				return xResultError;
			}

			if (listen(socket->Number, max_count) != 0)
			{
				close(socket->Number);
				return xResultError;
			}
		}
		break;

		case xNetRequesAccept:
		{
			xNetSocketT* socket = object;
			xNetSocketT* result = arg;

			// Large enough for both IPv4 or IPv6
			struct sockaddr_storage source_addr;
        	socklen_t addr_len = sizeof(source_addr);

			int socket_number = accept(socket->Number, (struct sockaddr *)&source_addr, &addr_len);

			if (socket_number < 0)
			{
				return xResultError;
			}

			result->Net = socket->Net;
			result->Address.Value = ((struct sockaddr_in*)&source_addr)->sin_addr.s_addr;
			result->Number = socket_number;
			
			setsockopt(socket_number, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
			setsockopt(socket_number, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
			setsockopt(socket_number, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
			setsockopt(socket_number, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));

			return xResultAccept;
		}

		default : return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------
static void PrivateEventListener(void* object, xNetEventSelector selector, void* arg)
{
	switch((int)selector)
	{
		default: return;
	}
}
//------------------------------------------------------------------------------
static int PrivateTransmit(xNetSocketT* socket, void* data, int size)
{
	if (socket->Number != -1)
	{
		int sended = 0;
		uint8_t* mem = data;

		while (sended < size)
		{
			int len = send(socket->Number, mem + sended, size - sended, 0);

			if(len < 0)
			{
				shutdown(socket->Number, 0);
				close(socket->Number);
				socket->Number = -1;

				return -xResultError;
			}

			sended += len;
		}
		
		return sended;
	}

	return -xResultError;
}
//------------------------------------------------------------------------------
static int PrivateReceive(xNetSocketT* socket, void* data, int size)
{
	if (socket->Number != -1)
	{
		int received = recv(socket->Number, data, size, 0);

		if (received < 0)
		{
			return -xResultError;
		}
		
		return received;
	}

	return -xResultError;
}
//==============================================================================
//initializations:

static xNetInterfaceT PrivateInterface =
{
	.Handler = (xNetHandlerT)PrivateHandler,
	.IRQ = (xNetIRQT)PrivateIRQ,
	.SocketHandler = (xNetSocketHandlerT)PrivateSocketHandler,

	.RequestListener = (xNetRequestListenerT)PrivateRequestListener,
	.EventListener = (xNetEventListenerT)PrivateEventListener,

	.Transmit = (xNetTransmitActionT)PrivateTransmit,
	.Receive = (xNetReceiveActionT)PrivateReceive,
};
//------------------------------------------------------------------------------
xResult LWIP_NetAdapterInit(xNetT* net, LWIP_NetAdapterInitT* init)
{
	if (!net || !init || !init->Adapter)
	{
		return xResultLinkError;
	}

	net->Adapter = (xNetAdapterBaseT*)init->Adapter;

	net->Adapter->Base.Parent = net;
	net->Adapter->Base.Note = nameof(LWIP_NetAdapterT);

	net->Adapter->Interface = &PrivateInterface;
  
	return xResultError;
}
//==============================================================================
