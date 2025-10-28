

#include "led.h"

const uint8_t LED_PINS[] = {1, 6, 21, 20, 0, 10, 5, 7};
const uint8_t LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

uint8_t ledStateBits = 0x00;

void LED_setup()
{
  for (const unsigned char i : LED_PINS)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  delay(100);
}

// ===================== LED控制 =====================
void LED_setAll(const bool state)
{
  for (const unsigned char i : LED_PINS)
  {
    digitalWrite(i, state ? HIGH : LOW);
  }
}

void LED_Update()
{
  LED_setByBits(ledStateBits);
}

/***
 *
 * 总之LED动画不能用delay给LED灯更新堵塞了，就设计了这么个函数
 * 在delay(10)的快速周期中每次调用就可以了
 *
 */

void LED_animationUpdate(uint8_t mode)
{
  switch (mode)
  {
  case 1:
    LED_animationSequence(800);
    break;
  default:
    break;
  }
}

void LED_animationSequence(uint32_t interval)
{
  // 操作led_stateBits来实现动画
  // 非堵塞
  static unsigned long lastUpdateTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= interval)
  {
    lastUpdateTime = currentTime;

    // 序列点亮
    static uint8_t currentLED = 0;
    ledStateBits = (1 << currentLED);
    currentLED = (currentLED + 1) % LED_COUNT;
  }
}

void LED_startupAnimation()
{
  // 序列点亮
  for (uint8_t i = 0; i < LED_COUNT; i++)
  {
    ledStateBits = (1 << i);
    LED_Update();
    delay(600);
  }
  // 全亮
  ledStateBits = 0xFF;
  LED_Update();
}

// 按位设置LED（保留原功能）
void LED_setByBits(uint8_t value)
{
  for (uint8_t i = 0; i < LED_COUNT; i++)
  {
    digitalWrite(LED_PINS[i], (value & (1 << i)) ? HIGH : LOW);
  }
}