#ifndef APID_CPP_H
#define APID_CPP_H

#include "apid_cfg.h"
#include "apid.h"

class PIDController
{
private:
    apid_t *pid; // 保持对C语言PID结构的指针
public:
    // 使用C++构造函数初始化PID控制器
    PIDController(ALL_PID_Mode mode, PID_TYPE kp, PID_TYPE ki, PID_TYPE kd)
        : pid(APID_CREATE_STATIC_ANONYMOUS(mode, kp, ki, kd))
    {
        APID_Init(pid, mode, kp, ki, kd);
    }

    // 析构函数，如果有资源需要释放，可以在这里处理
    ~PIDController()
    {
        // 通常C语言的资源管理需要手动处理，如果有的话
    }

    // 设置目标值
    void setTarget(PID_TYPE target)
    {
        APID_Set_Target(pid, target);
    }

    // 获取输出值
    PID_TYPE getOutput()
    {
        return APID_Get_Out(pid);
    }

    // 其他接口根据需要添加...
    void set_target(PID_TYPE target)
    {
        APID_Set_Target(pid, target);
    }
    void set_present(PID_TYPE present)
    {
        APID_Set_Present(pid, present);
    }
    void set_predict(PID_TYPE prediction)
    {
        APID_Set_Predict(pid, prediction);
    }


    void set_target_limit(PID_TYPE target_limit)
    {
        APID_Set_Target_Limit(pid, target_limit);
    }
    void set_bias_limit(PID_TYPE bias_limit)
    {
        APID_Set_Bias_Limit(pid, bias_limit);
    }
    void set_base_dead_zone(PID_TYPE base_cycle)
    {
        APID_Set_Bias_Dead_Zone(pid, base_cycle);
    }
    void set_integral_limit(PID_TYPE integral_limit)
    {
        APID_Set_Integral_Limit(pid, integral_limit);
    }
    void set_out_limit(PID_TYPE out_limit)
    {
        APID_Set_Out_Limit(pid, out_limit);
    }
    void set_feedforward(PID_TYPE feedforward)
    {
        APID_Set_Feedforward(pid, feedforward);
    }
    void set_kpre(PID_TYPE kpre)
    {
        APID_Set_KPre(pid, kpre);
    }



    void handler(PID_TYPE cycle)
    {
        APID_Hander(pid,cycle);
    }
    PID_TYPE get_out(void)
    {
        return APID_Get_Out(pid);
    }
};

#endif
