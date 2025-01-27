/*
 * @Author: dyyt 805207319@qq.com
 * @Date: 2023-07-19 11:36:27
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-01 10:42:59
 * @FilePath: \undefinedc:\Users\LENOVO\Documents\programs\PID\VS_Project\ConsoleApplication1\ConsoleApplication1\pid_exfunc.c
 * @Description: pid库
 */
#include "apid.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


var_list_t var_list;
var_list_t* _var_begin = 0;

#define var_logInfo(...) printf(__VA_ARGS__);
#define __NAME(restrc,x,name) strcpy(name,#x)
#define VAR_MALLOC(x) malloc(x)

extern void i_handle_Increment_Normal(apid_t *pid);
extern void i_handle_Increment_Separation(apid_t *pid);
extern void i_handle_Increment_Saturation(apid_t *pid);
extern void i_handle_Increment_Speed(apid_t *pid);
extern void i_handle_Increment_Trapeziod(apid_t *pid);

extern void d_handle_Increment_Complete(apid_t *pid);
extern void d_handle_Increment_Part(apid_t *pid);
extern void d_handle_Increment_Previous(apid_t *pid);

extern void d_handle_Position_Complete(apid_t *pid);
extern void d_handle_Position_Part(apid_t *pid);
extern void d_handle_Position_Previous(apid_t *pid);

extern void i_handle_Position_Normal(apid_t *pid);
extern void i_handle_Position_Separation(apid_t *pid);
extern void i_handle_Position_Saturation(apid_t *pid);
extern void i_handle_Position_Speed(apid_t *pid);
extern void i_handle_Position_Trapeziod(apid_t *pid);

/*
TODO:   实现变速积分的用户自定义
        实现前馈方程的自定义,同时保留kf参数
        实现输出滤波集成功能，包括滑膜滤波/用户自定义滤波
        完善宏配置项，由用户决定支持哪些模式以减小编译体积
        集成调试功能，与vofa对接，打图像(支持任意参数)，添加name属性
        集成命令功能
        集成自动调参
        完善asssert功能

*/

/**
 * @description: 限制积分项最大值
 * @param {apid_t*} pid 实例句柄
 * @param {PID_TYPE} value
 * @return {*}
 */
void APID_Set_Integral_Limit(apid_t *pid, PID_TYPE value)
{
    if (pid->flag.integral_way != PID_INTEGRAL_SATURATION)
    {
        APID_SET_I_Function( PID_INTEGRAL_SATURATION,pid, value); // 抗饱和积分
    }
    else
    {
        pid->parameter.integral_limit = value;
    }
}
/**
 * @description: 设置pid的积分模式(默认为普通模式)
 * @param {apid_t*} pid 实例句柄
 * @param {ALL_PID_I_Function} imode 支持的积分模式
 * @return {*}
 * @note 根据具不同的功能，可能要传入一个数或函数指针做特别的初始化
 *       PID_INTEGRAL_SEPARATION:pid->parameter.bias_for_integral //数
 *       PID_INTEGRAL_SATURATION:pid->parameter.integral_limit //数
 *       PID_INTEGRAL_SPEED:pid->variable //函数
 */
void APID_SET_I_Function( ALL_PID_I_Function imode,apid_t *pid, ...)
{
    va_list ap; // 声明一个va_list变量
	  va_start(ap, pid); // 初始化，第二个参数为最后一个确定的形参      
    pid->flag.integral_way = imode;
    switch (imode)
    {
    case PID_INTEGRAL_NORMAL:
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->i_handle = i_handle_Increment_Normal;
        }
        else
        {
            pid->i_handle = i_handle_Position_Normal;
        }
        break;
    case PID_INTEGRAL_SEPARATION:
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->i_handle = i_handle_Increment_Separation;
        }
        else
        {
            pid->i_handle = i_handle_Position_Separation;
        }
#if (PID_TYPE == float)
        pid->parameter.bias_for_integral = va_arg(ap, double);
#elif (PID_TYPE == double)
        pid->parameter.bias_for_integral = va_arg(ap, PID_TYPE);
#else
        pid->parameter.bias_for_integral = (PID_TYPE)va_arg(ap, int);
#endif
        break;
    case PID_INTEGRAL_SATURATION:
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->i_handle = i_handle_Increment_Saturation;
        }
        else
        {
            pid->i_handle = i_handle_Position_Saturation;
        }
#if (PID_TYPE == float)
        pid->parameter.integral_limit = va_arg(ap, double);
#elif (PID_TYPE == double)
        pid->parameter.integral_limit = va_arg(ap, PID_TYPE);
