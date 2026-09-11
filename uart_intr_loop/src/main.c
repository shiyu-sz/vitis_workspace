#include "xparameters.h"		//器件参数信息
#include "xuartps.h"			//包含PS UART的函数声明
#include "xil_printf.h"			//包含print()函数
#include "xscugic.h"			//包含中断的函数声明
#include "stdio.h"				//包含printf函数的声明

#define UART_DEVICE_ID     XPAR_PS7_UART_0_DEVICE_ID    //串口设备ID
#define INTC_DEVICE_ID     XPAR_SCUGIC_SINGLE_DEVICE_ID //中断ID
#define UART_INT_IRQ_ID    XPAR_XUARTPS_0_INTR          //串口中断ID

XScuGic Intc;              //中断控制器驱动程序实例
XUartPs Uart_Ps;           //串口驱动程序实例

//UART初始化函数
int uart_init(XUartPs* uart_ps)
{
	int status;
    XUartPs_Config *uart_cfg;

    uart_cfg = XUartPs_LookupConfig(UART_DEVICE_ID);
    if (NULL == uart_cfg)
       return XST_FAILURE;
    status = XUartPs_CfgInitialize(uart_ps, uart_cfg, uart_cfg->BaseAddress);
    if (status != XST_SUCCESS)
       return XST_FAILURE;

    //UART设备自检
     status = XUartPs_SelfTest(uart_ps);
    if (status != XST_SUCCESS)
       return XST_FAILURE;

    //设置工作模式:正常模式
    XUartPs_SetOperMode(uart_ps, XUARTPS_OPER_MODE_NORMAL);
    //设置波特率:115200
    XUartPs_SetBaudRate(uart_ps,115200);
    //设置RxFIFO的中断触发等级
    XUartPs_SetFifoThreshold(uart_ps, 1);

    return XST_SUCCESS;
}

//UART中断处理函数
void uart_intr_handler(void *call_back_ref)
{
	XUartPs *uart_instance_ptr = (XUartPs *) call_back_ref;
    u32 rec_data = 0 ;
    u32 isr_status ;                           //中断状态标志

    //读取中断ID寄存器，判断触发的是哪种中断
    isr_status = XUartPs_ReadReg(uart_instance_ptr->Config.BaseAddress,XUARTPS_IMR_OFFSET);
    isr_status &= XUartPs_ReadReg(uart_instance_ptr->Config.BaseAddress,XUARTPS_ISR_OFFSET);

    //判断中断标志位RxFIFO是否触发
    if (isr_status & (u32)XUARTPS_IXR_RXOVR){
        rec_data = XUartPs_RecvByte(XPAR_PS7_UART_0_BASEADDR);
        //清除中断标志
        XUartPs_WriteReg(uart_instance_ptr->Config.BaseAddress,XUARTPS_ISR_OFFSET, XUARTPS_IXR_RXOVR) ;
    }
    XUartPs_SendByte(XPAR_PS7_UART_0_BASEADDR,rec_data);
}

//串口中断初始化
int uart_intr_init(XScuGic *intc, XUartPs *uart_ps)
{
	int status;
    //初始化中断控制器
    XScuGic_Config *intc_cfg;
    intc_cfg = XScuGic_LookupConfig(INTC_DEVICE_ID);
    if (NULL == intc_cfg)
        return XST_FAILURE;
    status = XScuGic_CfgInitialize(intc, intc_cfg,intc_cfg->CpuBaseAddress);
    if (status != XST_SUCCESS)
        return XST_FAILURE;

    //设置并打开中断异常处理功能
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
            (Xil_ExceptionHandler)XScuGic_InterruptHandler,
            (void *)intc);
    Xil_ExceptionEnable();

    //为中断设置中断处理函数
    XScuGic_Connect(intc, UART_INT_IRQ_ID,
            (Xil_ExceptionHandler) uart_intr_handler,(void *) uart_ps);
    //设置UART的中断触发方式
    XUartPs_SetInterruptMask(uart_ps, XUARTPS_IXR_RXOVR);
    //使能GIC中的串口中断
    XScuGic_Enable(intc, UART_INT_IRQ_ID);
    return XST_SUCCESS;
}

//main函数
int main(void)
{
   int status;

   xil_printf("run uart_intr_loop\n\r");
   status = uart_init(&Uart_Ps);    //串口初始化
   if (status == XST_FAILURE) {
       xil_printf("Uart Initial Failed\r\n");
       return XST_FAILURE;
   }

   uart_intr_init(&Intc, &Uart_Ps); //串口中断初始化
   while (1);
   return status;
}
