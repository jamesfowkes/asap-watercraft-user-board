#ifndef _SERIAL_H_
#define _SERIAL_H_

void serial_setup(bool& rx_flag, bool& tx_complete);
void serial_print(char const * buffer, int n);
char * serial_get_tx_buffer();
char * serial_get_rx_buffer();
void serial_clear_rx();

#endif
