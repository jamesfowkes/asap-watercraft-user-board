/*
 * controller.h
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created March 2017
 * 
 * Modified from original file RTD-ASAP_Controller.h
 * by Rapid Technology Development ltd.
 */

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

// LED Timing
#define LED_INTRO_DELAY_MS 60

/*****************************************
*                                        *
* Do not change anything below this line *
*                                        *
*****************************************/

/*
 * Defines, Typedefs, Constants
 */

enum unit_mode
{
    UNIT_MODE_OFF,
    UNIT_MODE_ON,
    UNIT_MODE_CHARGING,
    UNIT_MODE_FLOOD
};
typedef enum unit_mode UNIT_MODE;

enum adc_channel
{
	ADC_CHANNEL_BATTERY_VOLTAGE,
	ADC_CHANNEL_CHARGE_INPUT
};
typedef enum adc_channel ADC_CHANNEL_ENUM;

/*
 * Public Functions
 */

UNIT_MODE controller_mode();
void debug_tick(uint32_t tick_ms);

#endif
