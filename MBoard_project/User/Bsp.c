#include <Bsp.h> //文件内包括延时函数初始化及Debug串口输出初始化；
/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     Bsp
 * @Author:     RanHongLiang
 * @Date:       2019-06-26 13:16:27
 * @Description: 
 *————BSP_Init()，板级支持设置初始化
 *---------------------------------------------------------------------------*/

void BSP_Init(void)
{
    bsp_delayinit();
    debugThread_Active();
}
