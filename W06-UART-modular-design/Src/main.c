/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "serial.h"

#include "stm32f303xc.h"
#define PART 'A'

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


void finished_transmission(uint32_t bytes_sent) {
	// This function will be called after a transmission is complete

	volatile uint32_t test = 0;
	// make a very simple delay
	for (volatile uint32_t i = 0; i < 0x8ffff; i++) {
		// waste time !
	}
}


int main(void)
{

	SerialInitialise(BAUD_115200, &USART1_PORT, &finished_transmission);

	if(PART == 'A'){
		char storage_buffer[BUFFER_SIZE];

		for(;;) {
			//SerialInputString(USART1_PORT.incoming_buffer, &USART1_PORT);
			SerialInputString(storage_buffer, &USART1_PORT);
			SerialOutputString(storage_buffer, &USART1_PORT);
		}
	}

	if(PART == 'B'){

	}

}
