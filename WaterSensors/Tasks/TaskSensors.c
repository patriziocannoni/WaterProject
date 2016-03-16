/*
 * TaskSensors.c
 *
 *  Created on: 29/08/2015
 *      Author: Patrizio
 */

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <rs485.h>
#include <string.h>
#include <TaskRS485Slave.h>
#include "TaskSensors.h"

// Mask do input PINC.
// Apenas precisa do estado dos bits 2-3-4-5.
#define sensorsState() ((PINC & 0b00111100) >> 2)

static void prvSensorsTask(void *arg);

// Estado do processo.
static unsigned char processState;

// Variáveis com estado dos sensores - leitura dupla para segurança.
static unsigned char sensorsState01;
static unsigned char sensorsState02;

enum {
	WAITING_SENSORS_COMMAND,
	READ_SENSORS_STEP1,
	READ_SENSORS_STEP2,
	SEND_RESPONSE
};

void xStartSensorsTask(void) {
	// Pin C5 as INPUT and Pull-up ON
	DDRC &= ~_BV(DDC5);
	PORTC |= _BV(PORTC5);

	// Pin C4 as INPUT and Pull-up ON
	DDRC &= ~_BV(DDC4);
	PORTC |= _BV(PORTC4);

	// Pin C3 as INPUT and Pull-up ON
	DDRC &= ~_BV(DDC3);
	PORTC |= _BV(PORTC3);

	processState = WAITING_SENSORS_COMMAND;
	xTaskCreate(prvSensorsTask, (signed portCHAR *) "SNRS", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
}

static void prvSensorsTask(void *arg) {
	unsigned char message[3];
	unsigned char response[3];

	for (;;) {
		switch (processState) {
			case WAITING_SENSORS_COMMAND:
				while (xQueueReceive(getRs485CommandQueue(), message, portMAX_DELAY) != pdPASS);
				if (strcmp((const char*) message, (const char*) "|S\n")) {
					processState = READ_SENSORS_STEP1;
				}
				break;

			case READ_SENSORS_STEP1:
				vTaskDelay(10);
				sensorsState01 = sensorsState();
				processState = READ_SENSORS_STEP2;
				break;

			case READ_SENSORS_STEP2:
				vTaskDelay(10);
				sensorsState02 = sensorsState();

				if (sensorsState02 == sensorsState01) {
					processState = SEND_RESPONSE;
				} else {
					processState = READ_SENSORS_STEP1;
				}
				break;

			case SEND_RESPONSE:
				response[0] = '|';
				response[1] = (sensorsState01 << 4) & 0xFF; // FIXME Melhorar este masking
				response[2] = '\n';
				xQueueSend(getRs485ResponseQueue(), response, portMAX_DELAY);
				processState = WAITING_SENSORS_COMMAND;
				break;
		}
	}
}
