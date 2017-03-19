/*
 * controller.cpp
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

#define ADCMAX 1023 //ADC output with input at 2.56V
#define VREF 2.47
#define DIVIDER 0.03508
#define SCALING_FACTOR ( (DIVIDER*ADCMAX)/VREF )
#define VADJ 1.01

#define VOLTAGE1 (245 * VADJ)
#define VOLTAGE2 (250 * VADJ)
#define VOLTAGE3 (258 * VADJ)
#define VOLTAGE4 (268 * VADJ)
#define VOLTAGE5 (282 * VADJ) 
#define VOLTAGE6 (295 * VADJ)
#define VOLTAGE7 (322 * VADJ)
#define VOLTAGE8 (340 * VADJ)

#define POWER_CONTROL_PIN (1 << 0);

#define ADC_CHANNEL_BATTERY_VOLTAGE 1
#define ADC_CHANNEL_CHARGE_INPUT 0
#define ADC_VREF_INT ((1<<REFS0) | (1<<REFS1))//Internal 2.56V reference

#define SPEED_STEPUP_DELAY 2	

#endif
