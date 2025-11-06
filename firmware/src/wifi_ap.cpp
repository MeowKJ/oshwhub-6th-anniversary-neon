#include "wifi_ap.h"
#include "led.h"
#include "adc.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// ===================== 配置参数 =====================
#define AP_SSID "LED_Control"
#define AP_PASSWORD "12345678"
#define ANIMATION_INTERVAL 500

// ===================== 内部变量 =====================
static WebServer server(80);
static DNSServer dnsServer;

static uint8_t animationMode = 0; // 0=关闭, 1=序列动画

// 电压缓存变量
static float cachedVoltage1 = 0.0f;
static float cachedVoltage2 = 0.0f;

// ===================== HTML网页 =====================
static const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>LED控制面板</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Arial', sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      padding: 20px;
    }
    .container {
      background: white;
      border-radius: 20px;
      padding: 30px;
      box-shadow: 0 20px 60px rgba(0,0,0,0.3);
      max-width: 600px;
      width: 100%;
    }
    h1 {
      text-align: center;
      color: #333;
      margin-bottom: 30px;
      font-size: 28px;
    }
    .voltage-section {
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      border-radius: 15px;
      padding: 20px;
      margin-bottom: 25px;
      color: white;
    }
    .voltage-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 15px;
      margin-top: 15px;
    }
    .voltage-card {
      background: rgba(255, 255, 255, 0.2);
      border-radius: 10px;
      padding: 15px;
      text-align: center;
      backdrop-filter: blur(10px);
    }
    .voltage-label {
      font-size: 14px;
      opacity: 0.9;
      margin-bottom: 5px;
    }
    .voltage-value {
      font-size: 24px;
      font-weight: bold;
      font-family: 'Courier New', monospace;
    }
    .led-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
      gap: 15px;
      margin-bottom: 30px;
    }
    .led-button {
      background: #f0f0f0;
      border: 3px solid #ddd;
      border-radius: 15px;
      padding: 20px;
      cursor: pointer;
      transition: all 0.3s;
      text-align: center;
      font-size: 18px;
      font-weight: bold;
      color: #666;
    }
    .led-button.active {
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      border-color: #667eea;
      color: white;
      box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
      transform: translateY(-2px);
    }
    .led-button:hover {
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(0,0,0,0.2);
    }
    .animation-section {
      border-top: 2px solid #eee;
      padding-top: 25px;
    }
    .section-title {
      font-size: 20px;
      color: #333;
      margin-bottom: 15px;
      font-weight: bold;
    }
    .animation-buttons {
      display: flex;
      gap: 10px;
      flex-wrap: wrap;
    }
    .animation-btn {
      flex: 1;
      min-width: 120px;
      padding: 15px 20px;
      border: none;
      border-radius: 10px;
      font-size: 16px;
      font-weight: bold;
      cursor: pointer;
      transition: all 0.3s;
      background: #f0f0f0;
      color: #666;
    }
    .animation-btn.active {
      background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
      color: white;
      box-shadow: 0 5px 15px rgba(245, 87, 108, 0.4);
    }
    .animation-btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 5px 15px rgba(0,0,0,0.2);
    }
    .status {
      text-align: center;
      margin-top: 20px;
      padding: 10px;
      border-radius: 10px;
      background: #e8f5e9;
      color: #2e7d32;
      font-weight: bold;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🎨 LED控制面板</h1>
    
    <div class="voltage-section">
      <div class="section-title" style="color: white; margin: 0;">⚡ 电压监测</div>
      <div class="voltage-grid">
        <div class="voltage-card">
          <div class="voltage-label">供电电压</div>
          <div class="voltage-value" id="voltage2">--.-V</div>
        </div>
        <div class="voltage-card">
          <div class="voltage-label">LED灯电压</div>
          <div class="voltage-value" id="voltage1">--.-V</div>
        </div>

      </div>
    </div>
    
    <div class="led-grid" id="ledGrid"></div>
    
    <div class="animation-section">
      <div class="section-title">动画模式</div>
      <div class="animation-buttons">
        <button class="animation-btn" onclick="setAnimation(0)">关闭</button>
        <button class="animation-btn" onclick="setAnimation(1)">序列</button>
        <button class="animation-btn" onclick="setAnimation(2)">闪烁</button>
        <button class="animation-btn" onclick="setAnimation(3)">随机</button>
        <button class="animation-btn" onclick="setAnimation(4)">呼吸</button>
        <button class="animation-btn" onclick="setAnimation(5)">波浪</button>
        <button class="animation-btn" onclick="setAnimation(6)">追逐</button>
        <button class="animation-btn" onclick="setAnimation(7)">6闪</button>
      </div>
    </div>
    
    <div class="status" id="status">已连接</div>
  </div>

  <script>
    const ledNames = ['立', '创', '开', '源', '6', '周', '年', '边框'];
    let ledStates = [false, false, false, false, false, false, false, false];
    let currentAnimation = 0;

    function initLEDs() {
      const grid = document.getElementById('ledGrid');
      ledNames.forEach((name, index) => {
        const button = document.createElement('div');
        button.className = 'led-button';
        button.textContent = name;
        button.onclick = () => toggleLED(index);
        button.id = `led${index}`;
        grid.appendChild(button);
      });
    }

    function toggleLED(index) {
      ledStates[index] = !ledStates[index];
      updateLEDUI(index);
      
      fetch(`/led?id=${index}&state=${ledStates[index] ? 1 : 0}`)
        .then(response => response.text())
        .then(data => showStatus('LED已更新'))
        .catch(err => showStatus('更新失败', true));
    }

    function updateLEDUI(index) {
      const button = document.getElementById(`led${index}`);
      if (ledStates[index]) {
        button.classList.add('active');
      } else {
        button.classList.remove('active');
      }
    }

    function setAnimation(mode) {
      currentAnimation = mode;
      document.querySelectorAll('.animation-btn').forEach((btn, index) => {
        if (index === mode) {
          btn.classList.add('active');
        } else {
          btn.classList.remove('active');
        }
      });

      fetch(`/animation?mode=${mode}`)
        .then(response => response.text())
        .then(data => showStatus(mode === 0 ? '动画已关闭' : '动画已启动'))
        .catch(err => showStatus('设置失败', true));
    }

    function showStatus(message, isError = false) {
      const status = document.getElementById('status');
      status.textContent = message;
      status.style.background = isError ? '#ffebee' : '#e8f5e9';
      status.style.color = isError ? '#c62828' : '#2e7d32';
    }

    function updateStatus() {
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          const bits = data.leds;
          for (let i = 0; i < 8; i++) {
            ledStates[i] = (bits & (1 << i)) !== 0;
            updateLEDUI(i);
          }
          currentAnimation = data.animation;
          document.querySelectorAll('.animation-btn').forEach((btn, index) => {
            if (index === currentAnimation) {
              btn.classList.add('active');
            } else {
              btn.classList.remove('active');
            }
          });
          
          // 更新电压显示
          if (data.voltage1 !== undefined) {
            document.getElementById('voltage1').textContent = data.voltage1.toFixed(1) + 'V';
          }
          if (data.voltage2 !== undefined) {
            document.getElementById('voltage2').textContent = data.voltage2.toFixed(1) + 'V';
          }
        })
        .catch(err => console.error('状态更新失败', err));
    }

    initLEDs();
    updateStatus();
    setInterval(updateStatus, 2000);
  </script>
