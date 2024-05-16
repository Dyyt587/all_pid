/*
 * @Author: dyyt 805207319@qq.com
 * @Date: 2023-05-29 16:03:17
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-05-13 10:59:51
 * @FilePath: \undefinedc:\Users\LENOVO\Documents\programs\PID\VS_Project\ConsoleApplication1\ConsoleApplication1\pid.c
 * @Description:pid库
 */

#include "apid.h"
#include "stdio.h"
#include <stdarg.h>
#include <string.h>
#ifdef __cplusplus
extern "C"
{
#endif
	void APID_Reset(apid_t *pid);

	/////////////////////////////////////////////////////////
#if EXAMPLE
	// 以下为pid初始化实例和解析
	apid_t pid = {0};
	apid_t next_pid = {0};
	void example(void)
	{
	}

#endif

	void inline APID_Hander(apid_t *pid, PID_TYPE cycle)
	{
		pid->handle(pid, cycle);
	}
	float FOLowPassFilter(float In, float LastOut, float a)
	{
		return a * In + (1 - a) * LastOut;
	}

	/**
	 * @description: 暂停pid计算,输出将等于输入
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void APID_Pause(apid_t *pid)
	{
		pid->flag.run_status = PAUSE;
	}
	/**
	 * @description: 停止pid,输出将恒等于0
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void APID_STOP(apid_t *pid)
	{
		pid->flag.run_status = STOP;
	}
	/**
	 * @description: 开启pid/pid正常运行
	 * @param {apid_t*} pid 实例句柄
	 * @note: 不要在已经 ENABLE 时调用重复调用
	 * @return {*}
	 */
	void APID_Enable(apid_t *pid)
	{
		/* 目标值等于当前值 防止切换时候产生阶跃冲击*/
		pid->parameter.target = pid->parameter.present;
		pid->flag.run_status = PID_ENABLE;
	}

	/**
	 * @description: 内部使用,增量式普通积分
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void i_handle_Increment_Normal(apid_t *pid)
	{
		// 普通积分
		pid->process.integral_bias = pid->process.bias * pid->cycle;
		pid->process.integral_bias = pid->process.bias * pid->cycle;
	}
	/**
	 * @description: 内部使用,增量式积分分离
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void i_handle_Increment_Separation(apid_t *pid)
	{
		// 积分分离
		if (pid->process.bias > pid->parameter.bias_for_integral ||
			pid->process.bias < -pid->parameter.bias_for_integral)
			return;
		pid->process.integral_bias = pid->process.bias * pid->cycle;
	}
	/**
	 * @description: 内部使用,增量式抗饱和积分
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void i_handle_Increment_Saturation(apid_t *pid)
	{
		// 抗积分饱和
		if (pid->process.integral_bias * pid->parameter.ki > pid->parameter.integral_limit)
			pid->process.integral_bias = pid->parameter.integral_limit / pid->parameter.ki;

		else if (pid->process.integral_bias * pid->parameter.ki < -pid->parameter.integral_limit)
			pid->process.integral_bias = -pid->parameter.integral_limit / pid->parameter.ki;

		else
			pid->process.integral_bias = pid->process.bias * pid->cycle;
		return;
	}
	/**
	 * @description: 内部使用,增量式变速积分
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void i_handle_Increment_Speed(apid_t *pid)
	{
		// 变速积分 -- 可自行添加函数或处理算法
		pid->process.integral_bias = (pid->process.bias + pid->process.last_bias) / (2.0f * pid->cycle);
		pid->process.integral_bias -= (pid->process.bias - pid->process.last_bias) * pid->cycle;
	}
	/**
	 * @description: 内部使用,增量式梯形积分
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void i_handle_Increment_Trapeziod(apid_t *pid)
	{
		// 梯形积分 -- 可自行添加函数或处理算法
		pid->process.integral_bias = (pid->process.bias + pid->process.last_bias) / (2.0f * pid->cycle);
	}

	/**
	 * @description: 内部使用,增量式全微分
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void d_handle_Increment_Complete(apid_t *pid)
	{
		pid->process.lpf_differential_bias = pid->process.differential_bias = pid->process.bias - 2 * pid->process.last_bias + pid->process.lastlast_bias;
	}
	/**
	 * @description: 内部使用,增量式不完全微分
	 * @param {apid_t*} pid 实例句柄
	 * @param {parameter.kd_lpf} *
	 * @return {*}
	 */
	void d_handle_Increment_Part(apid_t *pid)
	{
		// 求微分，再低通滤波
		pid->process.differential_bias = pid->process.bias - 2 * pid->process.last_bias + pid->process.lastlast_bias; // 本行进行d的计算
		pid->process.differential_bias = FOLowPassFilter(pid->process.differential_bias, pid->process.lpf_differential_bias, pid->parameter.kd_.kd_lpf);
		pid->process.lpf_differential_bias = pid->process.differential_bias;
		// pid->process.lpf_differential_bias = pid->parameter.kd_lpf *
		//	(3.1415926f * cycle * (pid->process.differential_bias - pid->process.lpf_differential_bias));//本行进行低通滤波 输入输出为d
		//	//(3.1415926f * cycle * (pid->process.bias - 2*pid->process.last_bias + pid->process.lastlast_bias));//本行进行低通滤波 输入输出为d
	}
	/**
	 * @description: 内部使用,增量式微分先行
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void d_handle_Increment_Previous(apid_t *pid)
	{
		// 微分先行
		pid->process.lpf_differential_bias = pid->process.differential_bias = (pid->process.bias - 2 * pid->process.last_bias + pid->process.lastlast_bias) - pid->parameter.kd_.kd_pre * (pid->parameter.target - pid->process.last_target);
	}

	/**
	 * @description: 内部使用,位置式全微分
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void d_handle_Position_Complete(apid_t *pid)
	{
		// 直接求微分
		pid->process.lpf_differential_bias = pid->process.differential_bias = pid->process.bias - pid->process.last_bias;
	}
	/**
	 * @description: 内部使用,位置式不完全微分
	 * @param {apid_t*} pid 实例句柄
	 * @param {parameter.kd_lpf * 3.1415926f * cycle		} *
	 * @return {*}
	 */
	void d_handle_Position_Part(apid_t *pid)
	{
		// 求微分，再低通滤波
		pid->process.differential_bias = pid->process.bias - pid->process.last_bias;
		pid->process.differential_bias = FOLowPassFilter(pid->process.differential_bias, pid->process.lpf_differential_bias, pid->parameter.kd_.kd_lpf);
		pid->process.lpf_differential_bias = pid->process.differential_bias;
		/*pid->process.lpf_differential_bias += pid->parameter.kd_lpf * 3.1415926f * cycle
		 * (pid->process.differential_bias - pid->process.lpf_differential_bias);*/
	}
	/**
	 * @description: 内部使用,位置式微分先行
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void d_handle_Position_Previous(apid_t *pid)
	{
		// 微分先行
		pid->process.lpf_differential_bias = pid->process.differential_bias = (pid->process.bias - pid->process.last_bias) - pid->parameter.kd_.kd_pre * (pid->parameter.target - pid->process.last_target);
	}

	/**
	 * @description: 内部使用,位置式普通积分
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void i_handle_Position_Normal(apid_t *pid)
	{
		// 普通积分
		pid->process.integral_bias += pid->process.bias * pid->cycle;
		pid->process.integral_bias += pid->process.bias * pid->cycle;
	}
	/**
	 * @description: 内部使用,位置式积分分离
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void i_handle_Position_Separation(apid_t *pid)
	{
		// 积分分离
		if (pid->process.bias > pid->parameter.bias_for_integral || pid->process.bias < -pid->parameter.bias_for_integral)
			return;

		pid->process.integral_bias += pid->process.bias * pid->cycle;
	}
	/**
	 * @description: 内部使用，位置式抗饱和积分
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void i_handle_Position_Saturation(apid_t *pid)
	{
		//		//抗积分饱和
		//		if (pid->process.integral_bias * pid->parameter.ki > pid->parameter.integral_limit)
		//			pid->process.integral_bias = pid->parameter.integral_limit * pid->parameter.ki;

		//		else if (pid->process.integral_bias * pid->parameter.ki < -pid->parameter.integral_limit)
		//			pid->process.integral_bias = -pid->parameter.integral_limit * pid->parameter.ki;

		//		else
		//			pid->process.integral_bias += pid->process.bias * pid->cycle;

		// 抗积分饱和
		if (pid->process.integral_bias * pid->parameter.ki > pid->parameter.integral_limit)
			pid->process.integral_bias = pid->parameter.integral_limit;

		else if (pid->process.integral_bias * pid->parameter.ki < -pid->parameter.integral_limit)
			pid->process.integral_bias = -pid->parameter.integral_limit;

		else
			pid->process.integral_bias += pid->process.bias * pid->cycle;
	}
	/**
	 * @description: 内部使用，位置式变速积分
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void i_handle_Position_Speed(apid_t *pid)
	{
		// 变速积分 -- 可自行添加函数或处理算法
		pid->process.integral_bias += (pid->process.bias + pid->process.last_bias) / (2.0f * pid->cycle);
		pid->process.integral_bias -= (pid->process.bias - pid->process.last_bias) * pid->cycle;
	}
	/**
	 * @description: 内部使用，位置式梯形积分
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void i_handle_Position_Trapeziod(apid_t *pid)
	{
		pid->process.integral_bias += (pid->process.bias + pid->process.last_bias) / (2.0f * pid->cycle);
	}

	/**
	 * @description: 内部使用
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void static __PID_Out(apid_t *pid)
	{
		////////////////////////////////输出合成
#if USE_HOOK_FIRST
		if (pid->user_hook_first)
			pid->user_hook_first(pid);
#endif
		// 计算输出
		pid->parameter.out = pid->parameter.kp * pid->process.bias +
							 pid->parameter.ki * pid->process.integral_bias +
							 pid->parameter.kd * pid->process.lpf_differential_bias / pid->cycle +
							 pid->process.feedforward;
#if USE_HOOK_OUT
		if (pid->user_hook_end)
			pid->user_hook_end(pid);
#endif
		// 输出限幅
		if (pid->parameter.out_limit > 0)
		{
			if (pid->parameter.out > pid->parameter.out_limit)
				pid->parameter.out = pid->parameter.out_limit;

			else if (pid->parameter.out < -pid->parameter.out_limit)
				pid->parameter.out = -pid->parameter.out_limit;
		}
		// 存储过去值
		pid->process.last_target = (pid->parameter.target);
		pid->process.lastlast_bias = pid->process.last_bias; // 仅增量式使用
		pid->process.last_bias = pid->process.bias;
	}

// 内部使用,检查是否要进行pid计算
#define __OPERATE_STATUS()                              \
	if (pid->flag.run_status != PID_ENABLE)             \
	{                                                   \
		if (pid->flag.run_status == STOP)               \
		{                                               \
			pid->parameter.out = 0;                     \
		}                                               \
		else                                            \
		{                                               \
			pid->parameter.out = pid->parameter.target; \
		}                                               \
		pid->process.bias = 0;                          \
		pid->process.differential_bias = 0;             \
		pid->process.lpf_differential_bias = 0;         \
		pid->process.feedforward = 0;                   \
		pid->process.integral_bias = 0;                 \
		pid->process.last_bias = 0;                     \
		pid->process.last_target = 0;                   \
		pid->process.lastlast_bias = 0;                 \
		pid->parameter.present = 0;                     \
		pid->parameter.predict = 0;                     \
		return;                                         \
	}

	/**
	 * @description: pid计算函数
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} cycle pid计算周期,单位ms
	 * @return {*}
	 */
	void _PID_Hander(apid_t *pid, PID_TYPE cycle)
	{
		pid->cycle = cycle;
		__OPERATE_STATUS(); // 检查运行状态
		////////////////////////////////期望值限幅
		if ((pid->parameter.target) > pid->parameter.target_limit)
			(pid->parameter.target) = pid->parameter.target_limit;
		else if ((pid->parameter.target) < -pid->parameter.target_limit)
			(pid->parameter.target) = -pid->parameter.target_limit;
		////////////////////////////////前馈操作--前馈值直接加入输出
		pid->process.feedforward = pid->parameter.kf * (pid->parameter.target);
		/////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////预测操作--预测值直接加入偏差
		pid->process.predict = pid->parameter.k_pre * pid->parameter.predict * ABS(pid->parameter.predict);
		/////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////偏差操作
		// 第一部分为期望
		// 第二部分为反馈
		// 第三部分为预测（一般为内环反馈,可不设置）
		PID_TYPE temp_bias = (pid->parameter.target) -
							 (pid->parameter.present) -
							 pid->process.predict;

		if (pid->parameter.bias_dead_zone > 0)
			temp_bias = (temp_bias < pid->parameter.bias_dead_zone &&
						 temp_bias > -pid->parameter.bias_dead_zone)
							? 0
							: temp_bias; // 误差死区判断--不用该功能时可以将其置-1 0

		if (pid->parameter.bias_limit > 0)
		{
			temp_bias = (temp_bias > pid->parameter.bias_limit) ? pid->parameter.bias_limit : temp_bias; // 误差限幅--不用该功能时可以将其置-1 0
			temp_bias = (temp_bias < -pid->parameter.bias_limit) ? -pid->parameter.bias_limit : temp_bias;
		}

		pid->process.bias = temp_bias; // 计算误差


#if APID_USING_FUZZY
		if (pid->fuzzy_ctrl)
			pid->fuzzy_ctrl(pid);
#endif
#if USE_HOOK_PRE_CPLT
		if (pid->user_hook_pre_cplt)
			pid->user_hook_pre_cplt(pid);
#endif
		////////////////////////////////积分操作
		pid->i_handle(pid);
		////////////////////////////////微分操作
		pid->d_handle(pid);
		////////////////////////////////输出操作，包含p操作
		__PID_Out(pid);
#if APID_USING_AUTO_PID
		if (pid->auto_pid_handler)
			pid->auto_pid_handler(pid, cycle);
#endif
		// LOG_RAW("t p o:%f,%f,%f\r\n",pid->parameter.target,pid->parameter.present,pid->parameter.out);
	}

	/**
	 * @description: 初始化pid,配置必要参数
	 * @param {apid_t*} pid 实例句柄
	 * @param {ALL_PID_Mode} mode 增量式或位置式
	 * @param {PID_TYPE} kp pid参数
	 * @param {PID_TYPE} ki pid参数
	 * @param {PID_TYPE} kd pid参数
	 * @return {*}
	 */
	void APID_Init(apid_t *pid, ALL_PID_Mode mode, PID_TYPE kp, PID_TYPE ki, PID_TYPE kd)
	{
		APID_Reset(pid);
		pid->fuzzy_ctrl=0;
		pid->flag.pid_mode = mode;
		pid->handle = _PID_Hander; // 注册处理函数
		// 注册函数
		if (mode == PID_INCREMENT)
		{
			pid->i_handle = i_handle_Increment_Normal;
			pid->d_handle = d_handle_Increment_Complete;
		}
		else
		{
			pid->i_handle = i_handle_Position_Normal;
			pid->d_handle = d_handle_Position_Complete;
		}
		pid->parameter.kp = kp;
		pid->parameter.ki = ki;
		pid->parameter.kd = kd;
	}

	/**
	 * @description: 将pid复位成原始状态
	 * @param {apid_t*} pid 实例句柄
	 * @return {*}
	 */
	void APID_Reset(apid_t *pid)
	{
		memset(pid,0,sizeof(apid_t));
#if APID_USING_AUTO_PID

		pid->auto_pid=0;
		pid->auto_pid_handler=0;
#endif
		
#if APID_USING_AUTO_PID

		pid->auto_pid=0;
		pid->auto_pid_handler=0;
#endif

		pid->flag.run_status = PID_ENABLE;

		pid->parameter.target = 0;
		pid->parameter.present = 0;
		pid->parameter.predict = 0;

		pid->parameter.kp = 0;
		pid->parameter.ki = 0;
		pid->parameter.kd = 0;
		pid->parameter.kf = 0;
		pid->parameter.kd_.kd_lpf = 0;
		pid->parameter.kd_.kd_pre = 0;
		pid->parameter.k_pre = 0;

		pid->parameter.target_limit = TARGET_MAX;
		pid->parameter.bias_limit = -1;
		pid->parameter.bias_dead_zone = -1;
		pid->parameter.bias_for_integral = -1;
		pid->parameter.integral_limit = -1;
		pid->parameter.out_limit = OUT_MAX;

		pid->parameter.out = 0;

		pid->process.bias = 0;
		pid->process.differential_bias = 0;
		pid->process.lpf_differential_bias = 0;
		pid->process.feedforward = 0;
		pid->process.integral_bias = 0;
		pid->process.last_bias = 0;
		pid->process.lastlast_bias = 0;
		pid->process.last_target = 0;
	}

	/**
	 * @description: 限制目标值/期望值最大值
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} value
	 * @return {*}
	 */
	void inline APID_Set_Target_Limit(apid_t *pid, PID_TYPE value)
	{
		pid->parameter.target_limit = value;
	}
	/**
	 * @description: 限制误差最大值
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} value
	 * @return {*}
	 */
	void inline APID_Set_Bias_Limit(apid_t *pid, PID_TYPE value)
	{
		pid->parameter.bias_limit = value;
	}
	/**
	 * @description: 设置误差响应死区
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} value
	 * @return {*}
	 */
	void inline APID_Set_Bias_Dead_Zone(apid_t *pid, PID_TYPE value)
	{
		pid->parameter.bias_dead_zone = value;
	}

	/**
	 * @description: 限制积分项最大值
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} value
	 * @return {*}
	 */
	void APID_Set_Integral_Limit(apid_t *pid, PID_TYPE value);
	/**
	 * @description: 限制pid输出最大值
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} value
	 * @return {*}
	 */
	void inline APID_Set_Out_Limit(apid_t *pid, PID_TYPE value)
	{
		pid->parameter.out_limit = value;
	}
	/**
	 * @description: 设置前馈系数
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} value
	 * @return {*}
	 */
	void inline APID_Set_Feedforward(apid_t *pid, PID_TYPE value)
	{
		pid->parameter.kf = value;
	}
	/**
	 * @description: 设置预测系数
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} value
	 * @return {*}
	 */
	void inline APID_Set_KPre(apid_t *pid, PID_TYPE value)
	{
		pid->parameter.k_pre = value;
	}
	/**
	 * @description: 限制目标值/期望值
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} value
	 * @return {*}
	 */
	void inline APID_Set_Target(apid_t *pid, PID_TYPE value)
	{
		pid->parameter.target = value;
	}
	/**
	 * @description: 设置当前值/反馈值
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} value
	 * @return {*}
	 */
	void inline APID_Set_Present(apid_t *pid, PID_TYPE value)
	{
		pid->parameter.present = value;
	}
	/**
	 * @description: 设置预测系数
	 * @param {apid_t*} pid 实例句柄
	 * @param {PID_TYPE} value
	 * @return {*}
	 */
	void inline APID_Set_Predict(apid_t *pid, PID_TYPE value)
	{
		pid->parameter.predict = value;
	}

	/**
	 * @description: 获取pid输出
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE inline APID_Get_Out(apid_t *pid)
	{
		return pid->parameter.out;
	}

	/**
	 * @description: 获取pid目标值/期望值最大值
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE APID_Get_Target_Limit(apid_t *pid)
	{
		return pid->parameter.target_limit;
	}

	/**
	 * @description: 获取pid误差最大值
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE APID_Get_Bias_Limit(apid_t *pid)
	{
		return pid->parameter.bias_limit;
	}

	/**
	 * @description: 获取pid误差响应死区
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE APID_Get_Bias_Dead_Zone(apid_t *pid)
	{
		return pid->parameter.bias_dead_zone;
	}

	/**
	 * @description: 获取pid积分项最大值
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE APID_Get_Integral_Limit(apid_t *pid)
	{
		return pid->parameter.integral_limit;
	}

	/**
	 * @description: 获取pid输出最大值
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE APID_Get_Out_Limit(apid_t *pid)
	{
		return pid->parameter.out_limit;
	}

	/**
	 * @description: 获取pid前馈系数
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE APID_Get_Feedforward(apid_t *pid)
	{
		return pid->parameter.kf;
	}

	/**
	 * @description: 获取pid预测系数
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE APID_Get_KPre(apid_t *pid)
	{
		return pid->parameter.k_pre;
	}

	/**
	 * @description: 获取pid期望
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE inline APID_Get_Target(apid_t *pid)
	{
		return pid->parameter.target;
	}

	/**
	 * @description: 获取pid当前值
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE inline APID_Get_Present(apid_t *pid)
	{
		return pid->parameter.present;
	}

	/**
	 * @description: 获取pid预测值
	 * @param {apid_t*} pid 实例句柄
	 * @return {PID_TYPE}
	 */
	PID_TYPE inline APID_Get_Predict(apid_t *pid)
	{
		return pid->parameter.predict;
	}
