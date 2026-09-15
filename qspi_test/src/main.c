#include "xparameters.h"    /* SDK generated parameters */
#include "xqspips.h"        /* QSPI device driver */
#include "xil_printf.h"

#define QSPI_DEVICE_ID      XPAR_XQSPIPS_0_DEVICE_ID

//发送到Flash器件的指令
#define WRITE_STATUS_CMD    0x01
#define WRITE_CMD           0x02
#define READ_CMD            0x03
#define WRITE_DISABLE_CMD   0x04
#define READ_STATUS_CMD     0x05
#define WRITE_ENABLE_CMD    0x06
#define FAST_READ_CMD       0x0B
#define DUAL_READ_CMD       0x3B
#define QUAD_READ_CMD       0x6B
#define BULK_ERASE_CMD      0xC7
#define SEC_ERASE_CMD       0xD8
#define READ_ID             0x9F

//Flash BUFFER中各数据的偏移量
#define COMMAND_OFFSET      0 // Flash instruction
#define ADDRESS_1_OFFSET    1 // MSB byte of address to read or write
#define ADDRESS_2_OFFSET    2 // Middle byte of address to read or write
#define ADDRESS_3_OFFSET    3 // LSB byte of address to read or write
#define DATA_OFFSET         4 // Start of Data for Read/Write
#define DUMMY_OFFSET        4 // Dummy byte offset for reads

#define DUMMY_SIZE          1 // Number of dummy bytes for reads
#define RD_ID_SIZE          4 // Read ID command + 3 bytes ID response
#define BULK_ERASE_SIZE     1 // Bulk Erase command size
#define SEC_ERASE_SIZE      4 // Sector Erase command + Sector address

#define OVERHEAD_SIZE       4 // control information: command and address

#define SECTOR_SIZE         0x10000
#define NUM_SECTORS         0x100
#define NUM_PAGES           0x10000
#define PAGE_SIZE           256

/* Number of Flash pages to be written.*/
#define PAGE_COUNT      16

/* Flash address to which data is to be written.*/
#define TEST_ADDRESS    0x00055000
#define UNIQUE_VALUE    0x05

#define MAX_DATA        (PAGE_COUNT * PAGE_SIZE)

void FlashErase(XQspiPs *QspiPtr, u32 Address, u32 ByteCount);
void FlashWrite(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command);
void FlashRead(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command);
int  FlashReadID(void);
void FlashQuadEnable(XQspiPs *QspiPtr);
int  QspiFlashPolledExample(XQspiPs *QspiInstancePtr, u16 QspiDeviceId);

static XQspiPs QspiInstance;

int Test = 5;

u8 ReadBuffer[MAX_DATA + DATA_OFFSET + DUMMY_SIZE];
u8 WriteBuffer[PAGE_SIZE + DATA_OFFSET];

int main(void)
{
    int Status;
    xil_printf("QSPI Flash Polled Example Test \r\n");

    /* Run the Qspi Interrupt example.*/
    Status = QspiFlashPolledExample(&QspiInstance, QSPI_DEVICE_ID);
    if (Status != XST_SUCCESS) {
        xil_printf("QSPI Flash Polled Example Test Failed\r\n");
        return XST_FAILURE;
    }

    xil_printf("Successfully ran QSPI Flash Polled Example Test\r\n");
 return XST_SUCCESS;
}
int QspiFlashPolledExample(XQspiPs *QspiInstancePtr, u16 QspiDeviceId)
{
	//int Status;
    u8 *BufferPtr;
    u8 UniqueValue;
    int Count;
    int Page;
    XQspiPs_Config *QspiConfig;

    //初始化QSPI驱动
    QspiConfig = XQspiPs_LookupConfig(QspiDeviceId);
    XQspiPs_CfgInitialize(QspiInstancePtr, QspiConfig, QspiConfig->BaseAddress);
    //初始化读写BUFFER
    for (UniqueValue = UNIQUE_VALUE, Count = 0; Count < PAGE_SIZE;
         Count++, UniqueValue++) {
        WriteBuffer[DATA_OFFSET + Count] = (u8)(UniqueValue + Test);
    }
    memset(ReadBuffer, 0x00, sizeof(ReadBuffer));

    //设置手动启动和手动片选模式
    XQspiPs_SetOptions(QspiInstancePtr, XQSPIPS_MANUAL_START_OPTION |
            XQSPIPS_FORCE_SSELECT_OPTION |
            XQSPIPS_HOLD_B_DRIVE_OPTION);
    //设置QSPI时钟的分频系数
    XQspiPs_SetClkPrescaler(QspiInstancePtr, XQSPIPS_CLK_PRESCALE_8);
    //片选信号置为有效
    XQspiPs_SetSlaveSelect(QspiInstancePtr);
    //读Flash ID
    FlashReadID();
    //使能Flash Quad模式
    FlashQuadEnable(QspiInstancePtr);
    //擦除Flash
    FlashErase(QspiInstancePtr, TEST_ADDRESS, MAX_DATA);
    //向Flash中写入数据
    for (Page = 0; Page < PAGE_COUNT; Page++) {
       FlashWrite(QspiInstancePtr, (Page * PAGE_SIZE) + TEST_ADDRESS,
              PAGE_SIZE, WRITE_CMD);
    }
    //使用QUAD模式从Flash中读出数据
    FlashRead(QspiInstancePtr, TEST_ADDRESS, MAX_DATA, QUAD_READ_CMD);

    //对比写入Flash与从Flash中读出的数据
    BufferPtr = &ReadBuffer[DATA_OFFSET + DUMMY_SIZE];
    for (UniqueValue = UNIQUE_VALUE, Count = 0; Count < MAX_DATA;
         Count++, UniqueValue++) {
        if (BufferPtr[Count] != (u8)(UniqueValue + Test)) {
             return XST_FAILURE;
        }
    }

  return XST_SUCCESS;
}

