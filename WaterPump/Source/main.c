/*
 * main.c
 *
 *  Created on: 31/07/2015
 *      Author: Patrizio
 */

#include <task.h>
#include <TaskPump.h>
#include <TaskButton.h>

int main(void) {
	// Inicializa a tarefa de polling do botão.
	xStartButtonTask();

	// Inicializa tarefa da bomba.
	xStartPumpTask();

	// Inicia o scheduler.
	vTaskStartScheduler();

	return 0;
}
