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

static const uint16_t LEVELS[] = {
	VADJ(245),
	VADJ(250),
	VADJ(258),
	VADJ(268),
	VADJ(282),
	VADJ(295),
	VADJ(322),
	VADJ(340)
};

static const uint8_t HYSTERESIS[] = {0, 2, 4, 6, 8, 10, 12, 14};

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
