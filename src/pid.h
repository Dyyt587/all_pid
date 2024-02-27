/*
 * @Author: dyyt 805207319@qq.com
 * @Date: 2023-05-29 16:03:17
 * @LastEditors: dyyt 805207319@qq.com
 * @LastEditTime: 2023-07-23 01:44:41
 * @FilePath: \undefinedc:\Users\LENOVO\Documents\programs\PID\VS_Project\ConsoleApplication1\ConsoleApplication1\pid.h
 * @Description: pid库
 */

#ifndef _PID_H
#define _PID_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "float.h"

#ifndef u8
#define u8 unsigned char
#endif

#define TARGET_MAX FLT_MAX //默认最大限幅值
#define OUT_MAX    FLT_MAX //默认最大限幅值
#define PID_TYPE float //该库使用的数据类型

    //注意，钩子函数对所有pid节点都有效，但每个节点都可自行编写属于自己的函数
#define USE_HOOK_FIRST 0//使用钩子函数，自行编写
#define USE_HOOK_END 0

#define ABS(x) ((x > 0) ? x : -x)

    enum {
        STOP = 0U,
		PID_ENABLE,
        PAUSE,

    };

    typedef enum PID_Mode
    {
        PID_POSITION_NULL = 0,
        //PID_POSITION_PARALLERL,
        //PID_POSITION_CASCADE,
        PID_INCREMENT_NULL,
        //PID_INCREMENT_PARALLERL,
        //PID_INCREMENT_CASCADE
    }PID_Mode;

    typedef enum PID_I_Function
    {
        PID_INTEGRAL_NORMAL = 0,		//普通积分
        PID_INTEGRAL_SEPARATION,	//积分分离
        PID_INTEGRAL_SATURATION,	//抗饱和积分
        PID_INTEGRAL_SPEED,			//变速积分
        PID_INTEGRAL_TRAPEZIOD		//梯形积分
    }PID_I_Function;

    typedef enum PID_D_Function
    {
        PID_DIFFERENTIAL_COMPLETE = 0,			//完全微分
        PID_DIFFERENTIAL_PART,					//不完全微分
        PID_DIFFERENTIAL_PREVIOUS 			//微分先行
    }PID_D_Function;

    typedef struct
    {
        u8 run_status;
        //u8 use_predict;
        PID_I_Function integral_way;
        PID_D_Function differential_way;
        PID_Mode pid_mode;//判定pid为增量或者位置
    }PID_Flag;

    typedef union {
        float kd_lpf;						//不完全微分系数
        PID_TYPE kd_pre;						//微分先行系数
    }kd_u;
    typedef struct PID_Parameter
    {
        PID_TYPE kp;								//比例系数
        PID_TYPE ki;								//积分系数
        PID_TYPE kd;								//微分系数
        PID_TYPE kf;								//前馈系数

        //PID_TYPE kd_lpf;						//不完全微分系数
        //PID_TYPE kd_pre;						//微分先行系数
        kd_u kd_;
        PID_TYPE k_pre;						//预测系数

        PID_TYPE target_limit;				//目标值限幅
        PID_TYPE bias_limit;					//误差限幅
        PID_TYPE bias_dead_zone;			//小于这个值将不进行PID操作
        PID_TYPE bias_for_integral;	//开始积分的误差	--	用于积分分离
        PID_TYPE integral_limit;			//积分限幅				--	用于抗积分饱和
        PID_TYPE out_limit;					//输出限幅
        //PID_TYPE a;               //低通滤波器参数          -- 用于带一阶低通的微分
        PID_TYPE k;               //并行pid相加系数          -- 用于并行pid

        PID_TYPE out; //此节点pid输出

        PID_TYPE target; //期望值
        PID_TYPE present;//当前值
        PID_TYPE predict;//预测值

#if USE_HOOK_FIRST
        void (*user_hook_first)(PID_T* pid);//钩子函数，在计算result之前，其他必要操作之后
#endif
#if USE_HOOK_FIRST
        void (*user_hook_out)(PID_T* pid);//钩子函数，在计算result之后，限幅之前
#endif
    }PID_Parameter;

    //增量式pid
    // n
    //比例P:    e(k) - e(k - 1)   当前误差 - 上次误差
    //
    //积分I : e(i)     当前误差
    //
    //微分D : e(k) - 2e(k - 1) + e(k - 2)   当前误差 - 2 * 上次误差 + 上上次误差

    //位置式
    //
    //e(k) : 用户设定的值（目标值） - 控制对象的当前的状态值
    //
    //比例P : e(k)
    //
    //积分I : ∑e(i)     误差的累加(包括e(k))
    //
    //微分D : e(k) - e(k - 1)  这次误差 - 上次误差

    typedef struct PID_Process
    {
        PID_TYPE bias; //总误差
        PID_TYPE integral_bias;//i误差
        PID_TYPE differential_bias;//d误差
        PID_TYPE lpf_differential_bias;//上次d误差

        PID_TYPE feedforward; //前馈值
        PID_TYPE predict;  //预测值

        PID_TYPE last_target; //上次期望值
        PID_TYPE last_bias; //上次误差
        PID_TYPE lastlast_bias; //上上次误差

    }PID_Process;

    typedef struct PID_T PID_t;
    typedef struct _PID_T PID_T;

    typedef struct {
        PID_Mode mode;
        PID_I_Function ifunc;
        PID_D_Function dfunc;
        PID_TYPE kp;
        PID_TYPE ki;
        PID_TYPE kd;
    }PID_Config_t;
    struct _PID_T
    {
        PID_Flag		flag;
        PID_Parameter	parameter;
        PID_Process 	process;
        PID_TYPE cycle;
        //PID_T* next_pid;//用于实现pid的并行或串级
        void(*handle)(PID_T* pid, PID_TYPE cycle);
        void(*i_handle)(PID_T* pid);
        void(*d_handle)(PID_T* pid);
        void(*variable)(PID_T* pid);//变速积分
    };

    //return a * X(n) + (1 - a) * Y(n - 1)


    void PID_STOP(PID_T* pid);
    void PID_Pause(PID_T* pid);
    void PID_Enable(PID_T* pid);
    void PID_Init(PID_T* pid, PID_Mode mode, PID_TYPE kp, PID_TYPE ki, PID_TYPE kd);

    void PID_SET_I_Function(PID_T* pid, PID_I_Function imode, ...);
    void PID_SET_D_Function(PID_T* pid, PID_D_Function dmode, ...);

	void  PID_Hander(PID_T* pid, PID_TYPE cycle);

