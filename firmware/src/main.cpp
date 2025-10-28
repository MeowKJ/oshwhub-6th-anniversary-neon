#include <Arduino.h>

#include "led.h"
#include "adc.h"
#include "wifi_ap.h"

#define STATUS_LED_PIN 8 // 状态指示LED

// 8, 9其实连接到CH224Q IIC接口，懒得去读CH224Q了
// 不知道为什么开开关会导致CH224Q烧毁

void setup()
{
  LED_setup();

  Serial.begin(115200);

  // 等待USB CDC连接（最多3秒）
  const unsigned long startTime = millis();
  while (!Serial && (millis() - startTime < 3000))
  {
    delay(100);
  }

  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(9, INPUT);

  // 检测ADC
  ADC_setup();
  float adc1_voltage = 0.0f;
  float adc2_voltage = 0.0f;
  ADC_getVoltages(&adc1_voltage, &adc2_voltage);

  printf("=== System Initialized ===\n");
  printf("ADC1 Voltage: %.3f V\n", adc1_voltage);
  printf("ADC2 Voltage: %.3f V\n", adc2_voltage);
  printf("==========================\n\n");

  // 快速闪烁指示错误
  while (adc1_voltage < 11.0f || adc2_voltage < 12.0f)
  {
    ADC_getVoltages(&adc1_voltage, &adc2_voltage);
    printf("Error: ADC1 Voltage: %.3f V, ADC2 Voltage: %.3f V\n", adc1_voltage, adc2_voltage);
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(200);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
  digitalWrite(STATUS_LED_PIN, LOW);

  // 正常启动动画
  delay(1000);
  Serial.println("=== Playing Startup Animation ===");
  LED_startupAnimation();
  delay(1000);

  Serial.println("\n=== Starting WiFi AP ===");
  WiFiAP_setup();
  Serial.println("========================\n");
}

// ===================== 主循环 =====================
void loop()
{
  // 处理WiFi和Web请求
  WiFiAP_loop();

  // 定期输出ADC电压
  static unsigned long lastAdcCheck = 0;
  if (millis() - lastAdcCheck >= 5000)
  {
    float adc1_voltage = 0.0f;
    float adc2_voltage = 0.0f;
    ADC_getVoltages(&adc1_voltage, &adc2_voltage);
    printf("ADC1 Voltage: %.3f V, ADC2 Voltage: %.3f V\n", adc1_voltage, adc2_voltage);
    printf("LED Status Bits: 0x%02X\n", ledStateBits);
    lastAdcCheck = millis();
  }

  delay(10);
}