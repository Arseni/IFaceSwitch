
/* Library includes. */
#include "hw_types.h"
#include "gpio.h"
#include "hw_memmap.h"

/* drivers include */
#include "uart.h"
#include "gpio.h"
#include "sysctl.h"

/* API include */
#include "comport.h"

/* Presets */
#define SERIALPORT_PORT_BASE		GPIO_PORTA_BASE
#define SERIALPORT_PINS				(GPIO_PIN_0 | GPIO_PIN_1)

/* ---------------------- private library --------------------- */
static tBoolean isInitialized = false;
static tBoolean isOpen = false;

static void Init(void)
{
	// if already passed initialization, return
	if(isInitialized)
		return;

	// enable peripheral clock
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	// set gpio pins to hardware used mode
	GPIOIntTypeSet(SERIALPORT_PORT_BASE, SERIALPORT_PINS, GPIO_DIR_MODE_HW);

	// initialization complete
	isInitialized = true;
}


/* ---------------------- public library --------------------- */
tBoolean comportOpen(eComport port)
{
	Init();

	// TODO mutex !
	if(isOpen)
		return false;
	isOpen = true;
}

tBoolean comportWrite(){}
