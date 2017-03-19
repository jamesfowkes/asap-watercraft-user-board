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
#include "adc.h"
#include "speed_leds.h"
#include "battery.h"
#include "battery_leds.h"
#include "switches.h"

/*
 * Defines, Typedefs, Constants
 */

enum mode
{
    MODE_OFF,
    MODE_ON,
    MODE_CHARGING
};
typedef enum mode MODE;

/*
 * Private Variables
 */

static volatile MODE s_mode = MODE_OFF;
static volatile bool s_timer_tick = false;

static bool check_and_clear(volatile bool& flag)
{
    bool val = flag;
    flag = false;
    return val;
}

static void leds_off(void)
{
    speed_leds_set_level(0);
    battery_leds_set_level(0);
    battery_leds_set_flash(false);
}

static void display_intro(void)
{
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

static void setup_ports(void)
{
    // Port A initialization
    DDRA = 0;//All pins are inputs
    PORTA=(1<<PORTA7) | (1<<PORTA6) | (1<<PORTA5) | (1<<PORTA4) | (1<<PORTA3) | (1<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);//Pullups for switches
    
    // PORT B initialisation
    DDRB=0xff;//All of pins are outputs

    // PORT C initialisation    
    DDRC=0xff;//All of pins are outputs
    
    // PORT D initialisation
    DDRD=0xff;//All of pins are outputs
}

static void setup_timers(void)
{

    // Timer/Counter 2 initialization. This provides interrupts for all timing
    // Clock: 15.625 kHz. Period: 9.984 ms with OCR2=0x9B.
    // Mode: CTC top=OCR2A. Interrupt on compare match.
    // May need higher frequency interrupts if LED dimming is needed. Might even need more than 1MHz clock speed.
    ASSR=0<<AS2;
    TCCR2=(0<<WGM20) | (0<<COM21) | (0<<COM20) | (1<<WGM21) | (1<<CS22) | (0<<CS21) | (0<<CS20);
    TCNT2=0x00;
    OCR2=0x9B;

    // TCn Interrupt initialization
    TIMSK=(1<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

    // Global enable interrupts
    sei();
}

static void display_voltage( uint8_t channel )
{
    setup_adc();
    
    uint8_t charge_state = battery_get_state(channel);

    battery_leds_set_level(charge_state);
}

static void allow_running( bool allow )
{
    if (allow)
    {
        PORTB |= POWER_CONTROL_PIN;
    }
    else
    {
        PORTB &= ~POWER_CONTROL_PIN;   
    }
}

static void handle_off_mode()
{
    leds_off();
}

static void handle_on_mode()
{
    if (switch_just_pressed(SWITCH_KEYFOB))
    {
        display_intro();
    }

    allow_running( switch_pressed(SWITCH_KEYFOB) );

    display_voltage(ADC_CHANNEL_BATTERY_VOLTAGE);
}

static void handle_charging()
{
    if (switch_just_pressed(SWITCH_KEYFOB))
    {
        display_intro();
    }

    speed_leds_set_level(1);

    allow_running( switch_pressed(SWITCH_KEYFOB) );
    display_voltage(ADC_CHANNEL_CHARGE_INPUT);
}

static MODE update_mode()
{
    bool keyfob_inserted = switch_pressed(SWITCH_KEYFOB);
    bool charging = battery_get_charge_mode() > 0;

    if (keyfob_inserted)
    {
        return charging ? MODE_CHARGING : MODE_ON;
    }
    else
    {
        return charging ? MODE_CHARGING : MODE_OFF;
    }   
}

int main(void)
{
    display_intro();
    setup_ports();
    setup_timers();
    setup_adc();
    
    leds_off();
    
    while(true)
    {
        if (check_and_clear(s_timer_tick))
        {
            switch_tick(10);

            battery_leds_tick(10);
            
            battery_tick(10);
        }

        s_mode = update_mode();

        switch(s_mode)
        {
        case MODE_OFF:
            handle_off_mode();
            break;
        case MODE_ON:
            handle_on_mode();
            break;
        case MODE_CHARGING:
            handle_charging();
            break;
        }
    }
}

// 100HZ Interrupts
ISR (TIMER2_COMP_vect){
    s_timer_tick = true;
}
