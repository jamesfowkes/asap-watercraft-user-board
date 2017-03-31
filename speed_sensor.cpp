/*
 * battery_leds.cpp
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

#include "util.h"
#include "battery_leds.h"

/*
 * Defines, Typedefs, Constants
 */

#define SPEED_SENSOR_PORT PORTA
#define SPEED_SENSOR_PINS PINA
#define SPEED_SENSOR_PIN (1<<PA4)

static const int PWM_PERIOD = 100;
static const int NUMBER_OF_SAMPLING_PERIODS = 2;
static const int SAMPLING_PERIOD = PWM_PERIOD * NUMBER_OF_SAMPLING_PERIODS;

static const int NUMBER_OF_SPEED_VALUES = 5;
static const int SPEED_RANGE = 100 / NUMBER_OF_SPEED_VALUES;

static const int THRESHOLD = 5;

/*
 * Private Variables
 */

static uint32_t s_timer = SAMPLING_PERIOD;
static int s_speed = 0;
static bool s_sensor_values[SAMPLING_PERIOD + 32];
static int s_array_index = 0;

/*
 * Private Functions
 */

static bool get_speed_sensor_value()
{
	return (bool)(SPEED_SENSOR_PINS & SPEED_SENSOR_PIN);
}

static void log_speed_sensor_value(bool * array, int& index, bool new_state)
{
	array[index++] = new_state;	
}

static int sensor_values_to_speed(bool * array, int count, int max_speed)
{
	if (count == 0) { return 0; }
	
	int speed = 0;
	int true_count = 0;
	int true_count_one_period = 0;
	int top_of_this_speed_range = 0;
	
	int i;
	for (i = 0; i < count; i++)
	{
		true_count += (int)array[i];
	}
	
	speed = true_count/(count/max_speed);
	
	// If the count is within THRESHOLD of a transition,
	// avoid flickering LEDs by always setting the higher value
	true_count_one_period = true_count / NUMBER_OF_SAMPLING_PERIODS;
	
	top_of_this_speed_range = (speed+1) * SPEED_RANGE;
	if ((top_of_this_speed_range > true_count_one_period) && ((top_of_this_speed_range - true_count_one_period) <= THRESHOLD))
	{
		speed++;
	}
	
	return speed;
}

/*
 * Public Functions
 */

int speed_get_latest()
{
	return s_speed;
}
 
void speed_sensor_tick(uint32_t tick_ms)
{
	bool new_state = get_speed_sensor_value();
	log_speed_sensor_value(s_sensor_values, s_array_index, new_state);
	
	s_timer = subtract_not_below_zero(s_timer, tick_ms);
	
	if (s_timer == 0)
	{
		s_speed	= sensor_values_to_speed(s_sensor_values, s_array_index, NUMBER_OF_SPEED_VALUES);
		s_array_index = 0;
		s_timer = SAMPLING_PERIOD;
	}
	
}
