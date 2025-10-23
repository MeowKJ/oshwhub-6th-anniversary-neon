#include <Arduino.h>
#include <math.h>  // for isnan()

// ===================== 硬件引脚 =====================
const uint8_t LED_PINS[] = {5, 6, 7, 10, 1, 0, 20, 21};
const uint8_t LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

#define STATUS_LED_PIN 8   // 状态指示LED
#define BUTTON_PIN     9

// ADC引脚（ESP32-C3 支持 analogReadMilliVolts）
#define ADC1_PIN 4
#define ADC2_PIN 3

// ===================== 状态变量 =====================
bool allLEDsOn = false;
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50; // 消抖延迟50ms

// ===================== ADC定时输出 =====================
unsigned long lastADCPrintTime = 0;
const unsigned long ADC_PRINT_INTERVAL = 5000; // 每500ms打印一次ADC值

// ===================== IIR 平滑（不增加采样次数） =====================
float adc1_filt_mv = NAN;
float adc2_filt_mv = NAN;
const float ADC_IIR_ALPHA = 0.25f; // 0~1，越小越平滑（0.25是个折中）

// ===================== LED控制 =====================
void setAllLEDs(bool state) {
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    digitalWrite(LED_PINS[i], state ? HIGH : LOW);
  }
}

// 按位设置LED（保留原功能）
void setLEDsByBits(uint8_t value) {
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    digitalWrite(LED_PINS[i], (value & (1 << i)) ? HIGH : LOW);
  }
}

// ===================== ADC读取与打印 =====================
void printADCValues() {
  // 单次读取（mV），analogReadMilliVolts 已做芯片校准并返回毫伏
  int adc1_mv = analogReadMilliVolts(ADC1_PIN);
  int adc2_mv = analogReadMilliVolts(ADC2_PIN);

  // IIR 指数平滑（不增加采样次数，只对时间序列做低通）
  if (isnan(adc1_filt_mv)) {
    adc1_filt_mv = adc1_mv;
    adc2_filt_mv = adc2_mv;
  } else {
    adc1_filt_mv += ADC_IIR_ALPHA * (adc1_mv - adc1_filt_mv);
    adc2_filt_mv += ADC_IIR_ALPHA * (adc2_mv - adc2_filt_mv);
  }

  // 换算为伏特
  float v1 = adc1_filt_mv / 1000.0f;
  float v2 = adc2_filt_mv / 1000.0f;

  Serial.println("=== ADC Reading ===");
  Serial.print("AD1 (GPIO");
  Serial.print(ADC1_PIN);
  Serial.print(") - ");
  Serial.print((int)adc1_filt_mv);
  Serial.print(" mV | ");
  Serial.print(v1, 3);
  Serial.print("V | x10: ");
  Serial.print(v1 * 10.0f, 2);
  Serial.println("V");

  Serial.print("AD2 (GPIO");
  Serial.print(ADC2_PIN);
  Serial.print(") - ");
  Serial.print((int)adc2_filt_mv);
  Serial.print(" mV | ");
  Serial.print(v2, 3);
  Serial.print("V | x10: ");
  Serial.print(v2 * 10.0f, 2);
  Serial.println("V");
  Serial.println();
}

// ===================== 初始化 =====================
void setup() {
  Serial.begin(115200);

  // 等待USB CDC连接（最多3秒）
  unsigned long startTime = millis();
  while (!Serial && (millis() - startTime < 3000)) {
    delay(100);
  }

  Serial.println("\n\n=== ESP32-C3 LED Control & ADC Monitor Started ===");

  // 初始化所有LED引脚
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // 配置ADC引脚
  pinMode(ADC1_PIN, INPUT);
  pinMode(ADC2_PIN, INPUT);

  // ESP32-C3 ADC配置
  analogSetAttenuation(ADC_11db);   // ~0-2500mV量程（由校准数据换算）
  analogReadResolution(12);         // 12-bit 原码宽度（对 analogRead 原码有效）

  //（可选）针对某个引脚单独设置衰减
  // analogSetPinAttenuation(ADC1_PIN, ADC_11db);
  // analogSetPinAttenuation(ADC2_PIN, ADC_11db);

  digitalWrite(STATUS_LED_PIN, LOW);

  Serial.println("Setup completed!");
  Serial.println("ADC Configuration:");
  Serial.println("  - Attenuation: 11dB (~0-2500mV)");
  Serial.println("  - Resolution: 12-bit (0-4095 原码宽度)");
  Serial.println("  - Read API: analogReadMilliVolts() 返回 mV（已校准）");
  Serial.print("LED Count: ");
  Serial.println(LED_COUNT);
  Serial.print("Button Pin: GPIO");
  Serial.println(BUTTON_PIN);
  Serial.print("ADC1 Pin: GPIO");
  Serial.println(ADC1_PIN);
  Serial.print("ADC2 Pin: GPIO");
  Serial.println(ADC2_PIN);
  Serial.println("\nNOTE: 若需更稳，可适当调小 ADC_IIR_ALPHA（更平滑）。");
  Serial.println("ADC readings will be printed every 500ms\n");
}

// ===================== 主循环 =====================
void loop() {
  // ===== 按钮处理（消抖） =====
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        allLEDsOn = !allLEDsOn;
        setAllLEDs(allLEDsOn);
        digitalWrite(STATUS_LED_PIN, allLEDsOn);

        Serial.print(">>> Button pressed! LEDs toggled: ");
        Serial.println(allLEDsOn ? "ON" : "OFF");
        Serial.println();
      }
    }
  }
  lastButtonState = reading;

  // ===== ADC读取和打印 =====
  unsigned long currentTime = millis();
  if (currentTime - lastADCPrintTime >= ADC_PRINT_INTERVAL) {
    lastADCPrintTime = currentTime;
    printADCValues();
  }

  delay(10);
}
