/*
 * TaskRS485.h
 *
 *  Created on: 10/09/2015
 *      Author: patrizio
 */

#ifndef TASKRS485MASTER_H_
#define TASKRS485MASTER_H_

#include <queue.h>

void xStartRS485MasterTask(void);
xQueueHandle getRs485CommandQueue(void);
xQueueHandle getRs485ResponseQueue(void);
void flushTaskRs485Master(void);

#endif /* TASKRS485MASTER_H_ */
