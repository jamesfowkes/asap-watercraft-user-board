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
#include <stdbool.h>

/*
 * Application Includes
 */

#include "controller.h"
#include "switches.h"
#include "util.h"

/*
 * Defines, Typedefs, Constants
 */

#define DEBOUNCE_MS 50 // Debounce time in ms

#define SWITCH_THROTTLES_PORT PORTB
#define SWITCH_THROTTLES_PINS PINB
#define SWITCH_LEFT_MASK (1<<0)
#define SWITCH_RIGHT_MASK (1<<1)

#define SWITCH_MAGSWITCH_PORT PORTA
#define SWITCH_MAGSWITCH_PINS PINA
#define SWITCH_MAGSWITCH1_MASK (1<<6)
#define SWITCH_MAGSWITCH2_MASK (1<<7)
#define SWITCH_KEYFOB_MASK (1<<7)

#define SWITCH_FLOODSENSE_PORT PORTB
#define SWITCH_FLOODSENSE_PINS PINB
#define SWITCH_FLOODSENSE_MASK (1<<3)

#define NUMBER_OF_SWITCHES (5)

struct debouncer
{
	volatile bool pressed, justpressed, justreleased;
	volatile unsigned char previousstate, currentstate;
	volatile uint16_t timer;
};
typedef struct debouncer DEBOUNCER;

struct _switch
{
	volatile uint8_t * port;
	volatile uint8_t * pins;
	uint8_t mask;
	bool invert;
};
typedef struct _switch SWITCH;

/*
 * Private Variables
 */

static const SWITCH s_switches[NUMBER_OF_SWITCHES] = {
	{&SWITCH_THROTTLES_PORT, &SWITCH_THROTTLES_PINS, SWITCH_LEFT_MASK, false},
	{&SWITCH_THROTTLES_PORT, &SWITCH_THROTTLES_PINS, SWITCH_RIGHT_MASK, false},
	{&SWITCH_MAGSWITCH_PORT, &SWITCH_MAGSWITCH_PINS, SWITCH_MAGSWITCH1_MASK, false},
	{&SWITCH_MAGSWITCH_PORT, &SWITCH_MAGSWITCH_PINS, SWITCH_MAGSWITCH2_MASK, false},
	{&SWITCH_FLOODSENSE_PORT, &SWITCH_FLOODSENSE_PINS, SWITCH_FLOODSENSE_MASK, true}
};

static DEBOUNCER s_debouncers[NUMBER_OF_SWITCHES];

/*
 * Private Functions
 */

static void update_switch(DEBOUNCER& sw, bool state, uint16_t tick_ms)
{
	sw.justpressed  = 0;
	sw.justreleased = 0;

	sw.timer = subtract_not_below_zero(sw.timer, tick_ms);

	if( sw.timer == 0 )
	{
		sw.timer = DEBOUNCE_MS; // Start the delay again

		sw.currentstate = state;   // read the button, all switches are on PINA.

		if (sw.currentstate == sw.previousstate)
		{
			if ((sw.pressed == 0) && (sw.currentstate == 0))
			{
				sw.justpressed = 1;  // just pressed
			}
			else if ((sw.pressed) && (sw.currentstate))
			{
				sw.justreleased = 1; // just released
			}

			sw.pressed = !sw.currentstate;  //Logic 1 in currentstate means NOT pressed
		}

		sw.previousstate = sw.currentstate;   // store the state
	}
}

/*
 * Public Functions
 */

void switch_setup()
{
	uint8_t sw;

	for( sw = 0; sw < NUMBER_OF_SWITCHES; sw++)
	{
		*(s_switches[sw].port) |= s_switches[sw].mask;	
	}
}

void switch_tick(uint32_t tick_ms)
{
	uint8_t sw;

	for( sw = 0; sw < NUMBER_OF_SWITCHES; sw++)
	{
		bool state = *(s_switches[sw].pins) & s_switches[sw].mask;
		update_switch(s_debouncers[sw], state ^ s_switches[sw].invert, tick_ms);
	}
}

bool switch_pressed(SWITCHES sw) { return s_debouncers[sw].pressed; }
bool switch_not_pressed(SWITCHES sw) { return !s_debouncers[sw].pressed; }
bool switch_just_pressed(SWITCHES sw) { return s_debouncers[sw].justpressed; }
bool switch_just_released(SWITCHES sw) { return s_debouncers[sw].justreleased; }
