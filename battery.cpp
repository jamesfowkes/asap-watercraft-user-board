/*
 * battery.cpp
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created March 2017
 * 
 * Modified from original file RTD-ASAP_Controller.h
 * by Rapid Technology Development ltd.
 */

/*
 * AVR Includes
 */

#include <avr/io.h>

/*
 * C standard library Includes
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * Application Includes
 */

#include "controller.h"
#include "adc.h"
#include "battery.h"
#include "switches.h"


/*
 * Defines, Typedefs, Constants
 */

enum charge_mode
{
	MODE_NOT_CHARGING,
	MODE_CHARGING,
	MODE_TESTING_CHARGE_LEVEL
};
typedef enum charge_mode CHARGE_MODE;

/*
 * Private Variables
 */

static uint8_t s_charge_state = 0;
static CHARGE_MODE s_charge_mode = MODE_NOT_CHARGING;
static uint16_t s_charge_input_voltage;
static uint32_t s_timer;

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
	CHARGE_MODE new_mode = MODE_NOT_CHARGING;

	if (s_charge_input_voltage > VOLTAGE1)
	{
		start_charging();
		new_mode = MODE_CHARGING;
	}

	return new_mode;
}

static CHARGE_MODE handle_charging()
{
	CHARGE_MODE new_mode = MODE_CHARGING;
	if (s_timer == 0)
	{
		stop_charging();
		start_settling_delay();
		new_mode = MODE_TESTING_CHARGE_LEVEL;
	}

	return new_mode;
}

static CHARGE_MODE handle_testing_charge_level()
{
	CHARGE_MODE new_mode = MODE_CHARGING;
	if (s_timer == 0)
	{
		//Measure if the charger is still connected.
		if (s_charge_input_voltage > VOLTAGE1)
		{
			start_charging();
			new_mode = MODE_CHARGING;
		}
		else
		{
			new_mode = MODE_NOT_CHARGING;
		}
	}
	return new_mode;
}

/*
 * Public Functions
 */


uint8_t battery_get_state(uint8_t channel)
{
	uint16_t voltage = read_adc( channel, ADC_VREF_INT) + (s_charge_state<<2);//This adds hysteresis
	if ( voltage < VOLTAGE1-2 ) { s_charge_state=0; }
	if ( voltage > VOLTAGE1 ) { s_charge_state=1; }
	if ( voltage > VOLTAGE2 ) { s_charge_state=2; }
	if ( voltage > VOLTAGE3 ) { s_charge_state=3; }
	if ( voltage > VOLTAGE4 ) { s_charge_state=4; }
	if ( voltage > VOLTAGE5 ) { s_charge_state=5; }
	if ( voltage > VOLTAGE6 ) { s_charge_state=6; }
	if ( voltage > VOLTAGE7 ) { s_charge_state=7; }
	if ( voltage > VOLTAGE8 ) { s_charge_state=8; }

	return s_charge_state;
}

uint8_t battery_get_charge_mode() { return s_charge_mode; }

void battery_tick(uint32_t ms_tick)
{
	if (s_timer > ms_tick) { s_timer--; }
	else if (s_timer > 0) { s_timer = 0; }
}

void battery_task()
{
	s_charge_input_voltage = read_adc(ADC_CHANNEL_CHARGE_INPUT, ADC_VREF_INT);

	switch(s_charge_mode)
	{

	case MODE_NOT_CHARGING:
		s_charge_mode = handle_not_charging();
		break;

	case MODE_CHARGING:
		s_charge_mode = handle_charging();
		break;

	case MODE_TESTING_CHARGE_LEVEL:
		s_charge_mode = handle_testing_charge_level();
		break;
	}
	
	//Terminate charge state more quickly if the keyfob is activated once.
	if (s_timer > 2)
	{
		if ( switch_just_pressed(SWITCH_KEYFOB) ) { s_timer=1; }
	}
}
