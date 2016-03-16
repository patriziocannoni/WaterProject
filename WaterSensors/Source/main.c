/*
 * main.c
 *
 *  Created on: 29/08/2015
 *      Author: Patrizio
 */

#include <task.h>
#include <rs485.h>
#include <TaskPumpSecurity.h>
#include <TaskRS485Slave.h>
#include <TaskSensors.h>

int main(void) {
	initRS485();

	// Inicializa a tarefa do protocolo RS485.
	xStartRS485SlaveTask();

	// Inicializa a tarefa de segurança da bomba.
	xStartPumpSecurityTask();

	// Inicializa a tarefa dos sensores.
	xStartSensorsTask();

	// Inicia o scheduler.
	vTaskStartScheduler();

	return 0;
}
