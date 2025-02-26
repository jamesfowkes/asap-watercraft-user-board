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

#define SPEED_LED_PWM_DDR DDRD
#define SPEED_LED_PWM_PORT PORTD
#define SPEED_LED_PWM_PIN (4)

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
	SPEED_LED_PWM_DDR |= (1 << SPEED_LED_PWM_PIN);

	// Timer/Counter 1 initialization.
    TCCR1A = (
    	  (0 << COM1A1) | (0 << COM1A0) // Channel A not used
    	| (1 << COM1B1) | (1 << COM1B0) // Channel B in set-on-compare-match, clear at TOP mode
    	| (0 << FOC1A) | (0 << FOC1B) // Clear force output compare bits
    	| (0 << WGM11) | (1 << WGM10) // 8-bit PWM node (lower 2 bits)
    );

    TCCR1B = (
    	  (1<<WGM12) // 8-bit PWM mode (upper bit)
    	| (0 << CS12) | (1 << CS11) | (1 << CS10) // (Fcpu/64 = 125kHz clock = ~490Hz PWM frequency)
    );

    TCNT1=0x00;

	speed_leds_set_brightness(0);
	speed_leds_set_level(0);
}

void speed_leds_set_brightness(uint8_t brightness)
{
	uint16_t linearised_brightness = brightness * brightness;
	linearised_brightness = (linearised_brightness & 0xFF00) >> 8;
	OCR1B = 255-linearised_brightness;
}

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

void speed_leds_blink(bool blink)
{
	s_blink = blink;
}

void speed_leds_tick(uint32_t tick_ms)
{
	if (s_blink)
	{
		s_blink_timer = subtract_not_below_zero(s_blink_timer, tick_ms);
		
		if (s_blink_timer == 0)
		{
			speed_leds_set_level(s_blink_state ? 4 : 0);
			s_blink_state = !s_blink_state;
			s_blink_timer = BLINK_PERIOD;
		}
	}
}
