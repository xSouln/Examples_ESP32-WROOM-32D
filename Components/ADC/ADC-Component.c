//==============================================================================
//includes:

#include "Components.h"
#include "Adapters/ADC-Adapter.h"
#include "Common/xADC/Communications/xADC-RxTransactions.h"
//==============================================================================
//defines:

#define ADC_POINTS_BUFFER_SIZE_MASK 0x1ff
#define TASK_STACK_SIZE 0x1400
//==============================================================================
//variables:

static TaskHandle_t task_handle;
static StaticTask_t task_buffer;
static StackType_t task_stack[TASK_STACK_SIZE];

struct
{
	xADC_TransactionEventNewPointsT Header;
	uint16_t Buffer[256 * 2];

} PointsPacket;
//==============================================================================
//import:


//==============================================================================
//functions:

static void EventListener(xADC_T* adc, xADC_ObjectEventSelector selector, void* arg)
{
	switch((int)selector)
	{
		default: break;
	}
}
//------------------------------------------------------------------------------

static xResult RequestListener(xADC_T* adc, xADC_ObjectRequestSelector selector, void* arg)
{
	switch ((uint8_t)selector)
	{
		default: return xResultRequestIsNotFound;
	}

	return xResultAccept;
}
//------------------------------------------------------------------------------

void ADC_ComponentIRQ()
{

}
//------------------------------------------------------------------------------
//component functions:
/**
 * @brief main handler
 */
void ADC_ComponentHandler()
{
	static char data[250];
	static int data_size;

	static uint32_t time_stamp = 0;

	xADC_Handler(&mADC);

	if (mADC.NotifiedChannels)
	{
		int number_of_points = xADC_GetNumberOfNewPoints(&mADC);
		
		if (xSystemGetTime(ADC_ComponentHandler) - time_stamp > 100 || number_of_points > 256)
		{
			number_of_points = xCircleBufferReadObject(&mADC.Points->Buffer, PointsPacket.Buffer, 256, 0, 0);

			if (number_of_points > 0)
			{
				PointsPacket.Header.Channels = 0x3;
				PointsPacket.Header.PointsCount = number_of_points;
				PointsPacket.Header.PointSize = SIZE_ELEMENT(PointsPacket.Buffer);

				xArgT args[] =
				{
					{
						.Element = &PointsPacket,
						.Size = sizeof(PointsPacket.Header) + mADC.Points->Buffer.TypeSize * number_of_points
					}
				};

				xRxTransactionTransmitEvent(&UsartPort,
						mADC.Base.Description->ObjectId,
						TRANSACTION_EVENT_NEW_POTINTS,
						args, 1);
			}

			time_stamp = xSystemGetTime(ADC_ComponentHandler);
		}
	}
}
//------------------------------------------------------------------------------
/**
 * @brief time synchronization of time-dependent processes
 */
void ADC_ComponentTimeSynchronization()
{

}
//------------------------------------------------------------------------------
static void Task(void* arg)
{
	xSystemDelay(Task, 10000);

	while (true)
	{
		ADC_ComponentHandler();
		
		xSystemDelay(Task, 1);
	}
}
//==============================================================================
//initialization:

static ADC_AdapterT Private_ADC_Adapter;
//------------------------------------------------------------------------------

static xADC_ObjectInterfaceT Private_ADC_SysInterface =
{
	.RequestListener = (xObjectRequestListenerT)RequestListener,
	.EventListener = (xObjectEventListenerT)EventListener
};
//------------------------------------------------------------------------------
static uint16_t PrivatePointsMemory[sizeof(uint16_t) * 2 * 1024];

static xADC_PointsT PrivatePoints;
//------------------------------------------------------------------------------
xADC_T mADC;
//==============================================================================
//component initialization:

xResult ADC_ComponentInit(void* parent)
{
	ADC_AdapterInitializationT adapter_init =
	{
		.Adapter = &Private_ADC_Adapter,

		.PointsBuffer = &PrivatePoints,
		.PointsMemmory = PrivatePointsMemory,
		.SizeOfPointsMemmory = sizeof(PrivatePointsMemory),
		.ADC_Unit = ADC_UNIT_1,
		.ChannelsCount = 2
	};

	ADC_AdapterInit(&mADC, &adapter_init);

	xADC_InitializationT adc_init =
	{
		.Parent = parent,
		.Number = xADC1,
		.Interface = &Private_ADC_SysInterface
	};

	xADC_Init(&mADC, &adc_init);

	xADC_RequestInitChannelsT channel_init =
	{
		.Channels = 0b11000000
	};

	xADC_InitChannels(&mADC, &channel_init);

	task_handle =
			xTaskCreateStatic(Task, // Function that implements the task.
								"adc task", // Text name for the task.
								TASK_STACK_SIZE, // Number of indexes in the xStack array.
								NULL, // Parameter passed into the task.
								5, // Priority at which the task is created.
								task_stack, // Array to use as the task's stack.
								&task_buffer);

	mADC.NotifiedChannels = 0;

	//xADC_Start(&mADC);
  
	return xResultAccept;
}
//==============================================================================

