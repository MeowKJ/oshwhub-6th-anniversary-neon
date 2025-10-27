#include "adc.h"


// ADC引脚
#define ADC1_PIN 4
#define ADC2_PIN 3

unsigned long lastADCPrintTime = 0;
const unsigned long ADC_PRINT_INTERVAL = 5000; // 每500ms打印一次ADC值



float adc1_filt_mv = NAN;
float adc2_filt_mv = NAN;
const float ADC_IIR_ALPHA = 0.25f;


void adc_setup() {
  // ESP32-C3 ADC配置
  analogSetAttenuation(ADC_11db);   // ~0-2500mV量程（由校准数据换算）
  analogReadResolution(12);         // 12-bit 原码宽度（对 analogRead 原码有效）

    pinMode(ADC1_PIN, INPUT);
    pinMode(ADC2_PIN, INPUT);
}

void adc_getVoltages(float* adc1_voltage, float* adc2_voltage) {

    int adc1_mv = analogReadMilliVolts(ADC1_PIN);
    int adc2_mv = analogReadMilliVolts(ADC2_PIN);
    
    // IIR 指数平滑
    if (isnan(adc1_filt_mv)) {
        adc1_filt_mv = adc1_mv;
        adc2_filt_mv = adc2_mv;
    } else {
        adc1_filt_mv += ADC_IIR_ALPHA * (adc1_mv - adc1_filt_mv);
        adc2_filt_mv += ADC_IIR_ALPHA * (adc2_mv - adc2_filt_mv);
    }
    
    // 换算为伏特
    *adc1_voltage = adc1_filt_mv / 1000.0f;
    *adc2_voltage = adc2_filt_mv / 1000.0f;
}


// // ===================== ADC读取与打印 =====================
// void printADCValues() {
//   // 单次读取（mV），analogReadMilliVolts 已做芯片校准并返回毫伏
//   int adc1_mv = analogReadMilliVolts(ADC1_PIN);
//   int adc2_mv = analogReadMilliVolts(ADC2_PIN);

//   // IIR 指数平滑（不增加采样次数，只对时间序列做低通）
//   if (isnan(adc1_filt_mv)) {
//     adc1_filt_mv = adc1_mv;
//     adc2_filt_mv = adc2_mv;
//   } else {
//     adc1_filt_mv += ADC_IIR_ALPHA * (adc1_mv - adc1_filt_mv);
//     adc2_filt_mv += ADC_IIR_ALPHA * (adc2_mv - adc2_filt_mv);
//   }

//   // 换算为伏特
//   float v1 = adc1_filt_mv / 1000.0f;
//   float v2 = adc2_filt_mv / 1000.0f;

//   Serial.println("=== ADC Reading ===");
//   Serial.print("AD1 (GPIO");
//   Serial.print(ADC1_PIN);
//   Serial.print(") - ");
//   Serial.print((int)adc1_filt_mv);
//   Serial.print(" mV | ");
//   Serial.print(v1, 3);
//   Serial.print("V | x10: ");
//   Serial.print(v1 * 10.0f, 2);
//   Serial.println("V");

//   Serial.print("AD2 (GPIO");
//   Serial.print(ADC2_PIN);
//   Serial.print(") - ");
//   Serial.print((int)adc2_filt_mv);
//   Serial.print(" mV | ");
//   Serial.print(v2, 3);
//   Serial.print("V | x10: ");
//   Serial.print(v2 * 10.0f, 2);
//   Serial.println("V");
//   Serial.println();
// }