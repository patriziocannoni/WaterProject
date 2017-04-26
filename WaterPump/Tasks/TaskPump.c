/*
 * TaskPump.c
 *
 *  Created on: 17/08/2015
 *      Author: Patrizio
 */

#include <avr/io.h>
#include "TaskPump.h"

static uint8_t pumpState;

void xStartPumpTask(void) {
	// Pin C3 as OUTPUT.
	DDRD |= _BV(DDD7);
	pumpStop();
}

void pumpStart(void) {
	// Port HIGH.
	PORTD |= _BV(PORTD7);
	// Guarda o estado lógico da bomba
	pumpState = 1;
}

void pumpStop(void) {
	// Port HIGH.
	PORTD &= ~_BV(PORTD7);
	// Guarda o estado lógico da bomba
	pumpState = 0;
}

void togglePump(void) {
	if (pumpState) {
		pumpStop();
	} else {
		pumpStart();
	}
}
