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

/*
 * Defines, constants, typedefs
 */

static const uint8_t ADC_CHANNEL_MAP[] ={
	5,	//ADC_CHANNEL_BATTERY_VOLTAGE
	4	//ADC_CHANNEL_CHARGE_INPUT
};

static const uint8_t ADMUX_VREF_SELECT ((0<<REFS1) | (1<<REFS0));

/*
 * Private Variables
 */

static uint16_t s_last_adc_values[2] = {0,0};

/*
 * Private Functions
 */

static void set_channel(uint8_t channel)
{
	channel = channel & 0x1F;
	ADMUX = ADMUX_VREF_SELECT | channel;
}

/*
 * Public Functions
 */

void adc_setup(void)
{
	ADCSRA = (1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

uint16_t adc_get_last(ADC_CHANNEL_ENUM channel)
{
	return (channel <= ADC_CHANNEL_CHARGE_INPUT) ? s_last_adc_values[channel] : 0;
}

uint16_t adc_read(ADC_CHANNEL_ENUM channel)
{
	set_channel(ADC_CHANNEL_MAP[channel]);
	// Delay needed for the stabilization of the ADC input voltage
	_delay_us(10);
	ADCSRA |= (1<<ADSC);// Start the AD conversion
	// Wait for the AD conversion to complete
	while ((ADCSRA & (1<<ADIF))==0);
	ADCSRA|=(1<<ADIF);//Clear the interrupt flag
	s_last_adc_values[channel] = ADCW;
	return s_last_adc_values[channel];
}
