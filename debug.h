#ifndef _DEBUG_H_
#define _DEBUG_H_

void debug_print(char const * buffer, int n);
void debug_handle_rx();

bool in_debug_mode();

uint16_t debug_get_adc_read(ADC_CHANNEL_ENUM channel);

#endif
