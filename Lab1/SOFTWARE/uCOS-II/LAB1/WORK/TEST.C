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

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];

OS_EVENT     *RandomSem;

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  Task1(void *data);                       /* Function prototypes of tasks                  */
		void  Task2(void *data);                       /* Function prototypes of tasks                  */
		void  Task3(void *data);                       /* Function prototypes of tasks                  */
        void  TaskStart(void *data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);

INT16U timeStamp_1;
INT16U needPrint_1;
INT16U from_1;
INT16U to_1;

INT16U timeStamp_2;
INT16U needPrint_2;
INT16U from_2;
INT16U to_2;

INT16U deadTime;
INT16U deadTask;
INT16U stopAllTask;



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

    RandomSem   = OSSemCreate(1);                          /* Random number semaphore                  */

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
	
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
    char       s[100];
    INT16S     key;




    pdata = pdata;                                         /* Prevent compiler warning                 */

	needPrint_1 = 0;
	needPrint_2 = 0;
	deadTask = 0;
	stopAllTask = 0;

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(1);                                     /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    TaskStartCreateTasks();                                /* Create all the application tasks         */
	
	OSTimeSet(0);
	

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
    INT8U  i;

    OSTaskCreate(Task1, (void *)0, &TaskStk[0][TASK_STK_SIZE - 1], 1);
	OSTaskCreate(Task2, (void *)0, &TaskStk[1][TASK_STK_SIZE - 1], 2);
	OSTaskCreate(Task3, (void *)0, &TaskStk[2][TASK_STK_SIZE - 1], 3);
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void  Task1 (void *pdata)
{
	int C = 1;
	int P = 3; 

    INT8U  err;
	
	int start; //the start time
	int end; //the end time
	int toDelay;
	
	
	OS_ENTER_CRITICAL(); 
	
	OSTCBCur->compTime=C;
	OSTCBCur->period=P;
	OSTCBCur->deadline=P;
	start=0;
	
	OS_EXIT_CRITICAL();
	
	
	
	while(stopAllTask == 0)
	{
		while(OSTCBCur->compTime>0 && stopAllTask == 0) //C ticks
		{

			if(deadTask != 0 && stopAllTask == 0)
			{
				stopAllTask = 1;
				printf("time:%d task%d exced deadline\n", deadTime,deadTask);
				break;
			}
			
			if(stopAllTask == 1)
			{
				break;
			}
			
			if(needPrint_1 != 0 && stopAllTask == 0)
			{
				if(needPrint_1 == 1)
				{
					printf("%d\tPreempt\t\t%d\t%d\n", timeStamp_1,from_1,to_1);
					needPrint_1 = 0;
				}
				else if(needPrint_1 == 2)
				{
					printf("%d\tComplete\t%d\t%d\n", timeStamp_1,from_1,to_1);
					needPrint_1 = 0;
				}
			}
			if(needPrint_2 != 0 && stopAllTask == 0)
			{
				if(needPrint_2 == 1)
				{
					printf("%d\tPreempt\t\t%d\t%d\n", timeStamp_2,from_2,to_2);
					needPrint_2 = 0;
				}
				else if(needPrint_2 == 2)
				{
					printf("%d\tComplete\t%d\t%d\n", timeStamp_2,from_2,to_2);
					needPrint_2 = 0;
				}
			}

			               
		}
		
		if(stopAllTask == 1)
		{
			break;
		}
		
		OS_ENTER_CRITICAL(); 
		
		end=OSTimeGet() ; // end time
		toDelay=(OSTCBCur->period)-(end-start) ;
		start=start+(OSTCBCur->period) ; // next start time
		OSTCBCur->compTime=C ;// reset the counter (c ticks for computation)
		OSTCBCur->deadline=start+OSTCBCur->period;
		
		OS_EXIT_CRITICAL();
		

		OSTimeDly (toDelay); // delay and wait (P-C) times
	}
	OSTaskDel(OS_PRIO_SELF);

}

