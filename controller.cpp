/*
 * controller.cpp
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
#include <avr/interrupt.h>
#include <util/delay.h>

/*
 * C standard library Includes
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Application Includes
 */

#include "controller.h"
#include "power_control.h"
#include "charge_fader.h"
#include "adc.h"
#include "speed_sensor.h"
#include "speed_leds.h"
#include "battery.h"
#include "battery_leds.h"
#include "switches.h"
#include "debug.h"
#include "debug_led.h"
#include "util.h"
#include "serial.h"

/*
 * Defines, Typedefs, Constants
 */

static const uint8_t TIMER_PERIOD_MS = 1;
static const uint32_t FADE_PERIOD = 200;

/*
 * Private Variables
 */

static volatile UNIT_MODE s_mode = UNIT_MODE_OFF;
static volatile bool s_timer_tick = false;

static bool s_rx_flag = false;
static bool s_tx_flag = false;

static bool s_flood_flag = false;

static bool check_and_clear(volatile bool& flag)
{
    cli();
    bool val = flag;
    flag = false;
    sei();
    return val;
}

static void leds_off(void)
{
    speed_leds_set_level(0);
    battery_leds_set_level(0);
    speed_leds_blink(false);
    battery_leds_blink(false);
}

static void display_intro(void)
{

    speed_leds_set_brightness(255);

    int i;
    for (i = 0; i < 2; i++)
    {
        speed_leds_set_level(1);
        battery_leds_set_level(2);
        _delay_ms(LED_INTRO_DELAY_MS);

        speed_leds_set_level(2);
        battery_leds_set_level(4);
        _delay_ms(LED_INTRO_DELAY_MS);

        speed_leds_set_level(3);
        battery_leds_set_level(6);
        _delay_ms(LED_INTRO_DELAY_MS);
        
        speed_leds_set_level(4);
        battery_leds_set_level(8);
        _delay_ms(LED_INTRO_DELAY_MS);
        
        speed_leds_set_level(3);
        battery_leds_set_level(6);
        _delay_ms(LED_INTRO_DELAY_MS);
        
        speed_leds_set_level(2);
        battery_leds_set_level(4);
        _delay_ms(LED_INTRO_DELAY_MS);
        
        speed_leds_set_level(1);
        battery_leds_set_level(2);
        _delay_ms(LED_INTRO_DELAY_MS);
        
        speed_leds_set_level(0);
        battery_leds_set_level(0);
        
        _delay_ms(100);
    }

    speed_leds_set_level(0);
    battery_leds_set_level(0);
}

static void timer_setup(void)
{

    // Timer/Counter 2 initialization. This provides interrupts for all timing
    // Clock: 125 kHz (F_CPU/64). Period: 1ms with OCR2=0x7D (125).
    // Mode: CTC top=OCR2A. Interrupt on compare match.
    ASSR=0<<AS2;
    TCCR2=(0<<WGM20) | (0<<COM21) | (0<<COM20) | (1<<WGM21) | (1<<CS22) | (0<<CS21) | (0<<CS20);
    TCNT2=0x00;
    OCR2=0x7D;

    // TCn Interrupt initialization
    TIMSK=(1<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);
}

static void display_voltage(ADC_CHANNEL_ENUM channel)
{
    adc_setup();
    
    uint8_t charge_state = battery_get_last_state(channel);
    bool flash_battery_low_led = !battery_is_charging() && (charge_state <= 1);
    
    battery_leds_set_level(charge_state);
    
    battery_leds_blink(flash_battery_low_led);
}

static void handle_off_mode()
{
    power_control_allow_running( switch_pressed(SWITCH_KEYFOB) );
}

static void handle_on_mode()
{
    if (switch_just_pressed(SWITCH_KEYFOB))
    {
        display_intro();
    }

    power_control_allow_running( switch_pressed(SWITCH_KEYFOB) );

    display_voltage(ADC_CHANNEL_BATTERY_VOLTAGE);

    if (switch_pressed(SWITCH_LEFT) || switch_pressed(SWITCH_RIGHT))
    {
        speed_leds_set_level(speed_get_latest());
    }
    else
    {
        speed_leds_set_level(0);
    }
}