void FlashWrite(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command)
{
	u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
	u8 ReadStatusCmd[] = { READ_STATUS_CMD, 0 };  /* must send 2 bytes */
	u8 FlashStatus[2];

	/*
	 * Send the write enable command to the FLASH so that it can be
	 * written to, this needs to be sent as a seperate transfer before
	 * the write
	 */
	XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL,
				sizeof(WriteEnableCmd));


	/*
	 * Setup the write command with the specified address and data for the
	 * FLASH
	 */
	WriteBuffer[COMMAND_OFFSET]   = Command;
	WriteBuffer[ADDRESS_1_OFFSET] = (u8)((Address & 0xFF0000) >> 16);
	WriteBuffer[ADDRESS_2_OFFSET] = (u8)((Address & 0xFF00) >> 8);
	WriteBuffer[ADDRESS_3_OFFSET] = (u8)(Address & 0xFF);

	/*
	 * Send the write command, address, and data to the FLASH to be
	 * written, no receive buffer is specified since there is nothing to
	 * receive
	 */
	XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL,
				ByteCount + OVERHEAD_SIZE);

	/*
	 * Wait for the write command to the FLASH to be completed, it takes
	 * some time for the data to be written
	 */
	while (1) {
		/*
		 * Poll the status register of the FLASH to determine when it
		 * completes, by sending a read status command and receiving the
		 * status byte
		 */
		XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus,
					sizeof(ReadStatusCmd));

		/*
		 * If the status indicates the write is done, then stop waiting,
		 * if a value of 0xFF in the status byte is read from the
		 * device and this loop never exits, the device slave select is
		 * possibly incorrect such that the device status is not being
		 * read
		 */
		if ((FlashStatus[1] & 0x01) == 0) {
			break;
		}
	}
}
void FlashRead(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command)
{
	/*
	 * Setup the write command with the specified address and data for the
	 * FLASH
	 */
	WriteBuffer[COMMAND_OFFSET]   = Command;
	WriteBuffer[ADDRESS_1_OFFSET] = (u8)((Address & 0xFF0000) >> 16);
	WriteBuffer[ADDRESS_2_OFFSET] = (u8)((Address & 0xFF00) >> 8);
	WriteBuffer[ADDRESS_3_OFFSET] = (u8)(Address & 0xFF);

	if ((Command == FAST_READ_CMD) || (Command == DUAL_READ_CMD) ||
	    (Command == QUAD_READ_CMD)) {
		ByteCount += DUMMY_SIZE;
	}
	/*
	 * Send the read command to the FLASH to read the specified number
	 * of bytes from the FLASH, send the read command and address and
	 * receive the specified number of bytes of data in the data buffer
	 */
	XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, ReadBuffer,
				ByteCount + OVERHEAD_SIZE);
}

