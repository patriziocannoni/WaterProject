/*
 * main.c
 *
 *  Created on: 31/07/2015
 *      Author: Patrizio
 */

#include <avr/interrupt.h>
#include <task.h>
#include <TaskPump.h>
#include <TaskButton.h>

int main(void) {
	cli();          			// disable global interrupts

	// Inicializa a tarefa de polling do botão.
	xStartButtonTask();

	// Inicializa tarefa da bomba.
	xStartPumpTask();

	sei();          			// enable global interrupts

	// Inicia o scheduler.
	vTaskStartScheduler();

	return 0;
}
