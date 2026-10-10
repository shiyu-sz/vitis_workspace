#include "xbreath_led.h"
#include "sleep.h"

int main()
{
	XBreath_led led;

	XBreath_led_Initialize(&led, XPAR_BREATH_LED_0_DEVICE_ID);

	xil_printf("run breath led hls \r\n");
	while(1)
	{
		XBreath_led_Set_freq_step(&led,80);
		XBreath_led_Set_sw_ctrl(&led,1);      //打开呼吸灯
		sleep(5);
		XBreath_led_Set_sw_ctrl(&led,0);      //关闭呼吸灯
		sleep(2);
		XBreath_led_Set_freq_step(&led,400);
		XBreath_led_Set_sw_ctrl(&led,1);      //打开呼吸灯
		sleep(5);
	}
	return 0;
}
