
/* Library includes. */
#include "hw_types.h"
#include "gpio.h"
#include "hw_memmap.h"

/* drivers include */
#include "gpio.h"
#include "sysctl.h"

/* API include */
#include "led.h"

/* Presets */
#define LED_PORT_BASE	GPIO_PORTF_BASE
#define LED_PIN			GPIO_PIN_0
#define LED_ON			(1)
#define LED_OFF			(0)

/* ---------------------- private library --------------------- */
static tBoolean isInitialized = false;
static void Init(void)
{
	// if already passed initialization, return
	if(isInitialized)
		return;

	// enable PORTF clock
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// set LED as output
	GPIODirModeSet(LED_PORT_BASE, LED_PIN, GPIO_DIR_MODE_OUT);

	// set driver to drive 8mA on the PIN
	GPIOPadConfigSet( GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD );

	// initialization complete
	isInitialized = true;
}


/* ---------------------- public library --------------------- */
void ledSwitch(tBoolean OnOff)
{
	Init();

	if(OnOff)
		GPIOPinWrite(GPIO_PORTF_BASE, LED_PIN, LED_ON);
	else
		GPIOPinWrite(GPIO_PORTF_BASE, LED_PIN, LED_OFF);
}
