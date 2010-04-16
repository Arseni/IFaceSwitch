
/* Library includes. */
#include "hw_types.h"
#include "gpio.h"
#include "hw_memmap.h"

/* drivers include */
#include "gpio.h"
#include "sysctl.h"

/* API include */
#include "buttons.h"

/* Presets */
#define BUTTON_MAX_CALLBACKS	10
/* ---------------------- private library --------------------- */
static tBoolean isInitialized = false;
static tButtonCallback callbackCollection[BUTTON_MAX_CALLBACKS];

static void Init(void)
{
	int i;
	for(i=0; i<BUTTON_MAX_CALLBACKS; i++)
		callbackCollection[i] = NULL;
}


/* ---------------------- public  library --------------------- */
tBoolean xButtonRegisterCallback(tButtonCallback btnCb)
{
	Init();

}
