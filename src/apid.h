/*
 * @Author: dyyt 805207319@qq.com
 * @Date: 2023-05-29 16:03:17
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-02 09:30:36
 * @FilePath: \undefinedc:\Users\LENOVO\Documents\programs\PID\VS_Project\ConsoleApplication1\ConsoleApplication1\pid.h
 * @Description: pid库
 */

#ifndef _APID_H
#define _APID_H

#ifdef __cplusplus
extern "C"
{
#endif
typedef struct _PID_T apid_t;

#include "float.h"
#include "stdint.h"
#include "apid_cfg.h"
#if APID_USING_AUTO_PID
#include "apid_auto_tune_ZNmode.h"
#endif
#if APID_USING_FUZZY
#include "apid_ex_fuzzy.h"
#endif



#ifndef ABS
#define ABS(x) ((x > 0) ? x : -x)
#endif

    typedef enum
    {
        _8 = 0U,

        _16,

        _32,
        _64,

        _f,
        _lf
    } var_type_e;
    typedef struct _var_list
    {
        const char *name;
        void *body;
        var_type_e type;
        struct _var_list *next;
    } var_list_t;

    enum
    {
        STOP = 0U,
        PID_ENABLE,
        PAUSE,

    };

    typedef enum ALL_PID_Mode
    {
        PID_POSITION = 0,
        PID_INCREMENT,

    } ALL_PID_Mode;

    typedef enum ALL_PID_I_Function
    {
        PID_INTEGRAL_NORMAL = 0, // 普通积分
        PID_INTEGRAL_SEPARATION, // 积分分离
        PID_INTEGRAL_SATURATION, // 抗饱和积分
        PID_INTEGRAL_SPEED,      // 变速积分
        PID_INTEGRAL_TRAPEZIOD   // 梯形积分
    } ALL_PID_I_Function;

    typedef enum ALL_PID_D_Function
    {
        PID_DIFFERENTIAL_COMPLETE = 0, // 完全微分
        PID_DIFFERENTIAL_PART,         // 不完全微分
        PID_DIFFERENTIAL_PREVIOUS      // 微分先行
    } ALL_PID_D_Function;

    typedef struct
    {
        uint8_t run_status;
        ALL_PID_I_Function integral_way;
        ALL_PID_D_Function differential_way;
        ALL_PID_Mode pid_mode; // 判定pid为增量或者位置
    } ALL_PID_Flag;

    typedef union
    {
        float kd_lpf;    // 不完全微分系数
        PID_TYPE kd_pre; // 微分先行系数
    } kd_u;

    typedef struct PID_Parameter
    {
        PID_TYPE kp; // 比例系数
        PID_TYPE ki; // 积分系数
        PID_TYPE kd; // 微分系数
        PID_TYPE kf; // 前馈系数

        kd_u kd_;
        PID_TYPE k_pre; // 预测系数

        PID_TYPE target_limit;      // 目标值限幅
        PID_TYPE bias_limit;        // 误差限幅
        PID_TYPE bias_dead_zone;    // 小于这个值将不进行PID操作
        PID_TYPE bias_for_integral; // 开始积分的误差	--	用于积分分离
        PID_TYPE integral_limit;    // 积分限幅				--	用于抗积分饱和
        PID_TYPE out_limit;         // 输出限幅

        PID_TYPE out; // 此节点pid输出

        PID_TYPE target;  // 期望值
        PID_TYPE present; // 当前值
        PID_TYPE predict; // 预测值

    } PID_Parameter;

    // 增量式pid
    //  n
    // 比例P:    e(k) - e(k - 1)   当前误差 - 上次误差
    //
    // 积分I : e(i)     当前误差
    //
    // 微分D : e(k) - 2e(k - 1) + e(k - 2)   当前误差 - 2 * 上次误差 + 上上次误差

    // 位置式
    //
    // e(k) : 用户设定的值（目标值） - 控制对象的当前的状态值
    //
    // 比例P : e(k)
    //
    // 积分I : ∑e(i)     误差的累加(包括e(k))
    //
    // 微分D : e(k) - e(k - 1)  这次误差 - 上次误差

    typedef struct PID_Process
    {
        PID_TYPE bias;                  // 总误差
        PID_TYPE integral_bias;         // i误差
        PID_TYPE differential_bias;     // d误差
        PID_TYPE lpf_differential_bias; // 上次d误差

        PID_TYPE feedforward; // 前馈值
        PID_TYPE predict;     // 预测值

        PID_TYPE last_target;   // 上次期望值
        PID_TYPE last_bias;     // 上次误差
        PID_TYPE lastlast_bias; // 上上次误差

    } PID_Process;

    typedef struct
    {
        ALL_PID_Mode mode;
        ALL_PID_I_Function ifunc;
        ALL_PID_D_Function dfunc;
        PID_TYPE kp;
        PID_TYPE ki;
        PID_TYPE kd;
    } PID_Config_t;

    struct _PID_T
    {
        ALL_PID_Flag flag;
        PID_Parameter parameter;
        PID_Process process;
        PID_TYPE cycle;


        void (*handle)(apid_t *pid, PID_TYPE cycle);
        void (*i_handle)(apid_t *pid);
        void (*d_handle)(apid_t *pid);
        void (*variable)(apid_t *pid); // 变速积分
#if APID_USING_FUZZY
        apid_fuzzy_ctrl_t fuzzy;
        void (*fuzzy_ctrl)(apid_t *pid); //
#endif

#if APID_USING_AUTO_PID
        void *auto_pid;
        void (*auto_pid_handler)(apid_t *pid, PID_TYPE cycle);
#endif
#if USE_HOOK_FIRST
        void (*user_hook_first)(apid_t *pid); // 钩子函数，在计算result之前，其他必要操作之后
#endif
#if USE_HOOK_FIRST
        void (*user_hook_out)(apid_t *pid); // 钩子函数，在计算result之后，限幅之前
#endif
#if USE_HOOK_PRE_CPLT
        void (*user_hook_pre_cplt)(apid_t *pid); // 钩子函数，在计算前馈和bais之后，经行pid运算前
#endif
    };

    /*用户不该使用该注释下面的函数*/
    void _PID_Hander(apid_t *pid, PID_TYPE cycle);
    void i_handle_Increment_Normal(apid_t *pid);
    void d_handle_Increment_Complete(apid_t *pid);
    void i_handle_Position_Normal(apid_t *pid);
    void d_handle_Position_Complete(apid_t *pid);
/*用户不该使用该注释上面的函数*/

/**
 * @brief 对于特殊的使用环境，可以使用该宏创建一个匿名的pid实例，但该实例不会被自动初始化，任然需要调用init函数
 *
 */
#define APID_CREATE_STATIC_ANONYMOUS(mode, kp, ki, kd) \
    &(apid_t)                                          \
    {                                                  \
        .flag = {.pid_mode = mode},                    \
        .parameter = {.kp = kp, .ki = ki, .kd = kd},   \
    }

/**
 * @brief 对于特殊的使用环境，可以使用下面宏创建一个匿名的pid实例，该实例不用调用初始化函数
 *
 */
#define APID_CREATE_STATIC_ANONYMOUS_INCREMENT(_kp, _ki, _kd) \
    &(apid_t)                                                 \
    {                                                         \
        .flag = {.pid_mode = PID_INCREMENT},                  \
        .parameter = {.kp = _kp, .ki = _ki, .kd = _kd},       \
        .handle = _PID_Hander,                                \
        .i_handle = i_handle_Increment_Normal,                \
        .d_handle = d_handle_Increment_Complete,              \
    }
#define APID_CREATE_STATIC_ANONYMOUS_POSITION(_kp, _ki, _kd) \
    &(apid_t)                                                \
    {                                                        \
        .flag = {.pid_mode = PID_POSITION},                  \
        .parameter = {.kp = _kp, .ki = _ki, .kd = _kd},      \
        .handle = _PID_Hander,                               \
        .i_handle = i_handle_Position_Normal,                \
        .d_handle = d_handle_Position_Complete,              \
    }

    void APID_STOP(apid_t *pid);
    void APID_Pause(apid_t *pid);
    void APID_Enable(apid_t *pid);
    void APID_Init(apid_t *pid, ALL_PID_Mode mode, PID_TYPE kp, PID_TYPE ki, PID_TYPE kd);
    void APID_Reset(apid_t *pid);

    void APID_SET_I_Function(apid_t *pid, ALL_PID_I_Function imode, ...);
    void APID_SET_D_Function(apid_t *pid, ALL_PID_D_Function dmode, ...);
#define APID_I_NORMAL(pid) APID_SET_I_Function(pid, PID_INTEGRAL_NORMAL)                       // 普通积分
#define APID_I_SEPARATION(pid, value) APID_SET_I_Function(pid, PID_INTEGRAL_SEPARATION, value) // 积分分离
#define APID_I_SATURATION(pid, value) APID_SET_I_Function(pid, PID_INTEGRAL_SATURATION, value) // 抗饱和积分
#define APID_I_SPEED(pid, func) APID_SET_I_Function(pid, PID_INTEGRAL_SPEED, func)             // 变速积分
#define APID_I_TRAPEZIOD(pid) APID_SET_I_Function(pid, PID_INTEGRAL_TRAPEZIOD)                 // 梯形积分

#define APID_D_COMPLETE(pid) APID_SET_D_Function(pid, PID_DIFFERENTIAL_COMPLETE)               // 完全微分
#define APID_D_PART(pid, value) APID_SET_D_Function(pid, PID_DIFFERENTIAL_PART, value)         // 不完全微分
#define APID_D_PREVIOUS(pid, value) APID_SET_D_Function(pid, PID_DIFFERENTIAL_PREVIOUS, value) // 微分先行

    void APID_Hander(apid_t *pid, PID_TYPE cycle);

    void APID_Set_Target_Limit(apid_t *pid, PID_TYPE value);
    void APID_Set_Bias_Limit(apid_t *pid, PID_TYPE value);
    void APID_Set_Bias_Dead_Zone(apid_t *pid, PID_TYPE value);
    void APID_Set_Integral_Limit(apid_t *pid, PID_TYPE value);
    void APID_Set_Out_Limit(apid_t *pid, PID_TYPE value);

    void APID_Set_Feedforward(apid_t *pid, PID_TYPE value);
    void APID_Set_KPre(apid_t *pid, PID_TYPE value);

    void APID_Set_Target(apid_t *pid, PID_TYPE value);
    void APID_Set_Present(apid_t *pid, PID_TYPE value);
    void APID_Set_Predict(apid_t *pid, PID_TYPE value);

    PID_TYPE APID_Get_Out(apid_t *pid);

    PID_TYPE APID_Get_Target_Limit(apid_t *pid);
    PID_TYPE APID_Get_Bias_Limit(apid_t *pid);
    PID_TYPE APID_Get_Bias_Dead_Zone(apid_t *pid);
    PID_TYPE APID_Get_Integral_Limit(apid_t *pid);
    PID_TYPE APID_Get_Out_Limit(apid_t *pid);

    PID_TYPE APID_Get_Feedforward(apid_t *pid);
    PID_TYPE APID_Get_KPre(apid_t *pid);

    PID_TYPE APID_Get_Target(apid_t *pid);
    PID_TYPE APID_Get_Present(apid_t *pid);
    PID_TYPE APID_Get_Predict(apid_t *pid);

#define VAR_CMD_REGISTER(var, type)     \
    do                                  \
    {                                   \
        var_register(&var, #var, type); \
    } while (0)
#define VAR_CMD_ARR_REGISTER(var, type, size)     \
    do                                            \
    {                                             \
        var_arr_register(&var, #var, type, size); \
    } while (0)
    void var_register(void *var, const char *name, var_type_e type);
    void var_arr_register(void *var, const char *name, var_type_e type, int size);
    void var_init(void);

#ifdef __cplusplus
}
#endif
#endif
