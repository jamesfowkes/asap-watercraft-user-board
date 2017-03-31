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

void adc_setup(void);
uint16_t adc_read(uint8_t adc_input, uint8_t adc_vref);

#endif