void FlashErase(XQspiPs *QspiPtr, u32 Address, u32 ByteCount)
{
	u8 WriteEnableCmd = { WRITE_ENABLE_CMD };
	u8 ReadStatusCmd[] = { READ_STATUS_CMD, 0 };  /* must send 2 bytes */
	u8 FlashStatus[2];
	int Sector;

	/*
	 * If erase size is same as the total size of the flash, use bulk erase
	 * command
	 */
	if (ByteCount == (NUM_SECTORS * SECTOR_SIZE)) {
		/*
		 * Send the write enable command to the FLASH so that it can be
		 * written to, this needs to be sent as a seperate transfer
		 * before the erase
		 */
		XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL,
				  sizeof(WriteEnableCmd));

		/* Setup the bulk erase command*/
		WriteBuffer[COMMAND_OFFSET]   = BULK_ERASE_CMD;

		/*
		 * Send the bulk erase command; no receive buffer is specified
		 * since there is nothing to receive
		 */
		XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL,
					BULK_ERASE_SIZE);

		/* Wait for the erase command to the FLASH to be completed*/
		while (1) {
			/*
			 * Poll the status register of the device to determine
			 * when it completes, by sending a read status command
			 * and receiving the status byte
			 */
			XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd,
						FlashStatus,
						sizeof(ReadStatusCmd));

			/*
			 * If the status indicates the write is done, then stop
			 * waiting; if a value of 0xFF in the status byte is
			 * read from the device and this loop never exits, the
			 * device slave select is possibly incorrect such that
			 * the device status is not being read
			 */
			if ((FlashStatus[1] & 0x01) == 0) {
				break;
			}
		}

		return;
	}

	/*
	 * If the erase size is less than the total size of the flash, use
	 * sector erase command
	 */
	for (Sector = 0; Sector < ((ByteCount / SECTOR_SIZE) + 1); Sector++) {
		/*
		 * Send the write enable command to the SEEPOM so that it can be
		 * written to, this needs to be sent as a seperate transfer
		 * before the write
		 */
		XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL,
					sizeof(WriteEnableCmd));

		/*
		 * Setup the write command with the specified address and data
		 * for the FLASH
		 */
		WriteBuffer[COMMAND_OFFSET]   = SEC_ERASE_CMD;
		WriteBuffer[ADDRESS_1_OFFSET] = (u8)(Address >> 16);
		WriteBuffer[ADDRESS_2_OFFSET] = (u8)(Address >> 8);
		WriteBuffer[ADDRESS_3_OFFSET] = (u8)(Address & 0xFF);

		/*
		 * Send the sector erase command and address; no receive buffer
		 * is specified since there is nothing to receive
		 */
		XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL,
					SEC_ERASE_SIZE);

		/*
		 * Wait for the sector erse command to the
		 * FLASH to be completed
		 */
		while (1) {
			/*
			 * Poll the status register of the device to determine
			 * when it completes, by sending a read status command
			 * and receiving the status byte
			 */
			XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd,
						FlashStatus,
						sizeof(ReadStatusCmd));

			/*
			 * If the status indicates the write is done, then stop
			 * waiting, if a value of 0xFF in the status byte is
			 * read from the device and this loop never exits, the
			 * device slave select is possibly incorrect such that
			 * the device status is not being read
			 */
			if ((FlashStatus[1] & 0x01) == 0) {
				break;
			}
		}

		Address += SECTOR_SIZE;
	}
}

int FlashReadID(void)
{
	int Status;

	/* Read ID in Auto mode.*/
	WriteBuffer[COMMAND_OFFSET]   = READ_ID;
	WriteBuffer[ADDRESS_1_OFFSET] = 0x23;		/* 3 dummy bytes */
	WriteBuffer[ADDRESS_2_OFFSET] = 0x08;
	WriteBuffer[ADDRESS_3_OFFSET] = 0x09;

	Status = XQspiPs_PolledTransfer(&QspiInstance, WriteBuffer, ReadBuffer,
				RD_ID_SIZE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	xil_printf("FlashID=0x%x 0x%x 0x%x\n\r", ReadBuffer[1], ReadBuffer[2],
		   ReadBuffer[3]);

	return XST_SUCCESS;
}
void FlashQuadEnable(XQspiPs *QspiPtr)
{
	u8 WriteEnableCmd = {WRITE_ENABLE_CMD};
	u8 ReadStatusCmd[] = {READ_STATUS_CMD, 0};
	u8 QuadEnableCmd[] = {WRITE_STATUS_CMD, 0};
	u8 FlashStatus[2];


	if (ReadBuffer[1] == 0x9D) {

		XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd,
					FlashStatus,
					sizeof(ReadStatusCmd));

		QuadEnableCmd[1] = FlashStatus[1] | 1 << 6;

		XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL,
				  sizeof(WriteEnableCmd));

		XQspiPs_PolledTransfer(QspiPtr, QuadEnableCmd, NULL,
					sizeof(QuadEnableCmd));
	}
}

