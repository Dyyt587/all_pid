// PID automated tuning (Ziegler-Nichols/relay method) for Arduino and compatible boards
// Copyright (c) 2016-2020 jackw01
// This code is distrubuted under the MIT License, see LICENSE for details

#include "apid_auto_tune_ZNmode.h"
#include "apid.h"

/*
	Set the target value to tune to
	This will depend on what you are tuning. This should be set to a value within
	the usual range of the setpoint. For low-inertia systems, values at the lower
	end of this range usually give better results. For anything else, start with a
	value at the middle of the range.
*/

static float tune_pid(apid_auto_tune_ZNmode_t *tuner, float input, unsigned long cycle);
void apid_auto_tune_ZNmode_deinit(apid_t *pid)
{
	pid->auto_pid = 0;
	pid->auto_pid_handler = 0;
}

void __auto_pid_handle1(apid_t *pid, PID_TYPE cycle)
{
	// 覆盖输出
	// APID_(pid);
	apid_auto_tune_ZNmode_t *auto_pid = (apid_auto_tune_ZNmode_t *)pid->auto_pid;
	// auto_pid->loopInterval = cycle;
	pid->parameter.out = tune_pid(pid->auto_pid, APID_Get_Present(pid), cycle);
	// LOG_D(":%f",pid->parameter.out);
	if (isFinished(auto_pid))
	{
		apid_auto_tune_ZNmode_deinit(pid);
		APID_Enable(pid);
		//LOG_D("kp=%f ki=%f kd=%f", auto_pid->kp, auto_pid->ki, auto_pid->kd);
	}
}

void apid_auto_tune_ZNmode_init(apid_t *pid, apid_auto_tune_ZNmode_t *tuner,ZNMode mode, float targetValue, float low_out_range, float up_out_range, int tuneCycles)
{
	pid->auto_pid = tuner;
	pid->auto_pid_handler = __auto_pid_handle1;

	tuner->target_value = targetValue;

	tuner->znMode = mode;
	tuner->cycles = tuneCycles;
	tuner->microseconds = 0;
	setOutputRange(tuner, low_out_range, up_out_range);
	start_tune_loop(tuner);
}

// Set target input for tuning
void setTargetInputValue(apid_auto_tune_ZNmode_t *tuner, float target)
{
	tuner->target_value = target;
}

// Set output range
void setOutputRange(apid_auto_tune_ZNmode_t *tuner, float min, float max)
{
	tuner->minOutput = min;
	tuner->maxOutput = max;
}

// Set Ziegler-Nichols tuning mode
void set_zn_mode(apid_auto_tune_ZNmode_t *tuner, ZNMode zn)
{
	tuner->znMode = zn;
}

// Initialize all variables before loop
void start_tune_loop(apid_auto_tune_ZNmode_t *tuner)
{
	tuner->i = 0;		  // Cycle counter
	tuner->output = true; // Current output state
	tuner->outputValue = tuner->maxOutput;
	tuner->t1 = tuner->t2 = 0;							  // Times used for calculating period
	tuner->microseconds = tuner->tHigh = tuner->tLow = 0; // More time variables
	tuner->pAverage = tuner->iAverage = tuner->dAverage = 0;
}

