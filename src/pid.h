/*
 * @Author: dyyt 805207319@qq.com
 * @Date: 2023-05-29 16:03:17
 * @LastEditors: dyyt 805207319@qq.com
 * @LastEditTime: 2023-07-23 01:44:41
 * @FilePath: \undefinedc:\Users\LENOVO\Documents\programs\PID\VS_Project\ConsoleApplication1\ConsoleApplication1\pid.h
 * @Description: pid��
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

#define TARGET_MAX FLT_MAX //Ĭ������޷�ֵ
#define OUT_MAX    FLT_MAX //Ĭ������޷�ֵ
#define PID_TYPE float //�ÿ�ʹ�õ���������

    //ע�⣬���Ӻ���������pid�ڵ㶼��Ч����ÿ���ڵ㶼�����б�д�����Լ��ĺ���
#define USE_HOOK_FIRST 0//ʹ�ù��Ӻ��������б�д
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
        PID_INTEGRAL_NORMAL = 0,		//��ͨ����
        PID_INTEGRAL_SEPARATION,	//���ַ���
        PID_INTEGRAL_SATURATION,	//�����ͻ���
        PID_INTEGRAL_SPEED,			//���ٻ���
        PID_INTEGRAL_TRAPEZIOD		//���λ���
    }PID_I_Function;

    typedef enum PID_D_Function
    {
        PID_DIFFERENTIAL_COMPLETE = 0,			//��ȫ΢��
        PID_DIFFERENTIAL_PART,					//����ȫ΢��
        PID_DIFFERENTIAL_PREVIOUS 			//΢������
    }PID_D_Function;

    typedef struct
    {
        u8 run_status;
        //u8 use_predict;
        PID_I_Function integral_way;
        PID_D_Function differential_way;
        PID_Mode pid_mode;//�ж�pidΪ��������λ��
    }PID_Flag;

    typedef union {
        float kd_lpf;						//����ȫ΢��ϵ��
        PID_TYPE kd_pre;						//΢������ϵ��
    }kd_u;
    typedef struct PID_Parameter
    {
        PID_TYPE kp;								//����ϵ��
        PID_TYPE ki;								//����ϵ��
        PID_TYPE kd;								//΢��ϵ��
        PID_TYPE kf;								//ǰ��ϵ��

        //PID_TYPE kd_lpf;						//����ȫ΢��ϵ��
        //PID_TYPE kd_pre;						//΢������ϵ��
        kd_u kd_;
        PID_TYPE k_pre;						//Ԥ��ϵ��

        PID_TYPE target_limit;				//Ŀ��ֵ�޷�
        PID_TYPE bias_limit;					//����޷�
        PID_TYPE bias_dead_zone;			//С�����ֵ��������PID����
        PID_TYPE bias_for_integral;	//��ʼ���ֵ����	--	���ڻ��ַ���
        PID_TYPE integral_limit;			//�����޷�				--	���ڿ����ֱ���
        PID_TYPE out_limit;					//����޷�
        //PID_TYPE a;               //��ͨ�˲�������          -- ���ڴ�һ�׵�ͨ��΢��
        PID_TYPE k;               //����pid���ϵ��          -- ���ڲ���pid

        PID_TYPE out; //�˽ڵ�pid���

        PID_TYPE target; //����ֵ
        PID_TYPE present;//��ǰֵ
        PID_TYPE predict;//Ԥ��ֵ

#if USE_HOOK_FIRST
        void (*user_hook_first)(PID_T* pid);//���Ӻ������ڼ���result֮ǰ��������Ҫ����֮��
#endif
#if USE_HOOK_FIRST
        void (*user_hook_out)(PID_T* pid);//���Ӻ������ڼ���result֮���޷�֮ǰ
#endif
    }PID_Parameter;

    //����ʽpid
    // n
    //����P:    e(k) - e(k - 1)   ��ǰ��� - �ϴ����
    //
    //����I : e(i)     ��ǰ���
    //
    //΢��D : e(k) - 2e(k - 1) + e(k - 2)   ��ǰ��� - 2 * �ϴ���� + ���ϴ����

    //λ��ʽ
    //
    //e(k) : �û��趨��ֵ��Ŀ��ֵ�� - ���ƶ���ĵ�ǰ��״ֵ̬
    //
    //����P : e(k)
    //
    //����I : ��e(i)     �����ۼ�(����e(k))
    //
    //΢��D : e(k) - e(k - 1)  ������ - �ϴ����

    typedef struct PID_Process
    {
        PID_TYPE bias; //�����
        PID_TYPE integral_bias;//i���
        PID_TYPE differential_bias;//d���
        PID_TYPE lpf_differential_bias;//�ϴ�d���

        PID_TYPE feedforward; //ǰ��ֵ
        PID_TYPE predict;  //Ԥ��ֵ

        PID_TYPE last_target; //�ϴ�����ֵ
        PID_TYPE last_bias; //�ϴ����
        PID_TYPE lastlast_bias; //���ϴ����

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
        //PID_T* next_pid;//����ʵ��pid�Ĳ��л򴮼�
        void(*handle)(PID_T* pid, PID_TYPE cycle);
        void(*i_handle)(PID_T* pid);
        void(*d_handle)(PID_T* pid);
        void(*variable)(PID_T* pid);//���ٻ���
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
 * @description: ����Ŀ��ֵ/����ֵ���ֵ
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Target_Limit(PID_T* pid, PID_TYPE value)
{
    pid->parameter.target_limit = value;
}
/**
 * @description: ����������ֵ
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Bias_Limit(PID_T* pid, PID_TYPE value)
{
    pid->parameter.bias_limit = value;
}
/**
 * @description: ���������Ӧ����
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Bias_Dead_Zone(PID_T* pid, PID_TYPE value)
{
    pid->parameter.bias_dead_zone = value;
}

/**
 * @description: ���ƻ��������ֵ
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void PID_Sst_Integral_Limit(PID_T* pid, PID_TYPE value);
/**
 * @description: ����pid������ֵ
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Out_Limit(PID_T* pid, PID_TYPE value)
{
    pid->parameter.out_limit = value;
}
/**
 * @description: ����ǰ��ϵ��
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Feedforward(PID_T* pid, PID_TYPE value)
{
    pid->parameter.kf = value;
}
/**
 * @description: ����Ԥ��ϵ��
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_KPre(PID_T* pid, PID_TYPE value)
{
    pid->parameter.k_pre = value;
}
/**
 * @description: ����Ŀ��ֵ/����ֵ
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Target(PID_T* pid, PID_TYPE value)
{
    pid->parameter.target = value;
}
/**
 * @description: ���õ�ǰֵ/����ֵ
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Present(PID_T* pid, PID_TYPE value)
{
    pid->parameter.present = value;
}
/**
 * @description: ����Ԥ��ϵ��
 * @param {PID_T*} pid ʵ�����
 * @param {PID_TYPE} value
 * @return {*}
 */
