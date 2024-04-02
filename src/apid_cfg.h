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



#if APID_USING_FUZZY
#undef USE_HOOK_PRE_CPLT
#define USE_HOOK_PRE_CPLT 1
#endif
#ifdef __cplusplus
}
#endif
#endif