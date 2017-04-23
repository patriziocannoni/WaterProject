/*
 * TaskButton.c
 *
 *  Created on: 29/08/2015
 *      Author: Patrizio
 */

#include <FreeRTOS.h>
#include <task.h>
#include "TaskButton.h"
#include <TaskPump.h>

#define BUTTON_D2_PRESSED() !(PIND & _BV(PORTD2))

static void prvButtonTask(void *arg);

static unsigned char processState;
static unsigned char buttonState = 0;

enum {
	BUTTON_READ,
	BUTTON_DEBOUNCE
};

void xStartButtonTask(void) {
	// Pin C0 as INPUT
	DDRD &= ~_BV(DDD2);

	// Pull-up ON
	PORTD |= _BV(PORTD2);

	processState = BUTTON_READ;
	xTaskCreate(prvButtonTask, (signed portCHAR *) "BUTN", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

static void prvButtonTask(void *arg) {
	for (;;) {
		switch (processState) {
		case BUTTON_READ:
			if (BUTTON_D2_PRESSED()) {	// Input LOW --> Button pressed
				processState = BUTTON_DEBOUNCE;
			} else {
				vTaskDelay(1); // 1ms de atraso, faz o polling do botão com frequência de 1KHz
			}
			break;

		case BUTTON_DEBOUNCE:
			vTaskDelay(18);
			if (BUTTON_D2_PRESSED() && buttonState == 0) {
				togglePump();
				buttonState = 1;
			} else if (!BUTTON_D2_PRESSED()) {
				buttonState = 0;
			}
			processState = BUTTON_READ;
			break;
		}
	}
}
