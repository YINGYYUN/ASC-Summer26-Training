# ASC智能车实验室26年暑期集训

---

## 代码目录 (更新于2026.7)

```
project/
├── code/                   # 用户核心代码
│   ├── Motor.c/h            # 电机驱动 & 编码器定义
│   ├── process.c/h         # 主流程控制
│   ├── pid.c/h             # PID 控制器
│   ├── Track_sweep.c       # 图像扫描
│   ├── TrackRecognition.h  # 赛道识别
│   ├── Debug.c/h           # 调试功能
│   ├── Param_Page.c/h      # 参数页面(以PID参数为主)
│   ├── Param_Storage.c/h   # 参数存储
│   ├── common_menu.c/h     # 通用菜单架构("驱动")
│   ├── Menu.c/h            # 菜单系统
│   └── Sim_IMU_Analysis.c/h# IMU 数据分析
├── user/                   # 用户入口
│   ├── src/
│   │   ├── main.c          # 主函数
│   │   └── isr.c           # 中断服务函数
│   └── inc/
│       └── isr.h
├── iar/                    # IAR 工程文件
└── mdk/                    # Keil MDK 工程文件
```

## 引脚分配 (更新于2026.7)

> **注意：** 以下引用逐飞推荐引脚分配，需要在头文件中二次确认，尤其是电机驱动引脚。

### 总钻风摄像头 — UART8 + TIM1

- `TXD`: E0  (UART8 RX)
- `RXD`: E1  (UART8 TX)
- `PLCK`: E7  (TIM1_ETR, DMA1_CH4 TIM1_TRIG)
- `VSYNC`: E6
- `HREF`: — (MM32 采集不需要)
- `DATA`: E8–E15

### 电机 (DRV8701 驱动) — TIM5

> 见 `Motor.h`

- `MOTOR_1_DIR`: A0  (IO)
- `MOTOR_1_PWM`: A1  (TIM5_CH2)
- `MOTOR_2_DIR`: A2  (IO)
- `MOTOR_2_PWM`: A3  (TIM5_CH4)

### AB相正交编码器 — TIM3/4 （编码器芯片侧引脚名）

> 见 `Motor.h`

- `编码器1 A相`: B4  (TIM3_ENC1)
- `编码器1 B相`: B5  (TIM3_ENC2)
- `编码器2 A相`: B6  (TIM4_ENC1)
- `编码器2 B相`: B7  (TIM4_ENC2)

### IPS200 屏幕

- `RD`: A5
- `WR`: A7
- `RS`: A6
- `RST`: D0
- `CS`: A4
- `BL`: D1
- `D0–D7`: D8–D15

### 陀螺仪 IMU963RA — SPI2

> 默认使用硬件 SPI

- `SCL` / `SPC`: B13  (SPI2_SCK)
- `SDA` / `SDI`: B15  (SPI2_MOSI)
- `SAO` / `SDO`: B14  (SPI2_MISO)
- `CS`: B12

### 串口模块 — UART6

- `RX`: C6  (UART6 TX)
- `TX`: C7  (UART6 RX)

---

## 计时器占用

- `TIM1`: 总钻风摄像头
- `TIM2`: 计时函数调用
- `TIM3`: 正交编码器
- `TIM4`: 正交编码器
- `TIM5`: 电机驱动PWM
- `TIM6`: 按键轮询调用,计时参考变量(如Time_Count1)累加

---
