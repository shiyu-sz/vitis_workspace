#ifndef ACZ702_LIB_COMMON_H_
#define ACZ702_LIB_COMMON_H_


//系统头文件
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>

//Xilinx 头文件
#include "xil_types.h"
#include "xil_cache.h"
#include "sleep.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xscutimer.h"


//ACZ702 头文件
#include "ISR.h"
#include "SCU_GIC.h"
#include "SCU_TIMER.h"

#include "lcd.h"
#include "Camera.h"

//用户头文件



//用户宏定义
#define	CPU_CLK_HZ	XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ	//CPU时钟频率（单位：Hz）
#define INPUT		1
#define OUTPUT		0
#define	REG8		8
#define	REG16		16

//用户自定义函数

#endif /* ACZ702_LIB_COMMON_H_ */
