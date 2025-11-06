#ifndef LED_H   
#define LED_H

#include <Arduino.h>

extern uint8_t ledStateBits;


void LED_setup();
void LED_setAll(const bool state);
void LED_setByBits(uint8_t value);

void LED_Update();

//LED动画播放函数，在loop中调用，选择是哪个动画
void LED_animationUpdate(uint8_t mode);

//LED动画列表
void LED_animationSequence(uint32_t interval);
void LED_animationBlink(uint32_t interval);
void LED_animationRandom(uint32_t interval);
void LED_animationBreathing(uint32_t interval);
void LED_animationWave(uint32_t interval);
void LED_animationChase(uint32_t interval);
void LED_animation6Blink(uint32_t interval);

//堵塞动画
void LED_startupAnimation();


#endif // LED_H