/*
 * TaskLCD.h
 *
 *  Created on: 14/07/2015
 *      Author: Patrizio
 */

#ifndef TASKLCD_H_
#define TASKLCD_H_

#include <queue.h>
#include <task.h>

xQueueHandle getLCDQueue(void);
void xStartLCDTask(void);

#endif /* TASKLCD_H_ */