#else
        pid->parameter.integral_limit = (PID_TYPE)va_arg(ap, int);
#endif
        break;
    case PID_INTEGRAL_SPEED:

        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->i_handle = i_handle_Increment_Speed;
        }
        else
        {
            pid->i_handle = i_handle_Position_Speed;
        }
        pid->variable = va_arg(ap, void *);
        break;
    case PID_INTEGRAL_TRAPEZIOD:
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->i_handle = i_handle_Increment_Trapeziod;
        }
        else
        {
            pid->i_handle = i_handle_Position_Trapeziod;
        }

        break;
    default:
        break;
    }
		        va_end(ap);

}

/**
 * @description: 设置pid的微分模式(默认为普通模式)
 * @param {apid_t*} pid 实例句柄
 * @param {ALL_PID_D_Function} dmode
 * @return {*}
 * @note 更具不同的功能，可能要传入一个参数做特别的初始化float 或函数指针
 *          PID_DIFFERENTIAL_COMPLETE:	//完全微分
 *          PID_DIFFERENTIAL_PART:pid->parameter.kd_lpf
 *          PID_DIFFERENTIAL_PREVIOUS:pid->parameter.kd_pre
 */
void APID_SET_D_Function( ALL_PID_D_Function dmode,apid_t *pid, ...)
{
    va_list ap; // 声明一个va_list变量
  	va_start(ap, pid);    // 初始化，第二个参数为最后一个确定的形参

    pid->flag.differential_way = dmode;
    switch (dmode)
    {
    case PID_DIFFERENTIAL_COMPLETE: // 完全微分
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->d_handle = d_handle_Increment_Complete;
        }
        else
        {
            pid->d_handle = d_handle_Position_Complete;
        }
        break;
    case PID_DIFFERENTIAL_PART: // 不完全微分
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->d_handle = d_handle_Increment_Part;
        }
        else
        {
            pid->d_handle = d_handle_Position_Part;
        }
#if (PID_TYPE == float)
        pid->parameter.kd_.kd_lpf = va_arg(ap, double);
#elif (PID_TYPE == double)
        pid->parameter.kd_.kd_lpf = va_arg(ap, PID_TYPE);
#else
        pid->parameter.kd_.kd_lpf = (PID_TYPE)va_arg(ap, int);
#endif
        break;
    case PID_DIFFERENTIAL_PREVIOUS: // 微分先行
        if (pid->flag.pid_mode == PID_INCREMENT)
        {
            pid->d_handle = d_handle_Increment_Previous;
        }
        else
        {
            pid->d_handle = d_handle_Position_Previous;
        }
#if (PID_TYPE == float)
        pid->parameter.kd_.kd_pre = va_arg(ap, double);
#elif (PID_TYPE == double)
        pid->parameter.kd_.kd_pre = va_arg(ap, PID_TYPE);
#else
        pid->parameter.kd_.kd_pre = (PID_TYPE)va_arg(ap, int);
#endif
        break;
    default:
        break;
    }
		va_end(ap);

}



