#include "lpc24xx.h"
#include "MCI.h"
/*	Multimedia Card Interface provides functions to generate clock, manage power
    data transfer and command with SD Card. Also control interrupts
    MCICLK Output Clock output
    MCICMD Input Command input/output.
    MCIPWR Output Power Supply Enable for external SD/MMC power supply
    
    CLK
    CMD
    DAT[3:0]
    
    //CRC cyclic redundancy check
    
    //Command path subunit comands to and receives responses from card    


*/


volatile unsigned long CardRCA;



void MCI_Init()
{

    int i;
    PCONP |= (1 <<28);
    //turn of everything before intialising
    if (MCI_CLOCK & (1 << 8))
    {
        MCI_CLOCK &= ~(1 <<8);
    }
    if (MCI_POWER & (0x02))
    {
        MCI_POWER &= ~0x02;
    }
    
    //disable all interrupts
    MCI_MASK0 = 0;
    MCI_MASK1 = 0;
    
    //Set I/O pins
    //MCICLK PIN P1[2] P0[19]
    PINSEL2 &= ~(1<<4);
    PINSEL2 |= 	1<<5  ;
    //MCICMD PIN P1[3]
    PINSEL2 &= ~(1<<6);
    PINSEL2 |= 	1<<7  ;
    //MCIPWR PIN P1[5]
    PINSEL2 &= ~(1<<10);
    PINSEL2 |= 	1<<11  ;
    //MCIDAT0 PIN P1[6]
    PINSEL2 &= ~(1<<12);
    PINSEL2 |= 	1<<13  ;
    //MCIDATA1 PIN P1[7]
    PINSEL2 &= ~(1<<14);
    PINSEL2 |= 	1<<15  ;
    //MCIDATA2 PIN P1[11]
    PINSEL2 &= ~(1<<22);
    PINSEL2 |= 	1<<23  ;
    //MCIDATA3 PIN P1[12]
    PINSEL2 &= ~(1<<24);
    PINSEL2 |= 	1<<25  ;
    
    
    
    //power on the MCI external power supply
    SCS |= (1<<3);
    
    MCI_COMMAND = 0;    
    for (i = 0; i< 0x100; i++);
    
    MCI_DATA_CTRL = 0;
    for (i = 0; i< 0x100; i++);
    
    MCI_CLEAR = 0x7FF;
    
    //Set to power up mode
    MCI_POWER = 0x02;
    //wait for power up
    while (!(MCI_POWER & 0x02))
    {
        for (i = 0; i< 0x100; i++);
    }
    /*
        During identification phase, the clock should be less than 400KHz. Then
        the normal clock can be set to 25Mhz		
    */
    MCI_CLOCK &= ~(0xFF);    
    
    
    MCI_CLOCK |= (1<<8) | MCLKDIV_SLOW;
    
    
    //power on
    MCI_POWER  |= 0x03;
    //wait for power on
    for (i = 0; i<0x2000 ;i++);
    
    /*
        Setup IRQ MCI interrupt source is 24
        .p118 of LPC24XX pdf
    */
    
    //Set Interrupt Vector
    VICVectAddr24 = (unsigned long)MCI_IRQHandler;
    //Set Interrupt Priority to highest
    VICVectPriority24 = 0;
    //Enable Interrupt
    VICIntEnable |= (1 << 24);    
    
    
    //Enable power save disables clock when bus idle
    MCI_CLOCK |= (1 << 9);
    
        
    
    
}


void MCI_IRQHandler (void) __irq
{
    unsigned long MCI_Status;
	MCI_Status = MCI_STATUS;
		
    if (MCI_Status & TX_DATA_AVAIL)
    {
        //write 1 block
        MCI_Send_Write_Block (1);
        VICVectAddr = 0;
    }

}


void MCI_SendCMD (int CMDIndex, unsigned long Argument, int ExpectResp, int AllowTimeout)
{
	int i;
    unsigned long CmdData, CmdStatus;
    //must diSable CPSM before modifying arguments
    while ((CmdStatus = MCI_STATUS) & CMD_ACTIVE)
    {
        MCI_COMMAND = 0;
        MCI_CLEAR = CmdStatus | CMD_ACTIVE;
    }
    //wait 3 MLCK + 2 PLCK
    for (i = 0; i <0x100; i++);
    //Command index
    CmdData |= CMDIndex;
    if (ExpectResp == EXPECT_NO_RESP)
    {
        //clear short resp and long resp bits
        CmdData &= ~((1 << 6) | (1 << 7));
    }
    else if (ExpectResp == EXPECT_SHORT_RESP)
    {
        //clear long resp and set short resp bits
        CmdData |= (1 << 6);
        CmdData &= ~(1 << 7);
    }
    else if (ExpectResp == EXPECT_LONG_RESP)
    {
        //set long resp and clear short resp bits
        CmdData |= (1 << 7);
        CmdData &= ~(1 << 6);
    }
    if ( AllowTimeout)
    {
        CmdData |= (1 << 8);
    }
    else
    {
        CmdData &= ~(1 << 8);
    }
    
    //Enabe CPSM last
    CmdData |= (1 <<10);
    MCI_ARGUMENT = Argument;
    MCI_COMMAND = CmdData;
    //wait
    for (i = 0; i <0x100; i++);
}

	


int MCI_SET_ADD (void)
{
  
    unsigned long respStatus;
    unsigned long respValue[4];
    
    
    MCI_SendCMD(SEND_RELATIVE_ADDR, 0, EXPECT_SHORT_RESP, 0); 
    respStatus = MCI_GetCmdResp(SEND_RELATIVE_ADDR, EXPECT_SHORT_RESP,(unsigned long*)&respValue[0]);
    CardRCA = respValue[0] & 0xFFFF0000;
    
    return 1;
}


