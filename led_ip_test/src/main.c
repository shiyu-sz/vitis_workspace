//----------------------------------------------------------------------------------------
//****************************************************************************************//

#include "stdio.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "led_ip.h"
#include "xil_io.h"
#include "sleep.h"

#define  LED_IP_BASEADDR   XPAR_LED_IP_0_S00_AXI_BASEADDR  //LED IP基地址
#define  LED_IP_REG0       LED_IP_S00_AXI_SLV_REG0_OFFSET  //LED IP寄存器地址0
#define  LED_IP_REG1       LED_IP_S00_AXI_SLV_REG1_OFFSET  //LED IP寄存器地址1

//main函数
int main()
{
 int freq_flag;      //定义频率状态，用于循环改变呼吸灯的呼吸频率
 int led_state;      //定义LED灯的状态

 xil_printf("LED User IP Test!\r\n");
 while(1){
     //根据freq_flag的标志位,切换呼吸灯的频率
     if(freq_flag == 0){
    	 LED_IP_mWriteReg(LED_IP_BASEADDR,LED_IP_REG1,0x80000032);
         freq_flag = 1;
     }
     else{
    	 LED_IP_mWriteReg(LED_IP_BASEADDR,LED_IP_REG1,0x800001f4);
         freq_flag = 0;
     }
     //获取LED当前开关状态   1:打开  0:关闭
     led_state = LED_IP_mReadReg(LED_IP_BASEADDR,LED_IP_REG0);
     //如果开关关闭,打开呼吸灯
     if(led_state == 0){
    	 LED_IP_mWriteReg (LED_IP_BASEADDR, LED_IP_REG0, 1);
         xil_printf("Breath LED ON\r\n");
     }
     sleep(8);
     //获取LED当前开关状态   1:打开  0:关闭
     led_state = LED_IP_mReadReg(LED_IP_BASEADDR,LED_IP_REG0);
     //如果开关打开,关闭呼吸灯
     if(led_state == 1){
    	 LED_IP_mWriteReg (LED_IP_BASEADDR, LED_IP_REG0, 0);
         xil_printf("Breath LED OFF\r\n");
     }
     sleep(1);
 }
}
