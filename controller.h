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
    UNIT_MODE_CHARGING
};
typedef enum unit_mode UNIT_MODE;

#define ADCMAX 1023 //ADC output with input at 2.56V
#define VREF 2.47
#define DIVIDER 0.03508
#define SCALING_FACTOR ( (DIVIDER*ADCMAX)/VREF )
#define VADJ(x) ((x*101)/100)

#define POWER_CONTROL_PIN (1 << 0);

#define ADC_CHANNEL_BATTERY_VOLTAGE 1
#define ADC_CHANNEL_CHARGE_INPUT 0

#define SPEED_STEPUP_DELAY 2	

/*
 * Public Functions
 */

UNIT_MODE controller_mode();
void debug_tick(uint32_t tick_ms);

#endif