//    void inline PID_Sst_Target_Limit(PID_T* pid, PID_TYPE value);
//    void inline PID_Sst_Bias_Limit(PID_T* pid, PID_TYPE value);
//    void inline PID_Sst_Bias_Dead_Zone(PID_T* pid, PID_TYPE value);
//    void inline PID_Sst_Integral_Limit(PID_T* pid, PID_TYPE value);
//    void inline PID_Sst_Out_Limit(PID_T* pid, PID_TYPE value);
//	
//    void inline PID_Sst_Feedforward(PID_T* pid, PID_TYPE value);
//    void inline PID_Sst_KPre(PID_T* pid, PID_TYPE value);
//	
//    void inline PID_Sst_Target(PID_T* pid, PID_TYPE value);
//    void inline PID_Sst_Present(PID_T* pid, PID_TYPE value);
//    void inline PID_Sst_Predict(PID_T* pid, PID_TYPE value);




/**
 * @description: 限制目标值/期望值最大值
 * @param {PID_T*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Target_Limit(PID_T* pid, PID_TYPE value)
{
    pid->parameter.target_limit = value;
}
/**
 * @description: 限制误差最大值
 * @param {PID_T*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Bias_Limit(PID_T* pid, PID_TYPE value)
{
    pid->parameter.bias_limit = value;
}
/**
 * @description: 设置误差响应死区
 * @param {PID_T*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Bias_Dead_Zone(PID_T* pid, PID_TYPE value)
{
    pid->parameter.bias_dead_zone = value;
}

/**
 * @description: 限制积分项最大值
 * @param {PID_T*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void PID_Sst_Integral_Limit(PID_T* pid, PID_TYPE value);
/**
 * @description: 限制pid输出最大值
 * @param {PID_T*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Out_Limit(PID_T* pid, PID_TYPE value)
{
    pid->parameter.out_limit = value;
}
/**
 * @description: 设置前馈系数
 * @param {PID_T*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Feedforward(PID_T* pid, PID_TYPE value)
{
    pid->parameter.kf = value;
}
/**
 * @description: 设置预测系数
 * @param {PID_T*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_KPre(PID_T* pid, PID_TYPE value)
{
    pid->parameter.k_pre = value;
}
/**
 * @description: 限制目标值/期望值
 * @param {PID_T*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Target(PID_T* pid, PID_TYPE value)
{
    pid->parameter.target = value;
}
/**
 * @description: 设置当前值/反馈值
 * @param {PID_T*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Present(PID_T* pid, PID_TYPE value)
{
    pid->parameter.present = value;
}
/**
 * @description: 设置预测系数
 * @param {PID_T*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Predict(PID_T* pid, PID_TYPE value)
{
    pid->parameter.predict = value;
}



#define PID_I_NORMAL(pid)               PID_SET_I_Function(pid,PID_INTEGRAL_NORMAL)//普通积分
#define PID_I_SEPARATION(pid,value)	    PID_SET_I_Function(pid,PID_INTEGRAL_SEPARATION,value)//积分分离
#define PID_I_SATURATION(pid,value)	    PID_SET_I_Function(pid,PID_INTEGRAL_SATURATION,value)//抗饱和积分
#define PID_I_SPEED(pid,func)		    PID_SET_I_Function(pid,PID_INTEGRAL_SPEED,func)//变速积分
#define PID_I_TRAPEZIOD(pid)	        PID_SET_I_Function(pid,PID_INTEGRAL_TRAPEZIOD)//梯形积分

#define PID_D_COMPLETE(pid)             PID_SET_D_Function(pid,PID_DIFFERENTIAL_COMPLETE)//完全微分
#define PID_D_PART(pid,value)           PID_SET_D_Function(pid,PID_DIFFERENTIAL_PART,value)//不完全微分
#define PID_D_PREVIOUS(pid,value)       PID_SET_D_Function(pid,PID_DIFFERENTIAL_PREVIOUS,value)//微分先行

#ifdef __cplusplus
}
#endif
#endif


