/*
 * serial.cpp
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created February 2018
 * 
 */

/*
 * C/C++ Library Includes
 */

#include <string.h>

/*
 * AVR Includes
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "debug_led.h"

/*
 * Defines, Typedefs, Constants
 */

#define SERIAL_TX_DDR (DDRD)
#define SERIAL_TX_PORT (PORTD)
#define SERIAL_TX_PIN (1)

#define SERIAL_RX_DDR (DDRD)
#define SERIAL_RX_PORT (PORTD)
#define SERIAL_RX_PIN (0)

static const uint16_t TARGET_BAUD_RATE = 9600;

static const uint16_t UBRR = (F_CPU / (TARGET_BAUD_RATE * 8UL)) - 1;

/*
 * Local Variables
 */

static const int TX_BUFFER_SIZE = 64;
static const int RX_BUFFER_SIZE = 12;
static volatile char s_tx_buffer[TX_BUFFER_SIZE];
static volatile char s_rx_buffer[RX_BUFFER_SIZE];

static volatile uint16_t s_rx_index = 0;

static volatile uint8_t s_transmit_index = 0;
static volatile uint8_t s_transmit_length = 0;

static volatile bool * sp_rx_flag = NULL;
static volatile bool * sp_tx_complete_flag = NULL;

/*
 * Private Functions
 */

static inline void disable_udre_interrupt() { UCSRB &= ~_BV(UDRIE); }
static inline void enable_udre_interrupt() { UCSRB |= _BV(UDRIE); }

static inline void disable_txc_interrupt() { UCSRB &= ~_BV(TXCIE); }
static inline void enable_txc_interrupt() { UCSRB |= _BV(TXCIE); }


/*
 * Public Functions
 */

void serial_setup(bool& rx_flag, bool& tx_complete)
{
    SERIAL_TX_DDR |= (1 << SERIAL_TX_PIN);
    SERIAL_RX_PORT |= (1 << SERIAL_RX_PIN);

    sp_rx_flag = &rx_flag;
    sp_tx_complete_flag = &tx_complete;

    UCSRA |= _BV(U2X);
    UCSRB |= (_BV(RXEN) | _BV(TXEN) | _BV(RXCIE));
    UBRRH = (UBRR & 0x0F00) >> 8;
    UBRRL = (UBRR & 0x00FF);
}

void serial_print(char const * buffer, int n)
{
    s_transmit_index = 0;
    s_transmit_length = n;  
        
    if (buffer != s_tx_buffer)
    {
        memcpy((void *)s_tx_buffer, buffer, s_transmit_length);
    }

    cli();
    UDR = s_tx_buffer[s_transmit_index++];
    enable_udre_interrupt();
    enable_txc_interrupt();
    sei();
}

char * serial_get_tx_buffer()
{
    return (char*)s_tx_buffer;
}

char * serial_get_rx_buffer()
{
    return (char*)s_rx_buffer;
}

void serial_clear_rx()
{
    s_rx_index = 0;
}

/*
 * UART ISRs
 */

ISR(USART_RX_vect)
{
    if (s_rx_index == RX_BUFFER_SIZE) { return; }
    uint8_t c = (uint8_t)UDR;
    if (c == '\n')
    {
        *sp_rx_flag = true;
    }
    else
    {
        s_rx_buffer[s_rx_index++] = c;
    }
}

ISR(USART_UDRE_vect)
{
    if (s_transmit_index < s_transmit_length)
    {
        UDR = s_tx_buffer[s_transmit_index++];
    }
    else
    {
        disable_udre_interrupt();
    }
}

ISR(USART_TX_vect)
{
    if (s_transmit_index == s_transmit_length)
    {
        *sp_tx_complete_flag = true;
    }
}
