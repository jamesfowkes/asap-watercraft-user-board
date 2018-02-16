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
#include "debug.h"

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

static bool s_charging = false;
static uint32_t s_timer = ADC_UPDATE_PERIOD;

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

static uint8_t update_state(ADC_CHANNEL_ENUM channel, uint8_t old_state)
{
	uint8_t new_state = 0;

	if (channel > ADC_CHANNEL_CHARGE_INPUT) { return 0; }

	uint16_t adc_reading = !in_debug_mode() ? adc_read(channel) : debug_get_adc_read(channel);

	// Get the levels for transitioning out of this charge state,
	uint16_t high_transition_level = get_high_level_transition(old_state);
	uint16_t low_transition_level = get_low_level_transition(old_state);

	// Move to new state if outside levels
	if ((adc_reading > high_transition_level) || (adc_reading < low_transition_level))
	{
		new_state = adc_read_to_charge_state(adc_reading);
	}
	else
	{
		new_state = old_state;
	}
	
	return new_state;
}

/*
 * Public Functions
 */

void battery_setup() { /* No setup required */ }

uint8_t battery_get_last_state(ADC_CHANNEL_ENUM channel)
{
	return channel <= ADC_CHANNEL_CHARGE_INPUT ? s_battery_states[channel] : 0;
}

bool battery_is_charging() { return s_charging; }

void battery_tick(uint32_t tick_ms)
{
	s_timer = subtract_not_below_zero(s_timer, tick_ms);

	if (s_timer == 0)
	{
		s_timer = ADC_UPDATE_PERIOD;
		s_battery_states[ADC_CHANNEL_CHARGE_INPUT] = update_state(ADC_CHANNEL_CHARGE_INPUT, s_battery_states[ADC_CHANNEL_CHARGE_INPUT]);
		s_battery_states[ADC_CHANNEL_BATTERY_VOLTAGE] = update_state(ADC_CHANNEL_BATTERY_VOLTAGE, s_battery_states[ADC_CHANNEL_BATTERY_VOLTAGE]);
	
		s_charging = s_battery_states[ADC_CHANNEL_CHARGE_INPUT] >= s_battery_states[ADC_CHANNEL_BATTERY_VOLTAGE];
		s_charging &= s_battery_states[ADC_CHANNEL_CHARGE_INPUT] > 0;
	}
}
