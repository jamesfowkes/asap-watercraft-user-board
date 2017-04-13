/*
 * adc.cpp
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created March 2017
 * 
 * Modified from original file RTD-ASAP_Controller.c
 * by Rapid Technology Development ltd.
 */

/*
 * C standard library Includes
 */

#include <stdint.h>

/*
 * AVR Includes
 */

#include <avr/io.h>
#include <util/delay.h>

/*
 * Application Includes
 */

#include "controller.h"
#include "adc.h"

#define ADC_VREF_INT //Internal 2.56V reference

/*
 * Public Functions
 */

void adc_setup(void)
{
	ADCSRA = (1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	ADMUX |= ((1<<REFS0) | (1<<REFS1));
}

uint16_t adc_read(uint8_t adc_input)
{
	ADMUX = adc_input;
	// Delay needed for the stabilization of the ADC input voltage
	_delay_us(10);
	ADCSRA |= (1<<ADSC);// Start the AD conversion
	// Wait for the AD conversion to complete
	while ((ADCSRA & (1<<ADIF))==0);
	ADCSRA|=(1<<ADIF);//Clear the interrupt flag
	return ADCW;
}
