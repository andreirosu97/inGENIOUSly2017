/*
 * rc522.c

 *
 *  Created on: 05/05/2015
 *      Author: Henry1
 */
#include <cstdio>
#include "rc522.h"
#include <string>
#include <wiringPiSPI.h>

#define HARWARE_SPI

#define SET_SPI_CS  IOPut(o3,on);
#define CLR_SPI_CS  IOPut(o3,off);
#define SET_RC522RST  IOPut(o4,on);
#define CLR_RC522RST  IOPut(o4,off);


void ClearBitMask(unsigned char reg,unsigned char mask);
void WriteRawRC(unsigned char Address, unsigned char value);
void SetBitMask(unsigned char reg,unsigned char mask);
char PcdComMF522(unsigned char Command,
                 unsigned char *pInData,
                 unsigned char InLenByte,
                 unsigned char *pOutData,
                 unsigned int  *pOutLenBit);
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData);
unsigned char ReadRawRC(unsigned char Address);
void MFRC522_AntennaOn(void);

void delay_ns(unsigned int16 ns)
{

  delayMicroseconds(ns / 10);
  /*uint i;
  for(i=0;i<ns;i++)
  {
    asm volatile ("nop" :: );
  }*/
}


char MFRC522_Request(unsigned char req_code,unsigned char *pTagType)
{
	char status;
	unsigned int unLen;
	unsigned char ucComMF522Buf[MAXRLEN];

	ClearBitMask(Status2Reg,0x08);
	WriteRawRC(BitFramingReg,0x07);
	SetBitMask(TxControlReg,0x03);

	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

	if ((status == MI_OK) && (unLen == 0x10))
	{
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   status = MI_ERR;   }

	return status;
}

  /*********************************************************************
    * Function:        char MFRC522_Anticoll(unsigned char *pSnr)
    *
    * PreCondition:     none
    *
    * Input:		    unsigned char *pSnr
    *
    * Output:		    return MI_OK if success
    *               return the 4 bytes serial number
    *
    * Side Effects:	    none
    *
    * Overview:		    This function prevent conflict and return the 4 bytes serial number
    *
    * Note:			    None
    ********************************************************************/
pair<char, string> MFRC522_Anticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];


    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    ucComMF522Buf[unLen] = '\0';

    SetBitMask(CollReg,0x80);
    return make_pair(status, string(ucComMF522Buf));
}

/*********************************************************************
    * Function:       char MFRC522_Select(unsigned char *pSnr)
    *
    * PreCondition:     SPI has been configured
    *
    * Input:		    unsigned char *pSnr
    *
    * Output:		    char - return MI_OK if success
    *
    *
    * Side Effects:	    none
    *
    * Overview:
    *
    * Note:			    None
    ********************************************************************/
char MFRC522_Select(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);

    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

 /*********************************************************************
    * Function:   char MFRC522_AuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
    *
    * PreCondition:     none
    *
    * Input:		    unsigned char auth_mode -   Password Authentication Mode
    *                                        0x60 = A key authentication
                                             0x61 = B key authentication
    *               unsigned char addr  -      Block Address
    *               unsigned char *pKey  -     Sector Password
    *               unsigned char *pSnr  -    4 bytes serial number
    *
    * Output:		    char - return MI_OK if success
    *
    *
    * Side Effects:	    none
    *
    * Overview:		    This function verify card password
    *
    * Note:			    None
    ********************************************************************/
char MFRC522_AuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
    //memcpy(&ucComMF522Buf[2], pKey, 6);
    //memcpy(&ucComMF522Buf[8], pSnr, 4);

    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }

    return status;
}

 /*********************************************************************
    * Function:        char MFRC522_Read(unsigned char addr, unsigned char *pData)
    *
    * PreCondition:     none
    *
    * Input:		    unsigned char addr   - block address
    *               unsigned char *pData  - block data
    *
    * Output:		    char - return MI_OK if success
    *
    * Side Effects:	    none
    *
    * Overview:		    This function read block data
    *
    * Note:			    None
    ********************************************************************/
