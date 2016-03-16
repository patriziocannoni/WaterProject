/*
 * TaskRS485.h
 *
 *  Created on: 10/09/2015
 *      Author: patrizio
 */

#ifndef TASKRS485SLAVE_H_
#define TASKRS485SLAVE_H_

#include <queue.h>

void xStartRS485SlaveTask(void);
xQueueHandle getRs485CommandQueue(void);
xQueueHandle getRs485ResponseQueue(void);

#endif /* TASKRS485SLAVE_H_ */
