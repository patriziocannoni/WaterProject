/*
 * TaskButton.c
 *
 *  Created on: 29/08/2015
 *      Author: Patrizio
 */

#include <avr/interrupt.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <TaskPump.h>
#include "TaskButton.h"

#define BUTTON_D2_PRESSED() !(PIND & _BV(PORTD2))

static void prvButtonTask(void *arg);

static xSemaphoreHandle xSemaphore;

void xStartButtonTask(void) {
	DDRD &= ~(_BV(DDD2));     	// Clear the PD2 pin.
	// PD2 (PCINT0 pin) is now an input.

	PORTD |= _BV(PORTD2);  		// turn On the Pull-up.
	// PD2 is now an input with pull-up enabled.

	EICRA = 0x02;				// The falling edge of INT0 generates an interrupt request.
	EIMSK = 0x01;				// Turns on INT0.

	vSemaphoreCreateBinary(xSemaphore);

	if (xSemaphore != NULL) {
		xTaskCreate(prvButtonTask, (signed portCHAR *) "BUTN", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	}
}

static void prvButtonTask(void *arg) {
	for (;;) {
		xSemaphoreTake(xSemaphore, portMAX_DELAY);

		// Some ms for debouncing.
		vTaskDelay(25);

		if (BUTTON_D2_PRESSED()) {
			togglePump();
		}

		EIMSK = 0x01;			// Turns on INT0.
	}
}

ISR(INT0_vect) {
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xSemaphore, pdFALSE);
	if (xHigherPriorityTaskWoken == pdTRUE) {
		EIMSK = 0x00;			// Turns off INT0.
	}
}
