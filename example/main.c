#include <stdio.h>
#include "pid.h"

int main(void)
{
    printf("Hello all pid\r\n");
    apid_t pid;
    APID_Init(&pid, PID_POSITION, 3, 0.1, 0);
    APID_Enable(&pid); // 默认已经开启

    APID_Set_Target(&pid, 50);

    for (int i = 0; i < 16; ++i)
    {
        APID_Set_Present(&pid, 32);//用户定时更新当前值数据

        APID_Hander(&pid, 1);//cycle 如果不使用，建议设置为1,使用请输入两次调用的间隔单位ms
        printf("pid out %f\r\n", APID_Get_Out(&pid));
    }

    return 0;
}