char MFRC522_Read(unsigned char addr, unsigned char *pData)
{
    char  status;
    unsigned int unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
   // {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }

    return status;
}

 /*********************************************************************
    * Function:      char MFRC522_Write(unsigned char addr,unsigned char *pData)
    *
    * PreCondition:     SPI has been configured
    *
    * Input:		    unsigned char addr  - block address
    *               unsigned char *pData  - data to write
    *
    * Output:		    char - return MI_OK if success
    *
    *
    * Side Effects:	    none
    *
    * Overview:		    This function write a block of data to addr
    *
    * Note:			    None
    ********************************************************************/
char MFRC522_Write(unsigned char addr,unsigned char *pData)
{
    unsigned char status;
    unsigned int unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    if (status == MI_OK)
    {
       // memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {
        	ucComMF522Buf[i] = *(pData+i);
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }

    return status;
}

/*
 * Function：MFRC522_Halt
 * Description：Command the cards into sleep mode
 * Input parameters：null
 * return：MI_OK
 */
 /*********************************************************************
    * Function:       char MFRC522_Halt(void)
    *
    * PreCondition:     none
    *
    * Input:		    none
    *
    * Output:		    none
    *
    * Side Effects:	    none
    *
    * Overview:		    This function command the cards into sleep mode
    *
    * Note:			    None
    ********************************************************************/
