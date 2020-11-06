#include <avr/io.h>
#include <avr/interrupt.h>

#include "AVR_TTC_scheduler.h"	// Scheduler data structure for storing task data, max tasks
#include "hardware.h"			// Hardware defenities
#include "adc.h"				// analoge input, lichtsensor en temperatuursensor
#include <util/delay.h>
#include "distanceheader.h"		// ultrasonor header
#include "distance.h"			// ultrasonor sensor
#include "uart.h"


// The array of tasks
sTask SCH_tasks_G[SCH_MAX_TASKS];

uint8_t useSensor = LOW;	// zonnescherm wordt niet meteen automatisch aangestuurd
uint8_t isUit = HIGH;		// zonnescherm is in.

/*------------------------------------------------------------------*-

  SCH_Dispatch_Tasks()

  This is the 'dispatcher' function.  When a task (function)
  is due to run, SCH_Dispatch_Tasks() will run it.
  This function must be called (repeatedly) from the main loop.

-*------------------------------------------------------------------*/

void SCH_Dispatch_Tasks(void)
{
   unsigned char Index;

   // Dispatches (runs) the next task (if one is ready)
   for(Index = 0; Index < SCH_MAX_TASKS; Index++)
   {
      if((SCH_tasks_G[Index].RunMe > 0) && (SCH_tasks_G[Index].pTask != 0))
      {
         (*SCH_tasks_G[Index].pTask)();  // Run the task
         SCH_tasks_G[Index].RunMe -= 1;   // Reset / reduce RunMe flag

         // Periodic tasks will automatically run again
         // - if this is a 'one shot' task, remove it from the array
         if(SCH_tasks_G[Index].Period == 0)
         {
            SCH_Delete_Task(Index);
         }
      }
   }
}

/*------------------------------------------------------------------*-

  SCH_Add_Task()

  Causes a task (function) to be executed at regular intervals 
  or after a user-defined delay

  pFunction - The name of the function which is to be scheduled.
              NOTE: All scheduled functions must be 'void, void' -
              that is, they must take no parameters, and have 
              a void return type. 
                   
  DELAY     - The interval (TICKS) before the task is first executed

  PERIOD    - If 'PERIOD' is 0, the function is only called once,
              at the time determined by 'DELAY'.  If PERIOD is non-zero,
              then the function is called repeatedly at an interval
              determined by the value of PERIOD (see below for examples
              which should help clarify this).


  RETURN VALUE:  

  Returns the position in the task array at which the task has been 
  added.  If the return value is SCH_MAX_TASKS then the task could 
  not be added to the array (there was insufficient space).  If the
  return value is < SCH_MAX_TASKS, then the task was added 
  successfully.  

  Note: this return value may be required, if a task is
  to be subsequently deleted - see SCH_Delete_Task().

  EXAMPLES:

  Task_ID = SCH_Add_Task(Do_X,1000,0);
  Causes the function Do_X() to be executed once after 1000 sch ticks.            

  Task_ID = SCH_Add_Task(Do_X,0,1000);
  Causes the function Do_X() to be executed regularly, every 1000 sch ticks.            

  Task_ID = SCH_Add_Task(Do_X,300,1000);
  Causes the function Do_X() to be executed regularly, every 1000 ticks.
  Task will be first executed at T = 300 ticks, then 1300, 2300, etc.            
 
-*------------------------------------------------------------------*/

unsigned char SCH_Add_Task(void (*pFunction)(), const unsigned int DELAY, const unsigned int PERIOD)
{
   unsigned char Index = 0;

   // First find a gap in the array (if there is one)
   while((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS))
   {
      Index++;
   }

   // Have we reached the end of the list?   
   if(Index == SCH_MAX_TASKS)
   {
      // Task list is full, return an error code
      return SCH_MAX_TASKS;  
   }

   // If we're here, there is a space in the task array
   SCH_tasks_G[Index].pTask = pFunction;
   SCH_tasks_G[Index].Delay =DELAY;
   SCH_tasks_G[Index].Period = PERIOD;
   SCH_tasks_G[Index].RunMe = 0;

   // return position of task (to allow later deletion)
   return Index;
}

/*------------------------------------------------------------------*-

  SCH_Delete_Task()

  Removes a task from the scheduler.  Note that this does
  *not* delete the associated function from memory: 
  it simply means that it is no longer called by the scheduler. 
 
  TASK_INDEX - The task index.  Provided by SCH_Add_Task(). 

  RETURN VALUE:  RETURN_ERROR or RETURN_NORMAL

-*------------------------------------------------------------------*/

unsigned char SCH_Delete_Task(const unsigned char TASK_INDEX)
{
   // Return_code can be used for error reporting, NOT USED HERE THOUGH!
   unsigned char Return_code = 0;

   SCH_tasks_G[TASK_INDEX].pTask = 0;
   SCH_tasks_G[TASK_INDEX].Delay = 0;
   SCH_tasks_G[TASK_INDEX].Period = 0;
   SCH_tasks_G[TASK_INDEX].RunMe = 0;

   return Return_code;
}

/*------------------------------------------------------------------*-

  SCH_Init_T1()

  Scheduler initialisation function.  Prepares scheduler
  data structures and sets up timer interrupts at required rate.
  You must call this function before using the scheduler.  

-*------------------------------------------------------------------*/

void SCH_Init_T1(void)
{
   unsigned char i;

   for(i = 0; i < SCH_MAX_TASKS; i++)
   {
      SCH_Delete_Task(i);
   }

   // Set up Timer 1
   // Values for 1ms and 10ms ticks are provided for various crystals

   // Hier moet de timer periode worden aangepast ....!
   TCCR1A = 0;
   TCCR1B = 0;		// timers op 0, geen prescale
}

