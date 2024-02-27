/*
 * @Author: dyyt 805207319@qq.com
 * @Date: 2023-07-19 11:36:27
 * @LastEditors: dyyt 805207319@qq.com
 * @LastEditTime: 2023-07-23 01:44:00
 * @FilePath: \undefinedc:\Users\LENOVO\Documents\programs\PID\VS_Project\ConsoleApplication1\ConsoleApplication1\pid_exfunc.c
 * @Description: pid��
 */
#include "pid.h"
#include <stdarg.h>


extern 	void i_handle_Increment_Normal(PID_T* pid);
extern 	void i_handle_Increment_Separation(PID_T* pid);
extern 	void i_handle_Increment_Saturation(PID_T* pid);
extern 	void i_handle_Increment_Speed(PID_T* pid);
extern 	void i_handle_Increment_Trapeziod(PID_T* pid);

extern 	void d_handle_Increment_Complete(PID_T* pid);
extern 	void d_handle_Increment_Part(PID_T* pid);
extern 	void d_handle_Increment_Previous(PID_T* pid);

extern 	void d_handle_Position_Complete(PID_T* pid);
extern 	void d_handle_Position_Part(PID_T* pid);
extern 	void d_handle_Position_Previous(PID_T* pid);

extern 	void i_handle_Position_Normal(PID_T* pid);
extern 	void i_handle_Position_Separation(PID_T* pid);
extern 	void i_handle_Position_Saturation(PID_T* pid);
extern 	void i_handle_Position_Speed(PID_T* pid);
extern 	void i_handle_Position_Trapeziod(PID_T* pid);

/*
TODO:   ʵ�ֱ��ٻ��ֵ��û��Զ���
        ʵ��ǰ�����̵��Զ���,ͬʱ����kf����
        ʵ������˲����ɹ��ܣ�������Ĥ�˲�/�û��Զ����˲�
        ���ƺ���������û�����֧����Щģʽ�Լ�С�������
        ���ɵ��Թ��ܣ���vofa�Խӣ���ͼ��(֧���������)�����name����
        ���������
        �����Զ�����
        ����asssert����

*/






/**
 * @description: ���ƻ��������ֵ
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void PID_Sst_Integral_Limit(PID_T* pid, PID_TYPE value)
{
	if(pid->flag.integral_way != PID_INTEGRAL_SATURATION)
	{
		PID_SET_I_Function(pid,PID_INTEGRAL_SATURATION,value);//�����ͻ���
	}else{
    pid->parameter.integral_limit = value;
	}
}
/**
 * @description: ����pid�Ļ���ģʽ(Ĭ��Ϊ��ͨģʽ)
 * @param {PID_T*} pid ʵ�����
 * @param {PID_I_Function} imode ֧�ֵĻ���ģʽ
 * @return {*}
 * @note ���ݾ߲�ͬ�Ĺ��ܣ�����Ҫ����һ��������ָ�����ر�ĳ�ʼ�� 
 *       PID_INTEGRAL_SEPARATION:pid->parameter.bias_for_integral //��
 *       PID_INTEGRAL_SATURATION:pid->parameter.integral_limit //��
 *       PID_INTEGRAL_SPEED:pid->variable //����
 */
void PID_SET_I_Function(PID_T* pid, PID_I_Function imode, ...)
{
    va_list ap;//����һ��va_list����
    pid->flag.integral_way = imode;
    switch (imode)
    {
    case PID_INTEGRAL_NORMAL:
        if (pid->flag.pid_mode == PID_INCREMENT_NULL) {
            pid->i_handle = i_handle_Increment_Normal;
        }
        else {
            pid->i_handle = i_handle_Position_Normal;
        }
        break;
    case PID_INTEGRAL_SEPARATION:
        va_start(ap, imode);   //��ʼ�����ڶ�������Ϊ���һ��ȷ�����β�
        if (pid->flag.pid_mode == PID_INCREMENT_NULL) {
            pid->i_handle = i_handle_Increment_Separation;
        }
        else {
            pid->i_handle = i_handle_Position_Separation;
        }
        pid->parameter.bias_for_integral = va_arg(ap, PID_TYPE);
        va_end(ap);
        break;
    case PID_INTEGRAL_SATURATION:
        if (pid->flag.pid_mode == PID_INCREMENT_NULL) {
            pid->i_handle = i_handle_Increment_Saturation;
        }
        else {
            pid->i_handle = i_handle_Position_Saturation;
        }
        va_start(ap, imode);   //��ʼ�����ڶ�������Ϊ���һ��ȷ�����β�
        pid->parameter.integral_limit = va_arg(ap, PID_TYPE);
        va_end(ap);
        break;
    case PID_INTEGRAL_SPEED:

        if (pid->flag.pid_mode == PID_INCREMENT_NULL) {
            pid->i_handle = i_handle_Increment_Speed;
        }
        else {
            pid->i_handle = i_handle_Position_Speed;
        }
        va_start(ap, imode);   //��ʼ�����ڶ�������Ϊ���һ��ȷ�����β�
        pid->variable = va_arg(ap, void*);
        va_end(ap);
        break;
    case PID_INTEGRAL_TRAPEZIOD:
        if (pid->flag.pid_mode == PID_INCREMENT_NULL) {
            pid->i_handle = i_handle_Increment_Trapeziod;
        }
        else {
            pid->i_handle = i_handle_Position_Trapeziod;
        }

        break;
    default:
        break;
    }
}

/**
 * @description: ����pid��΢��ģʽ(Ĭ��Ϊ��ͨģʽ)
 * @param {PID_T*} pid ʵ�����
 * @param {PID_D_Function} dmode
 * @return {*}
 * @note ���߲�ͬ�Ĺ��ܣ�����Ҫ����һ���������ر�ĳ�ʼ��float ����ָ��
 *          PID_DIFFERENTIAL_COMPLETE:	//��ȫ΢��
 *          PID_DIFFERENTIAL_PART:pid->parameter.kd_lpf
 *          PID_DIFFERENTIAL_PREVIOUS:pid->parameter.kd_pre
 */
void PID_SET_D_Function(PID_T* pid, PID_D_Function dmode, ...)
{
    va_list ap;//����һ��va_list����
    pid->flag.differential_way = dmode;
    switch (dmode)
    {
    case PID_DIFFERENTIAL_COMPLETE:	//��ȫ΢��
        if (pid->flag.pid_mode == PID_INCREMENT_NULL) {
            pid->d_handle = d_handle_Increment_Complete;
        }
        else {
            pid->d_handle = d_handle_Position_Complete;
        }
        break;
    case PID_DIFFERENTIAL_PART:					//����ȫ΢��
        va_start(ap, dmode);   //��ʼ�����ڶ�������Ϊ���һ��ȷ�����β�
        if (pid->flag.pid_mode == PID_INCREMENT_NULL) {
            pid->d_handle = d_handle_Increment_Part;
        }
        else {
            pid->d_handle = d_handle_Position_Part;
        }
        pid->parameter.kd_.kd_lpf = va_arg(ap, PID_TYPE);
        va_end(ap);
        break;
    case PID_DIFFERENTIAL_PREVIOUS:			//΢������
        va_start(ap, dmode);   //��ʼ�����ڶ�������Ϊ���һ��ȷ�����β�
        if (pid->flag.pid_mode == PID_INCREMENT_NULL) {
            pid->d_handle = d_handle_Increment_Previous;
        }
        else {
            pid->d_handle = d_handle_Position_Previous;
        }
        pid->parameter.kd_.kd_pre = va_arg(ap, PID_TYPE);
        va_end(ap);
        break;
    default:
        break;
    }
}
