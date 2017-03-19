/*
 * adc.h
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created March 2017
 * 
 * Modified from original file RTD-ASAP_Controller.h
 * by Rapid Technology Development ltd.
 */

#ifndef _ADC_H_
#define _ADC_H_

void setup_adc(void);
uint16_t read_adc(uint8_t adc_input, uint8_t adc_vref);

#endif
