/*
 * TaskLCD.c
 *
 *  Created on: 14/07/2015
 *      Author: Patrizio
 */

#include <TaskLCD.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <lcd.h>

#define lcdQueueSize 3

static xQueueHandle xLCDQueue;

static void prvLCDTask(void *arg);

xQueueHandle xStartLCDTask(void) {
	xLCDQueue = xQueueCreate(lcdQueueSize, sizeof(LCDMessage));
	if (xLCDQueue != NULL) {
		xTaskCreate(prvLCDTask, (signed portCHAR *) "LCD", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	}
	return xLCDQueue;
}

static void prvLCDTask(void *arg) {
	LCDMessage xMessage;

	for (;;) {
		while (xQueueReceive(xLCDQueue, &xMessage, portMAX_DELAY) != pdPASS);
		escreverLCD(&xMessage);
	}
}
