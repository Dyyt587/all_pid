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

### This is a powerful pid

This is a pid algorithm library that I have maintained since the beginning of my freshman year, and it is also the first work of the ALL series

Regarding the ALL series, I have this insistence: A small contribution, an excellent library, an A rating

—–Just a xxx but all in it.

## Why choose it

It's a hodgepodge of almost all the common pid's and improvements,
and you can easily build a great PID controller without much code modification,
which will be very beneficial during the development phase.

- Basic pid and common improvements
- Various advanced improvements
- Automatic pid tuning (beta) as a reference

### All functions


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

## Usage

See test/main.c
