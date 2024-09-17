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
#define  N_TASKS                         3       /* Number of identical tasks                          */


#define R1_Priority 1
#define R2_Priority 2

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
TASK_INFO  		TaskStart_Info;
TASK_INFO  		Task_Info[N_TASKS];


PRINT_BUFFER printBuffer;


//The priority of mutex lock is determined based on the usage of mutex lock for each task.
OS_EVENT     *r1;  //priority -> 1
OS_EVENT     *r2;  //priority -> 2




/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/


//=================================================LAB_1=================================================
        void  Task1(void *data);           
		void  Task2(void *data);       
		void  Task3(void *data);       		
        void  TaskStart(void *data);                  
static  void  TaskStartCreateTasks(void);
//=================================================LAB_1=================================================


/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main (void)
{
	INT8U err;
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

	r1 = OSMutexCreate(R1_Priority, &err);
	r2 = OSMutexCreate(R2_Priority, &err);

    printBuffer.rear = 0;
	printBuffer.front = 0;
	
	TaskStart_Info.compTime = 0;
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

	
	printBuffer.rear = 0;
	printBuffer.front = 0;

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
	
	OSTimeDly (30);
	
	while(1)
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
	
	//OSTaskDel(OS_PRIO_SELF);
}



/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    Task_Info[0].compTime = 2;
	OSTaskCreateExt(Task1,(void *)0, &TaskStk[0][TASK_STK_SIZE - 1], 3, 3, &TaskStk[0][0], TASK_STK_SIZE, (void *)&Task_Info[0], 0);
	
	Task_Info[1].compTime = 2;
	OSTaskCreateExt(Task2,(void *)0, &TaskStk[1][TASK_STK_SIZE - 1], 4, 4, &TaskStk[1][0], TASK_STK_SIZE, (void *)&Task_Info[1], 0);
	
	Task_Info[2].compTime = 2;
	OSTaskCreateExt(Task3,(void *)0, &TaskStk[2][TASK_STK_SIZE - 1], 5, 5, &TaskStk[2][0], TASK_STK_SIZE, (void *)&Task_Info[2], 0);
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void  Task1 (void *pdata)
{
	int currentTime;
	int previousPriority;
	int currentPriority;
	INT8U  err;
	TASK_INFO* taskData;
	taskData = (TASK_INFO*)(OSTCBCur->OSTCBExtPtr);

	OSTimeDly (8);
	
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
	
	taskData->compTime = 2;
	previousPriority = OSTCBCur->OSTCBPrio;
	OSMutexPend(r1,0,&err);
	currentPriority = OSTCBCur->OSTCBPrio;
	currentTime=OSTimeGet() ;
	printf("%d\tlock\tR1\t(Prio=%d changes to=%d)\n", currentTime,previousPriority,currentPriority);
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
	
	taskData->compTime = 2;
	previousPriority = OSTCBCur->OSTCBPrio;
	OSMutexPend(r2,0,&err);
	currentPriority = OSTCBCur->OSTCBPrio;
	currentTime=OSTimeGet() ;
	printf("%d\tlock\tR2\t(Prio=%d changes to=%d)\n", currentTime,previousPriority,currentPriority);
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
	previousPriority = OSTCBCur->OSTCBPrio;
	OSMutexPost(r2);
	currentPriority = OSTCBCur->OSTCBPrio;
	currentTime=OSTimeGet();
	printf("%d\tunlock\tR2\t(Prio=%d changes to=%d)\n", currentTime,previousPriority,currentPriority);
	
	previousPriority = OSTCBCur->OSTCBPrio;
	OSMutexPost(r1);
	currentPriority = OSTCBCur->OSTCBPrio;
	currentTime=OSTimeGet();
	printf("%d\tunlock\tR1\t(Prio=%d changes to=%d)\n", currentTime,previousPriority,currentPriority);
	
	OSTaskDel(OS_PRIO_SELF);

}

void  Task2 (void *pdata)
{
	int currentTime;
	int previousPriority;
	int currentPriority;
	INT8U  err;
	TASK_INFO* taskData;
	taskData = (TASK_INFO*)(OSTCBCur->OSTCBExtPtr);
	
	OSTimeDly (4);
	
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
	
	taskData->compTime = 4;
	previousPriority = OSTCBCur->OSTCBPrio;
	OSMutexPend(r2,0,&err);
	currentPriority = OSTCBCur->OSTCBPrio;
	currentTime=OSTimeGet() ;
	printf("%d\tlock\tR2\t(Prio=%d changes to=%d)\n", currentTime,previousPriority,currentPriority);
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
	previousPriority = OSTCBCur->OSTCBPrio;
	OSMutexPost(r2);
	currentPriority = OSTCBCur->OSTCBPrio;
	currentTime=OSTimeGet();
	printf("%d\tunlock\tR2\t(Prio=%d changes to=%d)\n", currentTime,previousPriority,currentPriority);
	
	
	OSTaskDel(OS_PRIO_SELF);

}

void  Task3 (void *pdata)
{
	int currentTime;
	int previousPriority;
	int currentPriority;
	INT8U  err;
	TASK_INFO* taskData;
	taskData = (TASK_INFO*)(OSTCBCur->OSTCBExtPtr);
	
	//OSTimeDly (0);
	
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
	
	taskData->compTime = 7;
	
	previousPriority = OSTCBCur->OSTCBPrio;
	OSMutexPend(r1,0,&err);
	currentPriority = OSTCBCur->OSTCBPrio;
	currentTime=OSTimeGet() ;
	printf("%d\tlock\tR1\t(Prio=%d changes to=%d)\n", currentTime,previousPriority,currentPriority);
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

	previousPriority = OSTCBCur->OSTCBPrio;
	OSMutexPost(r1);
	currentPriority = OSTCBCur->OSTCBPrio;
	currentTime=OSTimeGet();
	printf("%d\tunlock\tR1\t(Prio=%d changes to=%d)\n", currentTime,previousPriority,currentPriority);
	
	
	
	OSTaskDel(OS_PRIO_SELF);

}