unsigned long MCI_GetCmdResp (int ExpectCmdData, int ExpectResp, unsigned long *CmdResp)
{
    int i;
    unsigned long CmdRespStatus = 0;
    
        
    if (ExpectResp == EXPECT_NO_RESP)
    {
        return 0;
    }
    
    CmdRespStatus = MCI_STATUS;
    if (CmdRespStatus & CMD_RESP_END)
    {
        MCI_CLEAR = CmdRespStatus | CMD_RESP_END;
        
    }
    //wait
    for (i =0; i< 0x100; i++);
    
    *CmdResp = MCI_RESP0;
    return 0;
    
}

//send ACMD

int MCI_Send_ACMD (void)
{
    
    unsigned long CmdArgument;
    unsigned long respStatus;
    unsigned long respValue[4];
    
    CmdArgument = CardRCA;
    
    MCI_SendCMD(APP_CMD,CmdArgument, EXPECT_SHORT_RESP,0);
    respStatus = MCI_GetCmdResp(APP_CMD, EXPECT_SHORT_RESP, (unsigned long*)&respValue[0]);
    
    //check if APP_CMD enabled
    if  (!respStatus && (respValue[0] & CARD_STATUS_ACMD_ENABLE))
    {
        return 1;
    }
    
    else        
        return 0;    
    
}

//Send status
unsigned long MCI_Send_Status(void)
{
    int i ;
    unsigned long CmdArgument;
    unsigned long respStatus;
    unsigned long respValue[4];
    
    CmdArgument = CardRCA;
    
    //clear status before reading again
    MCI_CLEAR |= CMD_RESP_END;
    
    //wait
    for (i =0; i< 0x100; i++);
    
    MCI_SendCMD(SEND_STATUS, CmdArgument, EXPECT_SHORT_RESP, 0);
    respStatus = MCI_GetCmdResp(SEND_STATUS, EXPECT_SHORT_RESP, (unsigned long*)&respValue[0]);
    
    if (!respStatus && (respValue[0] & (1 << 8)))
    {
        return respValue[0];
    }
    return 0; 
    
}

//set blocklen before read and write

int MCI_Set_BlockLen( unsigned long blockLength)
{
    int i ;   
    unsigned long respStatus;
    unsigned long respValue[4];    
    
    
    //clear status before reading again
    MCI_CLEAR |= CMD_RESP_END;
    
    //wait
    for (i =0; i< 0x100; i++);
    
    MCI_SendCMD(SET_BLOCK_LEN, blockLength, EXPECT_SHORT_RESP, 0);
    respStatus = MCI_GetCmdResp(SET_BLOCK_LEN, EXPECT_SHORT_RESP, (unsigned long*)&respValue[0]);
    
    //bit 12:9 should be in trans state
    if (!respStatus && ((respValue[0] & (0x0F << 8)) == 0x900))
    {
        return 1;
    }
    return 0; 
}

//set bus width
int MCI_Send_ACMD_Bus_Width (int buswidth)
{
    
      
    unsigned long respStatus;
    unsigned long respValue[4];    
    
    if (MCI_Send_ACMD() == 0)
    {
        return 0;
    }
    
    MCI_SendCMD(SET_ACMD_BUS_WIDTH, buswidth, EXPECT_SHORT_RESP, 0);
    respStatus = MCI_GetCmdResp(SET_ACMD_BUS_WIDTH, EXPECT_SHORT_RESP, (unsigned long*)&respValue[0]);
    
    //bit 12:9 should be in trans state  
    if (!respStatus && ((respValue[0] & (0x0F << 8)) == 0x900))
    {
        return 1;
    }
    return 0; 
}



//send write block
int MCI_Send_Write_Block (int blockNum)
{
      
    unsigned long respStatus;
    unsigned long respValue[4];    
    
    MCI_CLEAR = 0x7FF;
    
    MCI_SendCMD(WRITE_BLOCK, blockNum * BLOCK_LENGTH, EXPECT_SHORT_RESP, 0);
    respStatus = MCI_GetCmdResp(WRITE_BLOCK, EXPECT_SHORT_RESP, (unsigned long*)&respValue[0]);
    
    //bit 12:9 should be in trans state  
    if (!respStatus && ((respValue[0] & (0x0F << 8)) == 0x900))
    {
        return 1;
    }
    return 0; 
}

//write block

int MCI_Write_Block (int blockNum)
{
    int i ;
    MCI_CLEAR = 0x7FF;
    MCI_DATA_CTRL = 0;
    //wait
    for (i =0; i< 0x100; i++);
    
    //timeout period
    MCI_DATA_TMR = DATA_TIMER_VALUE;
    //number of bytes to be transferred
    MCI_DATA_LEN = BLOCK_LENGTH;
    
    
    
    if (MCI_Send_Write_Block(blockNum)== 0 )
    {
        return 0;
    }
    
    //block transfer mode
    MCI_DATA_CTRL &= ~(1<<3);
    //wait
    for (i =0; i< 0x100; i++);
    
    //enable transfer and set direction and block length
    MCI_DATA_CTRL |= ((1 << 1) | (1 <<0) |(DATA_BLOCK_LEN <<4));
    //wait
    for (i =0; i< 0x100; i++);
    
    //turn on txfifo int
    MCI_MASK0 |= TX_DATA_AVAIL;
    MCI_MASK1 |= TX_DATA_AVAIL;
    
    return 1;
    
}
    




