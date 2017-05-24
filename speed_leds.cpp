/*
 * switches.cpp
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

/*
 * Application Includes
 */

#include "speed_leds.h"
#include "util.h"

/*
 * Defines, Typedefs, Constants
 */

#define SPEED_LEDS_DDR DDRC
#define SPEED_LEDS_PORT PORTC

static const int BLINK_PERIOD = 250;

static const int SPEED_LED_ROW0 = (1<<0);
static const int SPEED_LED_ROW1 = (1<<1);
static const int SPEED_LED_ROW2 = (1<<2);
static const int SPEED_LED_ROW3 = (1<<3);

static const int SPEED_LED_MASK = 0x0F;

/*
 * Private Variables
 */

static uint32_t s_blink_timer = BLINK_PERIOD;
static bool s_blink = false;
static bool s_blink_state = false;

/*
 * Public Functions
 */

void speed_leds_setup()
{
	SPEED_LEDS_DDR |= ( SPEED_LED_ROW0 | SPEED_LED_ROW1 | SPEED_LED_ROW2 | SPEED_LED_ROW3 );
}

void speed_leds_set_level(uint8_t level)
{
	s_blink = false;

	SPEED_LEDS_PORT &= ~SPEED_LED_MASK;

	switch(level)
	{
	case 0:
		break;
	case 1:
		SPEED_LEDS_PORT |= SPEED_LED_ROW0;			
		break;
	case 2:
		SPEED_LEDS_PORT |= SPEED_LED_ROW0 | SPEED_LED_ROW1;
		break;
	case 3:
		SPEED_LEDS_PORT |= SPEED_LED_ROW0 | SPEED_LED_ROW1 | SPEED_LED_ROW2;
		break;
	case 4:
	default:
		SPEED_LEDS_PORT |= SPEED_LED_ROW0 | SPEED_LED_ROW1 | SPEED_LED_ROW2 | SPEED_LED_ROW3;
		break;
		
	}
}

void speed_leds_start_blink()
{
	s_blink = true;
}

void speed_leds_tick(uint32_t tick_ms)
{
	s_blink_timer = subtract_not_below_zero(s_blink_timer, tick_ms);
	
	if (s_blink_timer == 0)
	{
		speed_leds_set_level(s_blink_state ? 4 : 0);
		s_blink_state = !s_blink_state;
		s_blink_timer = BLINK_PERIOD;
	}
}
