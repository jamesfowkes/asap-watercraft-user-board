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

#define CHARGE_RECHECK_TIME_MS 60000 //The number of milliseconds to wait between checking the charger is still connected.
#define INPUT_SETTLING_TIME_MS 150 //Number of ten millisecond interrupts to wait for charge input voltage to fall if the charger is not connected.

void battery_task();
void battery_tick(uint32_t tick_ms);

uint8_t battery_get_charge_mode();
uint8_t battery_get_state(uint8_t channel);

#endif
