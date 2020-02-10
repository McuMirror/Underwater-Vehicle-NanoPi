# Underwater-Vehicle-NanoPi

# 1. 介绍

本项目基于 [Underwater-Vehicle](https://github.com/zengwangfa/Underwater-Vehicle) 修改，使用NanoPi NEO Core进行开发。

- 本项目用到了以下软件包：
  - [WiringNP](https://github.com/chenxiqiyuan/WiringNP.git) This is a GPIO access library for NanoPi. It is based on the WiringOP for Orange PI which is based on original WiringPi for Raspberry Pi.
  - [EasyLogger](https://github.com/armink/EasyLogger) 是一款超轻量级(ROM<1.6K, RAM<0.3K)、高性能的 C/C++ 日志库，非常适合对资源敏感的软件项目。

# 2. 使用

## 2.1 动态库设置

```shell
sudo cp easylogger/libeasylogger.so /usr/lib/
```

## 2.2 编译与执行

```shell
make && sudo ./NanoPiMain
```

