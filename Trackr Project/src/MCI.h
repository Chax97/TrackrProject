/*
		Referenced from Physical Layer Simplified Specification Ver 8.00 (PLSS)
		and the	LPC24XX User Manual

*/

#ifndef MCI_h
#define MCI_h
#include "lpc24xx.h"



// SD/MMC Command List, SD Memory Card. SImplified Version .P97 PLSS
#define	G0_IDLE_STATE 0  	//
#define	ALL_SEND_CID 	2		//R2
#define SEND_RELATIVE_ADDR 3 //R6
#define	SET_DSR 4					
#define SET_ACMD_BUS_WIDTH 6 //R1
#define	SELECT_CARD 7			//R1B
#define	SEND_CSD 9	//R2
#define	STOP_TRANSMISSION 12 //R1B
#define	SEND_STATUS 13 //R1
#define	SET_BLOCK_LEN 16 //R1
// SDHC, SDXC SDUC Cards, block length is fixed 512 bytes
#define	READ_SINGLE_BLOCK 17	//R1
#define READ_MULTIPLE_BLOCK 18 //R1
#define	WRITE_BLOCK	24 //R1
#define SEND_APP_OP_COND 41 //R3
#define APP_CMD 55 //R1

#define OCR_PWR_STATUS	1<<31

#define CARD_STATUS_ACMD_ENABLE	1<<5
#define CARD_STATUS_RDY_DATA 1<< 8
#define CARD_STATUS_CURRENT_STATE 0xF << 9
#define CARD_STATUS_ERASE_RESET 1 << 13
//6MHz MCLKDIV
#define MCLKDIV_NORMAL 0x4-1 
//IDENTIFICATION STATE use slow clk
#define MCLKDIV_SLOW 0x60-1 


//Must write to DataTimer and data length before data ctrl reg
#define DATA_TIMER_VALUE 0x10000
//bc response
#define EXPECT_NO_RESP 0
#define EXPECT_SHORT_RESP 1
#define EXPECT_LONG_RESP 2

/*To use the 4 bit bus instead of 1 bit, need to use ACMD 6
	But to use ACMD6
*/
#define BUS_WIDTH_4BITS 10

/*	Data block length minimum is 512 for SD card
		.p98 PLSS pdf
*/
#define DATA_BLOCK_LEN 9
#define BLOCK_LENGTH (1 << DATA_BLOCK_LEN)
#define BLOCK_NUM 0x80
#define FIFO_SIZE 16

#define MCI_INT 24

//MCI Status register bit mask
#define CMD_RESP_END (1 << 6)
#define	CMD_SENT (1 << 7)
#define	DATA_END (1 << 8 )
#define DATA_BLK_END (1 << 10)
#define CMD_ACTIVE (1 << 11)
#define TX_ACTIVE (1 << 12)
#define TX_FIFO_FULL (1 << 16)
#define TX_DATA_AVAIL (1 << 21)
#define TX_HALF_EMPTY (1 << 15) 


int MCI_Send_ACMD (void);

unsigned long MCI_Send_Status(void);

int MCI_Send_Write_Block (int blockNum);

unsigned long MCI_GetCmdResp(int ExpectCmdData, int ExpectResp, unsigned long *CmdResp);

void MCI_SendCMD (int CMDIndex, unsigned long Argument, int ExpectResp, int AllowTimeout);

void MCI_Init(void);
	
void MCI_IRQHandler (void) __irq;

int MCI_SET_ADD (void);

int MCI_Write_Block (int blockNum);

int MCI_Set_BlockLen( unsigned long blockLength);

#endif
