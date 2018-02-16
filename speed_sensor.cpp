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

#define SPEED_SENSOR_PORT PORTB
#define SPEED_SENSOR_PINS PINB
#define SPEED_SENSOR_PIN (1<<2)

static const int PWM_PERIOD_MS = 100;
static const int NUMBER_OF_SAMPLING_PERIODS = 2;
static const int SAMPLING_PERIOD = PWM_PERIOD_MS * NUMBER_OF_SAMPLING_PERIODS;

static const int NUMBER_OF_SPEED_VALUES = 4;
static const int SPEED_RANGE = 100 / NUMBER_OF_SPEED_VALUES;

static const int THRESHOLD = 5;

/*
 * Private Variables
 */

static uint32_t s_timer = SAMPLING_PERIOD;
static int s_speed = 0;
static uint16_t s_pwm_low_count = 0;
static uint16_t s_pwm_high_count = 0;

/*
 * Private Functions
 */

static bool get_speed_sensor_value()
{
	return (bool)(SPEED_SENSOR_PINS & SPEED_SENSOR_PIN);
}

static int sensor_values_to_speed(uint16_t s_pwm_low_count, uint16_t s_pwm_high_count, int max_speed)
{
	uint16_t count = s_pwm_low_count + s_pwm_high_count;
	
	int speed = 0;
	int true_count_one_period = 0;
	int top_of_this_speed_range = 0;
	
	speed = s_pwm_high_count/(count/max_speed);
	
	// If the count is within THRESHOLD of a transition,
	// avoid flickering LEDs by always setting the higher value
	true_count_one_period = s_pwm_high_count / NUMBER_OF_SAMPLING_PERIODS;
	
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
	if (new_state)
	{
		s_pwm_high_count++;
	}
	else
	{
		s_pwm_low_count++;
	}

	s_timer = subtract_not_below_zero(s_timer, tick_ms);
	
	if (s_timer == 0)
	{
		s_speed	= sensor_values_to_speed(s_pwm_low_count, s_pwm_high_count, NUMBER_OF_SPEED_VALUES);
		s_pwm_low_count = 0;
		s_pwm_high_count = 0;
		s_timer = SAMPLING_PERIOD;
	}
}
