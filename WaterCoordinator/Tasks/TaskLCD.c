/*
 * TaskLCD.c
 *
 *  Created on: 14/07/2015
 *      Author: Patrizio
 */

#include <FreeRTOS.h>
#include <TaskLCD.h>
#include <lcd.h>

#define lcdQueueSize 3

static xQueueHandle lcdQueue;

static void prvLCDTask(void *arg);

xQueueHandle getLCDQueue(void) {
	return lcdQueue;
}

void xStartLCDTask(void) {
	lcdQueue = xQueueCreate(lcdQueueSize, sizeof(LCDMessage));
	if (lcdQueue != NULL) {
		xTaskCreate(prvLCDTask, (signed portCHAR *) "LCD", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	}
}

static void prvLCDTask(void *arg) {
	LCDMessage xMessage;

	for (;;) {
		while (xQueueReceive(lcdQueue, &xMessage, portMAX_DELAY) != pdPASS);
		escreverLCD(&xMessage);
	}
}
