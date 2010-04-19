
/* Library includes. */
#include "hw_types.h"
#include "gpio.h"
#include "hw_memmap.h"
#include "portmacro.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <string.h>

/* drivers include */
#include "gpio.h"
#include "sysctl.h"

/* API include */
#include "buttons.h"
#include "OLEDDisplay/lcd_message.h"

/* Presets */
#define BUTTON_MAX_CALLBACKS	10
#define BUTTON_ARROW_PINS		(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define BUTTON_SEL_PIN			GPIO_PIN_1
#define BUTTON_ARROW_PORT_BASE	GPIO_PORTE_BASE
#define BUTTON_SEL_PORT_BASE	GPIO_PORTF_BASE

extern xQueueHandle xOLEDQueue;

/* ---------------------- private library --------------------- */
static tBoolean isInitialized = false;
static tButtonCallback callbackCollection[BUTTON_MAX_CALLBACKS];

static void buttonPressISR(void)
{
	while(1)
	{
	}
	/*xOLEDMessage msg;
	tButton btnPressed = xButtonIsPressed();
	strcpy(msg.pcMessage, "btnPress: ");
	if(btnPressed & BUTTON_UP) strcat(msg.pcMessage, "up ");
	if(btnPressed & BUTTON_DOWN) strcat(msg.pcMessage, "down ");
	if(btnPressed & BUTTON_LEFT) strcat(msg.pcMessage, "left ");
	if(btnPressed & BUTTON_RIGHT) strcat(msg.pcMessage, "right ");
	if(btnPressed & BUTTON_SEL) strcat(msg.pcMessage, "sel ");
	xQueueSend(xOLEDQueue, &msg, portMAX_DELAY);

	GPIOPinIntClear(BUTTON_ARROW_PORT_BASE, BUTTON_ARROW_PINS);
	GPIOPinIntClear(BUTTON_SEL_PORT_BASE, BUTTON_SEL_PIN);
*/
}


static void Init(void)
{
	if(isInitialized)
		return;

	int i;
	for(i=0; i<BUTTON_MAX_CALLBACKS; i++)
		callbackCollection[i] = NULL;

	// enable PORTE clock
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// set Button pins as input
	GPIODirModeSet(BUTTON_ARROW_PORT_BASE, BUTTON_ARROW_PINS, GPIO_DIR_MODE_IN);
	GPIODirModeSet(BUTTON_SEL_PORT_BASE, BUTTON_SEL_PIN, GPIO_DIR_MODE_IN);

	// Set the buttons to weak pullup
	GPIOPadConfigSet( BUTTON_ARROW_PORT_BASE, BUTTON_ARROW_PINS, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );
	GPIOPadConfigSet( BUTTON_SEL_PORT_BASE, BUTTON_SEL_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );

	// Enable and register interrupt, clear
	GPIOIntTypeSet(BUTTON_ARROW_PORT_BASE, BUTTON_ARROW_PINS, GPIO_FALLING_EDGE);
	GPIOPinIntEnable(BUTTON_ARROW_PORT_BASE, BUTTON_ARROW_PINS);
	//GPIOPortIntRegister(BUTTON_ARROW_PORT_BASE, buttonPressISR);
	GPIOPinIntClear(BUTTON_ARROW_PORT_BASE, BUTTON_ARROW_PINS);


	isInitialized = true;
}


/* ---------------------- public  library --------------------- */
tBoolean xButtonRegisterCallback(tButtonCallback btnCb)
{
	Init();

}

tButton xButtonIsPressed(void)
{
	Init();

	tButton ret = 0;
	portBASE_TYPE pinState;
	pinState = GPIOPinRead(BUTTON_ARROW_PORT_BASE, BUTTON_ARROW_PINS);
	ret = (tButton)(~pinState & BUTTON_ARROW_PINS);

	if(!GPIOPinRead(BUTTON_SEL_PORT_BASE, BUTTON_SEL_PIN))
		ret |= BUTTON_SEL;

	return ret;
}

