//----------------------------------------------------------------------------------------
//****************************************************************************************//

 #include "xparameters.h"
 #include "xgpiops.h"
 #include "xgpio.h"
 #include "xscugic.h"
 #include "xil_exception.h"
 #include "xplatform_info.h"
 #include <xil_printf.h>
 #include "sleep.h"

 /************************** Constant Definitions *****************************/

 //以下常量映射到xparameters.h文件
 #define GPIOPS_DEVICE_ID    XPAR_XGPIOPS_0_DEVICE_ID     //PS端GPIO器件ID
 #define AXI_GPIO_DEVICE_ID  XPAR_AXI_GPIO_0_DEVICE_ID    //PL端AXI GPIO器件ID
 #define SCUGIC_ID           XPAR_SCUGIC_0_DEVICE_ID      //通用中断控制器ID
 #define AXI_GPIO_INT_ID     XPAR_FABRIC_GPIO_0_VEC_ID    //PL端AXI GPIO中断ID

 #define MIO_LED       7                                  //LED 连接到 MIO7
 #define KEY_CHANNEL1  1                                  //PL 按键使用 AXI GPIO 通道 1
 #define KEY_CH1_MASK  XGPIO_IR_CH1_MASK                  //通道 1的中断位定义

 /************************** Function Prototypes ******************************/
 void instance_init();
 int setup_interrupt_system(XScuGic *gic_inst_ptr, XGpio *axi_gpio_inst_ptr,
     u16 AXI_GpioIntrId);
 static void intr_handler(void *callback_ref);

 /**************************Global Variable Definitions ***********************/
 XGpioPs gpiops_inst;        //PS端GPIO驱动实例
 XGpio   axi_gpio_inst;      //PL端AXI GPIO驱动实例
 XScuGic scugic_inst;        //通用中断控制器驱动实例
 int led_value=1;            //ps端LED2的显示状态

 /************************** Function Definitions *****************************/

 int main(void)
 {
     int status;

     //初始化各器件驱动
     instance_init();

     xil_printf("AXI_Gpio interrupt test \r\n");

     //设置LED所连接的MIO引脚的方向为输出并使能输出
     XGpioPs_SetDirectionPin(&gpiops_inst, MIO_LED, 1);
     XGpioPs_SetOutputEnablePin(&gpiops_inst, MIO_LED, 1);
     XGpioPs_WritePin(&gpiops_inst, MIO_LED, led_value);

     //建立中断,出现错误则打印信息并退出
     status = setup_interrupt_system(&scugic_inst, &axi_gpio_inst, AXI_GPIO_INT_ID);
     if (status != XST_SUCCESS) {
         xil_printf("Setup interrupt system failed\r\n");
         return XST_FAILURE;
     }

     return XST_SUCCESS;
 }

 //初始化各器件驱动
 void instance_init()
 {
     XScuGic_Config *scugic_cfg_ptr;
     XGpioPs_Config *gpiops_cfg_ptr;

     //初始化中断控制器驱动
 	scugic_cfg_ptr = XScuGic_LookupConfig(SCUGIC_ID);
     XScuGic_CfgInitialize(&scugic_inst, scugic_cfg_ptr, scugic_cfg_ptr->CpuBaseAddress);

     //初始化PS端  GPIO驱动
     gpiops_cfg_ptr = XGpioPs_LookupConfig(GPIOPS_DEVICE_ID );
     XGpioPs_CfgInitialize(&gpiops_inst, gpiops_cfg_ptr, gpiops_cfg_ptr->BaseAddr);

     //初始化PL端  AXI GPIO驱动
     XGpio_Initialize(&axi_gpio_inst, AXI_GPIO_DEVICE_ID);
 }

 //建立中断系统，使能KEY按键中断
 //  @param   GicInstancePtr是一个指向XScuGic驱动实例的指针
 //  @param   gpio是一个指向连接到中断的GPIO组件实例的指针
 //  @param   GpioIntrId是Gpio中断ID
 //  @return  如果成功返回XST_SUCCESS, 否则返回XST_FAILURE
 int setup_interrupt_system(XScuGic *gic_inst_ptr, XGpio *axi_gpio_inst_ptr, u16 AXI_GpioIntrId)
 {
     //设置并使能中断异常
     Xil_ExceptionInit();
     Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
             (Xil_ExceptionHandler) XScuGic_InterruptHandler, gic_inst_ptr);
     Xil_ExceptionEnable();

     //设置中断源的优先级和请求类型(优先级为0xA0,高电平请求)
     XScuGic_SetPriorityTriggerType(gic_inst_ptr, AXI_GpioIntrId, 0xA0, 0x01);
     //为中断设置中断处理函数
     XScuGic_Connect(gic_inst_ptr, AXI_GpioIntrId,
             (Xil_ExceptionHandler) intr_handler, (void *) axi_gpio_inst_ptr);

     //使能来自于axi_Gpio器件的中断
     XScuGic_Enable(gic_inst_ptr, AXI_GpioIntrId);

     //配置PL端 AXI GPIO
	 //设置 AXI GPIO 通道 1方向为输入
     XGpio_SetDataDirection(axi_gpio_inst_ptr, KEY_CHANNEL1, 1);
     XGpio_InterruptEnable(axi_gpio_inst_ptr, KEY_CH1_MASK);  //使能通道1的中断
     XGpio_InterruptGlobalEnable(axi_gpio_inst_ptr);          //使能axi gpio全局中断

     return XST_SUCCESS;
 }

 //中断处理函数
 //  @param   CallBackRef是指向上层回调引用的指针
 static void intr_handler(void *callback_ref)
 {
     XGpio *axi_gpio_inst_ptr = (XGpio *)callback_ref;
     usleep(20000);                                               //延时20ms，按键消抖
   if (XGpio_DiscreteRead(axi_gpio_inst_ptr, KEY_CHANNEL1) == 0) {//按键有效按下
 	  print("Interrupt Detected!\r\n");
 	  led_value = ~led_value;
 	  XGpioPs_WritePin(&gpiops_inst, MIO_LED, led_value);	  //改变LED显示状态
 	  XGpio_InterruptDisable(axi_gpio_inst_ptr, KEY_CH1_MASK);//关闭 AXI GPIO中断使能
    }
 	  XGpio_InterruptClear(axi_gpio_inst_ptr, KEY_CH1_MASK);  //清除中断
 	  XGpio_InterruptEnable(axi_gpio_inst_ptr, KEY_CH1_MASK); //使能AXI GPIO中断
 }

