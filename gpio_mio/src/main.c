//****************************************************************************************//

#include "xparameters.h" //器件参数信息
#include "xstatus.h"     //包含XST_FAILURE和XST_SUCCESS的宏定义
#include "xil_printf.h"  //包含print()函数
#include "xgpiops.h"     //包含PS GPIO的函数
#include "sleep.h"       //包含sleep()函数

//宏定义GPIO_DEVICE_ID
#define GPIO_DEVICE_ID      XPAR_XGPIOPS_0_DEVICE_ID
//连接到MIO的LED
#define MIOLED0    16     //连接到MIO16

XGpioPs Gpio;            // GPIO设备的驱动程序实例

int main()
{
    int Status;
    XGpioPs_Config *ConfigPtr;

    print("MIO Test! \n\r");
    ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    Status = XGpioPs_CfgInitialize(&Gpio, ConfigPtr,
                    ConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS){
        return XST_FAILURE;
    }
    //设置指定引脚的方向：0输入，1输出
    XGpioPs_SetDirectionPin(&Gpio, MIOLED0, 1);
    //使能指定引脚输出：0禁止输出使能，1使能输出
    XGpioPs_SetOutputEnablePin(&Gpio, MIOLED0, 1);

    while (1) {
        XGpioPs_WritePin(&Gpio, MIOLED0, 0x0); //向指定引脚写入数据：0或1
        sleep(1);                              //延时1秒
        XGpioPs_WritePin(&Gpio, MIOLED0, 0x1);
        sleep(1);
        print("MIO run! \n\r");
    }
    return XST_SUCCESS;
}

