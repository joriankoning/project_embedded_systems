#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16E6
#include <util/delay.h>

#include "AVR_TTC_scheduler.h"	// Scheduler data structure for storing task data, max tasks
#include "hardware.h"			// Hardware defenities
#include "uart.h"				// Data verzenden/ontvangen via USB
#include "adc.h"				// analoge input, lichtsensor en temperatuursensor
#include "distance.h"			// ultrasonor sensor


volatile uint16_t gv_counter; // 16 bit counter value
volatile uint8_t gv_echo; // a flag


// The array of tasks
sTask SCH_tasks_G[SCH_MAX_TASKS];

uint8_t useSensor = LOW;	// zonnescherm wordt niet meteen automatisch aangestuurd
uint8_t isUit = LOW;		// zonnescherm is in.
uint8_t maxAfstand = 0xe0;	// maximale uitrol zonnescherm

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

void SCH_Init_T2(void)
{
   unsigned char i;

   for(i = 0; i < SCH_MAX_TASKS; i++)
   {
      SCH_Delete_Task(i);
   }

   // Set up Timer 1
   // Values for 1ms and 10ms ticks are provided for various crystals
	
	OCR2B = (uint16_t)625;   		     // 10ms = (256/16.000.000) * 625
	TCCR2B = (1 << CS12) | (1 << WGM12);  // prescale op 64, top counter = value OCR1A (CTC mode)
	TIMSK2 = 1 << OCIE2B;   		     // Timer 1 Output Compare A Match Interrupt Enable
}

