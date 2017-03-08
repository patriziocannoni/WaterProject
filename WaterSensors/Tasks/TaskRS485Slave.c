/*
 * TaskRS485.c
 *
 *  Created on: 10/09/2015
 *      Author: patrizio
 */

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <rs485.h>
#include <TaskRS485Slave.h>

// Estado do processo.
static unsigned char processState;

static xQueueHandle responseQueue;

enum {
	PARSING_RESPONSE,
	SENDING_MESSAGE
};

static void prvRS485SlaveTask(void *arg);

void xStartRS485SlaveTask(void) {
	processState = PARSING_RESPONSE;

	responseQueue = xQueueCreate(1, 3);

	if (responseQueue != NULL) {
		xTaskCreate(prvRS485SlaveTask, (signed portCHAR *) "R485", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	}
}

xQueueHandle getRs485ResponseQueue(void) {
	return responseQueue;
}

static void prvRS485SlaveTask(void *arg) {
	unsigned char response[3];

	for (;;) {
		switch (processState) {
			case PARSING_RESPONSE:
				while (xQueueReceive(responseQueue, response, portMAX_DELAY) != pdPASS);
				processState = SENDING_MESSAGE;
				break;

			case SENDING_MESSAGE:
				rs485SetWrite();
				vTaskDelay(RS485_TRANSITION_DELAY_MS);
				writeRS485(response);
				while (!rs485TransmitComplete());
				vTaskDelay(RS485_CHECK_TRANSMIT_OK_DELAY);
				processState = PARSING_RESPONSE;
				break;
		}
	}
}
