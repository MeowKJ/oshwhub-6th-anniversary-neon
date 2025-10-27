#ifndef ADC_H
#define ADC_H
#include <Arduino.h>

void adc_setup();
void adc_getVoltages(float* adc1_voltage, float* adc2_voltage);


#endif // ADC_H