static void handle_charging()
{
    power_control_allow_running( false );
    display_voltage(ADC_CHANNEL_CHARGE_INPUT);
}

static void handle_flooded()
{
    s_flood_flag = switch_not_pressed(SWITCH_FLOODSENSOR);
    power_control_allow_running(false);
}

static void handle_mode_change(UNIT_MODE new_mode, UNIT_MODE old_mode)
{
    (void)old_mode;

    switch (new_mode) {
    case UNIT_MODE_OFF:
        speed_leds_set_level(0);
        speed_leds_blink(false);
        battery_leds_set_level(0);
        battery_leds_blink(false);
        break;
    case UNIT_MODE_ON:
        speed_leds_set_brightness(255);
        speed_leds_blink(false);
        battery_leds_blink(false);
        break;
    case UNIT_MODE_CHARGING:
        charge_fade_reset();
        speed_leds_set_level(4);
        speed_leds_blink(false);
        battery_leds_blink(false);
        break;
    case UNIT_MODE_FLOOD:
        speed_leds_set_brightness(255);
        speed_leds_blink(true);
        battery_leds_set_level(0);
        battery_leds_blink(false);
        break;
    }
}

static UNIT_MODE update_mode(UNIT_MODE old_mode)
{
    UNIT_MODE new_mode;

    if (s_flood_flag)
    {
        new_mode = UNIT_MODE_FLOOD;
    }
    else
    {
        bool keyfob_inserted = switch_pressed(SWITCH_KEYFOB);
        bool charging = battery_is_charging();

        if (charging)
        {
            new_mode = UNIT_MODE_CHARGING;
        }
        else
        {
            new_mode = keyfob_inserted ? UNIT_MODE_ON : UNIT_MODE_OFF;
        }
    }

    if (new_mode != old_mode)
    {
        handle_mode_change(new_mode, old_mode);
    }

    return new_mode;
}


/*
 * Public Functions
 */

UNIT_MODE controller_mode()
{
    return s_mode;
}

int main(void)
{  
    speed_leds_setup();
    battery_leds_setup();
    debug_led_setup();

    display_intro();

    switch_setup();
    battery_setup();
    serial_setup(s_rx_flag, s_tx_flag);

    timer_setup();
    adc_setup();
    
    power_control_setup();

    leds_off();
    
    sei();

    while(true)
    {
        if (check_and_clear(s_timer_tick))
        {
            debug_tick(TIMER_PERIOD_MS);

            debug_led_tick(TIMER_PERIOD_MS);

            switch_tick(TIMER_PERIOD_MS);

            battery_leds_tick(TIMER_PERIOD_MS);
            
            battery_tick(TIMER_PERIOD_MS);
            
            speed_sensor_tick(TIMER_PERIOD_MS);

            speed_leds_tick(TIMER_PERIOD_MS);
            
            if (s_mode == UNIT_MODE_CHARGING)
            {
                charge_fade_tick(TIMER_PERIOD_MS);
            }
        }

        s_flood_flag = switch_pressed(SWITCH_FLOODSENSOR);
        s_mode = update_mode(s_mode);

        switch(s_mode)
        {
        case UNIT_MODE_OFF:
            handle_off_mode();
            break;
        case UNIT_MODE_ON:
            handle_on_mode();
            break;
        case UNIT_MODE_CHARGING:
            handle_charging();
            break;
        case UNIT_MODE_FLOOD:
            handle_flooded();
            break;
        }

        if (check_and_clear(s_rx_flag))
        {
            debug_handle_rx();
        }
    }
}

// 1000HZ Interrupts
ISR (TIMER2_COMP_vect){
    s_timer_tick = true;
}
