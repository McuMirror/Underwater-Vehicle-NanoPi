# [Underwater-Vehicle-NanoPi](https://github.com/chenxiqiyuan/Underwater-Vehicle-NanoPi.git)

# 1. 介绍

本项目基于 [Underwater-Vehicle](https://github.com/zengwangfa/Underwater-Vehicle) 修改，使用NanoPi NEO Core进行开发。

- 本项目用到了以下软件包：
  - [WiringNP](https://github.com/chenxiqiyuan/WiringNP.git) This is a GPIO access library for NanoPi. It is based on the WiringOP for Orange PI which is based on original WiringPi for Raspberry Pi.
  - [EasyLogger](https://github.com/armink/EasyLogger) 是一款超轻量级(ROM<1.6K, RAM<0.3K)、高性能的 C/C++ 日志库，非常适合对资源敏感的软件项目。

# 2. 使用

## 2.1 克隆 `WiringNP` 并安装

```shell
$ git clone https://github.com/chenxiqiyuan/WiringNP
$ cd WiringNP/
$ chmod 755 build
$ ./build
```

## 2.2 动态库设置

先进入 `easylogger` 文件夹中，输入命令 `make` 进行编译：

```shell
$ cd easylogger
$ make
```

再将 `easylogger` 文件夹内生成的动态库拷贝至系统库下：

```shell
$ sudo cp easylogger/libeasylogger.so /usr/lib/
```

## 2.3 编译与执行

```shell
make && sudo ./rovmaster
```

若提示无法打开某设备，输入 `sudo npi-config` > `Advanced Options` 中使能相关设备：

```shell
$ sudo npi-config
```

若时区不对，也可以在 `npi-config` > `Localisation Options` > `Change Timezone` 中选择修改。
## 2.3 注意事项
=======

## 2.4 注意事项

有时修改会后编译执行会出问题，未出现修改部分效果，此时 `make clean` 后再编译运行即可。

# 3. 进度

- [ ] 模块进度
  - [x] ADC
    - [x] 电压检测
    - [x] 电流检测
  - [x] RGB LED
  - [ ] 探照灯
  - [ ] Key
  - [x] Buzzer
  - [x] 深度传感器
  - [x] 显示屏
  - [x] 九轴
  - [x] PWM
    - [x] 电压
    - [x] 频率
  - [x] CPU状况
    - [x] CPU温度
    - [x] CPU使用率
  - [x] 推进器
- [ ] 整体进度
  - [x] 传感器整合
  - [ ] 系统自检
  - [ ] NanoPi与上位机通信
  - [ ] 根据控制数据控制推进器