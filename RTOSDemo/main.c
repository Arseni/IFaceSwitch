/*
    FreeRTOS V6.0.3 - Copyright (C) 2010 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS eBook                                  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks.
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Fast Interrupt Test" - A high frequency periodic interrupt is generated
 * using a free running timer to demonstrate the use of the
 * configKERNEL_INTERRUPT_PRIORITY configuration constant.  The interrupt
 * service routine measures the number of processor clocks that occur between
 * each interrupt - and in so doing measures the jitter in the interrupt timing.
 * The maximum measured jitter time is latched in the ulMaxJitter variable, and
 * displayed on the OLED display by the 'OLED' task as described below.  The
 * fast interrupt is configured and handled in the timertest.c source file.
 *
 * "OLED" task - the OLED task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the OLED send the message on a queue to the OLED task instead of
 * accessing the OLED themselves.  The OLED task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
 *
 * "Check" hook -  This only executes every five seconds from the tick hook.
 * Its main function is to check that all the standard demo tasks are still
 * operational.  Should any unexpected behaviour within a demo task be discovered
 * the tick hook will write an error to the OLED (via the OLED task).  If all the
 * demo tasks are executing with their expected behaviour then the check task
 * writes PASS to the OLED (again via the OLED task), as described above.
 *
 * "uIP" task -  This is the task that handles the uIP stack.  All TCP/IP
 * processing is performed in this task.
 */

/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Hardware library includes. */
#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_sysctl.h"
#include "sysctl.h"
#include "gpio.h"
#include "grlib.h"
#include "rit128x96x4.h"
#include "osram128x64x4.h"
#include "formike128x128x16.h"

/* Demo app includes. */
#include "lcd_message.h"
#include "bitmap.h"

/* API includes */
#include "led.h"
#include "comport.h"

/*-----------------------------------------------------------*/

/* The time between cycles of the 'check' functionality (defined within the
tick hook. */
#define mainCHECK_DELAY						( ( portTickType ) 5000 / portTICK_RATE_MS )

/* Size of the stack allocated to the uIP task. */
#define mainBASIC_WEB_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 3 )

/* The OLED task uses the sprintf function so requires a little more stack too. */
#define mainOLED_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE + 50 )

/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )
#define mainGEN_QUEUE_TASK_PRIORITY			( tskIDLE_PRIORITY )

/* The maximum number of message that can be waiting for display at any one
time. */
#define mainOLED_QUEUE_SIZE					( 3 )

/* Dimensions the buffer into which the jitter time is written. */
#define mainMAX_MSG_LEN						LCD_MESSAGE_MAX_LENGTH

/* The period of the system clock in nano seconds.  This is used to calculate
the jitter time in nano seconds. */
#define mainNS_PER_CLOCK					( ( unsigned portLONG ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )

/* Constants used when writing strings to the display. */
#define mainCHARACTER_HEIGHT				( 9 )
#define mainMAX_ROWS_128					( mainCHARACTER_HEIGHT * 14 )
#define mainMAX_ROWS_96						( mainCHARACTER_HEIGHT * 10 )
#define mainMAX_ROWS_64						( mainCHARACTER_HEIGHT * 7 )
#define mainFULL_SCALE						( 15 )
#define ulSSI_FREQUENCY						( 3500000UL )

/*-----------------------------------------------------------*/

/*
 * The display is written two by more than one task so is controlled by a
 * 'gatekeeper' task.  This is the only task that is actually permitted to
 * access the display directly.  Other tasks wanting to display a message send
 * the message to the gatekeeper.
 */
static void vOLEDTask( void *pvParameters );

void vTaskRefresh( void * pvParameters );

/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void );

/*
 * Configures the high frequency timers - those used to measure the timing
 * jitter while the real time kernel is executing.
 */
extern void vSetupHighFrequencyTimer( void );

/*-----------------------------------------------------------*/

/* The queue used to send messages to the OLED task. */
xQueueHandle xOLEDQueue;

/* The welcome text. */
const portCHAR * const pcWelcomeMessage = "   Better than ever";

/* Variables used to detect the test in the idle hook failing. */
unsigned portLONG ulIdleError = pdFALSE;

/*-----------------------------------------------------------*/

void vUARTTask(void * pvParameters)
{
	const portTickType xDelay = 1000 / portTICK_RATE_MS;
	//xComOpen(1,2,3,4,5,6);
	for(;;)
	{
		vTaskDelay(xDelay);
		//vComPutChar(1, 2, 3);
	}
}

void vLEDTask(void * pvParameters)
{
	const portTickType xDelay = 100 / portTICK_RATE_MS;
	xOLEDMessage msg;

	for(;;)
	{
		vTaskDelay(xDelay);
		ledSwitch(true);
		vTaskDelay(xDelay);
		ledSwitch(false);
	}
}

void vTestRoutine(void)
{
	xComOpen(1,2,3,4,5,6);
	for(;;)
	{
		vComPutChar(1, 2, 3);
	}
}

