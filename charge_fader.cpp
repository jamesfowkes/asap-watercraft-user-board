/*
 * charge_fader.cpp
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created February 2018
 * 
 */

/*
 * C standard library Includes
 */

#include <stdint.h>

/*
 * Application Includes
 */

#include "speed_leds.h"
#include "util.h"

/*
 * Defines, Typedefs, Constants
 */

static const uint32_t FADE_PERIOD = 20;

/*
 * Private Variables
 */

static uint32_t s_fade_timer = FADE_PERIOD;
static uint16_t s_brightness = 0;
static bool s_fade_direction_up = true;

/*
 * Public Functions
 */

void charge_fade_reset()
{
	s_brightness = 0;
	s_fade_direction_up = true;
}

void charge_fade_tick(uint32_t tick_ms)
{
	s_fade_timer = subtract_not_below_zero(s_fade_timer, tick_ms);
		
	if (s_fade_timer == 0)
	{
		s_fade_timer = FADE_PERIOD;
		s_brightness += s_fade_direction_up ? 1 : -1;
		if (s_brightness == 0) {s_fade_direction_up = true;}
		if (s_brightness == 255) {s_fade_direction_up = false;}
		speed_leds_set_brightness(s_brightness);
	}
}
