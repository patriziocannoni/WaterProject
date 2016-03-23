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

static xQueueHandle commandQueue;
static xQueueHandle responseQueue;

enum {
	WAITING_COMMAND_TO_SEND,
	SET_TX_STATE,
	SENDING_COMMAND,
	SET_RX_STATE,
	WAITING_RESPONSE,
	PARSING_RX_MESSAGE,
	PUBLISHING_SENSORS_RESPONSE
};

static unsigned char isValidCommand(unsigned char* commandPtr);
static unsigned char isValidMessageHeader(unsigned char c);
static unsigned char isValidMessageEnd(unsigned char c);
static void prvRS485MasterTask(void *arg);

xQueueHandle getRs485CommandQueue(void) {
	return commandQueue;
}

xQueueHandle getRs485ResponseQueue(void) {
	return responseQueue;
}

void xStartRS485MasterTask(void) {
	commandQueue = xQueueCreate(1, 3);
	responseQueue = xQueueCreate(1, 3);

	processState = WAITING_COMMAND_TO_SEND;

	if (commandQueue != NULL) {
		xTaskCreate(prvRS485MasterTask, (signed portCHAR *) "R485", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	}
}

void flushTaskRs485Master(void) {
	flushRs485RxBuffer();
	processState = WAITING_COMMAND_TO_SEND;
}

static void prvRS485MasterTask(void *arg) {
	unsigned char command[3];
	unsigned char rxMessage[3];
	unsigned char* msg;
	unsigned char rxChar;
	unsigned char rxIndex = 0;

	for (;;) {
		switch (processState) {
			case WAITING_COMMAND_TO_SEND:
				if (xQueueReceive(commandQueue, command, portMAX_DELAY) == pdPASS) {
					if (isValidCommand(command)) {
						processState = SET_TX_STATE;
					} else {
						flushRs485RxBuffer();
					}
				}
				break;

			case SET_TX_STATE:
				rs485SetWrite();
				vTaskDelay(RS485_TRANSITION_DELAY_MS);
				processState = SENDING_COMMAND;
				break;

			case SENDING_COMMAND:
				writeRS485(command);
				while (!rs485TransmitComplete()) {
					vTaskDelay(RS485_CHECK_TRANSMIT_OK_DELAY);
				}
				processState = SET_RX_STATE;
				break;

			case SET_RX_STATE:
				rs485SetRead();
				vTaskDelay(RS485_TRANSITION_DELAY_MS);
				processState = WAITING_RESPONSE;
				break;

			case WAITING_RESPONSE:
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
				xQueueSend(responseQueue, msg, portMAX_DELAY);
				processState = WAITING_COMMAND_TO_SEND;
				break;
		}
	}
}

static unsigned char isValidCommand(unsigned char* commandPtr) {
	// TODO Verificar a integridade da mensagem.
	return 1;
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

