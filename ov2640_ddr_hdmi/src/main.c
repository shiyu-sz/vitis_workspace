#include "COMMON.h"
#include "DVP_Capture2DDR.h"

#define	WRITE_CAMERA_REG	DVP_CAPTURE2DDR_mWriteReg
#define	REG_ONOFF			DVP_CAPTURE2DDR_S00_AXI_SLV_REG0_OFFSET
#define	CAMERA_REG_BASEADDR XPAR_DVP_CAPTURE2DDR_0_S00_AXI_BASEADDR
#define OPEN_CAMERA			WRITE_CAMERA_REG(CAMERA_REG_BASEADDR,REG_ONOFF,1)
#define	CLOSE_CAMERA		WRITE_CAMERA_REG(CAMERA_REG_BASEADDR,REG_ONOFF,0)
#define Image_Storage_Addr	0x01800000

/* Set to 0 to restore the camera-to-DDR capture path. */
#define DISPLAY_TEST_PATTERN	0
#define TEST_FRAME_COLOR		0xF800U /* RGB565 red */

int main(void)
{

#if DISPLAY_TEST_PATTERN
	int Status;
	uint32_t Pixel;
	volatile uint16_t *TestFrame = (volatile uint16_t *)(UINTPTR)Image_Storage_Addr;

	xil_printf("Display test pattern: RGB565 0x%04x\r\n", TEST_FRAME_COLOR);
	PS_IIC_Init(&SCCB, IIC_DEVICE_ID, 400000);
	SII9022_Init();
	xil_printf("SII9022_Init done\r\n");
	usleep(20000);

	/* Keep the PL capture engine from overwriting the test frame. */
	CLOSE_CAMERA;
	for (Pixel = 0; Pixel < RGB_LCD.width * RGB_LCD.height; ++Pixel)
		TestFrame[Pixel] = TEST_FRAME_COLOR;
	Xil_DCacheFlushRange((UINTPTR)TestFrame,
			RGB_LCD.width * RGB_LCD.height * BYTES_PIXEL);

	Status = run_vdma_frame_buffer(&Vdma, VDMA_ID, RGB_LCD.width,
			RGB_LCD.height, Image_Storage_Addr, 0, 0, ONLY_READ);
	xil_printf("VDMA read start status=%d\r\n", Status);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	Status = DisplayInitialize(&DispCtrl, DISP_VTC_ID, DYNCLK_BASEADDR);
	xil_printf("DisplayInitialize status=%d\r\n", Status);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	Status = DisplaySetMode(&DispCtrl, &RGB_LCD);
	xil_printf("DisplaySetMode status=%d\r\n", Status);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	Status = DisplayStart(&DispCtrl);
	xil_printf("DisplayStart status=%d\r\n", Status);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	xil_printf("Test frame is running.\r\n");
	return XST_SUCCESS;
#else
	xil_printf("%s:%d\n", __func__, __LINE__);
	PS_IIC_Init(&SCCB, IIC_DEVICE_ID, 400000);
	xil_printf("%s:%d\n", __func__, __LINE__);
	SII9022_Init();
	xil_printf("%s:%d\n", __func__, __LINE__);
	usleep(20000);
	Camera_check();
	xil_printf("%s:%d OV5640_Check=%d, OV2640_Check=%d\n", __func__, __LINE__, OV5640_Check, OV2640_Check);
	Camera_Init();
	xil_printf("%s:%d\n", __func__, __LINE__);
	OPEN_CAMERA;
	xil_printf("%s:%d\n", __func__, __LINE__);
	xil_printf("%s:%d OV5640_Check=%d, OV2640_Check=%d\n", __func__, __LINE__, OV5640_Check, OV2640_Check);

	//配置VDMA，读取图像存储地址的图像数据
	run_vdma_frame_buffer(&Vdma, VDMA_ID, RGB_LCD.width, RGB_LCD.height,
			Image_Storage_Addr,0, 0,ONLY_READ);

	//初始化Display controller
	DisplayInitialize(&DispCtrl, DISP_VTC_ID, DYNCLK_BASEADDR);

	//设置VideoMode
	DisplaySetMode(&DispCtrl, &RGB_LCD);
	DisplayStart(&DispCtrl);

	return 0;
#endif
}

