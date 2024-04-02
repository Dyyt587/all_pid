#include "apid.h"
#if APID_USING_FUZZY
#include "apid_ex_fuzzy.h"

#define NB -6
#define NM -4
#define NS -2
#define ZO 0
#define PS 2
#define PM 4
#define PB 6
static int ruleKp[7][7] = {{PB, PB, PM, PM, PS, ZO, ZO}, // kp规则表
                           {PB, PB, PM, PS, PS, ZO, NS},
                           {PM, PM, PM, PS, ZO, NS, NS},
                           {PM, PM, PS, ZO, NS, NM, NM},
                           {PS, PS, ZO, NS, NS, NM, NM},
                           {PS, ZO, NS, NM, NM, NM, NB},
                           {ZO, ZO, NM, NM, NM, NB, NB}};

static int ruleKi[7][7] = {{NB, NB, NM, NM, NS, ZO, ZO}, // ki规则表
                           {NB, NB, NM, NS, NS, ZO, ZO},
                           {NB, NM, NS, NS, ZO, PS, PS},
                           {NM, NM, NS, ZO, PS, PM, PM},
                           {NM, NS, ZO, PS, PS, PM, PB},
                           {ZO, ZO, PS, PS, PM, PB, PB},
                           {ZO, ZO, PS, PM, PM, PB, PB}};

static int ruleKd[7][7] = {{PS, NS, NB, NB, NB, NM, PS}, // kd规则表
                           {PS, NS, NB, NM, NM, NS, ZO},
                           {ZO, NS, NM, NM, NS, NS, ZO},
                           {ZO, NS, NS, NS, NS, NS, ZO},
                           {ZO, ZO, ZO, ZO, ZO, ZO, ZO},
                           {PB, NS, PS, PS, PS, PS, PB},
                           {PB, PM, PM, PM, PS, PS, PB}};

/*线性量化操作函数,论域{-6，-5，-4，-3，-2，-1，0，1，2，3，4，5，6}*/
/**
 * @brief
 *
 * @param ctrl
 * @param qValue 量化结果[0]偏差[1]偏差增量
 */
static void Linear_quantization(apid_fuzzy_ctrl_t *ctrl, PID_TYPE *qValue)
{
    apid_t *pid = ctrl->pid;

    float thisError;
    float deltaError;
    thisError = pid->process.bias;                    /* 获取当前偏差值 */
    deltaError = pid->process.last_bias / pid->cycle; /* 获取偏差值增量 */

    qValue[0] = 6.0 * thisError / (ctrl->maximum - ctrl->minimum);
    qValue[1] = 3.0 * deltaError / (ctrl->maximum - ctrl->minimum);
}
/**
 * @brief 反模糊化
 *
 * @param ctrl
 * @param max_kp
 * @param qValueK
 */
PID_TYPE Linear_realization(PID_TYPE max_kp, PID_TYPE min_kp, PID_TYPE qValueK)
{
    return max_kp;
}
/**
 * @brief 隶属度计算函数
 *
 * @param msp 隶属度
 * @param qv 目标值
 * @param index 隶属度在模糊表中的索引
 */
static void Calc_membership(float *msp, float qv, int *index)
{

    if ((qv >= -NB) && (qv < -NM))
    {
        index[0] = 0;
        index[1] = 1;
        msp[0] = -0.5 * qv - 2.0; // y=-0.5x-2.0
        msp[1] = 0.5 * qv + 3.0;  // y=0.5x+3.0
    }
    else if ((qv >= -NM) && (qv < -NS))
    {
        index[0] = 1;
        index[1] = 2;
        msp[0] = -0.5 * qv - 1.0; // y=-0.5x-1.0
        msp[1] = 0.5 * qv + 2.0;  // y=0.5x+2.0
    }
    else if ((qv >= -NS) && (qv < ZO))
    {
        index[0] = 2;
        index[1] = 3;
        msp[0] = -0.5 * qv;      // y=-0.5x
        msp[1] = 0.5 * qv + 1.0; // y=0.5x+1.0
    }
    else if ((qv >= ZO) && (qv < PS))
    {
        index[0] = 3;
        index[1] = 4;
        msp[0] = -0.5 * qv + 1.0; // y=-0.5x+1.0
        msp[1] = 0.5 * qv;        // y=0.5x
    }
    else if ((qv >= PS) && (qv < PM))
    {
        index[0] = 4;
        index[1] = 5;
        msp[0] = -0.5 * qv + 2.0; // y=-0.5x+2.0
        msp[1] = 0.5 * qv - 1.0;  // y=0.5x-1.0
    }
    else if ((qv >= PM) && (qv <= PB))
    {
        index[0] = 5;
        index[1] = 6;
        msp[0] = -0.5 * qv + 3.0; // y=-0.5x+3.0
        msp[1] = 0.5 * qv - 2.0;  // y=0.5x-2.0
    }
}