void inline PID_Sst_Predict(PID_T* pid, PID_TYPE value)
{
    pid->parameter.predict = value;
}



#define PID_I_NORMAL(pid)               PID_SET_I_Function(pid,PID_INTEGRAL_NORMAL)//��ͨ����
#define PID_I_SEPARATION(pid,value)	    PID_SET_I_Function(pid,PID_INTEGRAL_SEPARATION,value)//���ַ���
#define PID_I_SATURATION(pid,value)	    PID_SET_I_Function(pid,PID_INTEGRAL_SATURATION,value)//�����ͻ���
#define PID_I_SPEED(pid,func)		    PID_SET_I_Function(pid,PID_INTEGRAL_SPEED,func)//���ٻ���
#define PID_I_TRAPEZIOD(pid)	        PID_SET_I_Function(pid,PID_INTEGRAL_TRAPEZIOD)//���λ���

#define PID_D_COMPLETE(pid)             PID_SET_D_Function(pid,PID_DIFFERENTIAL_COMPLETE)//��ȫ΢��
#define PID_D_PART(pid,value)           PID_SET_D_Function(pid,PID_DIFFERENTIAL_PART,value)//����ȫ΢��
#define PID_D_PREVIOUS(pid,value)       PID_SET_D_Function(pid,PID_DIFFERENTIAL_PREVIOUS,value)//΢������

#ifdef __cplusplus
}
#endif
#endif


