<h1 align="center">ALL PID</h1>

<p align="center">
    <a href="https://github.com/sponsors/thinkyhead"><img alt="Static Badge" src="https://img.shields.io/badge/apid-ALL_LIBS-yellow?style=flat"></a>
    <a href="/LICENSE"><img alt="Static Badge" src="https://img.shields.io/badge/MIT-License-blue?style=flat"></a>
    <a href="https://github.com/MarlinFirmware/Marlin/graphs/contributors"><img alt="Contributors" src="https://img.shields.io/github/contributors/marlinfirmware/marlin.svg"></a>
    <a href="https://github.com/MarlinFirmware/Marlin/releases"><img alt="Last Release Date" src="https://img.shields.io/github/release-date/MarlinFirmware/Marlin"></a>
    <a href="https://github.com/MarlinFirmware/Marlin/actions"><img alt="CI Status" src="https://github.com/MarlinFirmware/Marlin/actions/workflows/test-builds.yml/badge.svg"></a>
    <a href="https://github.com/sponsors/thinkyhead"><img alt="Static Badge" src="https://img.shields.io/badge/apid-ALL_LIBS-yellow?style=flat"></a>
    <br />
    <a href="https://fosstodon.org/@marlinfirmware"><img alt="Follow MarlinFirmware on Mastodon" src="https://img.shields.io/mastodon/follow/109450200866020466?domain=https%3A%2F%2Ffosstodon.org&logoColor=%2300B&style=social"></a>
</p>

### 这是一个强大的PID库

## 项目简介

这是从大一开始维护的PID算法库，也是ALL系列的第一个作品。

关于ALL系列，我的坚持是：一点小贡献，一个优秀的库，A级品质。

——只是一个xxx，但里面包含了所有。

## 为什么选择它

这个库几乎包含了所有常见的PID和改进算法，
你可以轻松地构建一个出色的PID控制器，无需大量修改代码，
这对开发阶段非常有利。

- 基础PID和常见改进
- 各种高级改进
- 自动PID调谐（测试版）作为参考

## 所有功能


| Function    | Description             | State |
| ----------- | ----------------------- | ----- |
| pid基础功能 | 基本的增量式，位置式pid | ✅    |
| 积分限幅    |                         | ✅    |
| 误差限幅    |                         | ✅    |
| 死区限幅    |                         | ✅    |
| 目标值限幅  |                         | ✅    |
| 输出值限幅  |                         | ✅    |
| 积分分离    |                         | ✅    |
| 梯形积分    |                         | ✅    |
| 变速积分    |                         | ✅/✍ |
| 微分先行    |                         | ✅    |
| 不完全微分  |                         | ✅    |
| 前馈PID     |                         | ✅/✍ |
| 手自动切换  |                         | ✅    |


## 使用方法

请参阅`test/main.c`。

