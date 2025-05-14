/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-23 08:10:27
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-11-08 20:22:04
 * @FilePath: \arduino-pid-autotuner\apid_auto_tune_ZNmode.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef APID_AUTO_TUNE_ZNMODE_H
#define APID_AUTO_TUNE_ZNMODE_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdbool.h>
#include "apid.h"
#ifndef M_PI
#define M_PI (3.14159265358979323846f)     // 圆周率
#endif
//typedef struct _PID_T apid_t;
    typedef enum
    {
        ZNModeBasicPID,
        ZNModeLessOvershoot,
        ZNModeNoOvershoot
    } ZNMode;

    typedef struct apid_auto_tune_ZNmode
    {

        PID_TYPE target_value;
        PID_TYPE minOutput, maxOutput;
        ZNMode znMode;
        int cycles;

        // See start_tune_loop()
        int i;
        bool output;
        PID_TYPE outputValue;
        long microseconds, t1, t2, tHigh, tLow;
        PID_TYPE max, min;
        PID_TYPE pAverage, iAverage, dAverage;

        PID_TYPE kp, ki, kd;
    } apid_auto_tune_ZNmode_t;

    // Constants for Ziegler-Nichols tuning mode

void apid_auto_tune_ZNmode_init(apid_t *pid, apid_auto_tune_ZNmode_t *tuner,ZNMode mode, 
                                PID_TYPE targetValue, PID_TYPE low_out_range, PID_TYPE up_out_range, int tuneCycles);

    void setTargetInputValue(apid_auto_tune_ZNmode_t *tuner, PID_TYPE target);
    void setLoopInterval(apid_auto_tune_ZNmode_t *tuner, long interval);
    void setOutputRange(apid_auto_tune_ZNmode_t *tuner, PID_TYPE min, PID_TYPE max);
    void set_zn_mode(apid_auto_tune_ZNmode_t *tuner, ZNMode zn);
    void setTuningCycles(apid_auto_tune_ZNmode_t *tuner, int tuneCycles);

    void start_tune_loop(apid_auto_tune_ZNmode_t *tuner);

    // PID_TYPE tune_pid(apid_auto_tune_ZNmode_t *tuner, PID_TYPE input, unsigned long us);

    // Get results of most recent tuning
    PID_TYPE getKp(apid_auto_tune_ZNmode_t *tuner);
    PID_TYPE getKi(apid_auto_tune_ZNmode_t *tuner);
    PID_TYPE getKd(apid_auto_tune_ZNmode_t *tuner);

    bool isFinished(apid_auto_tune_ZNmode_t *tuner); // Is the tuning finished?

    int getCycle(apid_auto_tune_ZNmode_t *tuner); // return tuning cycle

#ifdef __cplusplus
}
#endif
#endif
