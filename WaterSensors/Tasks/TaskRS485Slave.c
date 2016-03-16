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

static xQueueHandle commandQueue;
static xQueueHandle responseQueue;

enum {
	SET_RX_STATE,
	SET_TX_STATE,
	WAITING_RX_MESSAGE,
	PARSING_MESSAGE,
	PUBLISHING_MESSAGE,
	PARSING_RESPONSE,
	SENDING_MESSAGE
};

static void prvRS485SlaveTask(void *arg);
static unsigned char isValidMessageHeader(unsigned char c);
static unsigned char isValidMessageEnd(unsigned char c);

void xStartRS485SlaveTask(void) {
	processState = SET_RX_STATE;

	commandQueue = xQueueCreate(1, 3);
	responseQueue = xQueueCreate(1, 3);

	if (commandQueue != NULL && responseQueue != NULL) {
		xTaskCreate(prvRS485SlaveTask, (signed portCHAR *) "R485", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	}
}

xQueueHandle getRs485CommandQueue(void) {
	return commandQueue;
}

xQueueHandle getRs485ResponseQueue(void) {
	return responseQueue;
}

static void prvRS485SlaveTask(void *arg) {
	unsigned char rxChar;
	unsigned char rxMessage[3];
	unsigned char* msg;
	unsigned char response[3];
	unsigned char rxIndex = 0;

	for (;;) {
		switch (processState) {
			case SET_RX_STATE:
				rs485SetRead();
				vTaskDelay(RS485_TRANSITION_DELAY_MS);
				processState = WAITING_RX_MESSAGE;
				break;

			case WAITING_RX_MESSAGE:
				vTaskDelay(RS485_TRANSITION_DELAY_MS);

				rxChar = readRS485();

				if (rxChar && isValidMessageHeader(rxChar)) {
					rxIndex = 0;
					rxMessage[rxIndex] = rxChar;
					rxIndex++;
					processState = PARSING_MESSAGE;
				} else {
					flushRs485RxBuffer();
				}
				break;

			case PARSING_MESSAGE:
				rxChar = readRS485();

				if (rxChar != '\0') {
					rxMessage[rxIndex] = rxChar;
					if (isValidMessageEnd(rxChar)) {
						flushRs485RxBuffer();
						processState = PUBLISHING_MESSAGE;
					} else {
						rxIndex++;
					}
				}
				break;

			case PUBLISHING_MESSAGE:
				msg = rxMessage;
				xQueueSend(commandQueue, msg, portMAX_DELAY);
				processState = PARSING_RESPONSE;
				break;

			case PARSING_RESPONSE:
				while (xQueueReceive(responseQueue, response, portMAX_DELAY) != pdPASS);
				vTaskDelay(100); // Espera um pouco para responder.
				processState = SET_TX_STATE;
				break;

			case SET_TX_STATE:
				rs485SetWrite();
				vTaskDelay(RS485_TRANSITION_DELAY_MS);
				processState = SENDING_MESSAGE;
				break;

			case SENDING_MESSAGE:
				writeRS485(response);

				while (!rs485TransmitComplete()) {
					vTaskDelay(RS485_CHECK_TRANSMIT_OK_DELAY);
				}

				processState = SET_RX_STATE;
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
