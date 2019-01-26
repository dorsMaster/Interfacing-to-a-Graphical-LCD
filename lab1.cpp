#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include <pinconstant.h>
#include <pins.h>
#include <basictypes.h>
#include "LCD.h"
#include "bitmaps.h"
#include "error_wrapper.h"
#include "point.h"


extern "C" {
void UserMain(void * pd);
void StartTask1(void);
void Task1Main(void * pd);

void StartTask2(void);
void Task2Main(void * pd);
void StartTask3(void);
void Task3Main(void * pd);
}




/* Task stacks for all the user tasks */
/* If you add a new task you'll need to add a new stack for that task */
DWORD Task1Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD Task2Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD Task3Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );



const char * AppName="Put your name here";


/* User task priorities always based on MAIN_PRIO */
/* The priorities between MAIN_PRIO and the IDLE_PRIO are available */
#define TASK1_PRIO 	MAIN_PRIO + 1
#define TASK2_PRIO 	MAIN_PRIO + 2
#define TASK3_PRIO	MAIN_PRIO + 3

#define WAIT_FOREVER 0

const BYTE * AtSymbol = ASCII_7[32];


LCD myLCD;
OS_SEM Task1_S;
OS_SEM Task2_S;
OS_SEM Task3_S;

void UserMain(void * pd) {
	BYTE err = OS_NO_ERR;
	char * welcome = "Welcome";

    InitializeStack();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();
    StartHTTP();
    EnableTaskMonitor();

    #ifndef _DEBUG
    EnableSmartTraps();
    #endif

    #ifdef _DEBUG
    InitializeNetworkGDB_and_Wait();
    #endif

    iprintf("Application started: %s\n",AppName );

    myLCD.Init();
    myLCD.Clear();
    myLCD.Home();
    OSSemInit(& Task1_S,1);
    OSSemInit(& Task2_S,0);
    OSSemInit(& Task3_S,0);


    StartTask1();
    StartTask2();
    StartTask3();


    while (1) {
        OSTimeDly(TICKS_PER_SECOND);
    }
}


/* Name: StartTask1
 * Description: Creates the task main loop.
 * Inputs: none
 * Outputs: none
 */
void StartTask1(void) {
	BYTE err = OS_NO_ERR;

	err = display_error( "StartTask1 fail:",
					OSTaskCreatewName(	Task1Main,
					(void *)NULL,
				 	(void *) &Task1Stk[USER_TASK_STK_SIZE],
				 	(void *) &Task1Stk[0],
				 	TASK1_PRIO, "Task 1" ));
}

/* Name: Task1Main
 * Description:
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	Task1Main( void * pd) {


	BYTE err = OS_NO_ERR;


	/* place semaphore usage code inside the loop */
	while (1) {

		for(int i=LINE1_ORIGIN; i<=LINE1_END; i++){
		    myLCD.DrawChar(AtSymbol, char_index[i]);
		    OSTimeDly(TICKS_PER_SECOND * 0.1);

		}
		myLCD.DrawChar(AtSymbol, char_index[LINE2_END]);
	    OSSemPost(&Task2_S);

	    OSSemPend(&Task2_S, WAIT_FOREVER);
		myLCD.DrawChar(AtSymbol, char_index[LINE2_ORIGIN]);
		OSTimeDly(TICKS_PER_SECOND * 0.1);
		myLCD.Clear();
	}
}


void StartTask2(void) {
	BYTE err = OS_NO_ERR;

	err = display_error( "StartTask2 fail:",
					OSTaskCreatewName(	Task2Main,
					(void *)NULL,
				 	(void *) &Task2Stk[USER_TASK_STK_SIZE],
				 	(void *) &Task2Stk[0],
				 	TASK2_PRIO, "Task 2" ));
}

/* Name: Task1Main
 * Description:
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	Task2Main( void * pd) {
	BYTE err = OS_NO_ERR;

	/* place semaphore usage code inside the loop */
	while (1) {
		OSSemPend(&Task2_S, WAIT_FOREVER);

		for(int i=LINE3_END; i<=LINE4_END; i+=12){
		    myLCD.DrawChar(AtSymbol, char_index[i]);
		    OSTimeDly(TICKS_PER_SECOND * 0.1);

		}
		OSSemPost(&Task3_S);

		OSSemPend(&Task3_S, WAIT_FOREVER);
		for(int i=LINE4_ORIGIN; i>=LINE3_ORIGIN; i-=12){
			    myLCD.DrawChar(AtSymbol, char_index[i]);
			    OSTimeDly(TICKS_PER_SECOND * 0.1);
			}
		OSSemPost(&Task2_S);
	}
}

void StartTask3(void) {
	BYTE err = OS_NO_ERR;

	err = display_error( "StartTask2 fail:",
					OSTaskCreatewName(	Task3Main,
					(void *)NULL,
				 	(void *) &Task3Stk[USER_TASK_STK_SIZE],
				 	(void *) &Task3Stk[0],
				 	TASK3_PRIO, "Task 3" ));
}

/* Name: Task1Main
 * Description:
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	Task3Main( void * pd) {
	BYTE err = OS_NO_ERR;

	/* place semaphore usage code inside the loop */
	while (1) {
		OSSemPend(&Task3_S, WAIT_FOREVER);
		myLCD.DrawChar(AtSymbol, char_index[LINE5_END]);

		for(int i=LINE6_END; i >= LINE6_ORIGIN; i--){
		    myLCD.DrawChar(AtSymbol, char_index[i]);
		    OSTimeDly(TICKS_PER_SECOND * 0.1);
		}

		myLCD.DrawChar(AtSymbol, char_index[LINE5_ORIGIN]);

		OSSemPost(&Task3_S);
	}
}

