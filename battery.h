/*
 * battery.h
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created March 2017
 * 
 * Modified from original file RTD-ASAP_Controller.h
 * by Rapid Technology Development ltd.
 */

#ifndef _BATTERY_H_
#define _BATTERY_H_

/*
 * Defines, Typedefs, Constants
 */

enum charge_mode
{
	CHARGE_MODE_NOT_CHARGING,
	CHARGE_MODE_CHARGING,
	CHARGE_MODE_TESTING_CHARGE_LEVEL
};
typedef enum charge_mode CHARGE_MODE;

/*
 * Public Functions
 */

void battery_setup();
void battery_task();
void battery_tick(uint32_t tick_ms);

uint8_t battery_get_charge_mode();
uint8_t battery_get_last_state();
uint8_t battery_update_state(uint8_t channel);

#endif