// Run one cycle of the loop
float tune_pid(apid_auto_tune_ZNmode_t *tuner, float input, unsigned long cycle)
{
	// Useful information on the algorithm used (Ziegler-Nichols method/Relay method)
	// http://www.processcontrolstuff.net/wp-content/uploads/2015/02/relay_autot-2.pdf
	// https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method
	// https://www.cds.caltech.edu/~murray/courses/cds101/fa04/caltech/am04_ch8-3nov04.pdf

	// Basic explanation of how this works:
	//  * Turn on the output of the PID controller to full power
	//  * Wait for the output of the system being tuned to reach the target input value
	//      and then turn the controller output off
	//  * Wait for the output of the system being tuned to decrease below the target input
	//      value and turn the controller output back on
	//  * Do this a lot
	//  * Calculate the ultimate gain using the amplitude of the controller output and
	//      system output
	//  * Use this and the period of oscillation to calculate PID gains using the
	//      Ziegler-Nichols method

	// Calculate time delta
	// long prevMicroseconds = microseconds;

	tuner->microseconds += cycle;
	// float deltaT = microseconds - prevMicroseconds;

	// Calculate max and min
	tuner->max = (tuner->max > input) ? tuner->max : input;
	tuner->min = (tuner->min < input) ? tuner->min : input;

	// Output is on and input signal has risen to target
	if (tuner->output && input > tuner->target_value)
	{
		// Turn output off, record current time as t1, calculate tHigh, and reset maximum
		tuner->output = false;
		tuner->outputValue = tuner->minOutput;
		tuner->t1 = tuner->microseconds;
		tuner->tHigh = tuner->t1 - tuner->t2;
		tuner->max = tuner->target_value;
	}

	// Output is off and input signal has dropped to target
	if (!tuner->output && input < tuner->target_value)
	{
		// Turn output on, record current time as t2, calculate tLow
		tuner->output = true;
		tuner->outputValue = tuner->maxOutput;
		tuner->t2 = tuner->microseconds;
		tuner->tLow = tuner->t2 - tuner->t1;

		// Calculate Ku (ultimate gain)
		// Formula given is Ku = 4d / πa
		// d is the amplitude of the output signal
		// a is the amplitude of the input signal
		// float ku = (4.0 * ((tuner->maxOutput - tuner->minOutput) / 2.0)) / (M_PI * (tuner->max - tuner->min) / 2.0);
		float ku = (4.0 * ((tuner->maxOutput - tuner->minOutput))) / (M_PI * (tuner->max - tuner->min));

		// Calculate Tu (period of output oscillations)
		float tu = tuner->tLow + tuner->tHigh;

		// How gains are calculated
		// PID control algorithm needs Kp, Ki, and Kd
		// Ziegler-Nichols tuning method gives Kp, Ti, and Td
		//
		// Kp = 0.6Ku = Kc
		// Ti = 0.5Tu = Kc/Ki
		// Td = 0.125Tu = Kd/Kc
		//
		// Solving these equations for Kp, Ki, and Kd gives this:
		//
		// Kp = 0.6Ku
		// Ki = Kp / (0.5Tu)
		// Kd = 0.125 * Kp * Tu

		// Constants
		// https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method

		float kpConstant, tiConstant, tdConstant;
		if (tuner->znMode == ZNModeBasicPID)
		{
			kpConstant = 0.6;
			tiConstant = 0.5;
			tdConstant = 0.125;
		}
		else if (tuner->znMode == ZNModeLessOvershoot)
		{
			kpConstant = 0.33;
			tiConstant = 0.5;
			tdConstant = 0.33;
		}
		else
		{ // Default to No Overshoot mode as it is the safest
			kpConstant = 0.2;
			tiConstant = 0.5;
			tdConstant = 0.33;
		}

		// Calculate gains
		tuner->kp = kpConstant * ku;
		// tuner->ki = (tuner->kp / (tiConstant * tu)) / cycle;
		// tuner->kd = (tdConstant * tuner->kp * tu) * cycle;

		tuner->ki = (tuner->kp / (tiConstant * tu));
		tuner->kd = (tdConstant * tuner->kp * tu);

		// Average all gains after the first two cycles
		if (tuner->i > 1)
		{
			tuner->pAverage += tuner->kp;
			tuner->iAverage += tuner->ki;
			tuner->dAverage += tuner->kd;
		}

		// Reset minimum
		tuner->min = tuner->target_value;

		// Increment cycle count
		tuner->i++;
	}

	// If loop is done, disable output and calculate averages
	if (tuner->i >= tuner->cycles)
	{
		tuner->output = false;
		tuner->outputValue = tuner->minOutput;
		tuner->kp = tuner->pAverage / (tuner->i - 1);
		tuner->ki = tuner->iAverage / (tuner->i - 1);
		tuner->kd = tuner->dAverage / (tuner->i - 1);
	}
	if (tuner->output)
	{
		return tuner->outputValue;
	}
	else
	{
		return 0;
	}
}

// Get PID constants after tuning
float getKp(apid_auto_tune_ZNmode_t *tuner) { return tuner->kp; };
float getKi(apid_auto_tune_ZNmode_t *tuner) { return tuner->ki; };
float getKd(apid_auto_tune_ZNmode_t *tuner) { return tuner->kd; };

// Is the tuning loop finished?
bool isFinished(apid_auto_tune_ZNmode_t *tuner)
{
	return (tuner->i >= tuner->cycles);
}

// return number of tuning cycle
int getCycle(apid_auto_tune_ZNmode_t *tuner)
{
	return tuner->i;
}