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
#include <stdlib.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <stdio.h>
/*
 * AVR Includes
 */

#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * Application Includes
 */

#include "controller.h"
#include "switches.h"
#include "battery.h"
#include "speed_sensor.h"
#include "util.h"
#include "serial.h"
#include "debug_led.h"
#include "adc.h"

/*
 * Defines, Typedefs, Constants
 */

static const uint32_t TIMER_RELOAD = 1000;
static const char s_debug_mode_key[] = "DBG";

/*
 * Local Variables
 */

static uint32_t s_timer = TIMER_RELOAD;

static char s_debug_mode_field[] = "Md";
static char s_debug_switch_field[] = "Sw";
static char s_debug_speed_field[] = "Spd";
static char s_debug_batt_field[] = "Bat";
static char s_debug_charge_field[] = "Chrg";

static bool s_debug_mode = false;

static uint16_t s_fake_adc_inputs[2] = {0,0};

/*
 * Private Functions
 */

static inline void disable_udre_interrupt() { UCSRB &= ~_BV(UDRIE); }
static inline void enable_udre_interrupt() { UCSRB |= _BV(UDRIE); }

static inline void disable_txc_interrupt() { UCSRB &= ~_BV(TXCIE); }
static inline void enable_txc_interrupt() { UCSRB |= _BV(TXCIE); }

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

static uint8_t build_debug_string(char * s)
{
    uint8_t i = 0;
    
    i += copy_header(s, s_debug_mode_field);
    s[i++] = controller_mode() + '0';
    if (s_debug_mode)
    {
        s[i++] = 'd';
    }

    i += add_separator(s+i, '\t');
    
    i += copy_header(s+i, s_debug_switch_field);
    s[i++] = switch_pressed(SWITCH_LEFT) ? '1' : '0';
    s[i++] = switch_pressed(SWITCH_RIGHT) ? '1' : '0';
    s[i++] = switch_pressed(SWITCH_KEYFOB) ? '1' : '0';
    s[i++] = switch_pressed(SWITCH_FLOODSENSOR) ? '1' : '0';
    i += add_separator(s+i, '\t');
    
    i += copy_header(s+i, s_debug_speed_field);
    s[i++] = (char)speed_get_latest() + '0';
    i += add_separator(s+i, '\t');
    
    i += copy_header(s+i, s_debug_batt_field);
    s[i++] = battery_get_last_state(ADC_CHANNEL_BATTERY_VOLTAGE) + '0';
    s[i++] = '(';
    if (!s_debug_mode)
    {
        i += sprintf(s+i, "%d", adc_get_last(ADC_CHANNEL_BATTERY_VOLTAGE));
    }
    else
    {
        i += sprintf(s+i, "%d", s_fake_adc_inputs[ADC_CHANNEL_BATTERY_VOLTAGE]);
    }
    s[i++] = ')';
    i += add_separator(s+i, '\t');
    
    i += copy_header(s+i, s_debug_charge_field);
    s[i++] = battery_get_last_state(ADC_CHANNEL_CHARGE_INPUT) + '0';
    s[i++] = '(';
    if (!s_debug_mode)
    {
        i += sprintf(s+i, "%d", adc_get_last(ADC_CHANNEL_CHARGE_INPUT));
    }
    else
    {
        i += sprintf(s+i, "%d", s_fake_adc_inputs[ADC_CHANNEL_CHARGE_INPUT]);
    }
    s[i++] = ')';
    i += add_separator(s+i, '\n');
    s[i] = '\0';

    return i;
}

void handle_debug_cmd(char * cmd)
{
    long temp;
    char * end = NULL;

    switch(cmd[0])
    {
    case 'B':
        temp = strtol(&cmd[1], &end, 10);
        if (end != &cmd[1] && temp < UINT16_MAX)
        {
            s_fake_adc_inputs[ADC_CHANNEL_BATTERY_VOLTAGE] = temp;
        }
        break;
    }
}

/*
 * Public Functions
 */

void debug_tick(uint32_t tick_ms)
{
    s_timer = subtract_not_below_zero(s_timer, tick_ms);
    if (s_timer == 0)
    {
        s_timer = TIMER_RELOAD;
        char * p = serial_get_tx_buffer();

        uint8_t n = build_debug_string(p);
        serial_print(p, n);
    }
}

void debug_handle_rx()
{
    char * prx = serial_get_rx_buffer();

    if (!s_debug_mode)
    {
        if ((strcmp(prx, s_debug_mode_key) == 0))
        {
            s_debug_mode = true;
        }
    }
    else
    {
        handle_debug_cmd(prx);
    }

    serial_clear_rx();
}

bool in_debug_mode() { return s_debug_mode; }

uint16_t debug_get_adc_read(ADC_CHANNEL_ENUM channel)
{
    return s_fake_adc_inputs[channel];
}
