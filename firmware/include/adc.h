#ifndef ADC_H
#define ADC_H
#include <Arduino.h>

void ADC_setup();
void ADC_getVoltages(float* adc1_voltage, float* adc2_voltage);


#endif // ADC_H