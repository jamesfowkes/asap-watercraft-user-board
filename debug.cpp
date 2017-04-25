/*
 * debug.cpp
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created March 2017
 * 
 */

/*
 * C/C++ Library Includes
 */

#include <string.h>

/*
 * AVR Includes
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
/*
 * Application Includes
 */

#include "controller.h"
#include "switches.h"
#include "battery.h"
#include "speed_sensor.h"
#include "util.h"

/*
 * Defines, Typedefs, Constants
 */

#define SERIAL_TX_DDR (DDRA)
#define SERIAL_TX_PORT (PORTA)
#define SERIAL_TX_PIN (5)

#define SERIAL_TX_BAUD (9600)
#define SERIAL_TX_BIT_DELAY (((F_CPU / SERIAL_TX_BAUD ) / 4) - 3)

static const uint32_t TIMER_RELOAD = 1000;

/*
 * Local Variables
 */

static uint32_t s_timer = TIMER_RELOAD;

static char s_debug_string[32];

static char s_debug_mode_field[] = "Md";
static char s_debug_switch_field[] = "Sw";
static char s_debug_speed_field[] = "Spd";
static char s_debug_batt_field[] = "Bat";
static char s_debug_charge_field[] = "Chrg";

/*
 * Private Functions
 */

static void debug_print(char const * p)
{
	char c;
	uint8_t i;

	if (!p) { return; }
	
	cli();

	while ((c = *p++))
	{
		SERIAL_TX_PORT &= ~(1 << SERIAL_TX_PIN); // Start bit
	
		_delay_loop_2(SERIAL_TX_BIT_DELAY);
	
		for (i = 8; i > 0; --i)
		{
			if (c & 1)
			{
				SERIAL_TX_PORT |= (1 << SERIAL_TX_PIN);
			}
			else
			{
				SERIAL_TX_PORT &= ~(1 << SERIAL_TX_PIN);
			}
			_delay_loop_2(SERIAL_TX_BIT_DELAY);
			c >>= 1;
		}

		SERIAL_TX_PORT |= (1 << SERIAL_TX_PIN); // Stop bit
		_delay_loop_2(SERIAL_TX_BIT_DELAY);

	}
	
	
	sei();
}

static int add_separator(char * s, char sep)
{
	s[0] = sep;
	return 1;
}

static int copy_header(char * s, char const * const header)
{
	strcpy(s, header);
	return strlen(header);
}

static void build_debug_string(char * s)
{
	uint8_t i = 0;
	
	i += copy_header(s, s_debug_mode_field);
	s[i++] = controller_mode() + '0';
	i += add_separator(s+i, '\t');
	
	i += copy_header(s+i, s_debug_switch_field);
	s[i++] = switch_pressed(SWITCH_LEFT) ? '1' : '0';
	s[i++] = switch_pressed(SWITCH_RIGHT) ? '1' : '0';
	s[i++] = switch_pressed(SWITCH_KEYFOB) ? '1' : '0';
	i += add_separator(s+i, '\t');
	
	i += copy_header(s+i, s_debug_speed_field);
	s[i++] = (char)speed_get_latest() + '0'; // TODO: read speed selection
	i += add_separator(s+i, '\t');
	
	i += copy_header(s+i, s_debug_batt_field);
	s[i++] = battery_get_last_state(ADC_CHANNEL_BATTERY_VOLTAGE) + '0';
	i += add_separator(s+i, '\t');
	
	i += copy_header(s+i, s_debug_charge_field);
	s[i++] = battery_get_charge_mode() + '0';
	
	i += add_separator(s+i, '\n');
	s[i] = '\0';
}

/*
 * Public Functions
 */

void debug_setup()
{
	SERIAL_TX_DDR |= (1 << SERIAL_TX_PIN);
}

void debug_tick(uint32_t tick_ms)
{
	s_timer = subtract_not_below_zero(s_timer, tick_ms);
	if (s_timer == 0)
	{
		s_timer = TIMER_RELOAD;
		build_debug_string(s_debug_string);
		debug_print(s_debug_string);
	}
}

