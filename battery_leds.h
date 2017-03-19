#ifndef _BATTERY_LEDS_H_
#define _BATTERY_LEDS_H_

void battery_leds_set_level(uint8_t level);
void battery_leds_set_flash(bool on);
void battery_leds_tick(uint32_t tick_ms);

#endif