/**
 * @brief 解模糊化操作,根据具体的量化函数和隶属度函数调整
 *
 * @param ctrl
 * @param deltaK
 */
static void Fuzzy_computation(apid_fuzzy_ctrl_t *ctrl)
{
    float qValue[2] = {0, 0};   // 偏差及其增量的量化值
    uint8_t indexE[2] = {0, 0}; // 偏差隶属度索引
    float msE[2] = {0, 0};      // 偏差隶属度

    uint8_t indexEC[2] = {0, 0}; // 偏差增量隶属度索引
    float msEC[2] = {0, 0};      // 偏差增量隶属度

    float qValueK[3];
    float deltaK[3] = {0};
    ctrl->linear_quantization(ctrl, qValue);

    ctrl->calc_membership(msE, qValue[0], indexE);
    ctrl->calc_membership(msEC, qValue[1], indexEC);

    qValueK[0] = msE[0] * (msEC[0] * ruleKp[indexE[0]][indexEC[0]] + msEC[1] * ruleKp[indexE[0]][indexEC[1]]) + msE[1] * (msEC[0] * ruleKp[indexE[1]][indexEC[0]] + msEC[1] * ruleKp[indexE[1]][indexEC[1]]);

    qValueK[1] = msE[0] * (msEC[0] * ruleKi[indexE[0]][indexEC[0]] + msEC[1] * ruleKi[indexE[0]][indexEC[1]]) + msE[1] * (msEC[0] * ruleKi[indexE[1]][indexEC[0]] + msEC[1] * ruleKi[indexE[1]][indexEC[1]]);

    qValueK[2] = msE[0] * (msEC[0] * ruleKd[indexE[0]][indexEC[0]] + msEC[1] * ruleKd[indexE[0]][indexEC[1]]) + msE[1] * (msEC[0] * ruleKd[indexE[1]][indexEC[0]] + msEC[1] * ruleKd[indexE[1]][indexEC[1]]);

    deltaK[0] = ctrl->linear_realization(ctrl->maxdKp, ctrl->mindKp, qValueK[0]);
    deltaK[1] = ctrl->linear_realization(ctrl->maxdKi, ctrl->mindKi, qValueK[1]);
    deltaK[2] = ctrl->linear_realization(ctrl->maxdKd, ctrl->mindKd, qValueK[2]);

    ctrl->pid->parameter.kp += deltaK[0];
    ctrl->pid->parameter.ki += deltaK[1];
    ctrl->pid->parameter.kd += deltaK[2];
}

typedef struct
{
    apid_t *pid;
    float maximum; /*最大输出限幅值*/
    float minimum; /*最小输出限幅值*/

    float maxdKp; /*Kp增量的最大限值*/
    float mindKp; /*Kp增量的最小限值*/
    float qKp;    /*Kp增量的影响系数*/

    float maxdKi; /*Ki增量的最大限值*/
    float mindKi; /*Ki增量的最小限值*/
    float qKi;    /*Ki增量的影响系数*/

    float maxdKd; /*Kd增量的最大限值*/
    float mindKd; /*Kd增量的最小限值*/
    float qKd;    /*Kd增量的影响系数*/
} APID_Fuzzy_Init_t;
/**
 * @brief 初始化模糊控制,使用默认的函数
 *
 * @param ctrl
 * @param pid
 */
void APID_Fuzzy_Fast_Init(apid_fuzzy_ctrl_t *ctrl, APID_Fuzzy_Init_t *init,
 PID_TYPE (*linear_realization)(PID_TYPE max_kp, PID_TYPE min_kp, PID_TYPE qValueK))
{
    ctrl->pid = init->pid;
    ctrl->linear_quantization = Linear_quantization;
    ctrl->calc_membership = Calc_membership;
    ctrl->linear_realization = linear_realization;
    ctrl->pid->user_hook_pre_cplt =Fuzzy_computation;
}

#endif
