/*
 * battery.cpp
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

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <stdbool.h>

/*
 * Application Includes
 */

#include "controller.h"
#include "adc.h"
#include "battery.h"
#include "switches.h"
#include "util.h"

/*
 * Defines, Typedefs, Constants
 */

#define CHARGE_RECHECK_TIME_MS 60000 //The number of milliseconds to wait between checking the charger is still connected.
#define INPUT_SETTLING_TIME_MS 150 //Number of ten millisecond interrupts to wait for charge input voltage to fall if the charger is not connected.
#define ADC_UPDATE_PERIOD 100

/*
 * Private Variables
 */

static uint8_t s_battery_states[2] = {0,0};
static CHARGE_MODE s_charge_mode = CHARGE_MODE_NOT_CHARGING;
static uint32_t s_timer = ADC_UPDATE_PERIOD;

static void start_charging()
{
	// TODO: Turn on charging?
	s_timer=CHARGE_RECHECK_TIME_MS;
}

static void stop_charging()
{
	// TODO: Turn off charging
	s_timer=CHARGE_RECHECK_TIME_MS;
}

static void start_settling_delay()
{
	s_timer = INPUT_SETTLING_TIME_MS;
}

static CHARGE_MODE handle_not_charging()
{
	CHARGE_MODE new_mode = CHARGE_MODE_NOT_CHARGING;

	if (s_battery_states[ADC_CHANNEL_CHARGE_INPUT] > LEVELS[0])
	{
		start_charging();
		new_mode = CHARGE_MODE_CHARGING;
	}

	return new_mode;
}

static CHARGE_MODE handle_charging()
{
	CHARGE_MODE new_mode = CHARGE_MODE_CHARGING;
	if (s_timer == 0)
	{
		stop_charging();
		start_settling_delay();
		new_mode = CHARGE_MODE_TESTING_CHARGE_LEVEL;
	}

	return new_mode;
}

static CHARGE_MODE handle_testing_charge_level()
{
	CHARGE_MODE new_mode = CHARGE_MODE_CHARGING;
	if (s_timer == 0)
	{
		//Measure if the charger is still connected.
		if (s_battery_states[ADC_CHANNEL_CHARGE_INPUT] > LEVELS[0])
		{
			start_charging();
			new_mode = CHARGE_MODE_CHARGING;
		}
		else
		{
			new_mode = CHARGE_MODE_NOT_CHARGING;
		}
	}
	return new_mode;
}

static uint16_t get_high_level_transition(uint8_t state)
{
	if (state == 8) { return UINT16_MAX; }

	return LEVELS[state + 1] + HYSTERESIS[state + 1];
}

static uint16_t get_low_level_transition(uint8_t state)
{
	if (state == 0) { return 0; }

	return LEVELS[state] - HYSTERESIS[state];
}

static uint8_t adc_read_to_charge_state(uint16_t adc_read)
{
	if (adc_read > LEVELS[8]) { return 8; }
	if (adc_read > LEVELS[7]) { return 7; }
	if (adc_read > LEVELS[6]) { return 6; }
	if (adc_read > LEVELS[5]) { return 5; }
	if (adc_read > LEVELS[4]) { return 4; }
	if (adc_read > LEVELS[3]) { return 3; }
	if (adc_read > LEVELS[2]) { return 2; }
	if (adc_read > LEVELS[1]) { return 1; }
	
	return 0;
}

static uint8_t update_state(uint8_t channel)
{
	if (channel > 1) { return 0; }

	uint16_t adc_reading = adc_read(channel);

	// Get the levels for transitioning out of this charge state,
	uint16_t high_transition_level = get_high_level_transition(s_battery_states[channel]);
	uint16_t low_transition_level = get_low_level_transition(s_battery_states[channel]);

	// Move to new state if outside levels
	if ((adc_reading > high_transition_level) || (adc_reading < low_transition_level))
	{
		s_battery_states[channel] = adc_read_to_charge_state(adc_reading);
	}
	
	return s_battery_states[channel];
}

/*
 * Public Functions
 */

void battery_setup() { /* No setup required */ }

uint8_t battery_get_last_state(uint8_t channel)
{
	return channel < 2 ? s_battery_states[channel] : 0;
}


uint8_t battery_get_charge_mode() { return s_charge_mode; }

void battery_tick(uint32_t tick_ms)
{
	s_timer = subtract_not_below_zero(s_timer, tick_ms);

	if (s_timer == 0)
	{
		s_timer = ADC_UPDATE_PERIOD;
		update_state(ADC_CHANNEL_CHARGE_INPUT);
		update_state(ADC_CHANNEL_BATTERY_VOLTAGE);
	}
}

void battery_task()
{
	switch(s_charge_mode)
	{

	case CHARGE_MODE_NOT_CHARGING:
		s_charge_mode = handle_not_charging();
		break;

	case CHARGE_MODE_CHARGING:
		s_charge_mode = handle_charging();
		break;

	case CHARGE_MODE_TESTING_CHARGE_LEVEL:
		s_charge_mode = handle_testing_charge_level();
		break;
	}
}