</body>
</html>
)rawliteral";

// ===================== Web服务器处理函数 =====================
static void handleRoot()
{
  server.send(200, "text/html", HTML_PAGE);
}

static void handleLED()
{
  if (server.hasArg("id") && server.hasArg("state"))
  {
    int ledId = server.arg("id").toInt();
    int state = server.arg("state").toInt();

    if (ledId >= 0 && ledId < 8)
    {
      // 更新LED状态位
      if (state == 1)
      {
        ledStateBits |= (1 << ledId); // 设置对应位为1
      }
      else
      {
        ledStateBits &= ~(1 << ledId); // 清除对应位为0
      }

      Serial.printf("LED %d set to %s, bits=0x%02X\n",
                    ledId,
                    state ? "ON" : "OFF",
                    ledStateBits);

      server.send(200, "text/plain", "OK");
      return;
    }
  }
  server.send(400, "text/plain", "Invalid parameters");
}

static void handleAnimation()
{
  if (server.hasArg("mode"))
  {
    int mode = server.arg("mode").toInt();
    if (mode >= 0)
    {
      animationMode = mode;
      ledStateBits = 0x00; // 切换动画模式时关闭所有LED

      Serial.printf("Animation mode set to: %d\n", mode);
      server.send(200, "text/plain", "OK");
      return;
    }
  }
  server.send(400, "text/plain", "Invalid parameters");
}

static void handleStatus()
{
  String json = "{\"leds\":";
  json += String(ledStateBits);
  json += ",\"animation\":";
  json += String(animationMode);
  json += ",\"voltage1\":";
  json += String(cachedVoltage1, 2);
  json += ",\"voltage2\":";
  json += String(cachedVoltage2, 2);
  json += "}";

  server.send(200, "application/json", json);
}

// Captive Portal: 捕获所有请求并重定向
static void handleCaptivePortal()
{
  // 如果不是访问我们的IP，则重定向
  if (server.hostHeader() != "192.168.4.1")
  {
    server.sendHeader("Location", "http://192.168.4.1", true);
    server.send(302, "text/plain", "");
    return;
  }
  handleRoot();
}

void WiFiAP_setup()
{
  WiFi.mode(WIFI_MODE_AP);
  WiFi.setSleep(false);

  // 硬控我半天的玩意
  WiFi.setTxPower(WIFI_POWER_7dBm);

  IPAddress ip(192, 168, 4, 1), gw(192, 168, 4, 1), mask(255, 255, 255, 0);
  WiFi.softAPConfig(ip, gw, mask);

  bool ok = WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.printf("[AP] start=%d, SSID=%s, PASS=%s, IP=%s\n",
                ok, AP_SSID, AP_PASSWORD, WiFi.softAPIP().toString().c_str());

  // 启动DNS服务器 - 将所有域名解析到192.168.4.1
  dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
  Serial.println("DNS server started");

  // 注册路由
  server.on("/led", handleLED);
  server.on("/animation", handleAnimation);
  server.on("/status", handleStatus);

  // Captive Portal - 捕获所有其他请求
  server.onNotFound(handleCaptivePortal);

  server.begin();
  Serial.println("HTTP server started with Captive Portal");
}

void WiFiAP_loop()
{
  // 处理DNS请求（Captive Portal关键）
  dnsServer.processNextRequest();

  // 处理Web客户端请求
  server.handleClient();

  // 检查是否需要播放动画
  if (animationMode >= 1)
  {
    LED_animationUpdate(animationMode);
  }
  LED_Update();
}

void WiFiAP_updateVoltages(float voltage1, float voltage2)
{
  cachedVoltage1 = voltage1;
  cachedVoltage2 = voltage2;
}

uint8_t WiFiAP_getLEDStates()
{
  return ledStateBits;
}

uint8_t WiFiAP_getAnimationMode()
{
  return animationMode;
}