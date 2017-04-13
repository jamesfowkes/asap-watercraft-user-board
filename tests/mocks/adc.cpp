/*
 * C standard library Includes
 */

#include <stdint.h>

static uint16_t s_mock_inputs[2] = {0,0};

void adc_mock_set(uint8_t adc_input, uint16_t value)
{
	if (adc_input < 2)
	{
		s_mock_inputs[adc_input] = value;
	}
}

uint16_t adc_read(uint8_t adc_input)
{
	return adc_input < 2 ? s_mock_inputs[adc_input] : 0;
}
