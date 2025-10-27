# OSHWHub 6th Anniversary Neon Sign
**立创开源（OSHWHub）6 周年 · 桌/挂两用 LED 霓虹灯牌（60×45 cm）**

[![made-with-esp32](https://img.shields.io/badge/made%20with-ESP32--C3-blue)](#firmware)
[![power-12v](https://img.shields.io/badge/power-12V%20LED%20neon-informational)](#power-specifications)
[![usb-pd](https://img.shields.io/badge/input-USB--PD%2020V%20%7C%20DC12--26V-success)](#power-specifications)
[![license](https://img.shields.io/badge/license-GPL--3.0-lightgrey)](#license)

> A celebratory desktop neon that gloriously evolved into a **wall-mounted masterpiece**—because bigger is always better.  
> 霓虹灯牌本想做"桌面版"，但最终越做越大（60×45 cm），干脆做成"挂板霓虹灯牌"。满载约 **50 W**，**USB-PD** 供电，**ESP32-C3** 无线控制 **7 个字与外壳** 的发光状态。

---

## ✨ Highlights | 特性

- **🌟 Big & Bright**: 60×45 cm footprint, ~50W max power, illuminates half a room  
- **⚡ Flexible Power**: USB-PD 20V or DC 12–26V input with onboard multi-channel Buck converters  
- **📡 Wireless Control**: ESP32-C3 Super Mini with Bluetooth/Wi-Fi support  
- **🎛️ Independent Control**: 8 channels (7 characters + border) with high-side switching  
- **💡 Plug-and-Play LEDs**: 12V LED neon strips, constant voltage (no current regulation needed)  
- **🎨 OSHWHub Colors**: Ice blue / Warm white-pink / Yellow / Orange / Green-white

---

## 📸 Gallery | 成品展示

### Design Render | 设计图
![Neon Sign Design](https://image.lceda.cn/oshwhub/pullImage/ebeab3d752f1420fb357e1edb69743a9.png)

### Assembled Light Board | 灯板成品
![Neon Lit](https://image.lceda.cn/oshwhub/pullImage/a26465bdb91a4994bbe34f0ef45d0d5f.jpg)

### PCB with Color Silkscreen | PCB（嘉立创彩色丝印工艺）
![PCB Front](https://image.lceda.cn/oshwhub/pullImage/9eb9fa04af1e465c85296e8ed7e76ef2.jpeg)

### Board Testing | 板上点亮测试
![Board Test](https://image.lceda.cn/oshwhub/pullImage/4d973e79349548e68580c6892ade1854.jpg)

---

## 🧱 Project Structure | 仓库结构
```
.
├── hardware/          # Schematics, PCB layouts, BOM, Gerber files
│                      # 原理图、PCB、物料清单、Gerber 文件
├── firmware/          # Arduino/ESP32-C3 source code
│                      # Arduino/ESP32-C3 固件源码
├── enclosure/         # Enclosure designs
│                      # 外壳设计
└── docs/              # Design notes, debugging
```

---

## ⚙️ Technical Specifications | 技术规格

### Power Specifications | 供电规格
- **Input Voltage**: USB-PD 20V or DC 12–26V
- **Max Power**: ~50W (all channels active)
- **LED Type**: 12V constant voltage neon strips
- **Power Distribution**: Onboard multi-channel Buck regulators

### Control System | 控制系统
- **MCU**: ESP32-C3 Super Mini
- **Connectivity**: Wi-Fi 802.11 b/g/n, Bluetooth 5.0 (LE)
- **Channels**: 8 independent high-side switches
- **Control Interface**: Wireless (BLE/Wi-Fi) + physical button

---

## 📍 Pin Mapping | 引脚映射

| Function | GPIO Pin(s) | Notes |
|----------|-------------|-------|
| LED Channels (7 chars + border) | `5, 6, 7, 10, 1, 0, 20, 21` | High-side switching |
| Status LED | `8` | Onboard indicator |
| Button | `9` | Pull-up configuration |
| ADC 1 | `4` | Analog input |
| ADC 2 | `3` | Analog input |

> **Note**: The ESP32-C3 Super Mini's PCB antenna may be positioned inside the main PCB. Real-time performance is adequate for this application despite potential signal attenuation.


## 🛠️ Customization | 定制化

### Color Schemes | 配色方案
The default color scheme replicates the OSHWHub logo:
- Ice Blue (冰蓝)
- Warm White-Pink (暖白玫红)
- Yellow (黄)
- Orange (橙)
- Green-White (绿白)

You can modify colors by replacing LED strips.

### Animation Patterns | 动画模式
Edit `firmware/` to add custom lighting sequences:
- Sequential character illumination
- Breathing effects
- Random sparkle patterns

---

## 📦 Bill of Materials | 物料清单

See `hardware/BOM.csv` for complete component list including:
- ESP32-C3 Super Mini module
- Buck converter ICs
- High-side MOSFET switches
- USB-PD trigger module
- 12V LED neon strips (custom lengths)
- Connectors and passives

## 📜 License | 许可证

This project is licensed under **GPL-3.0**.

You are free to:
- ✅ Use commercially
- ✅ Modify and distribute
- ✅ Use privately

Under the conditions:
- 📋 Disclose source
- 📋 Same license (copyleft)
- 📋 State changes

See [LICENSE](LICENSE) file for full terms.

---

## 🙏 Acknowledgments | 致谢

- **OSHWHub** for inspiring the open-source hardware community
- **JLCPCB** for color silkscreen PCB manufacturing
- **ESP32 community** for excellent firmware support

---

*For questions, issues, or collaboration opportunities, please open an issue or reach out via [contact method].*