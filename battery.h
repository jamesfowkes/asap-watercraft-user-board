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
	CHARGE_MODE_CHARGING
};
typedef enum charge_mode CHARGE_MODE;

static const uint16_t LEVELS[] = {
	511,
	519,
	536,
	561,
	592,
	628,
	670,
	716,
	767
};

static const uint8_t HYSTERESIS[] = {0, 2, 4, 6, 8, 10, 12, 14, 16};

/*
 * Public Functions
 */

void battery_setup();
void battery_tick(uint32_t tick_ms);

bool battery_is_charging();
uint8_t battery_get_last_state(ADC_CHANNEL_ENUM channel);

#endif
