/*
 * switches,cpp
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

#define SWITCH_LEFT_MASK (1<<5)
#define SWITCH_RIGHT_MASK (1<<3)
#define SWITCH_KEYFOB_MASK (1<<7)
#define SWITCH_MAGSWITCH2_MASK (1<<6)

#define NUMBER_OF_SWITCHES (4)

static const uint8_t SWITCH_MASKS[NUMBER_OF_SWITCHES] = { SWITCH_LEFT_MASK, SWITCH_RIGHT_MASK, SWITCH_KEYFOB_MASK, SWITCH_MAGSWITCH2_MASK};

struct debouncer
{
	volatile bool pressed, justpressed, justreleased;
	volatile unsigned char previousstate, currentstate;
	volatile uint32_t timer;
};
typedef struct debouncer DEBOUNCER;

/*
 * Private Variables
 */

static DEBOUNCER s_switches[NUMBER_OF_SWITCHES];

/*
 * Private Functions
 */

static void update_switch(DEBOUNCER& sw, uint8_t mask, uint32_t tick_ms)
{
	sw.justpressed  = 0;
	sw.justreleased = 0;

	sw.timer = subtract_not_below_zero(sw.timer, tick_ms);

	if( sw.timer == 0 )
	{
		sw.timer = DEBOUNCE_MS; // Start the delay again

		sw.currentstate  =  PINA & mask;   // read the button, all switches are on PINA.

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

void switch_tick(uint32_t tick_ms)
{
	uint8_t sw;

	for( sw = 0; sw < NUMBER_OF_SWITCHES; sw++) // Loop through our array of Button structures 
	{
		update_switch(s_switches[sw], SWITCH_MASKS[sw], tick_ms);
	}
}

bool switch_pressed(SWITCHES sw) { return s_switches[sw].pressed; }
bool switch_not_pressed(SWITCHES sw) { return !s_switches[sw].pressed; }
bool switch_just_pressed(SWITCHES sw) { return s_switches[sw].justpressed; }
bool switch_just_released(SWITCHES sw) { return s_switches[sw].justreleased; }
