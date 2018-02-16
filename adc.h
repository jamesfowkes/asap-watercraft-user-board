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
uint16_t adc_read(ADC_CHANNEL_ENUM channel);
uint16_t adc_get_last(ADC_CHANNEL_ENUM channel);

#endif
