#include "led.h"

const uint8_t LED_PINS[] = {1, 6, 21, 20, 0, 10, 5, 7};
const uint8_t LED_COUNT = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
const uint8_t FRAME_LED_COUNT = 7; // 前7个为字体LED
const uint8_t SHELL_LED_BIT = 7;   // 第8个(bit7)为外壳LED

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
 * 动画更新总入口
 * 在delay(10)的快速周期中每次调用
 */
void LED_animationUpdate(uint8_t mode)
{
  switch (mode)
  {
  case 1:
    LED_animationSequence(600); // 序列点亮
    break;
  case 2:
    LED_animationBlink(300); // 闪烁动画
    break;
  case 3:
    LED_animationRandom(400); // 随机显示(外壳常亮)
    break;
  case 4:
    LED_animationBreathing(50); // 呼吸灯效果
    break;
  case 5:
    LED_animationWave(150); // 波浪效果
    break;
  case 6:
    LED_animationChase(200); // 追逐效果
    break;
  case 7:
    LED_animation6Blink(150); // 6字爆闪
    break;
  default:
    break;
  }
}

// ===================== 动画1: 序列点亮(优化版) =====================
void LED_animationSequence(uint32_t interval)
{
  static unsigned long lastUpdateTime = 0;
  static unsigned long shellLightTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastUpdateTime >= interval)
  {
    lastUpdateTime = currentTime;

    static uint8_t currentLED = 0;

    if (currentLED < FRAME_LED_COUNT)
    {
      // 逐个点亮前7个字体LED，外壳不亮
      ledStateBits = (1 << currentLED);
    }
    else
    {
      // 最后阶段：所有字体LED + 外壳一起亮
      ledStateBits = 0xFF;
      shellLightTime = currentTime;
    }

    currentLED = (currentLED + 1) % LED_COUNT;
  }

  // 外壳保持亮2秒后开始下一轮
  if ((ledStateBits == 0xFF) && (currentTime - shellLightTime >= 2000))
  {
    ledStateBits = 0x00;
  }
}

// ===================== 动画2: 闪烁动画(外壳常亮) =====================
void LED_animationBlink(uint32_t interval)
{
  static unsigned long lastUpdateTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastUpdateTime >= interval)
  {
    lastUpdateTime = currentTime;

    static bool isOn = false;
    isOn = !isOn;

    // 前7个字体LED闪烁，外壳常亮
    if (isOn)
    {
      ledStateBits = 0xFF; // 全部亮
    }
    else
    {
      ledStateBits = 0x80; // 只有外壳亮(bit7)
    }
  }
}

// ===================== 动画3: 随机显示(外壳常亮) =====================
void LED_animationRandom(uint32_t interval)
{
  static unsigned long lastUpdateTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastUpdateTime >= interval)
  {
    lastUpdateTime = currentTime;

    // 前7个字体LED随机状态
    ledStateBits = random(0, 128); // 0-127，只影响前7位

    // 外壳LED(bit7)始终点亮
    ledStateBits |= 0x80;
  }
}

// ===================== 动画4: 呼吸灯效果(外壳常亮) =====================
void LED_animationBreathing(uint32_t interval)
{
  static unsigned long lastUpdateTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastUpdateTime >= interval)
  {
    lastUpdateTime = currentTime;

    static uint8_t phase = 0;
    phase = (phase + 1) % 28; // 调整为28步，只控制前7个LED

    // 使用sin曲线模拟呼吸效果
    uint8_t lightCount = 0;

    if (phase < 14)
    {
      // 渐亮：0 -> 7个字体灯
      lightCount = (phase / 2);
    }
    else
    {
      // 渐暗：7个字体灯 -> 0
      lightCount = 7 - ((phase - 14) / 2);
    }

    // 根据lightCount设置前7个LED状态
    ledStateBits = (1 << lightCount) - 1;
    if (lightCount == 7)
      ledStateBits = 0x7F; // 全亮前7个

    // 外壳LED始终点亮
    ledStateBits |= 0x80;
  }
}

// ===================== 动画5: 波浪效果(外壳慢闪) =====================
void LED_animationWave(uint32_t interval)
{
  static unsigned long lastUpdateTime = 0;
  static unsigned long shellToggleTime = 0;
  static bool shellOn = true;
  unsigned long currentTime = millis();

  if (currentTime - lastUpdateTime >= interval)
  {
    lastUpdateTime = currentTime;

    static uint8_t wavePos = 0;

    // 创建3个字体LED宽度的波浪，只在前7个LED中移动
    ledStateBits = 0x00;
    for (uint8_t i = 0; i < 3; i++)
    {
      uint8_t pos = (wavePos + i) % FRAME_LED_COUNT;
      ledStateBits |= (1 << pos);
    }

    wavePos = (wavePos + 1) % FRAME_LED_COUNT;
  }

  // 外壳LED每2秒闪一次
  if (currentTime - shellToggleTime >= 2000)
  {
    shellToggleTime = currentTime;
    shellOn = !shellOn;
  }

  if (shellOn)
  {
    ledStateBits |= 0x80;
  }
}

// ===================== 动画6: 追逐效果(外壳常亮) =====================
void LED_animationChase(uint32_t interval)
{
  static unsigned long lastUpdateTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastUpdateTime >= interval)
  {
    lastUpdateTime = currentTime;

    static uint8_t chasePos = 0;
    static bool reverse = false;

    // 点亮当前字体LED位置
    ledStateBits = (1 << chasePos);

    // 移动位置（只在前7个字体LED中追逐）
    if (!reverse)
    {
      chasePos++;
      if (chasePos >= FRAME_LED_COUNT)
      {
        chasePos = FRAME_LED_COUNT - 1;
        reverse = true;
      }
    }
    else
    {
      if (chasePos == 0)
      {
        reverse = false;
        chasePos = 1;
      }
      else
      {
        chasePos--;
      }
    }

    // 外壳LED始终点亮
    ledStateBits |= 0x80;
  }
}

// ===================== 动画7: 6字爆闪 =====================
// 只有第5个灯闪烁.,其余常亮
void LED_animation6Blink(uint32_t interval)
{
  static unsigned long lastUpdateTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastUpdateTime >= interval)
  {
    lastUpdateTime = currentTime;

    static bool isOn = false;
    isOn = !isOn;

    // 前7个字体LED除第5个外常亮，第5个根据isOn闪烁，外壳常亮
    ledStateBits = 0xFF; // 全部亮
    if (!isOn)
    {
      ledStateBits &= ~(1 << 4); // 熄灭第5个LED(bit4)
    }
  }
}

// ===================== 启动动画 =====================
void LED_startupAnimation()
{
  // 序列点亮前7个字体LED
  for (uint8_t i = 0; i < FRAME_LED_COUNT; i++)
  {
    ledStateBits = (1 << i);
    LED_Update();
    delay(600);
  }
  // 全亮（包括外壳）
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