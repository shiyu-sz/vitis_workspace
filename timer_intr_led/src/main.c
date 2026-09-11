#include "xparameters.h"				//包含器件的参数信息
#include "xscutimer.h"					//定时器中断的函数声明
#include "xscugic.h"					//包含中断的函数声明
#include "xgpiops.h"					//PS端GPIO的函数声明
#include "xil_printf.h"

#define TIMER_DEVICE_ID     XPAR_XSCUTIMER_0_DEVICE_ID   //定时器ID
#define INTC_DEVICE_ID      XPAR_SCUGIC_SINGLE_DEVICE_ID //中断ID
#define TIMER_IRPT_INTR     XPAR_SCUTIMER_INTR           //定时器中断ID
#define GPIO_DEVICE_ID      XPAR_XGPIOPS_0_DEVICE_ID     //宏定义 GPIO_PS ID
#define MIO_LED             7                            //led连接到 MIO0

//私有定时器的时钟频率 = CPU时钟频率/2 = 333MHz
//0.2s闪烁一次,0.2*1000_000_000/(1000/333) - 1 = 3F83C3F
#define TIMER_LOAD_VALUE    0x3F83C3F                    //定时器装载值

XScuGic Intc;               //中断控制器驱动程序实例
XScuTimer Timer;            //定时器驱动程序实例
XGpioPs Gpio;               //GPIO设备的驱动程序实例

//MIO引脚初始化
int mio_init(XGpioPs *mio_ptr)
{
	int status;

    XGpioPs_Config *mio_cfg_ptr;
    mio_cfg_ptr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    if (NULL == mio_cfg_ptr)
        return XST_FAILURE;
    status = XGpioPs_CfgInitialize(mio_ptr, mio_cfg_ptr, mio_cfg_ptr->BaseAddr);
    if (status != XST_SUCCESS)
        return XST_FAILURE;

    //设置指定引脚的方向： 0 输入， 1 输出
    XGpioPs_SetDirectionPin(&Gpio, MIO_LED, 1);
    //使能指定引脚输出： 0 禁止输出使能， 1 使能输出
    XGpioPs_SetOutputEnablePin(&Gpio, MIO_LED, 1);
    return XST_SUCCESS;
}

//定时器初始化程序
int timer_init(XScuTimer *timer_ptr)
{
	int status;
    //私有定时器初始化
    XScuTimer_Config *timer_cfg_ptr;
    timer_cfg_ptr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);
    if (NULL == timer_cfg_ptr)
        return XST_FAILURE;
    status = XScuTimer_CfgInitialize(timer_ptr, timer_cfg_ptr,timer_cfg_ptr->BaseAddr);
    if (status != XST_SUCCESS)
        return XST_FAILURE;

    XScuTimer_LoadTimer(timer_ptr, TIMER_LOAD_VALUE); // 加载计数周期
    XScuTimer_EnableAutoReload(timer_ptr);            // 设置自动装载模式

    return XST_SUCCESS;
}

//定时器中断处理程序
void timer_intr_handler(void *CallBackRef)
{
    //LED状态,用于控制LED灯状态翻转
    static int led_state = 0;
    XScuTimer *timer_ptr = (XScuTimer *) CallBackRef;
    if(led_state == 0)
        led_state = 1;
    else
        led_state = 0;
    //向指定引脚写入数据： 0 或 1
    XGpioPs_WritePin(&Gpio, MIO_LED,led_state);
    //清除定时器中断标志
    XScuTimer_ClearInterruptStatus(timer_ptr);
}

//定时器中断初始化
void timer_intr_init(XScuGic *intc_ptr,XScuTimer *timer_ptr)
{
	//初始化中断控制器
    XScuGic_Config *intc_cfg_ptr;
    intc_cfg_ptr = XScuGic_LookupConfig(INTC_DEVICE_ID);
    XScuGic_CfgInitialize(intc_ptr, intc_cfg_ptr,intc_cfg_ptr->CpuBaseAddress);
    //设置并打开中断异常处理功能
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
            (Xil_ExceptionHandler)XScuGic_InterruptHandler, intc_ptr);
    Xil_ExceptionEnable();

    //设置定时器中断
    XScuGic_Connect(intc_ptr, TIMER_IRPT_INTR,
          (Xil_ExceptionHandler)timer_intr_handler, (void *)timer_ptr);

    XScuGic_Enable(intc_ptr, TIMER_IRPT_INTR); //使能GIC中的定时器中断
    XScuTimer_EnableInterrupt(timer_ptr);      //使能定时器中断
}

//main函数
int main()
{
	int status;
    xil_printf("SCU Timer Interrupt Test \r\n");

    mio_init(&Gpio);                 //MIO引脚初始化
    status = timer_init(&Timer);     //定时器初始化
    if (status != XST_SUCCESS) {
        xil_printf("Timer Initial Failed\r\n");
        return XST_FAILURE;
    }
    timer_intr_init(&Intc,&Timer);   //定时器中断初始化
    XScuTimer_Start(&Timer);         //启动定时器

    while(1);
    return 0;
}
