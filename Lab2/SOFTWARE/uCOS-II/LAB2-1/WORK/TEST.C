/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                         2       /* Number of identical tasks                          */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK      	TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK      	TaskStartStk[TASK_STK_SIZE];

TASK_INFO  		TaskStart_Info;
TASK_INFO  		Task_Info[2];

INT16U           task1comp = 1;
INT16U           task2comp = 3;

PRINT_BUFFER printBuffer;

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  Task(void *data);                       /* Function prototypes of tasks                  */
        void  TaskStart(void *data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);



/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main (void)
{
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    printBuffer.rear = 0;
	printBuffer.front = 0;
	
	TaskStart_Info.compTime = 0;
    TaskStart_Info.period = 65535;
	TaskStart_Info.deadline = 0;
	TaskStart_Info.startTime = 0;
	OSTaskCreateExt(TaskStart,(void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0, 0, &TaskStartStk[0], TASK_STK_SIZE, (void *)&TaskStart_Info, 0);
	
    OSStart();                                             /* Start multitasking                       */
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif

    INT16S     key;

    pdata = pdata;                                         /* Prevent compiler warning                 */


    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(1);                                     /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    //OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    OSSchedLock();
    TaskStartCreateTasks();         						 /* Create application tasks                 */
    OSTimeSet(0);
    OSSchedUnlock();
	
	OSTaskDel(OS_LOWEST_PRIO-1);
	OSTaskDel(OS_PRIO_SELF);


}



/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
	Task_Info[0].compTime = 1;
    Task_Info[0].period = 3;
	Task_Info[0].deadline = 3;
	Task_Info[0].startTime = 0;
	OSTaskCreateExt(Task,(void *)&task1comp, &TaskStk[0][TASK_STK_SIZE - 1], 1, 1, &TaskStk[0][0], TASK_STK_SIZE, (void *)&Task_Info[0], 0);
	
	Task_Info[1].compTime = 3;
    Task_Info[1].period = 5;
	Task_Info[1].deadline = 5;
	Task_Info[1].startTime = 0;
	OSTaskCreateExt(Task,(void *)&task2comp, &TaskStk[1][TASK_STK_SIZE - 1], 2, 2, &TaskStk[1][0], TASK_STK_SIZE, (void *)&Task_Info[1], 0);
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void  Task (void *pdata)
{
	
	int start;
	int end; 
	int toDelay;
	TASK_INFO* taskData;
	
	
	start = 0;
	taskData = (TASK_INFO*)(OSTCBCur->OSTCBExtPtr);
	
	
	while(1)
	{
		while(taskData->compTime>0) //C ticks
		{	
			while(printBuffer.rear != printBuffer.front)
			{
				if(printBuffer.isPreempted[printBuffer.rear] == 1)
				{
					printf("%d\tPreempt\t\t%d\t%d\n", printBuffer.timeStamp[printBuffer.rear],printBuffer.from[printBuffer.rear],printBuffer.to[printBuffer.rear]);
				}
				else
				{
					printf("%d\tComplete\t%d\t%d\n", printBuffer.timeStamp[printBuffer.rear],printBuffer.from[printBuffer.rear],printBuffer.to[printBuffer.rear]);
				}
				printBuffer.rear = (printBuffer.rear+1) % MAX_PRINT_BUFFER;
			}      
		}
		
		
		OS_ENTER_CRITICAL(); 
		
		end=OSTimeGet() ; // end time
		toDelay=(taskData->period)-(end-start) ;
		start=start+(taskData->period) ; // next start time
		
		taskData->compTime= *(INT16U *)pdata;// reset the counter (c ticks for computation)
		taskData->deadline=start+taskData->period;
		taskData->startTime = start;
		
		OS_EXIT_CRITICAL();
		
		OSTimeDly (toDelay); // delay and wait (P-C) times
	}
	OSTaskDel(OS_PRIO_SELF);

}
