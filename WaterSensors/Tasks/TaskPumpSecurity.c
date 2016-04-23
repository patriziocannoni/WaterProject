/*
 * TaskPumpSecurity.c
 *
 *  Created on: 08/09/2015
 *      Author: patrizio
 */

#include <FreeRTOS.h>
#include <task.h>
#include "TaskPumpSecurity.h"

#define canEnabledPump() 	((PINC & 0b00000100) >> 2)
#define portLow() 			PORTC &= ~_BV(PORTC1)
#define portHigh()			PORTC |= _BV(PORTC1)

enum {
	VERIFY_LOW_SENSOR,
	VERIFY_TIMER,
	SWITCH_PUMP_ON,
	SWITCH_PUMP_OFF,
	STAND_BY_OFF
};

// Timer para verificar se pode ligar a bomba uma vez que a agua voltou.
static uint8_t timer;

// Estado do processo.
static uint8_t processState;

static void prvPumpSecurityTask(void *arg);

void xStartPumpSecurityTask(void) {
	// Pin C2 as INPUT and Pull-up ON
	DDRC &= ~_BV(DDC2);
	PORTC |= _BV(PORTC2);

	// Pin C1 as OUTPUT and Port LOW.
	DDRC |= _BV(DDC1);
	// Bomba pode ligar.
	portLow();

	timer = 0;
	processState = VERIFY_LOW_SENSOR;

	xTaskCreate(prvPumpSecurityTask, (signed portCHAR *) "PMSC", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

static void prvPumpSecurityTask(void *arg) {
	for (;;) {

		switch (processState) {
			case VERIFY_LOW_SENSOR:
				if (canEnabledPump()) {
					processState = VERIFY_TIMER;
				} else {
					timer = 0;
					processState = SWITCH_PUMP_OFF;
				}
				break;

			case VERIFY_TIMER:
				if (timer == 0 || timer > 60) {
					timer = 0;
					processState = SWITCH_PUMP_ON;
				} else {
					processState = SWITCH_PUMP_OFF;
				}
				break;

			case SWITCH_PUMP_OFF:
				portHigh();
				processState = STAND_BY_OFF;
				break;

			case SWITCH_PUMP_ON:
				portLow();
				vTaskDelay(100);
				processState = VERIFY_LOW_SENSOR;
				break;

			case STAND_BY_OFF:
				timer++;
				vTaskDelay(1000);
				processState = VERIFY_LOW_SENSOR;
				break;
		}
	}
}
