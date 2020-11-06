/*
 * distance.h
 *
 * meausers distance between 2 and 70 cm
 *
 * HC-SR04
 * trigger to sensor : uno 8 (PB5) output
 * echo from sensor  : uno 3 (PD3 = INT1) input
 *
 * Created: 29-6-2016 14:44:43
 *  Author: Jorian
 */ 

volatile uint16_t gv_counter; // 16 bit counter value
volatile uint8_t gv_echo; // a flag

void initUltrasonoor(void) {
	// timers zijn ingesteld in main.c.
	// dit zorgt er voor dat wanneer de ultrasonoor sensor een interupt geeft, dat deze ook wordt opgevangen
    EICRA = (1 << ISC10);
    EIMSK = (1 << INT1);	// int1 = PD3
	//interups worden aan gezet in main.c
}

uint8_t caclCM(uint16_t counter)
{
    return counter/58;	// counter / 58 betekend de afstand in cm 
}

void sendSignal(void) {
	if(gv_echo == 0){		//check of er niet al een pulse gestuurd is
		//_delay_ms(50);	//Restart HC-SR04
		PORTB &=~ (1 << TRIGGER);	//sensor staat uit
		_delay_us(1);
		PORTB |= (1 << TRIGGER);	//een pulse van 10 microseconden
		_delay_us(10);
		PORTB &=~ (1 << TRIGGER);	//sensor wordt weer uit gezet
	}
}

uint8_t getDistance(void) {	// dit moet misschien nog aangepast worden.
	sendSignal();
	while(gv_echo == 1){
		_delay_ms(90);
		sendSignal();
	}
	return caclCM(gv_counter);
}

ISR (INT1_vect)				// interups die gegenereerd worden door de ultra sonoor sensor
{
	if(gv_echo == 1){		// is true wanneer de tweede pulse ontvangen wordt
		TCCR1B = 0;			// timer stopt
		gv_counter = TCNT1;	// waarde van timer wordt in gv_counter opgeslagen
		gv_echo = 0;		// echo = 0, er kan weer een nieuwe pulse verstuurd worden
	}
	else{					// de eerste interupt wordt gegenereerd wanneer de sensor een pulse geeft, dat is deze else
		gv_counter = 0;		// counter komt op 0 te staan.
		TCCR1B = _BV(CS10);	// timer wordt gestart
		TCNT1 = 0;			// waarde van timer wordt 0
		gv_echo = 1;		// gv_echo wordt 1, dat betekend dat de pulse is gestuurd
	}
}