//参数一命令
//参数2 变量名
//参数3 变量值//可选
void var(int argc, char** argv)
{
    var_list_t* _var = _var_begin;
    var_list_t* _var_second=0;
    int is_assign=0;
    //var_logInfo("var enter\r\n",0);
    if(argc==1)
    {
        var_logInfo("\r\n");
       while (_var)
       {
                var_logInfo("\t-- %s \r\n", _var->name);
              _var = _var->next;

       }
       return;

    }

    if (argc > 4) {
        var_logInfo("paramer error\r\n");
        return;
    }

    if(0==strcmp(argv[1], "all")){

        while (_var)
        {
            var_logInfo("\t--%s \r\n", _var->name);
            _var = _var->next;

        }
        return;
    }

    while (0!=strcmp(argv[1], _var->name))
    {

        _var = _var->next;
        if (_var == 0)
        {
            var_logInfo("We don't have that variable here\r\n");
            return;
        }//未找到变量
    }//已经找到变量
    if(argc==4){

        if(strcmp(argv[2],"=")==0){
                if(argc==4){
                    _var_second=_var_begin;
                    while (0!=strcmp(argv[3], _var_second->name))
                    {

                        _var_second = _var_second->next;
                        if (_var_second == 0)
                        {
                            var_logInfo("There's no variable here(var tow)\r\n");
                            return;
                        }//未找到变量
                    }//已经找到变量
                    is_assign=1;
                }

            }
    }else is_assign=0;



    switch (_var->type)
    {

    case _8:
        if(argc == 3 && is_assign==0){
            *(char*)_var->body = (char)atoi(argv[2]);
        }else if(is_assign==1){
            *(char*)_var->body = *(char*)_var_second->body;
        }
        var_logInfo("%s is %d\r\n", _var->name, *(char*)_var->body);
        break;
    //case _8:
    //    var_logInfo("%s is %d", _var->name, *(unsigned char*)_var->body);

    //    break;
    case _16:
        if(argc == 3&& is_assign==0){
            *(short*)_var->body = (short)atoi(argv[2]);
        }else if(is_assign==1){
            *(short*)_var->body = *(short*)_var_second->body;
        }
        var_logInfo("%s is %d\r\n", _var->name, *(short*)_var->body);

        break;
    //case _u16:
    //    var_logInfo("%s is %d", _var->name, *( unsigned short*)_var->body);

    //    break;
    case _32:
        if(argc == 3&& is_assign==0){
            *(int*)_var->body = atoi(argv[2]);
        }else if(is_assign==1){
            //var_logInfo("test\r\n",0);

            *(int*)_var->body = *(int*)_var_second->body;
        }
        var_logInfo("%s is %d\r\n", _var->name, *(int*)_var->body);

        break;
    //case _u32:
    //    var_logInfo("%s is %d", _var->name, *(unsigned int*)_var->body);
    //    break;
    case _64:
        if(argc == 3&& is_assign==0){
            *(long long*)_var->body = atoi(argv[2]);
        }else if(is_assign==1){
            *(long long*)_var->body = *(long long*)_var_second->body;
        }
        var_logInfo("%s is %d\r\n", _var->name, *(int32_t*)_var->body);//由于使用microlib所以只能输出32位字符输出

        break;
    //case _u64:
    //    var_logInfo("%s is %d", _var->name, *(unsigned long long*)_var->body);

    //    break;
    case _f:
        if(argc == 3&& is_assign==0){
            *(float*)_var->body = (float)atof(argv[2]);
        }else if(is_assign==1){
            *(float*)_var->body = *(float*)_var_second->body;
        }
        var_logInfo("%s is %f\r\n", _var->name, *(float*)_var->body);

        break;
    case _lf:
        if(argc == 3&& is_assign==0){
            *(double*)_var->body = atof(argv[2]);
        }else if(is_assign==1){
            *(double*)_var->body = *(double*)_var_second->body;
        }
        var_logInfo("%s is %lf\r\n", _var->name, *(double*)_var->body);

        break;
    default:
        break;
    }
}
//MSH_CMD_EXPORT(var, set get var);


void var_arr_register(void *var,const char* name, var_type_e type,int size)
{
    char name1[20]={0};
    switch (type)
        {

        case _8:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((char*)var)[i],name1,type);
            }
            break;
			
			
        //case _8:
        //    var_logInfo("%s is %d", _var->name, *(unsigned char*)_var->body);

        //    break;
        case _16:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((short*)var)[i],name1,type);
            }
            break;
        //case _u16:
        //    var_logInfo("%s is %d", _var->name, *( unsigned short*)_var->body);

        //    break;
        case _32:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((int*)var)[i],name1,type);
            }
            break;
        //case _u32:
        //    var_logInfo("%s is %d", _var->name, *(unsigned int*)_var->body);
        //    break;
        case _64:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((long long*)var)[i],name1,type);
            }
            break;
        //case _u64:
        //    var_logInfo("%s is %d", _var->name, *(unsigned long long*)_var->body);

        //    break;
        case _f:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((float*)var)[i],name1,type);
            }
            break;
        case _lf:
            for(int i=0;i<size;++i){
                sprintf(name1,"%s[%d]",name,i);
                var_register(&((double*)var)[i],name1,type);
            }
            break;
        default:
            break;
        }



}
void var_register(void* var,const char* name, var_type_e type)
{
    var_list_t* p = (var_list_t*)VAR_MALLOC(sizeof(var_list_t));

    if (p == NULL)return;
    var_list_t* p2;
    if (var != NULL)p->body = var;
    else return;
    if (name != NULL)
    {
        int len=strlen(name);
        char* pname = (char*)VAR_MALLOC(sizeof(char)*len+1);
        strcpy(pname,name);
        p->name = pname;
    }
    else return;
    p->type = type;
    p->next = 0;
    if (!_var_begin) {//
        _var_begin = p;
        return;
    }
    p2 = _var_begin;
    while ((p2->next)!=0)
    {
        p2 = p2->next;
    }//找到最后一个变量
    p2->next = p;//添加变量到单向链表
}



