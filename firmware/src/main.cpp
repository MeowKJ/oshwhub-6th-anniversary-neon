#include <Arduino.h>

const uint8_t LED_PINS[] = {1, 6, 21, 20, 0, 10, 5, 7};
constexpr uint8_t LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

#define STATUS_LED_PIN 8 // 状态指示LED
#define BUTTON_PIN 9

// ===================== 状态变量 =====================
bool allLEDsOn = false;
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
constexpr unsigned long DEBOUNCE_DELAY = 50; // 消抖延迟50ms

// ===================== LED控制 =====================
void setAllLEDs(const bool state)
{
  for (const unsigned char i : LED_PINS)
  {
    digitalWrite(i, state ? HIGH : LOW);
  }
}

void ledAnimationSequence(bool state)
{
  if (state)
  // 每次只亮一个，最终全亮
  {
    for (uint8_t i = 0; i < LED_COUNT - 1; i++)
    {
      setAllLEDs(false);
      digitalWrite(LED_PINS[i], HIGH);
      delay(800);
    }
    setAllLEDs(false);
  }
}

// 按位设置LED（保留原功能）
void setLEDsByBits(uint8_t value)
{
  for (uint8_t i = 0; i < LED_COUNT; i++)
  {
    digitalWrite(LED_PINS[i], (value & (1 << i)) ? HIGH : LOW);
  }
}

// ===================== 初始化 =====================
void setup()
{
  Serial.begin(115200);

  // 等待USB CDC连接（最多3秒）
  const unsigned long startTime = millis();
  while (!Serial && (millis() - startTime < 3000))
  {
    delay(100);
  }

  Serial.println("\n\n=== ESP32-C3 LED Control & ADC Monitor Started ===");

  // 初始化所有LED引脚
  for (const unsigned char i : LED_PINS)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(STATUS_LED_PIN, LOW);
}

// ===================== 主循环 =====================
void loop()
{
  // ===== 按钮处理（消抖） =====
  const bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
  {
    if (reading != buttonState)
    {
      buttonState = reading;

      if (buttonState == LOW)
      {
        allLEDsOn = !allLEDsOn;
        if (allLEDsOn)
        {
          ledAnimationSequence(true);
          delay(800);
        }
        setAllLEDs(allLEDsOn);
        digitalWrite(STATUS_LED_PIN, allLEDsOn);

        Serial.print(">>> Button pressed! LEDs toggled: ");
        Serial.println(allLEDsOn ? "ON" : "OFF");
        Serial.println();
      }
    }
  }
  lastButtonState = reading;
  delay(10);
}
