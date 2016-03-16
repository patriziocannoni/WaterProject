/*
 * TaskPump.c
 *
 *  Created on: 17/08/2015
 *      Author: Patrizio
 */

#include <FreeRTOS.h>
#include <task.h>
#include "TaskPump.h"

static void prvPumpTask(void *arg);

static unsigned char processState;
static unsigned char pumpState;

enum {
	PUMP_STOP,
	PUMP_START,
	PUMP_IDLE
};

void xStartPumpTask(void) {
	// Pin C3 as OUTPUT.
	DDRC |= _BV(DDC3);
	pumpState = processState = PUMP_STOP;
	xTaskCreate(prvPumpTask, (signed portCHAR *) "PUMP", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
}

void pumpStart(void) {
	processState = PUMP_START;
}

void pumpStop(void) {
	processState = PUMP_STOP;
}

void togglePump(void) {
	if (pumpState == PUMP_START) {
		processState = PUMP_STOP;
	} else {
		processState = PUMP_START;
	}
}

static void prvPumpTask(void *arg) {
	for (;;) {
		switch (processState) {
		case PUMP_STOP:
			// Port LOW.
			PORTC &= ~_BV(PORTC3);
			// Guarda o estado lógico da bomba
			pumpState = PUMP_STOP;
			processState = PUMP_IDLE;
			vTaskDelay(250);
			break;

		case PUMP_START:
			// Port HIGH.
			PORTC |= _BV(PORTC3);
			// Guarda o estado lógico da bomba
			pumpState = PUMP_START;
			processState = PUMP_IDLE;
			vTaskDelay(250);
			break;

		case PUMP_IDLE:
			vTaskDelay(50);
			break;
		}
	}
}
