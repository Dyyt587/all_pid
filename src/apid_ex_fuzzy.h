/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-02 09:03:54
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-11-08 20:21:45
 * @FilePath: \all_pid\src\apid_ex_fuzzy.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef APID_EX_FUZZY_H_
#define APID_EX_FUZZY_H_
#ifdef __cplusplus
extern "C" {
#endif
#if APID_USING_FUZZY
//typedef struct _PID_T apid_t;

#include "apid.h"

typedef struct
{
    PID_TYPE maximum; /*最大输出限幅值*/
    PID_TYPE minimum; /*最小输出限幅值*/

    PID_TYPE maxdKp; /*Kp增量的最大限值*/
    PID_TYPE mindKp; /*Kp增量的最小限值*/
    PID_TYPE qKp;    /*Kp增量的影响系数*/

    PID_TYPE maxdKi; /*Ki增量的最大限值*/
    PID_TYPE mindKi; /*Ki增量的最小限值*/
    PID_TYPE qKi;    /*Ki增量的影响系数*/

    PID_TYPE maxdKd; /*Kd增量的最大限值*/
    PID_TYPE mindKd; /*Kd增量的最小限值*/
    PID_TYPE qKd;    /*Kd增量的影响系数*/
} APID_Fuzzy_Init_t;

typedef struct apid_fuzzy_ctrl apid_fuzzy_ctrl_t;
struct apid_fuzzy_ctrl
{
    void (*linear_quantization)(apid_fuzzy_ctrl_t *ctrl, PID_TYPE thisError, PID_TYPE deltaError,PID_TYPE *qValue);
    void (*calc_membership)(PID_TYPE *msp, PID_TYPE qv, uint8_t *index);
    PID_TYPE(*linear_realization)(PID_TYPE max_kp, PID_TYPE min_kp, PID_TYPE qValueK);

    //apid_t *pid;

    PID_TYPE maximum;/*最大输出限幅值*/
    PID_TYPE minimum;/*最小输出限幅值*/

    PID_TYPE maxdKp; /*Kp增量的最大限值*/
    PID_TYPE mindKp; /*Kp增量的最小限值*/
    PID_TYPE qKp;    /*Kp增量的影响系数*/

    PID_TYPE maxdKi; /*Ki增量的最大限值*/
    PID_TYPE mindKi; /*Ki增量的最小限值*/
    PID_TYPE qKi;    /*Ki增量的影响系数*/

    PID_TYPE maxdKd; /*Kd增量的最大限值*/
    PID_TYPE mindKd; /*Kd增量的最小限值*/
    PID_TYPE qKd;    /*Kd增量的影响系数*/
};
void APID_Fuzzy_Fast_Init(apid_t *pid, APID_Fuzzy_Init_t *init);

#endif
#ifdef __cplusplus
}
#endif
#endif
