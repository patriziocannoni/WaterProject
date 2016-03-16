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

static void prvPumpSecurityTask(void *arg);

void xStartPumpSecurityTask(void) {
	// Pin C2 as INPUT and Pull-up ON
	DDRC &= ~_BV(DDC2);
	PORTC |= _BV(PORTC2);

	// Pin C1 as OUTPUT and Port LOW.
	DDRC |= _BV(DDC1);
	portLow();

	xTaskCreate(prvPumpSecurityTask, (signed portCHAR *) "PMSC", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

static void prvPumpSecurityTask(void *arg) {
	for (;;) {
		if (canEnabledPump()) {
			// Port LOW - Pump Enabled (Rele normally closed).
			portLow();
			vTaskDelay(100);
		} else {
			// Port HIGH - Pump Disabled (Rele open).
			portHigh();
			vTaskDelay(portMAX_DELAY);	// Volta verificar se pode habilitar a bomba cada portMAX_DELAY segundos.
		}
	}
}
