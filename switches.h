#ifndef _SWITCHES_H_
#define _SWITCHES_H_

/*
 * switches.h
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created March 2017
 * 
 */

//Button array positions
enum switches
{
	SWITCH_LEFT,
	SWITCH_RIGHT,
	SWITCH_KEYFOB,
	SWITCH_MAGSWITCH2
};
typedef enum switches SWITCHES;

void switch_setup();
void switch_tick(uint32_t tick_ms);
void switch_update_states();

bool switch_pressed(SWITCHES sw);
bool switch_not_pressed(SWITCHES sw);
bool switch_just_pressed(SWITCHES sw);
bool switch_just_released(SWITCHES sw);

#endif
