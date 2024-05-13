/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-01 10:43:13
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-13 10:57:59
 * @FilePath: \all_pid\src\apid_cfg.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef APID_CFG_H
#define APID_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#define PID_TYPE float // 该库使用的数据类型
#define TARGET_MAX FLT_MAX // 默认最大限幅值
#define OUT_MAX FLT_MAX    // 默认最大限幅值


// 注意，钩子函数对所有pid节点都有效，但每个节点都可自行编写属于自己的函数
#define USE_HOOK_FIRST 0 // 使用钩子函数，自行编写
#define USE_HOOK_END 0
#define USE_HOOK_PRE_CPLT 0 //fuzzy将会占用

#define APID_USING_LOG
#define APID_USING_LOG_LEVEL

#define APID_USING_RTOS_RTTHREAD
#define APID_USING_LETTERSHELL

#define APID_USING_FUZZY 1

#define APID_USING_AUTO_PID 1


// #if APID_USING_FUZZY
// #undef USE_HOOK_PRE_CPLT
// #define USE_HOOK_PRE_CPLT 1
// #endif
#ifdef __cplusplus
}
#endif
#endif