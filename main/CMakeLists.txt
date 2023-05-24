set(srcs
    "main.c")

set(ink
    ".")
#===============================================================================
#flags:

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-local-typedefs")
#===============================================================================
#variables:

set(COMMON_LIB_DIR "C:/Users/Afhib/Desktop/Projects/xLibs")
#===============================================================================
#common lib:

#src:
list(APPEND srcs
    ${COMMON_LIB_DIR}/Common/xCircleBuffer.c
    ${COMMON_LIB_DIR}/Common/xConverter.c
    ${COMMON_LIB_DIR}/Common/xDataBuffer.c
    ${COMMON_LIB_DIR}/Common/xMemory.c
    ${COMMON_LIB_DIR}/Common/xPort/xPort.c
    ${COMMON_LIB_DIR}/Common/xTerminal/Terminal.c
    ${COMMON_LIB_DIR}/Common/xRxReceiver.c
    ${COMMON_LIB_DIR}/Common/xRxRequest.c
    ${COMMON_LIB_DIR}/Common/xRxTransaction.c
    ${COMMON_LIB_DIR}/Common/xADC/xADC.c
    ${COMMON_LIB_DIR}/Common/xADC/Communications/xADC-RxTransactions.c
    ${COMMON_LIB_DIR}/Common/xSystem/xSystem.c
    ${COMMON_LIB_DIR}/Common/xWiFi/Communications/xWiFi-RxTransactions.c
    ${COMMON_LIB_DIR}/Common/xWiFi/xWiFi.c
)

#includes path:
list(APPEND ink
    "${COMMON_LIB_DIR}"
    "${COMMON_LIB_DIR}/Components"
    "${COMMON_LIB_DIR}/Common"
    "${COMMON_LIB_DIR}/Common/xPort"
    "${COMMON_LIB_DIR}/Common/xTerminal"
    "${COMMON_LIB_DIR}/Common/xADC"
    "${COMMON_LIB_DIR}/Common/xADC/Communications"
    "${COMMON_LIB_DIR}/Common/xSystem"
    "${COMMON_LIB_DIR}/Common/xWiFi"
)
#===============================================================================
#terminal component:

#src:
list(APPEND srcs
    ${COMMON_LIB_DIR}/Components/Terminal/Communication/Terminal_RxTransactions.c
    ${COMMON_LIB_DIR}/Components/Terminal/Communication/Terminal_RxRequests.c
    ${COMMON_LIB_DIR}/Components/Terminal/Terminal_Component.c
)

#includes path:
list(APPEND ink
    "${COMMON_LIB_DIR}/Components/Terminal"
)
#===============================================================================
#serial port component:

#src:
list(APPEND srcs
    ../Components/SerialPort/UsartPort-Component.c
    ../Components/SerialPort/Adapter/UsartPort-Adapter.c
)

#includes path:
list(APPEND ink
    "../Components/SerialPort"
)
#===============================================================================
#ADC component:

#src:
list(APPEND srcs
    ../Components/ADC/ADC-Component.c
    ../Components/ADC/Adapters/ADC-Adapter.c
)

#includes path:
list(APPEND ink
    "../Components/ADC"
)
#===============================================================================
#WiFi component:

#src:
list(APPEND srcs
    ../Components/WiFi/WiFi-Component.c
    ../Components/WiFi/Adapters/WiFi-Adapter.c
)

#includes path:
list(APPEND ink
    "../Components/WiFi"
)
#===============================================================================
#components:

#src:
list(APPEND srcs
    ../Components/Components.c
    ../Components/Interfaces/xSystem-Interface.c
)

#includes path:
list(APPEND ink
    "../Components"
    "../Components/Configurations"
    "../Components/Interfaces"
)
#===============================================================================
#file registration:

idf_component_register(SRCS ${srcs} INCLUDE_DIRS ${ink})
#===============================================================================
#definations:

#-when defining macros not at the end of the file causes an error
add_definitions(-DxTYPES_CONFIG)

#-the definition is also possible in this way:
#add_compile_definitions(xTYPES_CONFIG)
#===============================================================================
