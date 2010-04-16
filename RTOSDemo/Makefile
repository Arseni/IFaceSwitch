#/*
#    FreeRTOS V6.0.3 - Copyright (C) 2010 Real Time Engineers Ltd.
#
#    ***************************************************************************
#    *                                                                         *
#    * If you are:                                                             *
#    *                                                                         *
#    *    + New to FreeRTOS,                                                   *
#    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
#    *    + Looking for basic training,                                        *
#    *    + Wanting to improve your FreeRTOS skills and productivity           *
#    *                                                                         *
#    * then take a look at the FreeRTOS eBook                                  *
#    *                                                                         *
#    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
#    *                  http://www.FreeRTOS.org/Documentation                  *
#    *                                                                         *
#    * A pdf reference manual is also available.  Both are usually delivered   *
#    * to your inbox within 20 minutes to two hours when purchased between 8am *
#    * and 8pm GMT (although please allow up to 24 hours in case of            *
#    * exceptional circumstances).  Thank you for your support!                *
#    *                                                                         *
#    ***************************************************************************
#
#    This file is part of the FreeRTOS distribution.
#
#    FreeRTOS is free software; you can redistribute it and/or modify it under
#    the terms of the GNU General Public License (version 2) as published by the
#    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
#    ***NOTE*** The exception to the GPL is included to allow you to distribute
#    a combined work that includes FreeRTOS without being obliged to provide the
#    source code for proprietary components outside of the FreeRTOS kernel.
#    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
#    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#    more details. You should have received a copy of the GNU General Public 
#    License and the FreeRTOS license exception along with FreeRTOS; if not it 
#    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
#    by writing to Richard Barry, contact details for whom are available on the
#    FreeRTOS WEB site.
#
#    1 tab == 4 spaces!
#
#    http://www.FreeRTOS.org - Documentation, latest information, license and
#    contact details.
#
#    http://www.SafeRTOS.com - A version that is certified for use in safety
#    critical systems.
#
#    http://www.OpenRTOS.com - Commercial support, development, porting,
#    licensing and training services.
#*/


#/************************************************************************* 
# * Please ensure to read http://www.freertos.org/portLM3Sxxxx_Eclipse.html
# * which provides information on configuring and running this demo for the
# * various Luminary Micro EKs.
# *************************************************************************/

RTOS_SOURCE_DIR=../../../Source
UIP_COMMON_DIR=../../Common/ethernet/uIP/uip-1.0/uip
#LUMINARY_DRIVER_DIR=../../Common/drivers/LuminaryMicro
LUMINARY_DRIVER_DIR=./
DRIVERS_DIR=./Drivers
API_DIR=./API

CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
LDSCRIPT=standalone.ld

# should use --gc-sections but the debugger does not seem to be able to cope with the option.
LINKER_FLAGS=-nostartfiles -Xlinker -oRTOSDemo.axf -Xlinker -M -Xlinker -Map=rtosdemo.map -Xlinker --no-gc-sections

DEBUG=-g
OPTIM=-O0


CFLAGS=$(DEBUG) -I . -I $(RTOS_SOURCE_DIR)/include -I $(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM3 \
		-D GCC_ARMCM3_LM3S102 -D inline= -mthumb -mcpu=cortex-m3 $(OPTIM) -T$(LDSCRIPT) \
		-D PACK_STRUCT_END=__attribute\(\(packed\)\) -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) -D sprintf=usprintf -D snprintf=usnprintf \
		-D printf=uipprintf -D sourcerygxx -I $(DRIVERS_DIR) \
		-I $(UIP_COMMON_DIR) -I ./webserver -ffunction-sections -fdata-sections -I $(LUMINARY_DRIVER_DIR) -I $(API_DIR)

DRIVERS=$(DRIVERS_DIR)/adc.c $(DRIVERS_DIR)/can.c $(DRIVERS_DIR)/comp.c $(DRIVERS_DIR)/cpu.c $(DRIVERS_DIR)/epi.c $(DRIVERS_DIR)/ethernet.c \
		$(DRIVERS_DIR)/flash.c $(DRIVERS_DIR)/gpio.c $(DRIVERS_DIR)/hibernate.c $(DRIVERS_DIR)/i2c.c $(DRIVERS_DIR)/i2s.c $(DRIVERS_DIR)/interrupt.c \
		$(DRIVERS_DIR)/mpu.c $(DRIVERS_DIR)/pwm.c $(DRIVERS_DIR)/qei.c $(DRIVERS_DIR)/ssi.c $(DRIVERS_DIR)/sysctl.c $(DRIVERS_DIR)/systick.c \
		$(DRIVERS_DIR)/timer.c $(DRIVERS_DIR)/uart.c $(DRIVERS_DIR)/udma.c $(DRIVERS_DIR)/usb.c $(DRIVERS_DIR)/watchdog.c

APIS=$(API_DIR)/led.c $(API_DIR)/comport.c $(API_DIR)/buttons.c $(API_DIR)/ustdlib.c

SOURCE=	$(DRIVERS) \
		$(APIS) \
		main.c \
		rit128x96x4.c \
		osram128x64x4.c \
		formike128x128x16.c \
		$(RTOS_SOURCE_DIR)/list.c \
		$(RTOS_SOURCE_DIR)/queue.c \
		$(RTOS_SOURCE_DIR)/tasks.c \
		$(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM3/port.c \
		$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c

LIBS= $(DRIVERS_DIR)/libgr.a

OBJS = $(SOURCE:.c=.o)

all: RTOSDemo.bin
	 
RTOSDemo.bin : RTOSDemo.axf
	$(OBJCOPY) RTOSDemo.axf -O binary RTOSDemo.bin

RTOSDemo.axf : $(OBJS) startup.o Makefile
	$(CC) $(CFLAGS) $(OBJS) startup.o $(LIBS) $(LINKER_FLAGS)

$(OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

startup.o : startup.c Makefile
	$(CC) -c $(CFLAGS) -O1 startup.c -o startup.o
		
clean :
	cs-rm $(OBJS)
	



