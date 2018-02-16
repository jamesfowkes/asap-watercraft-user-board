/*
 * debug_led.cpp
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created March 2017
 * 
 */

 /*
 * AVR Includes
 */

#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * Application Includes
 */

#include "controller.h"
#include "util.h"
#include "debug.h"

/*
 * Defines, Typedefs, Constants
 */

#define LED_DDR (DDRB)
#define LED_PORT (PORTB)
#define LED_PIN (7)

static const uint8_t TIMER_RELOAD_DEBUG = 50;
static const uint8_t TIMER_RELOAD_NORMAL = 250;

/*
 * Local Variables
 */

static uint8_t s_timer = TIMER_RELOAD_NORMAL;
static bool s_led_state = false;

/*
 * Private Functions
 */

static void led_set(bool on)
{
    if (on)
    {
        LED_PORT |= (1 << LED_PIN);
    }
    else
    {
        LED_PORT &= ~(1 << LED_PIN);
    }
    s_led_state = on;
}

/*
 * Public Functions
 */


void debug_led_setup()
{
    LED_DDR |= (1 << LED_PIN);
}

void debug_led_control(bool on)
{
    led_set(on);
}

void debug_led_toggle()
{
    led_set(!s_led_state);
}

void debug_led_tick(uint32_t tick_ms)
{
    s_timer = subtract_not_below_zero(s_timer, (uint8_t)tick_ms);
    if (s_timer == 0)
    {
        s_timer = in_debug_mode() ? TIMER_RELOAD_DEBUG : TIMER_RELOAD_NORMAL;
        debug_led_toggle();
    }
}
