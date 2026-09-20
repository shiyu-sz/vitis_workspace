#include "xparameters.h"
#include "xscugic.h"
#include "xil_printf.h"
#include "xil_exception.h"
#include "xil_mmu.h"
#include "stdio.h"
#include "sleep.h"

//宏定义
#define INTC_DEVICE_ID	     XPAR_SCUGIC_SINGLE_DEVICE_ID //中断ID
#define SHARE_BASE           0xffff0000                   //共享OCM首地址
#define CPU1_COPY_ADDR       0xfffffff0                   //存放CPU1应用起始地址的地址
#define CPU1_START_ADDR      0x10000000                   //CPU1应用起始地址

#define CPU1_ID              XSCUGIC_SPI_CPU1_MASK        //CPU1 ID
#define SOFT_INTR_ID_TO_CPU0 0                            //软件中断号 0 ,范围：0~15
#define SOFT_INTR_ID_TO_CPU1 1                            //软件中断号 1 ,范围：0~15

//"SEV"指令唤醒CPU1并跳转至相应的程序
#define sev()                __asm__("sev")               //C语言内嵌汇编写法 send event指令

//函数声明
void start_cpu1();
void cpu0_intr_init(XScuGic *intc_ptr);
void soft_intr_handler(void *CallbackRef);

//全局变量
XScuGic Intc;                    //中断控制器驱动程序实例
int rec_freq_flag = 0;           //接收到呼吸灯频率设置的标志
int freq_gear = 0;                   //频率档位

//CPU0 main函数
int main()
{
	//S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
	Xil_SetTlbAttributes(SHARE_BASE,0x14de2);    //禁用OCM的Cache属性

	//S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
	Xil_SetTlbAttributes(CPU1_COPY_ADDR,0x14de2);//禁用0xfffffff0的Cache属性

	//CPU0中断初始化
	cpu0_intr_init(&Intc);
	//启动CPU1
	start_cpu1();
	while(1){
		if(rec_freq_flag == 0){
			xil_printf("This is CPU0,Please input the numbers 1~5 to change "
					"breath led frequency\r\n");
//			scanf("%d",&freq_gear);
			sleep(3);
			freq_gear ++;
			if(freq_gear > 5) {
				freq_gear = 0;
			}
			if(freq_gear >= 1 && freq_gear <=5){
				xil_printf("You input number is %d\r\n",freq_gear);
				Xil_Out8(SHARE_BASE, freq_gear);
				rec_freq_flag = 1;
				dmb();
				XScuGic_SoftwareIntr(&Intc, SOFT_INTR_ID_TO_CPU1, CPU1_ID);
			}
			else{
				xil_printf("Error,The number range is 1~5\r\n");
				xil_printf("\r\n");
			}
		}
	}
	return 0 ;
}

//启动CPU1，用于固化程序
void start_cpu1()
{
	//向 CPU1_COPY_ADDR(0Xffffffff0)地址写入 CPU1 的访问内存基地址
	Xil_Out32(CPU1_COPY_ADDR, CPU1_START_ADDR);
	dmb();  //等待内存写入完成(同步)
	sev();  //通过"SEV"指令唤醒CPU1并跳转至相应的程序
}

//CPU0中断初始化
void cpu0_intr_init(XScuGic *intc_ptr)
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

    XScuGic_Connect(intc_ptr, SOFT_INTR_ID_TO_CPU0,
          (Xil_ExceptionHandler)soft_intr_handler, (void *)intc_ptr);

    XScuGic_Enable(intc_ptr, SOFT_INTR_ID_TO_CPU0); //CPU0软件中断
}

//软件中断函数
void soft_intr_handler(void *CallbackRef)
{
	xil_printf("This is CPU0,Soft Interrupt from CPU1\r\n");
	xil_printf("\r\n");
	rec_freq_flag = 0;
}
