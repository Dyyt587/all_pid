/*
 * @Author: dyyt 805207319@qq.com
 * @Date: 2023-05-29 16:03:17
 * @LastEditors: dyyt 805207319@qq.com
 * @LastEditTime: 2023-07-23 01:19:13
 * @FilePath: \undefinedc:\Users\LENOVO\Documents\programs\PID\VS_Project\ConsoleApplication1\ConsoleApplication1\pid.c
 * @Description:pid��
 */

#include"pid.h"
#include"stdio.h"
#include <stdarg.h>
#ifdef __cplusplus
extern "C"
{
#endif
	void PID_Reset(PID_T* pid);

	/////////////////////////////////////////////////////////
#if EXAMPLE
//����Ϊpid��ʼ��ʵ���ͽ���
	PID_T pid = { 0 };
	PID_T next_pid = { 0 };
	void example(void)
	{


	}

#endif

    void  inline PID_Hander(PID_T* pid, PID_TYPE cycle)
    {
        pid->handle(pid, cycle);
    }
    float  FOLowPassFilter(float In, float LastOut, float a)
    {
        return a * In + (1 - a) * LastOut;
    }

 /**
  * @description: ��ͣpid����,�������������
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void PID_Pause(PID_T* pid)
	{
		pid->flag.run_status = PAUSE;
	}
 /**
  * @description: ֹͣpid,����������0
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void PID_STOP(PID_T* pid)
	{
		pid->flag.run_status = STOP;
	}
 /**
  * @description: ����pid/pid��������
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void PID_Enable(PID_T* pid)
	{
		pid->flag.run_status = PID_ENABLE;
	}

 /**
  * @description: �ڲ�ʹ��,����ʽ��ͨ����
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void i_handle_Increment_Normal(PID_T* pid)
	{
		//��ͨ����
		pid->process.integral_bias = pid->process.bias * pid->cycle;
		pid->process.integral_bias = pid->process.bias * pid->cycle;

	}
 /**
  * @description: �ڲ�ʹ��,����ʽ���ַ��� 
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void i_handle_Increment_Separation(PID_T* pid)
	{
		//���ַ���
		if (pid->process.bias > pid->parameter.bias_for_integral ||
			pid->process.bias < -pid->parameter.bias_for_integral)
			return;
		pid->process.integral_bias = pid->process.bias * pid->cycle;
	}
 /**
  * @description: �ڲ�ʹ��,����ʽ�����ͻ���
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void i_handle_Increment_Saturation(PID_T* pid)
	{
		//�����ֱ���
		if (pid->process.integral_bias * pid->parameter.ki > pid->parameter.integral_limit)
			pid->process.integral_bias = pid->parameter.integral_limit / pid->parameter.ki;

		else if (pid->process.integral_bias * pid->parameter.ki < -pid->parameter.integral_limit)
			pid->process.integral_bias = -pid->parameter.integral_limit / pid->parameter.ki;

		else
			pid->process.integral_bias = pid->process.bias * pid->cycle;
		return;
	}
 /**
  * @description: �ڲ�ʹ��,����ʽ���ٻ���
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void i_handle_Increment_Speed(PID_T* pid)
	{
		//���ٻ��� -- ��������Ӻ��������㷨
		pid->process.integral_bias = (pid->process.bias + pid->process.last_bias) / (2.0f * pid->cycle);
		pid->process.integral_bias -= (pid->process.bias - pid->process.last_bias) * pid->cycle;

	}
 /**
  * @description: �ڲ�ʹ��,����ʽ���λ���
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void i_handle_Increment_Trapeziod(PID_T* pid)
	{
		//���λ��� -- ��������Ӻ��������㷨
		pid->process.integral_bias = (pid->process.bias + pid->process.last_bias) / (2.0f * pid->cycle);
	}

 /**
  * @description: �ڲ�ʹ��,����ʽȫ΢��
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void d_handle_Increment_Complete(PID_T* pid)
	{
		pid->process.lpf_differential_bias
			= pid->process.differential_bias
			= pid->process.bias - 2 * pid->process.last_bias + pid->process.lastlast_bias;
	}
 /**
  * @description: �ڲ�ʹ��,����ʽ����ȫ΢��
  * @param {PID_T*} pid ʵ�����
  * @param {parameter.kd_lpf} *
  * @return {*}
  */
	void d_handle_Increment_Part(PID_T* pid)
	{
		//��΢�֣��ٵ�ͨ�˲�
		pid->process.differential_bias = pid->process.bias - 2 * pid->process.last_bias + pid->process.lastlast_bias;//���н���d�ļ���
		pid->process.differential_bias = FOLowPassFilter(pid->process.differential_bias, pid->process.lpf_differential_bias, pid->parameter.kd_.kd_lpf);
		pid->process.lpf_differential_bias = pid->process.differential_bias;
		//pid->process.lpf_differential_bias = pid->parameter.kd_lpf * 
		//	(3.1415926f * cycle * (pid->process.differential_bias - pid->process.lpf_differential_bias));//���н��е�ͨ�˲� �������Ϊd
		//	//(3.1415926f * cycle * (pid->process.bias - 2*pid->process.last_bias + pid->process.lastlast_bias));//���н��е�ͨ�˲� �������Ϊd
	}
 /**
  * @description: �ڲ�ʹ��,����ʽ΢������
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void d_handle_Increment_Previous(PID_T* pid)
	{
		//΢������
		pid->process.lpf_differential_bias
			= pid->process.differential_bias
			= (pid->process.bias - 2 * pid->process.last_bias + pid->process.lastlast_bias) - pid->parameter.kd_.kd_pre * (pid->parameter.target - pid->process.last_target);
	}

 /**
  * @description: �ڲ�ʹ��,λ��ʽȫ΢��
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void d_handle_Position_Complete(PID_T* pid)
	{
		//ֱ����΢��
		pid->process.lpf_differential_bias
			= pid->process.differential_bias
			= pid->process.bias - pid->process.last_bias;
	}
 /**
  * @description: �ڲ�ʹ��,λ��ʽ����ȫ΢��
  * @param {PID_T*} pid ʵ�����
  * @param {parameter.kd_lpf * 3.1415926f * cycle		} *
  * @return {*}
  */
	void d_handle_Position_Part(PID_T* pid)
	{
		//��΢�֣��ٵ�ͨ�˲�
		pid->process.differential_bias = pid->process.bias - pid->process.last_bias;
		pid->process.differential_bias = FOLowPassFilter(pid->process.differential_bias, pid->process.lpf_differential_bias, pid->parameter.kd_.kd_lpf);
		pid->process.lpf_differential_bias = pid->process.differential_bias;
		/*pid->process.lpf_differential_bias += pid->parameter.kd_lpf * 3.1415926f * cycle
			* (pid->process.differential_bias - pid->process.lpf_differential_bias);*/
	}
 /**
  * @description: �ڲ�ʹ��,λ��ʽ΢������
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void d_handle_Position_Previous(PID_T* pid)
	{
		//΢������
		pid->process.lpf_differential_bias
			= pid->process.differential_bias
			= (pid->process.bias - pid->process.last_bias) - pid->parameter.kd_.kd_pre * (pid->parameter.target - pid->process.last_target);
	}

 /**
  * @description: �ڲ�ʹ��,λ��ʽ��ͨ����
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void i_handle_Position_Normal(PID_T* pid)
	{
		//��ͨ����
		pid->process.integral_bias += pid->process.bias * pid->cycle;
		pid->process.integral_bias += pid->process.bias * pid->cycle;

	}
 /**
  * @description: �ڲ�ʹ��,λ��ʽ���ַ���
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void i_handle_Position_Separation(PID_T* pid)
	{
		//���ַ���
		if (pid->process.bias > pid->parameter.bias_for_integral || pid->process.bias < -pid->parameter.bias_for_integral)
			return;

		pid->process.integral_bias += pid->process.bias * pid->cycle;

	}
 /**
  * @description: �ڲ�ʹ�ã�λ��ʽ�����ͻ���
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void i_handle_Position_Saturation(PID_T* pid)
	{
		//�����ֱ���
		if (pid->process.integral_bias * pid->parameter.ki > pid->parameter.integral_limit)
			pid->process.integral_bias = pid->parameter.integral_limit / pid->parameter.ki;

		else if (pid->process.integral_bias * pid->parameter.ki < -pid->parameter.integral_limit)
			pid->process.integral_bias = -pid->parameter.integral_limit / pid->parameter.ki;

		else
			pid->process.integral_bias += pid->process.bias * pid->cycle;
	}
 /**
  * @description: �ڲ�ʹ�ã�λ��ʽ���ٻ���
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void i_handle_Position_Speed(PID_T* pid)
	{
		//���ٻ��� -- ��������Ӻ��������㷨
		pid->process.integral_bias += (pid->process.bias + pid->process.last_bias) / (2.0f * pid->cycle);
		pid->process.integral_bias -= (pid->process.bias - pid->process.last_bias) * pid->cycle;
	}
 /**
  * @description: �ڲ�ʹ�ã�λ��ʽ���λ���
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void i_handle_Position_Trapeziod(PID_T* pid)
	{
		pid->process.integral_bias += (pid->process.bias + pid->process.last_bias) / (2.0f * pid->cycle);
	}


 /**
  * @description: �ڲ�ʹ��
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void inline __PID_Out(PID_T* pid)
	{
		////////////////////////////////����ϳ�
#if USE_HOOK_FIRST
		pid->parameter.user_hook_first(pid);
#endif
		//�������
		pid->parameter.out = pid->parameter.kp * pid->process.bias +
			pid->parameter.ki * pid->process.integral_bias +
			pid->parameter.kd * pid->process.lpf_differential_bias / pid->cycle + 	//���Խ�cycleע��
			pid->process.feedforward;
#if USE_HOOK_OUT
		pid->parameter.user_hook_end(pid);
#endif
		//����޷�
		if (pid->parameter.out_limit >= 0)
		{
			if (pid->parameter.out > pid->parameter.out_limit)
				pid->parameter.out = pid->parameter.out_limit;

			else if (pid->parameter.out < -pid->parameter.out_limit)
				pid->parameter.out = -pid->parameter.out_limit;
		}
		//�洢��ȥֵ
		pid->process.last_target = (pid->parameter.target);
		pid->process.lastlast_bias = pid->process.last_bias;//������ʽʹ��
		pid->process.last_bias = pid->process.bias;
	}

//�ڲ�ʹ��,����Ƿ�Ҫ����pid����
#define __OPERATE_STATUS \
	if (pid->flag.run_status != PID_ENABLE)\
	{\
		if (pid->flag.run_status == STOP)\
		{\
			pid->parameter.out = 0;\
		}\
		else {\
			pid->parameter.out = pid->parameter.target;\
		}\
		pid->process.bias = 0;\
		pid->process.differential_bias = 0;\
		pid->process.lpf_differential_bias = 0;\
		pid->process.feedforward = 0;\
		pid->process.integral_bias = 0;\
		pid->process.last_bias = 0;\
		pid->process.last_target = 0;\
		pid->process.lastlast_bias = 0;\
		pid->parameter.present = 0;\
		pid->parameter.predict = 0;\
		return;\
	}
 /**
  * @description: �ڲ�ʹ��
  * @param {PID_T*} pid ʵ�����1
  * @return {*}
  */
	void inline __Is_Target_Limit(PID_T* pid)
	{
		////////////////////////////////����ֵ�޷�
		if ((pid->parameter.target) > pid->parameter.target_limit)
			(pid->parameter.target) = pid->parameter.target_limit;
		else if ((pid->parameter.target) < -pid->parameter.target_limit)
			(pid->parameter.target) = -pid->parameter.target_limit;
	}

 /**
  * @description: pid���㺯��
  * @param {PID_T*} pid ʵ�����
  * @param {PID_TYPE} cycle pid��������,��λms
  * @return {*}
  */
	void _PID_Hander(PID_T* pid, PID_TYPE cycle)
	{
		pid->cycle = cycle;
		__OPERATE_STATUS;//�������״̬
		////////////////////////////////����ֵ�޷�
		__Is_Target_Limit(pid);
		////////////////////////////////ǰ������--ǰ��ֱֵ�Ӽ������
		pid->process.feedforward = pid->parameter.kf * (pid->parameter.target);
		/////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////Ԥ�����--Ԥ��ֱֵ�Ӽ���ƫ��	
		pid->process.predict = pid->parameter.k_pre * pid->parameter.predict * ABS(pid->parameter.predict);
		/////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////ƫ�����	
			//��һ����Ϊ����
			//�ڶ�����Ϊ����
			//��������ΪԤ�⣨һ��Ϊ�ڻ�����,�ɲ����ã�
		PID_TYPE temp_bias = (pid->parameter.target) -
			(pid->parameter.present) -
			pid->process.predict;

		if (pid->parameter.bias_dead_zone >= 0)
			temp_bias = (temp_bias < pid->parameter.bias_dead_zone&&
				temp_bias > -pid->parameter.bias_dead_zone) ? 0 : temp_bias;	//��������ж�--���øù���ʱ���Խ�����-1

		if (pid->parameter.bias_limit >= 0)
		{
			temp_bias = (temp_bias > pid->parameter.bias_limit) ? pid->parameter.bias_limit : temp_bias;//����޷�--���øù���ʱ���Խ�����-1
			temp_bias = (temp_bias < -pid->parameter.bias_limit) ? -pid->parameter.bias_limit : temp_bias;
		}

		pid->process.bias = temp_bias;		//�������
		////////////////////////////////���ֲ���
		pid->i_handle(pid);
		////////////////////////////////΢�ֲ���	
		pid->d_handle(pid);
		////////////////////////////////�������������p����
		__PID_Out(pid);
	}

 /**
  * @description: ��ʼ��pid,���ñ�Ҫ����
  * @param {PID_T*} pid ʵ�����
  * @param {PID_Mode} mode ����ʽ��λ��ʽ
  * @param {PID_TYPE} kp pid����
  * @param {PID_TYPE} ki pid����
  * @param {PID_TYPE} kd pid����
  * @return {*}
  */
	void PID_Init(PID_T* pid, PID_Mode mode, PID_TYPE kp, PID_TYPE ki, PID_TYPE kd)
	{
		PID_Reset(pid);
		pid->flag.pid_mode = mode;
		pid->handle = _PID_Hander;//ע�ᴦ����
		//ע�ắ��
		if (mode == PID_INCREMENT_NULL) {
			//pid->handle = _PID_Hander_Increment;
			pid->i_handle = i_handle_Increment_Normal;
			pid->d_handle = d_handle_Increment_Complete;
		}
		else {
			//pid->handle = _PID_Hander_Position;
			pid->i_handle = i_handle_Position_Normal;
			pid->d_handle = d_handle_Position_Complete;
		}
		pid->parameter.kp = kp;
		pid->parameter.ki = ki;
		pid->parameter.kd = kd;

	}


 /**
  * @description: ��pid��λ��ԭʼ״̬
  * @param {PID_T*} pid ʵ�����
  * @return {*}
  */
	void PID_Reset(PID_T* pid)
	{
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

	/*

	pid->flag.run = 0;
	pid->flag.use_predict = 0;

	pid->parameter.target = 0;
	pid->parameter.present = 0;
	pid->parameter.predict = 0;

	pid->parameter.kp = 0;
	pid->parameter.ki = 0;
	pid->parameter.kd = 0;
	pid->parameter.kf = 0;
	pid->parameter.kd_lpf = 0;
	pid->parameter.kd_pre = 0;
	pid->parameter.k_pre = 0;

	pid->parameter.target_limit = -1;
	pid->parameter.bias_limit = -1;
	pid->parameter.bias_dead_zone = -1;
	pid->parameter.bias_for_integral = -1;
	pid->parameter.integral_limit = -1;
	pid->parameter.out_limit = -1;

	pid->parameter.out = 0;
	pid->parameter.a = 1;

	pid->process.bias = 0;
	pid->process.differential_bias = 0;
	pid->process.lpf_differential_bias = 0;
	pid->process.feedforward = 0;
	pid->process.integral_bias = 0;
	pid->process.last_bias = 0;
	pid->process.lastlast_bias = 0;
	pid->process.last_target = 0;
	*/

