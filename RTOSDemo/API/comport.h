
#ifndef COMPORT_H
#define COMPORT_H

typedef void * xComPortHandle;

typedef enum
{
	comCOM1,
	comCOM2,
	comCOM3,
	comCOM4,
	comCOM5,
	comCOM6,
	comCOM7,
	comCOM8
} tCOMPort;

typedef enum
{
	comNO_PARITY,
	comODD_PARITY,
	comEVEN_PARITY,
	comMARK_PARITY,
	comSPACE_PARITY
} tParity;

typedef enum
{
	comSTOP_1,
	comSTOP_2
} tStopBits;

typedef enum
{
	comBITS_5,
	comBITS_6,
	comBITS_7,
	comBITS_8
} tDataBits;

typedef enum
{
	com50,
	com75,
	com110,
	com134,
	com150,
	com200,
	com300,
	com600,
	com1200,
	com1800,
	com2400,
	com4800,
	com9600,
	com19200,
	com38400,
	com57600,
	com115200
} tBaud;

xComPortHandle xComOpen( tCOMPort ePort, tBaud eWantedBaud, tParity eWantedParity, tDataBits eWantedDataBits, tStopBits eWantedStopBits, unsigned portBASE_TYPE uxBufferLength );
void vComWrite( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength );
signed portBASE_TYPE xComGetChar( xComPortHandle pxPort, signed char *pcRxedChar, portTickType xBlockTime );
void vComPutChar( xComPortHandle pxPort, signed char cOutChar, portTickType xBlockTime );
portBASE_TYPE xComWaitForSemaphore( xComPortHandle xPort );
void vComClose( xComPortHandle xPort );
#endif

