#ifndef _SPEED_LEDS_H_
#define _SPEED_LEDS_H_

void speed_leds_setup();
void speed_leds_set_level(uint8_t level);
void speed_leds_blink(bool blink);
void speed_leds_tick(uint32_t tick_ms);

#endif