char MFRC522_Halt(void)
{
    char status;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

 /*********************************************************************
    * Function:   void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
    *
    * PreCondition:     none
    *
    * Input:		    unsigned char *pIndata - input datas
    *               unsigned char len       - data length
    *               unsigned char *pOutData  - output data
    *
    * Output:		    unsigned char - 2 bytes CRC result
    *
    * Side Effects:	    none
    *
    * Overview:		    This function calculate the CRC
    *
    * Note:			    None
    ********************************************************************/
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/*********************************************************************
    * Function:        char MFRC522_Reset(void)
    *
    * PreCondition:     none
    *
    * Input:		    none
    *
    * Output:		    return MI_OK
    *
    * Side Effects:	    reset the RC522
    *
    * Overview:		    This function reset the RC522
    *
    * Note:			    None
    ********************************************************************/
char MFRC522_Reset(void)
{
  	SET_RC522RST;
    delay_ns(10);
	  CLR_RC522RST;
    delay_ns(10);
	  SET_RC522RST;
    delay_ns(10);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_ns(10);
    WriteRawRC(ModeReg,0x3D);
    WriteRawRC(TReloadRegL,30);
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
	  WriteRawRC(TxAutoReg,0x40);
    return MI_OK;
}
   /*********************************************************************
    * Function:        char MFRC522_ConfigISOType(unsigned char type)
    *
    * PreCondition:     none
    *
    * Input:		    unsigned char type
    *
    * Output:		  return MI_OK if type == 'A'
    *
    * Side Effects:	    none
    *
    * Overview:		    This function configure the ISO type
    *
    * Note:			    None
    ********************************************************************/
char MFRC522_ConfigISOType(unsigned char type)
{
   if (type == 'A')
   {
       ClearBitMask(Status2Reg,0x08);
       WriteRawRC(ModeReg,0x3D);
       WriteRawRC(RxSelReg,0x86);
       WriteRawRC(RFCfgReg,0x7F);
   	   WriteRawRC(TReloadRegL,30);
	     WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
	     WriteRawRC(TPrescalerReg,0x3E);
	     delay_ns(1000);
       MFRC522_AntennaOn();
   }
   else{ return -1; }

   return MI_OK;
}
   /*********************************************************************
    * Function:        unsigned char ReadRawRC(unsigned char Address)
    *
    * PreCondition:     SPI has been configured
    *
    * Input:		    unsigned char Address - register address
    *
    * Output:		    return a byte of data read from the register
    *
    * Side Effects:	    none
    *
    * Overview:		    This function read a byte of data register
    *
    * Note:			    None
    ********************************************************************/

void Write_MFRC522 (unsigned char addr, unsigned char val) {
	uint8_t cmd = ((addr << 1) & 0x7E);
  wiringPiSPIDataRW(0, (uint8_t *) &cmd, 1)
}



unsigned char ReadRawRC(unsigned char addr)
{
    uint8_t cmd[2];
    cmd[0] = ((addr << 1) & 0x7E) | 0x80;
    wiringPiSPIDataRW(0, (uint8_t *) &cmd, 1)
  	return cmd[0];
}

   /*********************************************************************
    * Function:        void WriteRawRC(unsigned char Address, unsigned char value)
    *
    * PreCondition:     none
    *
    * Input:		    unsigned char Address  -   register address
    *               unsigned char value    -    the value to be written
    *
    * Output:		    none
    *
    *
    * Side Effects:	    none
    *
    * Overview:		    This function write a byte into a given register
    *
    * Note:			    None
    ********************************************************************/
void WriteRawRC(unsigned char addr, unsigned char value)
{
    uint8_t cmd = ((addr << 1) & 0x7E);
    wiringPiSPIDataRW(0, (uint8_t *) &cmd, 1);
    wiringPiSPIDataRW(0, value, 1)
}

   /*********************************************************************
    * Function:        void SetBitMask(unsigned char reg,unsigned char mask)
    *
    * PreCondition:     none
    *
    * Input:		   unsigned char reg  - register address
    *              unsigned char mask - bit mask
    *
    * Output:		    none
    *
    * Side Effects:	    none
    *
    * Overview:		    This function set a bit mask on RC522 register
    *
    * Note:			    None
    ********************************************************************/
void SetBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

   /*********************************************************************
    * Function:        void ClearBitMask(unsigned char reg,unsigned char mask)
    *
    * PreCondition:     none
    *
    * Input:		    unsigned char reg - register address
    *               unsigned char mask - set value
    *
    * Output:		    none
    *
    * Side Effects:	    none
    *
    * Overview:		    This function clear a bit mask on RC522 register
    *
    * Note:			    None
    ********************************************************************/
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char  tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

   /*********************************************************************
    * Function:        char PcdComMF522(unsigned char Command,
    *                                    unsigned char *pInData,
    *                                     unsigned char InLenByte,
    *                                      unsigned char *pOutData,
    *                                       unsigned int *pOutLenBit)
    *
    * PreCondition:     SPI has been configured
    *
    * Input:		    unsigned char Command   -  command type
    *               unsigned char *pInData  -  input data
    *               unsigned char InLenByte -  input data length
    *               unsigned char *pOutData -  output Data
    *               unsigned int *pOutLenBit -  output data length
    *
    * Output:		    char - return MI_OK if success
    *
    *
    * Side Effects:	    none
    *
    * Overview:		    This function search card and return card types
    *
    * Note:			    None
    ********************************************************************/
char PcdComMF522(unsigned char Command,
                 unsigned char *pInData,
                 unsigned char InLenByte,
                 unsigned char *pOutData,
                 unsigned int *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
        case PCD_AUTHENT:
			irqEn   = 0x12;
			waitFor = 0x10;
			break;
		case PCD_TRANSCEIVE:
			irqEn   = 0x77;
			waitFor = 0x30;
			break;
		default:
			break;
    }

    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);

    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }
    WriteRawRC(CommandReg, Command);


    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }

	i = 2000;
    do
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0)
    {
        if(!(ReadRawRC(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   status = MI_NOTAGERR;   }
            if (Command == PCD_TRANSCEIVE)
            {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg);    }
            }
        }
        else
        {   status = MI_ERR;   }

    }
    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE);
    return status;
}

/*********************************************************************
    * Function:        void MFRC522_AntennaOn(void)
    *
    * PreCondition:     none
    *
    * Input:		    none
    *
    * Output:		    none
    *
    * Side Effects:	    Antenna On
    *
    * Overview:	  	This function command the RC522 to switch on the antenna
    *
    * Note:			    None
    ********************************************************************/
void MFRC522_AntennaOn(void)
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

/*********************************************************************
    * Function:        void MFRC522_AntennaOff(void)
    *
    * PreCondition:     none
    *
    * Input:		   none
    *
    * Output:		    none
    *
    * Side Effects:	  Antenna Off
    *
    * Overview:		 This function command the RC522 to switch off the antenna
    *
    * Note:			    None
    ********************************************************************/
void MFRC522_AntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}
