/*
 * TaskSensors.c
 *
 *  Created on: 31/07/2015
 *      Author: Patrizio
 */

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <rs485.h>
#include <lcd.h>
#include "TaskRS485Master.h"
#include "TaskSensors.h"
#include "TaskLCD.h"

static void prvSensorsTask(void *arg);
static void prvShowSensorsState(unsigned char sensorsState);
static void prvShowSensorsOffLine(void);
static void prvShowSensorsOnLine(void);

// Estado do processo.
static unsigned char processState;

// Constante de TIMEOUT
// Se o Coordinator não recebe nada dos sensores dentro do timeout, mostra sensores off-line no display
static uint16_t TIMEOUT = 500;

static char* lcdSensorsOffLineMsg = "SNS OFF";
static char* lcdSensorsOnLineMsg = "SNS OK ";
static char* lcdSensorsError = "----";
static unsigned char sensorsResponse[3];

enum {
	WAIT_SENSORS_STATE,
	SHOW_SENSORS_STATE,
	SHOW_SENSORS_OFF_LINE
};

void xStartSensorsTask() {
	processState = WAIT_SENSORS_STATE;
	xTaskCreate(prvSensorsTask, (signed portCHAR *) "SNRS", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
}

static void prvSensorsTask(void *arg) {
	for (;;) {
		switch (processState) {
			case WAIT_SENSORS_STATE:
				// Espera uma resposta por 5 segundos. Depois disso TIMEOUT.
				if (xQueueReceive(getRs485ResponseQueue(), sensorsResponse, TIMEOUT) == pdPASS) {
					prvShowSensorsOnLine();
					processState = SHOW_SENSORS_STATE;
				} else {
					flushTaskRs485Master();
					processState = SHOW_SENSORS_OFF_LINE;
				}
				break;

			case SHOW_SENSORS_STATE:
				prvShowSensorsState(sensorsResponse[1]);
				processState = WAIT_SENSORS_STATE;
				break;

			case SHOW_SENSORS_OFF_LINE:
				prvShowSensorsOffLine();
				processState = WAIT_SENSORS_STATE;
				break;
		}
	}
}

static void prvShowSensorsState(unsigned char sensorsState) {
	// A mensagem para exibir no LCD o estado dos sensores.
	LCDMessage message;
	char lcdMsg[4];

	lcdMsg[3] = (sensorsState >> 4) & 0x08 ? '1' : '0' ;
	lcdMsg[2] = (sensorsState >> 4) & 0x04 ? '1' : '0' ;
	lcdMsg[1] = (sensorsState >> 4) & 0x02 ? '1' : '0' ;
	lcdMsg[0] = (sensorsState >> 4) & 0x01 ? '1' : '0' ;

	message.line = LCD_LINHA_1;
	message.column = 0;
	message.pcMessage = (uint8_t *) lcdMsg;
	xQueueSend(getLCDQueue(), &message, portMAX_DELAY);
}

static void prvShowSensorsOffLine(void) {
	LCDMessage message;

	message.line = LCD_LINHA_1;
	message.column = 0;
	message.pcMessage = (uint8_t *) lcdSensorsError;
	xQueueSend(getLCDQueue(), &message, portMAX_DELAY);

	message.line = LCD_LINHA_2;
	message.pcMessage = (uint8_t *) lcdSensorsOffLineMsg;
	xQueueSend(getLCDQueue(), &message, portMAX_DELAY);
}

static void prvShowSensorsOnLine(void) {
	LCDMessage message;

	message.line = LCD_LINHA_2;
	message.column = 0;
	message.pcMessage = (uint8_t *) lcdSensorsOnLineMsg;
	xQueueSend(getLCDQueue(), &message, portMAX_DELAY);
}
