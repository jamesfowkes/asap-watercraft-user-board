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

//Battery indicator LEDs
#define BATLEDS PORTD
#define LED1 (1<<PD2)
#define LED2 (1<<PD3)
#define LED3 (1<<PD6)
#define LED4 (1<<PD7)

#define BATLEDS2 PORTC
#define LED5 (1<<PC4)
#define LED6 (1<<PC5)
#define LED7 (1<<PC6)
#define LED8 (1<<PC7)

static const uint32_t LED_FLASH_PERIOD_MS = 250;

/*
 * Private Variables
 */

static bool s_flash_bottom_led = false;
static uint8_t s_flash_timer = 0;

/*
 * Public Functions
 */

void battery_leds_set_level(uint8_t level)
{
	
	switch (level)
	{
	case 0:
		BATLEDS &= ~( LED1 | LED2 | LED3 | LED4 );
		BATLEDS2 &= ~( LED5 | LED6  | LED7 | LED8 );
		break;
	case 1:
		BATLEDS |= ( LED1 );
		BATLEDS &= ~( LED2 | LED3 | LED4 );
		BATLEDS2 &= ~( LED5 | LED6 | LED7 | LED8 );
		break;
	case 2:
		BATLEDS |= ( LED1 | LED2 );
		BATLEDS &= ~( LED3 | LED4 );
		BATLEDS2 &= ~( LED5 | LED6 | LED7 | LED8 );
		break;
	case 3:
		BATLEDS |= ( LED1 | LED2 | LED3 );
		BATLEDS &= ~ LED4 ;
		BATLEDS2 &= ~( LED5 | LED6 | LED7 | LED8 );
		break;
	case 4:
		BATLEDS |= ( LED1 | LED2 | LED3 | LED4 );
		BATLEDS2 &= ~( LED5 | LED6 | LED7 | LED8 );
		break;
	case 5:
		BATLEDS |= ( LED1 | LED2 | LED3 | LED4 );
		BATLEDS2 |=  LED5; 
		BATLEDS2 &= ~( LED6 | LED7 | LED8 );
		break;
	case 6:
		BATLEDS |= ( LED1 | LED2 | LED3 | LED4 );
		BATLEDS2 |= ( LED5 | LED6);
		BATLEDS2 &= ~( LED7 | LED8 );
		break;
	case 7:
		BATLEDS |= ( LED1 | LED2 | LED3 | LED4  );
		BATLEDS2 |= (LED5 | LED6 |LED7);
		BATLEDS2 &= ~LED8;
		break;
	case 8:
		BATLEDS |= ( LED1 | LED2 | LED3 | LED4 );
		BATLEDS2 |= ( LED5 | LED6 | LED7 | LED8 );
		break;
	}
}

void battery_leds_set_flash(bool on)
{
	s_flash_bottom_led = on;
}

void battery_leds_tick(uint32_t tick_ms)
{
	s_flash_timer = subtract_not_below_zero(s_flash_timer, tick_ms);

	if ( s_flash_timer == 0 )
	{
		s_flash_timer = LED_FLASH_PERIOD_MS;
		if (s_flash_bottom_led) BATLEDS ^= LED1;
	}
}