void init_timer() {	// timer voor ultra sonor
	TCCR1A = 0;
	TCCR1B = 0;
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

void init_ext_int(void) {
	// any change triggers ext interrupt 1
	// dit zorgt er voor dat wanneer de ultrasonoor sensor een interupt geeft, dat deze ook wordt opgevangen
	EICRA = (1 << ISC10);
	EIMSK = (1 << INT1);
	
	gv_echo = 0;	// klaar om een pulse te sturen
}

void init_ports(void) {
	DDRB = 0xff;	//pb0, pb1 en pb2 output
	DDRD = 0x00;	//pd1 output, de rest(ook pd3) input
	
	PORTB |= 1 << GROEN;		// groene led want scherm is ingerold
	PORTB |= 1 << BLAUW;		// blauwe led want auto staat uit
}

void toggleGeel() {
	PORTB ^= 1 << GEEL;			// gele led krijgt tegenovergestelde waarde
}

void inrollen(void) {
	PORTB |= 1 << GROEN;	// lampje dat aangeeft dat het scherm in (aan het rollen) is.
	PORTB &= ~(1 << ROOD);
	unsigned char index = SCH_Add_Task(toggleGeel,0,400);	// gele led gaat knipperen.
	uint8_t afstand = 0xff;
	while(afstand > 0x20) {			// minder dan 2 cm
		SCH_Dispatch_Tasks();		// check of er taak is om uit te voeren
		if(gv_echo == 0){	// als gv_echo 0 is, is er nog geen pulse verstuurd.
			sendSignal();	// pulse sturen.
		}
		_delay_ms(120);		// delay van 100 ms, als er dan nog geen return pulse is, wordt hij ook niet meer verwacht.
		afstand = (caclCM(gv_counter));	// afstand komt op het schermpje
		gv_echo = 0;		// gv_echo weer op 0, zodat er een nieuwe pulse verstuurd kan worden
	}
	isUit = LOW;
	SCH_Delete_Task(index);	// gele led stopt met knipperen.
	PORTB &= ~(1 << GEEL);	// andere lampjes uit.
	_delay_ms(200);			// delay zodat niet gelijk weer een knopje press gezien wordt
}

void uitrollen(void) {
	PORTB |= 1 << ROOD;	// lampje dat aangeeft dat het scherm uit (aan het rollen) is.
	PORTB &= ~(1 << GROEN);
	unsigned char index = SCH_Add_Task(toggleGeel,0,400);	// gele led gaat knipperen.
	uint8_t afstand = 0;
	while(afstand < maxAfstand) {	// afstand is hardcoded, moet nog veranderd worden.
		SCH_Dispatch_Tasks();		// check of er taak is om uit te voeren
		if(gv_echo == 0){	// als gv_echo 0 is, is er nog geen pulse verstuurd.
			sendSignal();	// pulse sturen.
		}
		_delay_ms(120);		// delay van 100 ms, als er dan nog geen return pulse is, wordt hij ook niet meer verwacht.
		afstand = (caclCM(gv_counter));	// afstand komt op het schermpje
		gv_echo = 0;		// gv_echo weer op 0, zodat er een nieuwe pulse verstuurd kan worden
	}
	isUit = HIGH;
	SCH_Delete_Task(index);	// gele led stopt met knipperen.
	PORTB &= ~(1 << GEEL);	// andere lampjes uit.
	_delay_ms(200);			// delay zodat niet gelijk weer een knopje press gezien wordt
}

void autoBestuur(void) {	// temp & licht is hardcoded, moet nog veranderd worden.
	if(isUit == HIGH) {								// wanneer scherm uitgerold is ->
		if(licht() < 0x80 || temperatuur() < 0x14){	// wanneer er weinig licht is OF een temperatuur onder 20 graden ->
			inrollen();								// scherm inrollen
		}
	}
	if(isUit == LOW){								// wanneer scherm ingerold is ->
		if(licht() > 0x80 && temperatuur() > 0x14){	// wanneer er veel licht is EN een temperatuur boven 20 graden ->
			uitrollen();							// scherm uitrollen
		}
	}
}

void readData(uint8_t data){
		maxAfstand = data;			// maximale uitrol is ingesteld vanaf de centrale
}

void verstuurTemp(void) {
	transmit(0x01);					// 0x01 wordt verstuurd zodat de centrale weet dat het om temperatuur gaat
	uint8_t tmp = temperatuur();	
	transmit(tmp);					// huidige temperatuur wordt verstuurd
}

void verstuurLicht(void) {
	transmit(0x02);				// 0x01 wordt verstuurd zodat de centrale weet dat het om het licht gaat
	uint8_t tmp = licht();
	transmit(tmp);				// huidige lichtintensiteit wordt verstuurd
}

int main(void) {
	uart_init();				// usb verbinding
	cli();						// zorgen dat eventuele interrupts weg gaan
	SCH_Init_T2();				// timer voor de scheduler instellen
	init_adc();					// temp en licht sensor instellen
	init_timer();				// timer voor ultra sonor instellen
	init_ext_int();				// interrupts voor ultra sonor instellen
	init_ports();				// poorten instellen
	SCH_Add_Task(verstuurTemp,0,40000);			// iedere 40 seconden temperatuur sturen
	SCH_Add_Task(verstuurLicht, 5000, 30000);	// iedere 30 seconden lichtintensiteit sturen 
	SCH_Start();				// start interrupts
    while (1) {
		SCH_Dispatch_Tasks();			// taken uitvoeren
		uint8_t tmp = PIND;				// ingedrukte knopjes registreren
		if(tmp & (1 << INOFUIT)){		// knopje om handmatig in/uit te rollen is ingedrukt.
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
		else if(tmp & (1 << automaat)){	// wanneer automatische bedienings knopje ingedrukt is ->
			useSensor = HIGH;			// automatische bediening aan.
			PORTB &= ~(1 << BLAUW);		// led die handmatige bedingen aangeeft gaat uit.
		}
		if(useSensor == HIGH) {		// wanneer automatische bediening aan staat ->
			autoBestuur();			// zonnescherm aansturen volgens ingestelde waarde.
		}
		if(UCSR0A & (1<<RXC0)){		// data komt binnen
			readData(UDR0);			// data verwerken
		}
	}
	return 0;
}

ISR(TIMER2_COMPB_vect)
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