void  Task2 (void *pdata)
{
	int C = 3;
	int P = 6; 

    INT8U  err;
	
	int start; //the start time
	int end; //the end time
	int toDelay;
	
	
	OS_ENTER_CRITICAL(); 
	
	OSTCBCur->period=P;
	OSTCBCur->compTime = C;
	OSTCBCur->deadline=P;
	start=0;
	
	OS_EXIT_CRITICAL();
	
	while(stopAllTask == 0)
	{
		while(OSTCBCur->compTime>0 && stopAllTask == 0) //C ticks
		{
			
			if(deadTask != 0 && stopAllTask == 0)
			{
				stopAllTask = 1;
				printf("time:%d task%d exced deadline\n", deadTime,deadTask);
				break;
			}
			
			if(stopAllTask == 1)
			{
				break;
			}
			
			if(needPrint_1 != 0 && stopAllTask == 0)
			{
				if(needPrint_1 == 1)
				{
					printf("%d\tPreempt\t\t%d\t%d\n", timeStamp_1,from_1,to_1);
					needPrint_1 = 0;
				}
				else if(needPrint_1 == 2)
				{
					printf("%d\tComplete\t%d\t%d\n", timeStamp_1,from_1,to_1);
					needPrint_1 = 0;
				}
			}
			if(needPrint_2 != 0 && stopAllTask == 0)
			{
				if(needPrint_2 == 1)
				{
					printf("%d\tPreempt\t\t%d\t%d\n", timeStamp_2,from_2,to_2);
					needPrint_2 = 0;
				}
				else if(needPrint_2 == 2)
				{
					printf("%d\tComplete\t%d\t%d\n", timeStamp_2,from_2,to_2);
					needPrint_2 = 0;
				}
			}

		}
		
		if(stopAllTask == 1)
		{
			break;
		}
		
		OS_ENTER_CRITICAL(); 
		
		end=OSTimeGet() ; // end time
		toDelay=(OSTCBCur->period)-(end-start);
		start=start+(OSTCBCur->period) ; // next start time
		OSTCBCur->compTime=C ;// reset the counter (c ticks for computation)
		OSTCBCur->deadline=start+OSTCBCur->period;
		
		OS_EXIT_CRITICAL();
		
		

		OSTimeDly (toDelay); // delay and wait (P-C) times
	}
	OSTaskDel(OS_PRIO_SELF);
}


void  Task3 (void *pdata)
{
	int C = 4;
	int P = 9; 

    INT8U  err;
	
	int start; //the start time
	int end; //the end time
	int toDelay;
	
	
	OS_ENTER_CRITICAL(); 
	
	OSTCBCur->period=P;
	OSTCBCur->compTime = C;
	OSTCBCur->deadline=P;
	start=0;
	
	OS_EXIT_CRITICAL();
	
	while(stopAllTask == 0)
	{
		while(OSTCBCur->compTime>0 && stopAllTask == 0) //C ticks
		{
			
			if(deadTask != 0 && stopAllTask == 0)
			{
				stopAllTask = 1;
				printf("time:%d task%d exced deadline\n", deadTime,deadTask);
				break;
			}
			
			if(stopAllTask == 1)
			{
				break;
			}
			
			if(needPrint_1 != 0 && stopAllTask == 0)
			{
				if(needPrint_1 == 1)
				{
					printf("%d\tPreempt\t\t%d\t%d\n", timeStamp_1,from_1,to_1);
					needPrint_1 = 0;
				}
				else if(needPrint_1 == 2)
				{
					printf("%d\tComplete\t%d\t%d\n", timeStamp_1,from_1,to_1);
					needPrint_1 = 0;
				}
			}
			if(needPrint_2 != 0 && stopAllTask == 0)
			{
				if(needPrint_2 == 1)
				{
					printf("%d\tPreempt\t\t%d\t%d\n", timeStamp_2,from_2,to_2);
					needPrint_2 = 0;
				}
				else if(needPrint_2 == 2)
				{
					printf("%d\tComplete\t%d\t%d\n", timeStamp_2,from_2,to_2);
					needPrint_2 = 0;
				}
			}

		}
		
		if(stopAllTask == 1)
		{
			break;
		}
		
		OS_ENTER_CRITICAL(); 
		
		end=OSTimeGet() ; // end time
		toDelay=(OSTCBCur->period)-(end-start);
		start=start+(OSTCBCur->period) ; // next start time
		OSTCBCur->compTime=C ;// reset the counter (c ticks for computation)
		OSTCBCur->deadline=start+OSTCBCur->period;
		
		
		OS_EXIT_CRITICAL();
		
		

		OSTimeDly (toDelay); // delay and wait (P-C) times
	}
	OSTaskDel(OS_PRIO_SELF);
}