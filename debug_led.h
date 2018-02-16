#ifndef _DEBUG_LED_H_
#define _DEBUG_LED_H_

void debug_led_setup();
void debug_led_control(bool on);
void debug_led_tick(uint32_t tick_ms);
void debug_led_toggle();

#endif
