/*
 * adc.h
 *
 * analoge input, lichtsensor en temperatuursensor
 *
 * gebruik init_adc() om te beginnen
 * temperatuur() geeft de tempertuur in graden C op dit moment
 * licht geeft een waarde van het licht op dit moment
 *
 * Created: 4-11-2020 17:01:33
 *  Author: Jorian
 */ 

#include <avr/io.h>
#include <stdint.h>

void init_adc() {
	// ref=Vcc, left adjust the result (8 bit resolution),
	ADMUX = (1<<REFS0);
	// enable the ADC & prescale = 128
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t read_adc(uint8_t pin) {
	pin &= 0x07; // zorgt er voor dat de gekozen pin wat beter klopt
	ADMUX = (ADMUX & 0xF8)|pin;	// selecteer gekozen pin
	
	// start conversie
	ADCSRA |= (1<<ADSC);		// schrijf 1 naar ADSC
	while(ADCSRA & (1<<ADSC));	// wachten tot ADSC weer 0 is, resultaat ligt klaar
	
	return ADC;
}

uint8_t temperatuur(void) {
	float volt = read_adc(0);	// volts gegeven door temp sensor
	volt *= 5;		// 5 volt
	volt /= 1024;	// voltage wordt in 1024 stapjes gelezen
	return (uint8_t)((volt - 0.5) * 100);	// conversie naar graden C
}

uint8_t licht(void) {
	uint16_t volt = read_adc(1);
	uint8_t byte = (volt>>2) & 0xff; // logical shift
	return byte;	// ldr aangesloten op PC1
}