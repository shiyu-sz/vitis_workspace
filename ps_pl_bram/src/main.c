
#include "xil_printf.h"
#include "stdio.h"
#include <string.h>
#include "pl_bram_rd.h"
#include "xbram.h"
#include "sleep.h"

#define PL_BRAM_BASE        XPAR_PL_BRAM_RD_0_S00_AXI_BASEADDR   //PL_RAM_RD基地址
#define PL_BRAM_START       PL_BRAM_RD_S00_AXI_SLV_REG0_OFFSET   //RAM读开始寄存器地址
#define PL_BRAM_START_ADDR  PL_BRAM_RD_S00_AXI_SLV_REG1_OFFSET   //RAM起始寄存器地址
#define PL_BRAM_LEN         PL_BRAM_RD_S00_AXI_SLV_REG2_OFFSET   //PL读RAM的深度

#define START_ADDR          0  //RAM起始地址 范围:0~1023
#define BRAM_DATA_BYTE      4  //BRAM数据字节个数

char ch_data[32];            //写入BRAM的字符数组
int ch_data_len;               //写入BRAM的字符个数

//函数声明
void str_wr_bram();
void str_rd_bram();

//main函数
int main()
{
	int i = 0;
    while(1)
    {
        printf("Please enter data to read and write BRAM\n") ;

//        scanf("%1024s", ch_data);        //用户输入字符串
        sleep(1);
        snprintf(ch_data, sizeof(ch_data), "test %d", i);
        i ++;
        printf("str = %s\n", ch_data);

        ch_data_len = strlen(ch_data);   //计算字符串的长度

        str_wr_bram();                   //将用户输入的字符串写入BRAM中
        str_rd_bram();                   //从BRAM中读出数据
    }
}

//将字符串写入BRAM
void str_wr_bram()
{
    int i=0,wr_cnt = 0;
    //每次循环向BRAM中写入1个字符
    for(i = BRAM_DATA_BYTE*START_ADDR ; i < BRAM_DATA_BYTE*(START_ADDR + ch_data_len) ;
            i += BRAM_DATA_BYTE){
        XBram_WriteReg(XPAR_BRAM_0_BASEADDR,i,ch_data[wr_cnt]) ;
        wr_cnt++;
    }
    //设置BRAM写入的字符串长度
    PL_BRAM_RD_mWriteReg(PL_BRAM_BASE, PL_BRAM_LEN , BRAM_DATA_BYTE*ch_data_len) ;
    //设置BRAM的起始地址
    PL_BRAM_RD_mWriteReg(PL_BRAM_BASE, PL_BRAM_START_ADDR, BRAM_DATA_BYTE*START_ADDR) ;
    //拉高BRAM开始信号
    PL_BRAM_RD_mWriteReg(PL_BRAM_BASE, PL_BRAM_START , 1) ;
    //拉低BRAM开始信号
    PL_BRAM_RD_mWriteReg(PL_BRAM_BASE, PL_BRAM_START , 0) ;
}

//从BRAM中读出数据
void str_rd_bram()
{
    int read_data=0,i=0;
    //循环从BRAM中读出数据
    for(i = BRAM_DATA_BYTE*START_ADDR ; i < BRAM_DATA_BYTE*(START_ADDR + ch_data_len) ;
            i += BRAM_DATA_BYTE){
        read_data = XBram_ReadReg(XPAR_BRAM_0_BASEADDR,i) ;
        printf("BRAM address is %d\t,Read data is %c\n",i/BRAM_DATA_BYTE ,read_data) ;
    }
}
