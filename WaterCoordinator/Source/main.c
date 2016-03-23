/*
 * main.c
 *
 *  Created on: 31/07/2015
 *      Author: Patrizio
 */

#include <task.h>
#include <queue.h>
#include <lcd.h>
#include <rs485.h>
#include <TaskLCD.h>
#include <TaskRS485Master.h>
#include <TaskSensors.h>

int main(void) {
	xQueueHandle lcdQueueHandle;

	// Inicializa o hardware.
	inicializarLCD();
	_delay_ms(100);
	initRS485();
	_delay_ms(100);

	// Inicializa a tarefa do protocolo RS485.
	xStartRS485MasterTask();

	// Inicializa a tarefa do LCD.
	lcdQueueHandle = xStartLCDTask();

	// Inicializa a tarefa de leitura dos sensores.
	xStartSensorsTask(lcdQueueHandle);

	_delay_ms(100);

	// Inicia o scheduler.
	vTaskStartScheduler();

	return 0;
}
