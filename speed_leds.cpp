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

/*
 * Defines, Typedefs, Constants
 */

#define SPEED_LEDS_PORT PORTC

static const int SPEED_LED_ROW0 = (1<<0);
static const int SPEED_LED_ROW1 = (1<<1);
static const int SPEED_LED_ROW2 = (1<<2);
static const int SPEED_LED_ROW3 = (1<<3);

static const int SPEED_LED_MASK = 0x0F;

/*
 * Public Functions
 */

void speed_leds_set_level(uint8_t level)
{
	
	SPEED_LEDS_PORT &= ~SPEED_LED_MASK;

	switch(level)
	{
	case 0:
		break;
	case 1:
		SPEED_LEDS_PORT |= SPEED_LED_ROW0;			
		break;
	case 2:
		SPEED_LEDS_PORT |= SPEED_LED_ROW0;
		break;
	case 3:
		SPEED_LEDS_PORT |= SPEED_LED_ROW0;
		break;
	case 4:
		SPEED_LEDS_PORT |= SPEED_LED_ROW0;
		break;
	}
}