int main( void )
{
	prvSetupHardware();

	//vTestRoutine();

	/* Create the queue used by the OLED task.  Messages for display on the OLED
	are received via this queue. */
	xOLEDQueue = xQueueCreate( mainOLED_QUEUE_SIZE, sizeof( xOLEDMessage ) );

	/* Start the tasks defined within this file/specific to this demo. */
	xTaskCreate( vOLEDTask, ( signed portCHAR * ) "OLED", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

	xTaskCreate( vTaskRefresh, ( signed portCHAR * ) "REFRESH", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vUARTTask, (signed portCHAR *) "UART", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
	xTaskCreate( vLEDTask, (signed portCHAR *) "LED", mainOLED_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);

	/* Configure the high frequency interrupt used to measure the interrupt	jitter time. */
	//vSetupHighFrequencyTimer();

	/* Start the scheduler. */
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle task. */
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/

void prvSetupHardware( void )
{
    /* If running on Rev A2 silicon, turn the LDO voltage up to 2.75V.  This is
    a workaround to allow the PLL to operate reliably. */
    if( DEVICE_IS_REVA2 )
    {
        SysCtlLDOSet( SYSCTL_LDO_2_75V );
    }

	/* Set the clocking to run from the PLL at 50 MHz */
	SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ );

	/* 	Enable Port F for Ethernet LEDs
		LED0        Bit 3   Output
		LED1        Bit 2   Output */
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
	GPIODirModeSet( GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3), GPIO_DIR_MODE_HW );
	GPIOPadConfigSet( GPIO_PORTF_BASE, (GPIO_PIN_2 | GPIO_PIN_3 ), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD );
}

/*-----------------------------------------------------------*/

void vOLEDTask( void *pvParameters )
{
xOLEDMessage xMessage;
unsigned portLONG ulY, ulMaxY;
static portCHAR cMessage[ mainMAX_MSG_LEN ];
extern volatile unsigned portLONG ulMaxJitter;
unsigned portBASE_TYPE uxUnusedStackOnEntry, uxUnusedStackNow;
const unsigned portCHAR *pucImage;

/* Functions to access the OLED.  The one used depends on the dev kit
being used. */
void ( *vOLEDInit )( unsigned portLONG ) = NULL;
void ( *vOLEDStringDraw )( const portCHAR *, unsigned portLONG, unsigned portLONG, unsigned portCHAR ) = NULL;
void ( *vOLEDImageDraw )( const unsigned portCHAR *, unsigned portLONG, unsigned portLONG, unsigned portLONG, unsigned portLONG ) = NULL;
void ( *vOLEDClear )( void ) = NULL;

	/* Just for demo purposes. */
	uxUnusedStackOnEntry = uxTaskGetStackHighWaterMark( NULL );

	/* Map the OLED access functions to the driver functions that are appropriate
	for the evaluation kit being used. */
	switch( HWREG( SYSCTL_DID1 ) & SYSCTL_DID1_PRTNO_MASK )
	{
		case SYSCTL_DID1_PRTNO_6965	:
		case SYSCTL_DID1_PRTNO_2965	:	vOLEDInit = OSRAM128x64x4Init;
										vOLEDStringDraw = OSRAM128x64x4StringDraw;
										vOLEDImageDraw = OSRAM128x64x4ImageDraw;
										vOLEDClear = OSRAM128x64x4Clear;
										ulMaxY = mainMAX_ROWS_64;
										pucImage = pucBasicBitmap;
										break;

		case SYSCTL_DID1_PRTNO_1968	:
		case SYSCTL_DID1_PRTNO_8962 :	vOLEDInit = RIT128x96x4Init;
										vOLEDStringDraw = RIT128x96x4StringDraw;
										vOLEDImageDraw = RIT128x96x4ImageDraw;
										vOLEDClear = RIT128x96x4Clear;
										ulMaxY = mainMAX_ROWS_96;
										pucImage = pucBasicBitmap;
										break;

		default						:	vOLEDInit = vFormike128x128x16Init;
										vOLEDStringDraw = vFormike128x128x16StringDraw;
										vOLEDImageDraw = vFormike128x128x16ImageDraw;
										vOLEDClear = vFormike128x128x16Clear;
										ulMaxY = mainMAX_ROWS_128;
										pucImage = pucGrLibBitmap;
										break;
	}

	ulY = ulMaxY;

	/* Initialise the OLED and display a startup message. */
	vOLEDInit( ulSSI_FREQUENCY );
	vOLEDStringDraw( "POWERED BY FreeRTOS", 0, 0, mainFULL_SCALE );
	vOLEDImageDraw( pucImage, 0, mainCHARACTER_HEIGHT + 1, bmpBITMAP_WIDTH, bmpBITMAP_HEIGHT );

	for( ;; )
	{
		/* Wait for a message to arrive that requires displaying. */
		xQueueReceive( xOLEDQueue, &xMessage, portMAX_DELAY );

		/* Write the message on the next available row. */
		ulY += mainCHARACTER_HEIGHT;
		if( ulY >= ulMaxY )
		{
			ulY = mainCHARACTER_HEIGHT;
			vOLEDClear();
			vOLEDStringDraw( pcWelcomeMessage, 0, 0, mainFULL_SCALE );
		}

		/* Display the message along with the maximum jitter time from the
		high priority time test. */
		sprintf( cMessage, "%s", xMessage.pcMessage );
		vOLEDStringDraw( cMessage, 0, ulY, mainFULL_SCALE );
	}
}
/*-----------------------------------------------------------*/
void vTaskRefresh( void * pvParameters )
{
	/* Block for 500ms. */
	const portTickType xDelay = 1000 / portTICK_RATE_MS;
	xOLEDMessage msg;
	int cycle;

	/* init */
	cycle = 1;
	sprintf(msg.pcMessage, "duchlauf %2d", cycle++);

    for( ;; )
    {
        /* Simply toggle the LED every 500ms, blocking between each toggle. */
        xQueueSend(xOLEDQueue, &msg, portMAX_DELAY);
        vTaskDelay( xDelay );
        sprintf(msg.pcMessage, "duchlauf %2d", cycle++);
    }
}
/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{

	for( ;; );
}


