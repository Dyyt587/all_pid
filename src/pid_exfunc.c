/*
 * @Author: dyyt 805207319@qq.com
 * @Date: 2023-07-19 11:36:27
 * @LastEditors: dyyt 805207319@qq.com
 * @LastEditTime: 2023-07-23 01:44:00
 * @FilePath: \undefinedc:\Users\LENOVO\Documents\programs\PID\VS_Project\ConsoleApplication1\ConsoleApplication1\pid_exfunc.c
 * @Description: pid库
 */
#include "pid.h"
#include <stdarg.h>

extern void i_handle_Increment_Normal(apid_t *pid);
extern void i_handle_Increment_Separation(apid_t *pid);
extern void i_handle_Increment_Saturation(apid_t *pid);
extern void i_handle_Increment_Speed(apid_t *pid);
extern void i_handle_Increment_Trapeziod(apid_t *pid);

extern void d_handle_Increment_Complete(apid_t *pid);
extern void d_handle_Increment_Part(apid_t *pid);
extern void d_handle_Increment_Previous(apid_t *pid);

extern void d_handle_Position_Complete(apid_t *pid);
extern void d_handle_Position_Part(apid_t *pid);
extern void d_handle_Position_Previous(apid_t *pid);

extern void i_handle_Position_Normal(apid_t *pid);
extern void i_handle_Position_Separation(apid_t *pid);
extern void i_handle_Position_Saturation(apid_t *pid);
extern void i_handle_Position_Speed(apid_t *pid);
extern void i_handle_Position_Trapeziod(apid_t *pid);

/*
TODO:   实现变速积分的用户自定义
        实现前馈方程的自定义,同时保留kf参数
        实现输出滤波集成功能，包括滑膜滤波/用户自定义滤波
        完善宏配置项，由用户决定支持哪些模式以减小编译体积
        集成调试功能，与vofa对接，打图像(支持任意参数)，添加name属性
        集成命令功能
        集成自动调参
        完善asssert功能

*/

/**
 * @description: 限制积分项最大值
 * @param {apid_t*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void PID_Sst_Integral_Limit(apid_t *pid, PID_TYPE value)
{
    if (pid->flag.integral_way != PID_INTEGRAL_SATURATION)
    {
        APID_SET_I_Function(pid, PID_INTEGRAL_SATURATION, value); // 抗饱和积分
    }
    else
    {
        pid->parameter.integral_limit = value;
    }
}
/**
 * @description: 设置pid的积分模式(默认为普通模式)
 * @param {apid_t*} pid 实例句柄
 * @param {ALL_PID_I_Function} imode 支持的积分模式
 * @return {*}
 * @note 根据具不同的功能，可能要传入一个数或函数指针做特别的初始化
 *       PID_INTEGRAL_SEPARATION:pid->parameter.bias_for_integral //数
 *       PID_INTEGRAL_SATURATION:pid->parameter.integral_limit //数
 *       PID_INTEGRAL_SPEED:pid->variable //函数
 */
void APID_SET_I_Function(apid_t *pid, ALL_PID_I_Function imode, ...)
{
    va_list ap; // 声明一个va_list变量
    pid->flag.integral_way = imode;
    switch (imode)
    {
    case PID_INTEGRAL_NORMAL:
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->i_handle = i_handle_Increment_Normal;
        }
        else
        {
            pid->i_handle = i_handle_Position_Normal;
        }
        break;
    case PID_INTEGRAL_SEPARATION:
        va_start(ap, imode); // 初始化，第二个参数为最后一个确定的形参
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->i_handle = i_handle_Increment_Separation;
        }
        else
        {
            pid->i_handle = i_handle_Position_Separation;
        }
#if (PID_TYPE == float)
        pid->parameter.bias_for_integral = va_arg(ap, double);
#elif (PID_TYPE == double)
        pid->parameter.bias_for_integral = va_arg(ap, PID_TYPE);
#else
        pid->parameter.bias_for_integral = (PID_TYPE)va_arg(ap, int);
#endif
        va_end(ap);
        break;
    case PID_INTEGRAL_SATURATION:
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->i_handle = i_handle_Increment_Saturation;
        }
        else
        {
            pid->i_handle = i_handle_Position_Saturation;
        }
        va_start(ap, imode); // 初始化，第二个参数为最后一个确定的形参
#if (PID_TYPE == float)
        pid->parameter.integral_limit = va_arg(ap, double);
#elif (PID_TYPE == double)
        pid->parameter.integral_limit = va_arg(ap, PID_TYPE);
#else
        pid->parameter.integral_limit = (PID_TYPE)va_arg(ap, int);
#endif
        va_end(ap);
        break;
    case PID_INTEGRAL_SPEED:

        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->i_handle = i_handle_Increment_Speed;
        }
        else
        {
            pid->i_handle = i_handle_Position_Speed;
        }
        va_start(ap, imode); // 初始化，第二个参数为最后一个确定的形参
        pid->variable = va_arg(ap, void *);
        va_end(ap);
        break;
    case PID_INTEGRAL_TRAPEZIOD:
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->i_handle = i_handle_Increment_Trapeziod;
        }
        else
        {
            pid->i_handle = i_handle_Position_Trapeziod;
        }

        break;
    default:
        break;
    }
}

/**
 * @description: 设置pid的微分模式(默认为普通模式)
 * @param {apid_t*} pid 实例句柄
 * @param {ALL_PID_D_Function} dmode
 * @return {*}
 * @note 更具不同的功能，可能要传入一个参数做特别的初始化float 或函数指针
 *          PID_DIFFERENTIAL_COMPLETE:	//完全微分
 *          PID_DIFFERENTIAL_PART:pid->parameter.kd_lpf
 *          PID_DIFFERENTIAL_PREVIOUS:pid->parameter.kd_pre
 */
void APID_SET_D_Function(apid_t *pid, ALL_PID_D_Function dmode, ...)
{
    va_list ap; // 声明一个va_list变量
    pid->flag.differential_way = dmode;
    switch (dmode)
    {
    case PID_DIFFERENTIAL_COMPLETE: // 完全微分
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->d_handle = d_handle_Increment_Complete;
        }
        else
        {
            pid->d_handle = d_handle_Position_Complete;
        }
        break;
    case PID_DIFFERENTIAL_PART: // 不完全微分
        va_start(ap, dmode);    // 初始化，第二个参数为最后一个确定的形参
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->d_handle = d_handle_Increment_Part;
        }
        else
        {
            pid->d_handle = d_handle_Position_Part;
        }
#if (PID_TYPE == float)
        pid->parameter.kd_.kd_lpf = va_arg(ap, double);
#elif (PID_TYPE == double)
        pid->parameter.kd_.kd_lpf = va_arg(ap, PID_TYPE);
#else
        pid->parameter.kd_.kd_lpf = (PID_TYPE)va_arg(ap, int);
#endif
        va_end(ap);
        break;
    case PID_DIFFERENTIAL_PREVIOUS: // 微分先行
        va_start(ap, dmode);        // 初始化，第二个参数为最后一个确定的形参
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->d_handle = d_handle_Increment_Previous;
        }
        else
        {
            pid->d_handle = d_handle_Position_Previous;
        }
#if (PID_TYPE == float)
        pid->parameter.kd_.kd_pre = va_arg(ap, double);
#elif (PID_TYPE == double)
        pid->parameter.kd_.kd_pre = va_arg(ap, PID_TYPE);
#else
        pid->parameter.kd_.kd_pre = (PID_TYPE)va_arg(ap, int);
#endif
        va_end(ap);
        break;
    default:
        break;
    }
}
