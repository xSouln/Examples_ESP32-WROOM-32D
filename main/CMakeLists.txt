set(srcs
    "blink_example_main.c")

set(ink
    ".")
#===============================================================================
#components:

#src:
list(APPEND srcs
    "C:/Users/Afhib/Desktop/Projects/Examples_ESP32-WROOM-32D/Components/Components.c"
    "C:/Users/Afhib/Desktop/Projects/Examples_ESP32-WROOM-32D/Components/TCPServer/TCPServer_WIZspiComponent.c"
    "C:/Users/Afhib/Desktop/Projects/Examples_ESP32-WROOM-32D/Components/sfc_spi/sfc_spi_component.c"
    "C:/Users/Afhib/Desktop/Projects/Examples_ESP32-WROOM-32D/Components/sfc_spi/adapters/sfc_spi_adapter.c"
    "C:/Users/Afhib/Desktop/Projects/Examples_ESP32-WROOM-32D/Components/sfc_spi/adapters/sfc_spi_core.c"
    $ENV{XLIB_PATH}/Components/Terminal/Terminal_Component.c
    $ENV{XLIB_PATH}/Components/Terminal/Adapters/Terminal_TransferAdapter.c
    $ENV{XLIB_PATH}/Components/Terminal/Communication/Terminal_RxRequests.c
    $ENV{XLIB_PATH}/Components/Terminal/Communication/Terminal_RxTransactions.c
    $ENV{XLIB_PATH}/Components/Terminal/Controls/Terminal.c)

#includes path:
list(APPEND ink
    "C:/Users/Afhib/Desktop/Projects/Examples_ESP32-WROOM-32D/Components")
#===============================================================================
#sfc_spi lib:

#src:
list(APPEND srcs
    $ENV{XLIB_PATH}/Components/sfc_spi/controls/sfc_spi.c)

#includes path:
list(APPEND ink
    "C:/Users/Afhib/Desktop/Projects/xLibs/Components/sfc_spi/controls")
#===============================================================================
#common lib:

#src:
list(APPEND srcs
    $ENV{XLIB_PATH}/Common/xCircleBuffer.c
    $ENV{XLIB_PATH}/Common/xConverter.c
    $ENV{XLIB_PATH}/Common/xDataBuffer.c
    $ENV{XLIB_PATH}/Common/xMemory.c
    $ENV{XLIB_PATH}/Common/xRx.c
    $ENV{XLIB_PATH}/Common/xRxReceiver.c
    $ENV{XLIB_PATH}/Common/xRxRequest.c
    $ENV{XLIB_PATH}/Common/xRxTransaction.c
    $ENV{XLIB_PATH}/Common/xTx.c
    $ENV{XLIB_PATH}/Common/xTxTransfer.c
    $ENV{XLIB_PATH}/Common/Adapters/xRxTransactionTransferAdapter.c)

#includes path:
list(APPEND ink
    "C:/Users/Afhib/Desktop/Projects/xLibs"
    "C:/Users/Afhib/Desktop/Projects/xLibs/Components"
    "C:/Users/Afhib/Desktop/Projects/xLibs/Common"
    "C:/Users/Afhib/Desktop/Projects/xLibs/Common/Adapters")
#===============================================================================
#TCP_Server lib:

#src:
list(APPEND srcs
    $ENV{XLIB_PATH}/Components/TCPServer/Controls/TCPServer.c
    $ENV{XLIB_PATH}/Components/TCPServer/Templates/WIZspi/Adapters/TCPServer_WIZspiAdapter.c
    $ENV{XLIB_PATH}/Components/TCPServer/Templates/WIZspi/Adapters/TCPServer_WIZspiRxAdapter.c
    $ENV{XLIB_PATH}/Components/TCPServer/Templates/WIZspi/Adapters/TCPServer_WIZspiTxAdapter.c)

#includes path:
list(APPEND ink
    "C:/Users/Afhib/Desktop/Projects/xLibs/Components/TCPServer/Templates/WIZspi/Adapters"
    "C:/Users/Afhib/Desktop/Projects/xLibs/Components/TCPServer/Controls")
#===============================================================================
#WIZ5500 lib:

#src:
list(APPEND srcs
    $ENV{XLIB_PATH}/Drivers/WIZ/W5500/w5500.c
    $ENV{XLIB_PATH}/Drivers/WIZ/socket.c
    $ENV{XLIB_PATH}/Drivers/WIZ/wizchip_conf.c)

#includes path:
list(APPEND ink
    "C:/Users/Afhib/Desktop/Projects/xLibs/Drivers"
    "C:/Users/Afhib/Desktop/Projects/xLibs/Drivers/WIZ"
    "C:/Users/Afhib/Desktop/Projects/xLibs/Drivers/WIZ/W5500")
#===============================================================================

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${ink})