/*------------------------------------------------------------------*-

  SCH_Start()

  Starts the scheduler, by enabling interrupts.

  NOTE: Usually called after all regular tasks are added,
  to keep the tasks synchronised.

  NOTE: ONLY THE SCHEDULER INTERRUPT SHOULD BE ENABLED!!! 
 
-*------------------------------------------------------------------*/

void SCH_Start(void)
{
      sei();
}

/*------------------------------------------------------------------*-

  SCH_Update

  This is the scheduler ISR.  It is called at a rate 
  determined by the timer settings in SCH_Init_T1().

-*------------------------------------------------------------------*/

ISR(TIMER1_COMPA_vect)
{
   unsigned char Index;
   for(Index = 0; Index < SCH_MAX_TASKS; Index++)
   {
      // Check if there is a task at this location
      if(SCH_tasks_G[Index].pTask)
      {
         if(SCH_tasks_G[Index].Delay == 0)
         {
            // The task is due to run, Inc. the 'RunMe' flag
            SCH_tasks_G[Index].RunMe += 1;

            if(SCH_tasks_G[Index].Period)
            {
               // Schedule periodic tasks to run again
               SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period;
               SCH_tasks_G[Index].Delay -= 1;
            }
         }
         else
         {
            // Not yet ready to run: just decrement the delay
            SCH_tasks_G[Index].Delay -= 1;
         }
      }
   }
}

// ------------------------------------------------------------------

void initPorts(void) {
	DDRD |= 0xfc;	// pd 2-7 input (knoppen & sonor echo), TX en RX worden niet veranderd
	DDRB = 0x00;	// pb 0-7 output (sonor trigger en leds).
}

void toggleGeel() {
	PORTB ^= 1 << GEEL;
}

void inrollen(void) {
	unsigned char index = SCH_Add_Task(toggleGeel,0,5000);	// gele led gaat knipperen.
	uint8_t afstand = 0;
	while(afstand > 1 || afstand < 20) {// afstand is hardcoded, moet nog veranderd worden
		SCH_Dispatch_Tasks();			// check of er taak is om uit te voeren
		afstand = getDistance();
	}
	SCH_Delete_Task(index);	// gele led stopt met knipperen.
	PORTB |= 1 << GROEN;	// lampje dat aangeeft dat het scherm in is.
	PORTB &= ~(1 << GEEL);	// andere lampjes uit.
	PORTB &= ~(1 << ROOD);
}

void uitrollen(void) {
	unsigned char index = SCH_Add_Task(toggleGeel,0,5000);	// gele led gaat knipperen.
	uint8_t afstand = 0;
	while(afstand > 20) {			// afstand is hardcoded, moet nog veranderd worden.
		SCH_Dispatch_Tasks();		// check of er taak is om uit te voeren
		afstand = getDistance();
	}
	SCH_Delete_Task(index);	// gele led stopt met knipperen.
	PORTB |= 1 << ROOD;	// lampje dat aangeeft dat het scherm uit is.
	PORTB &= ~(1 << GEEL);	// andere lampjes uit.
	PORTB &= ~(1 << GROEN);
}

void autoBestuur(void) {	// temp & licht is hardcoded, moet nog veranderd worden.
	if(isUit == HIGH) {								// wanneer scherm uitgerold is ->
		if(licht() < 0x80 || temperatuur() < 0x14){	// wanneer er weinig licht is OF een temperatuur onder 20 graden ->
			inrollen();								// scherm inrollen
		}
	}
	if(isUit == LOW){								// wanneer scherm ingerold is ->
		if(licht() > 0x80 && temperatuur() > 0x14){	// wanneer er veel licht is EN een temperatuur boven 20 graden ->
			inrollen();								// scherm uitrollen
		}
	}
}

void verstuurData(void) {
	transmit(licht());
	_delay_ms(1000);
	transmit(temperatuur());
	_delay_ms(1000);
	transmit(isUit);
}

int main(void) {
	SCH_Init_T1();
	initPorts();
	initUltrasonoor();
	uart_init();
	SCH_Start();
	//SCH_Add_Task(verstuurData,0,1);
	_delay_ms(500);
	PORTB |= 1 << BLAUW;		// blauwe led aan omdat zonnescherm op handmatig staat
	while(1) {
		verstuurData();
		uint8_t pin = PIND;
		SCH_Dispatch_Tasks();		// check of er taak is om uit te voeren
		if(pin & (1 << aanUit)){		// knopje om handmatig in/uit te rollen is ingedrukt.
			if(useSensor == HIGH){		// wanneer automatische bediening aan staat ->
				useSensor = LOW;		// automatische bediening uit.
				PORTB |= 1 << BLAUW;	// led die handmatige bedingen aangeeft gaat aan.
			}
			if(isUit == HIGH){		// wanneer zonnescherm uitgerold is ->
				inrollen();			// zonnescherm inrollen.
			}
			else{					// wanneer zonnescherm ingerold is ->
				uitrollen();		// zonnescherm inrollen.
			}
		}
		else if(pin & (1 << automaat)){	// wanneer automatische bedienings knopje ingedrukt is ->
			useSensor = HIGH;			// automatische bediening aan.
			PORTB &= ~(1 << BLAUW);		// led die handmatige bedingen aangeeft gaat uit.
		}
		if(useSensor == HIGH) {		// wanneer automatische bediening aan staat ->
			autoBestuur();			// zonnescherm aansturen volgens ingestelde waarde.
		}
		
	}
	// Hier moeten nog enkele statements komen ....!
	return 0;
}