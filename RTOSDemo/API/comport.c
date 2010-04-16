
/* Library includes. */
#include "hw_types.h"
#include "gpio.h"
#include "hw_memmap.h"
#include "portmacro.h"

/* drivers include */
#include "uart.h"
#include "gpio.h"
#include "sysctl.h"

/* API include */
#include "comport.h"

/* Presets */
#define SERIALPORT_PORT_BASE		GPIO_PORTA_BASE
#define SERIALPORT_PINS				(GPIO_PIN_0 | GPIO_PIN_1)

typedef struct
{
	tCOMPort ePort;
	tParity eParity;
	tStopBits eStopBits;
	tDataBits eDataBits;
	tBaud eBaudRate;
	tBoolean xIsOpen;

}tComportDescriptor;

/* ---------------------- private library --------------------- */
static tBoolean isInitialized = false;
static tBoolean isOpen = false;

static void Init(void)
{
	// if already passed initialization, return
	if(isInitialized)
		return;

	// enable peripheral clock
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	// set gpio pins to hardware used mode
	GPIOPinTypeUART(SERIALPORT_PORT_BASE, SERIALPORT_PINS);
	GPIOIntTypeSet(SERIALPORT_PORT_BASE, SERIALPORT_PINS, GPIO_DIR_MODE_HW);

	// initialization complete
	isInitialized = true;
}

/*-------------------------- public API --------------------------*/

xComPortHandle xComOpen( tCOMPort ePort, tBaud eWantedBaud, tParity eWantedParity, tDataBits eWantedDataBits, tStopBits eWantedStopBits, unsigned portBASE_TYPE uxBufferLength )
{
	Init();

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTEnable(UART0_BASE);
}
void vComWrite( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
}
signed portBASE_TYPE vComRead( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
}
signed portBASE_TYPE xComGetChar( xComPortHandle pxPort, portTickType xBlockTime )
{
	return UARTCharGet(UART0_BASE);
}
void vComPutChar( xComPortHandle pxPort, signed char cOutChar, portTickType xBlockTime )
{
	UARTCharPut(UART0_BASE, 'A');
}
portBASE_TYPE xComWaitForSemaphore( xComPortHandle xPort )
{
}
void vComClose( xComPortHandle xPort )
{
}
