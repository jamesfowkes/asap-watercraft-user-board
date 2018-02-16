/*
 * power_control.cpp
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created February 2018
 * 
 */

/*
 * AVR Includes
 */

#include <avr/io.h>

/*
 * Defines, Typedefs, Constants
 */

#define POWER_CONTROL_DDR DDRB
#define POWER_CONTROL_PORT PORTB
#define POWER_CONTROL_PIN (1 << 4);

/*
 * Public Functions
 */

void power_control_setup()
{
    POWER_CONTROL_DDR |= POWER_CONTROL_PIN;
}

void power_control_allow_running( bool allow )
{
    if (allow)
    {
        POWER_CONTROL_PORT |= POWER_CONTROL_PIN;
    }
    else
    {
        POWER_CONTROL_PORT &= ~POWER_CONTROL_PIN;   
    }
}