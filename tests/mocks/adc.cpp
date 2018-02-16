/*
 * C standard library Includes
 */

#include <stdint.h>

#include "controller.h"

static uint16_t s_mock_inputs[2] = {0,0};

void adc_mock_set(ADC_CHANNEL_ENUM adc_input, uint16_t value)
{
	if (adc_input < 2)
	{
		s_mock_inputs[adc_input] = value;
	}
}

uint16_t debug_get_adc_read(ADC_CHANNEL_ENUM adc_input)
{
	return adc_input < 2 ? s_mock_inputs[adc_input] : 0;	
}

uint16_t adc_read(ADC_CHANNEL_ENUM adc_input)
{
	return adc_input < 2 ? s_mock_inputs[adc_input] : 0;
}
