
#include "xparameters.h"
#include "xil_printf.h"
#include "ff.h"
#include "xdevcfg.h"

#define FILE_NAME "ZDYZ.txt"                //定义文件名

char src_str[30] = "www.openedv.com/forum.php"; //定义文本内容
static FATFS fatfs;                         //文件系统

static FRESULT platform_init_fs(void)
{
  FRESULT fr;
  TCHAR *path = "0:/";
  BYTE work[FF_MAX_SS];

  fr = f_mount(&fatfs, path, 1);
  xil_printf("f_mount returned %d\r\n", (int)fr);

  if (fr == FR_OK)
	  return FR_OK;

  if (fr != FR_NO_FILESYSTEM)
	  return fr;  /* 不是格式问题，不要格式化 */

  xil_printf("No FAT filesystem, formatting...\r\n");

  fr = f_mkfs(path, FM_FAT32, 0, work, sizeof(work));
  xil_printf("f_mkfs returned %d\r\n", (int)fr);
  if (fr != FR_OK)
	  return fr;

  fr = f_mount(&fatfs, path, 1);
  xil_printf("second f_mount returned %d\r\n", (int)fr);
  return fr;
}

//挂载SD(TF)卡
int sd_mount()
{
    FRESULT status;
    //初始化文件系统（挂载SD卡，如果挂载不成功，则格式化SD卡）
    status = platform_init_fs();
    if(status){
        xil_printf("ERROR: f_mount returned %d!\r\n",status);
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}

//SD卡写数据
int sd_write_data(char *file_name,u32 src_addr,u32 byte_len)
{
    FIL fil;         //文件对象
    UINT bw;         //f_write函数返回已写入的字节数

    //打开一个文件,如果不存在，则创建一个文件
    f_open(&fil,file_name,FA_CREATE_ALWAYS | FA_WRITE);
    //移动打开的文件对象的文件读/写指针     0:指向文件开头
    f_lseek(&fil, 0);
    //向文件中写入数据
    f_write(&fil,(void*) src_addr,byte_len,&bw);
    //关闭文件
    f_close(&fil);
    return 0;
}

//SD卡读数据
int sd_read_data(char *file_name,u32 src_addr,u32 byte_len)
{
	FIL fil;         //文件对象
    UINT br;         //f_read函数返回已读出的字节数

    //打开一个只读的文件
    f_open(&fil,file_name,FA_READ);
    //移动打开的文件对象的文件读/写指针     0:指向文件开头
    f_lseek(&fil,0);
    //从SD卡中读出数据
    f_read(&fil,(void*)src_addr,byte_len,&br);
    //关闭文件
    f_close(&fil);
    return 0;
}

//main函数
int main()
{
    int status,len;
    char dest_str[30] = "";

    status = sd_mount();           //挂载SD卡
    if(status != XST_SUCCESS){
		xil_printf("Failed to open SD card!\r\n");
		return 0;
    }
    else
        xil_printf("Success to open SD card!\r\n");

    len = strlen(src_str);         //计算字符串长度
    //SD卡写数据
    sd_write_data(FILE_NAME,(u32)src_str,len);
    //SD卡读数据
    sd_read_data(FILE_NAME,(u32)dest_str,len);

    //比较写入的字符串和读出的字符串是否相等
    if (strcmp(src_str, dest_str) == 0)
    	xil_printf("src_str is equal to dest_str,SD card test success!\r\n");
    else
    	xil_printf("src_str is not equal to dest_str,SD card test failed!\r\n");

    return 0;
  }
