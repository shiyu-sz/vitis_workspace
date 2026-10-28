#ifndef ACZ702_LIB_SCU_TIMER_H_
#define ACZ702_LIB_SCU_TIMER_H_

#include "COMMON.h"

XScuTimer ScuTimer;		//私有定时器

void ScuTimer_Int_Init(uint32_t Load_Val);

#endif /* ACZ702_LIB_SCU_TIMER_H_ */
