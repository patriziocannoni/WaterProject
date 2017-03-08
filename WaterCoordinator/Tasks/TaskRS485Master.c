/*
 * TaskRS485.c
 *
 *  Created on: 10/09/2015
 *      Author: patrizio
 */

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <rs485.h>
#include <TaskRS485Master.h>

// Estado do processo.
static unsigned char processState;

static xQueueHandle responseQueue;

enum {
	WAITING_RESPONSE,
	PARSING_RX_MESSAGE,
	PUBLISHING_SENSORS_RESPONSE
};

static unsigned char isValidMessageHeader(unsigned char c);
static unsigned char isValidMessageEnd(unsigned char c);
static void prvRS485MasterTask(void *arg);

xQueueHandle getRs485ResponseQueue(void) {
	return responseQueue;
}

void xStartRS485MasterTask(void) {
	responseQueue = xQueueCreate(1, 3);

	processState = WAITING_RESPONSE;

	if (responseQueue != NULL) {
		xTaskCreate(prvRS485MasterTask, (signed portCHAR *) "R485", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	}
}

void flushTaskRs485Master(void) {
	flushRs485RxBuffer();
	flushRs485TxBuffer();
	processState = WAITING_RESPONSE;
}

static void prvRS485MasterTask(void *arg) {
	unsigned char rxMessage[3];
	unsigned char* msg;
	unsigned char rxChar;
	unsigned char rxIndex = 0;

	for (;;) {
		switch (processState) {
			case WAITING_RESPONSE:
				rs485SetRead();
				vTaskDelay(RS485_TRANSITION_DELAY_MS);
				rxChar = readRS485();

				if (rxChar != '\0' && isValidMessageHeader(rxChar)) {
					rxIndex = 0;
					rxMessage[rxIndex] = rxChar;
					rxIndex++;
					processState = PARSING_RX_MESSAGE;
				}
				break;

			case PARSING_RX_MESSAGE:
				rxChar = readRS485();

				if (rxChar != '\0') {
					rxMessage[rxIndex] = rxChar;
					if (isValidMessageEnd(rxChar)) {
						flushRs485RxBuffer();
						processState = PUBLISHING_SENSORS_RESPONSE;
					} else {
						rxIndex++;
					}
				}
				break;

			case PUBLISHING_SENSORS_RESPONSE:
				msg = rxMessage;
				while (xQueueSend(responseQueue, msg, portMAX_DELAY) != pdPASS);
				processState = WAITING_RESPONSE;
				break;
		}
	}
}

static unsigned char isValidMessageHeader(unsigned char c) {
	if (c == '|') {
		return 1;
	}
	return 0;
}

static unsigned char isValidMessageEnd(unsigned char c) {
	if (c == '\n') {
		return 1;
	}
	return 0;
}

