#ifndef WIFI_AP_H
#define WIFI_AP_H

#include <Arduino.h>

/**
 * @brief 初始化WiFi AP和Web服务器
 * 
 * 启动WiFi接入点模式，配置Web服务器路由
 * AP SSID: LED_Control
 * AP 密码: 12345678
 * 访问地址: 192.168.4.1
 */
void WiFiAP_setup();

/**
 * @brief WiFi AP主循环处理函数
 * 
 * 处理Web客户端请求和动画播放
 * 需要在主循环中持续调用
 */
void WiFiAP_loop();

/**
 * @brief 获取当前LED状态的位图值
 * 
 * @return uint8_t 8位值，每位代表一个LED的状态
 */
uint8_t WiFiAP_getLEDStates();

/**
 * @brief 设置当前动画模式
 * 
 * @param mode 0=关闭动画, 1=序列动画
 */
void WiFiAP_setAnimationMode(uint8_t mode);

/**
 * @brief 获取当前动画模式
 * 
 * @return uint8_t 0=关闭动画, 1=序列动画
 */
uint8_t WiFiAP_getAnimationMode();


/**
 * @brief 更新ADC电压值
 * 
 * @param voltage1 ADC1电压值
 * @param voltage2 ADC2电压值
 */
void WiFiAP_updateVoltages(float voltage1, float voltage2);

#endif // WIFI_AP_H