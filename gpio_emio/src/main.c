//----------------------------------------------------------------------------------------
//****************************************************************************************//

#include "stdio.h"
#include "xparameters.h"
#include "xgpiops.h"

#define GPIOPS_ID XPAR_XGPIOPS_0_DEVICE_ID   //PS端  GPIO器件 ID

#define MIO_LED0 7   //PS_LED0 连接到 MIO7
#define MIO_KEY0 47   //PS_KEY0 连接到 MIO47

#define EMIO_KEY0 54  //PL_KEY0 连接到EMIO0
#define EMIO_LED0 55  //PL_LED0 连接到EMIO1

int main()
{
    printf("EMIO TEST!\n");

    XGpioPs gpiops_inst;            //PS端 GPIO 驱动实例
    XGpioPs_Config *gpiops_cfg_ptr; //PS端 GPIO 配置信息

    //根据器件ID查找配置信息
    gpiops_cfg_ptr = XGpioPs_LookupConfig(GPIOPS_ID);
    //初始化器件驱动
    XGpioPs_CfgInitialize(&gpiops_inst, gpiops_cfg_ptr, gpiops_cfg_ptr->BaseAddr);

    //设置LED为输出
    XGpioPs_SetDirectionPin(&gpiops_inst, MIO_LED0, 1);
    XGpioPs_SetDirectionPin(&gpiops_inst, EMIO_LED0, 1);
    //使能LED输出
    XGpioPs_SetOutputEnablePin(&gpiops_inst, MIO_LED0, 1);
    XGpioPs_SetOutputEnablePin(&gpiops_inst, EMIO_LED0, 1);

    //设置KEY为输入
    XGpioPs_SetDirectionPin(&gpiops_inst, EMIO_KEY0, 0);
    XGpioPs_SetDirectionPin(&gpiops_inst, MIO_KEY0, 0);

    //读取按键状态，用于控制LED亮灭
    while(1){

        XGpioPs_WritePin(&gpiops_inst, MIO_LED0, ~XGpioPs_ReadPin(&gpiops_inst, EMIO_KEY0));

        XGpioPs_WritePin(&gpiops_inst, EMIO_LED0, ~XGpioPs_ReadPin(&gpiops_inst, MIO_KEY0));

        /*
        printf("MIO_KEY0 = %d\n", XGpioPs_ReadPin(&gpiops_inst, MIO_KEY0));
        XGpioPs_WritePin(&gpiops_inst, EMIO_LED0, 0);
        XGpioPs_WritePin(&gpiops_inst, MIO_LED0, 0);
        sleep(1);
        XGpioPs_WritePin(&gpiops_inst, EMIO_LED0, 1);
        XGpioPs_WritePin(&gpiops_inst, MIO_LED0, 1);
        sleep(1);
        */
    }

    return 0;
}
