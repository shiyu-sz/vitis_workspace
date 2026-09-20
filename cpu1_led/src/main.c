#include "xparameters.h"
#include "xscugic.h"
#include "xil_printf.h"
#include "xil_exception.h"
#include "xil_mmu.h"
#include "stdio.h"
#include "led_ip.h"

//宏定义
#define INTC_DEVICE_ID	     XPAR_SCUGIC_SINGLE_DEVICE_ID //中断ID
#define SHARE_BASE  	     0xffff0000                   //共享OCM首地址

#define CPU0_ID              XSCUGIC_SPI_CPU0_MASK        //CPU0 ID
#define SOFT_INTR_ID_TO_CPU0 0                            //软件中断号 0 ,范围：0~15
#define SOFT_INTR_ID_TO_CPU1 1                            //软件中断号 1 ,范围：0~15

#define  LED_IP_BASEADDR   XPAR_LED_IP_0_S00_AXI_BASEADDR  //LED IP基地址
#define  LED_IP_REG0       LED_IP_S00_AXI_SLV_REG0_OFFSET  //LED IP寄存器地址0
#define  LED_IP_REG1       LED_IP_S00_AXI_SLV_REG1_OFFSET  //LED IP寄存器地址1

//函数声明
void cpu1_intr_init(XScuGic *intc_ptr);
void soft_intr_handler(void *CallbackRef);

//全局变量
XScuGic Intc;               //中断控制器驱动程序实例
int soft_intr_flag = 0;     //软件中断的标志
int freq_gear;              //频率档位

//CPU1 main函数
int main()
{
	int freq_step = 10;
	//S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
	Xil_SetTlbAttributes(SHARE_BASE,0x14de2);    //禁用OCM的Cache属性

	//CPU1中断初始化
	cpu1_intr_init(&Intc);
	//打开呼吸灯
	LED_IP_mWriteReg(LED_IP_BASEADDR, LED_IP_REG0, 1);//1
	while(1){
		if(soft_intr_flag){
			freq_gear = Xil_In8(SHARE_BASE);     //从共享OCM中读出数据
			xil_printf("CUP1 Received data is %d\r\n",freq_gear) ;
			switch(freq_gear){
				case 1 : freq_step = 10;break;
				case 2 : freq_step = 20;break;
				case 3 : freq_step = 30;break;
				case 4 : freq_step = 50;break;
				case 5 : freq_step = 90;break;
				default : freq_step = 10;break;
			}
			//设置呼吸灯频率,最高位为1，设置有效
			LED_IP_mWriteReg(LED_IP_BASEADDR,LED_IP_REG1,(0x80000000|freq_step));
			//给给CPU0触发中断
			XScuGic_SoftwareIntr(&Intc,SOFT_INTR_ID_TO_CPU0,CPU0_ID);
			soft_intr_flag = 0;
		}
	}
	return 0 ;
}

//CPU1中断初始化
void cpu1_intr_init(XScuGic *intc_ptr)
{
	//初始化中断控制器
	XScuGic_Config *intc_cfg_ptr;
	intc_cfg_ptr = XScuGic_LookupConfig(INTC_DEVICE_ID);
    XScuGic_CfgInitialize(intc_ptr, intc_cfg_ptr,
    		intc_cfg_ptr->CpuBaseAddress);
    //设置并打开中断异常处理功能
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
    		(Xil_ExceptionHandler)XScuGic_InterruptHandler, intc_ptr);
    Xil_ExceptionEnable();

    XScuGic_Connect(intc_ptr, SOFT_INTR_ID_TO_CPU1,
          (Xil_ExceptionHandler)soft_intr_handler, (void *)intc_ptr);

    XScuGic_Enable(intc_ptr, SOFT_INTR_ID_TO_CPU1); //CPU1软件中断
}

//软件中断函数
void soft_intr_handler(void *CallbackRef)
{
	xil_printf("This is CUP1,Soft Interrupt from CPU0\r\n") ;
	soft_intr_flag = 1;
}
