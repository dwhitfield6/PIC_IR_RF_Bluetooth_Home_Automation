/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_Smart_Rf'.
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
/******************************************************************************/

/******************************************************************************/
/* Contains functions that controm the interal EEPROM memory.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "EEPROM.h"
#include "UART.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* ReadEEPROM_1Byte
 *
 * The function returns the 8 bit value stored at the 10 bit memory address.
/******************************************************************************/
unsigned int ReadEEPROM_1Byte(unsigned int address)
{
    unsigned char GIEstatus = INTCONbits.GIE;//read GIE setting
    EECON1bits.EEPGD = 0; // EEPROM memory
    EECON1bits.CFGS = 0;  // EEPROM
    EEADRH = (unsigned char)((address & 0x300) >> 8);
    EEADR = (unsigned char)(address & 0xFF);
    INTCONbits.GIE =0;//disable global interrupts
    EECON1bits.RD = 1;
    while(EECON1bits.RD);
    NOP();
    NOP();
    if(GIEstatus)
    {
        INTCONbits.GIE =1;//enable global interrupts
    }
    return (EEDATA);
}

/******************************************************************************/
/* EEPROM_UNLOCK
 *
 * The function sends the unlock sequence required for modifying the internal
 *  EEPROM.
/******************************************************************************/
void EEPROM_UNLOCK(void)
{
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR =1;
    while(EECON1bits.WR);
    NOP();
    NOP();
}

/******************************************************************************/
/* WriteEEPROM_1Byte
 *
 * The function writes one byte of data to the EEPROM at the 10 bit address.
/******************************************************************************/
void WriteEEPROM_1Byte(unsigned int address, unsigned char data)
{
    unsigned char GIEstatus = INTCONbits.GIE;//read GIE setting
    EEADRH = (unsigned char)((address & 0x300) >> 8);
    EEADR = (unsigned char)(address & 0xFF);
    EEDATA = data;
    EECON1bits.EEPGD = 0; // EEPROM memory
    EECON1bits.CFGS = 0;  // EEPROM
    EECON1bits.WREN =1;
    INTCONbits.GIE =0;//disable global interrupts
    EEPROM_UNLOCK();
    EECON1bits.WR =1;
    while(EECON1bits.WR);
    EECON1bits.WREN =0;
    if(GIEstatus)
    {
        INTCONbits.GIE =1;//enable global interrupts
    }
}

/******************************************************************************/
/* GetEEPROM
 *
 * The function gets the EEdata struct from memory.
/******************************************************************************/
void GetEEPROM(GBLdata *Temp)
{
    unsigned char i,j,k;
    unsigned char Arrayspot=EE_RemoteButtonNEC;

    Temp->BlueToothFlag          = GetMemoryChar(EE_BlueToothFlag);
    Temp->SWNECcode              = GetMemoryLong(EE_NECcodeBYTE1);
    for(i=0; i< ButtonAmount; i++)
    {
        for(j=0; j < 2; j++)
        {
            Temp->RemoteButtonNEC[i][j] = GetMemoryChar(Arrayspot++);
        }
    }
    Temp->EEPROMinitFlag         = GetMemoryChar(EE_EEPROMinitFlag);
    Arrayspot=EE_RemoteButtonRF;
    for(i=0; i< RFcodesAmount; i++)
    {
        for(j=0; j < MirrorButtonsAmount; j++)
        {
            for(k=0; k < 2; k++)
            {
                Temp->RemoteButtonRF[i][j][k] = GetMemoryChar(Arrayspot++);
            }
        }
    }    
}

/******************************************************************************/
/* SetEEPROM
 *
 * The function burns the EEdata struct to memory.
/******************************************************************************/
unsigned long SetEEPROM(GBLdata Temp,unsigned long burn)
{
    unsigned long fail = 0;
    unsigned char i,j,k,temp;
    unsigned char Arrayspot=EE_RemoteButtonNEC;

    if(burn & 0x00000001)
    {
        if(!SetMemoryChar(Temp.BlueToothFlag,EE_BlueToothFlag))
        {
            /* Failed to burn BlueToothFlag */
            fail |= 0x00000001;
        }
    }
    if(burn & 0x00000002)
    {
        if(!SetMemoryLong(Temp.SWNECcode,EE_NECcodeBYTE1))
        {
            /* Failed to burn NECcode */
            fail |= 0x00000002;
        }
    }
    if(burn & 0x00000004)
    {
        for(i=0; i< ButtonAmount; i++)
        {
            for(j=0; j < 2; j++)
            {
                temp = Temp.RemoteButtonNEC[i][j];
                if(!SetMemoryChar(temp,Arrayspot++))
                {
                    /* Failed to burn RemoteButtonNEC */
                    fail |= 0x00000004;
                }
            }
        }
    }
    if(burn & 0x00000008)
    {
        if(!SetMemoryChar(Temp.EEPROMinitFlag,EE_EEPROMinitFlag))
        {
            /* Failed to burnEEPROMinitFlag */
            fail |= 0x00000008;
        }
    }
    if(burn & 0x00000010)
    {
        Arrayspot=EE_RemoteButtonRF;
        for(i=0; i< RFcodesAmount; i++)
        {
            for(j=0; j < MirrorButtonsAmount; j++)
            {
                for(k=0; k < 2; k++)
                {
                    temp = Temp.RemoteButtonRF[i][j][k];
                    if(!SetMemoryChar(temp,Arrayspot++))
                    {
                        /* Failed to burn RemoteButtonRF */
                        fail |= 0x00000010;
                    }
                }
            }
        }
    }
    return fail;
}

/******************************************************************************/
/* SyncEEPROMandGLOBAL
 *
 * The function saves the global data to the EEPROM.
/******************************************************************************/
unsigned char SyncGlobalToEEPROM(void)
{
    /* Burn all memory */
    if(!SetEEPROM(Global,0xFFFFFFFF))
    {
        return PASS;
    }
    return FAIL;
}

/******************************************************************************/
/* SetEEPROMdefault
 *
 * The function sets the EEPROM and global struct to the default values if the
 *  EEPROm flag is not set.
/******************************************************************************/
void SetEEPROMdefault(void)
{
    unsigned char i,j,k;

    Global.BlueToothFlag = 0;
    Global.SWNECcode = 0x00FF00FF; // address is 0 and command is 0
    Global.EEPROMinitFlag = 0;
    for(i=0; i< ButtonAmount; i++)
    {
        for(j=0; j < 2; j++)
        {
            Global.RemoteButtonNEC[i][j] = 0x00;
        }
    }
    for(i=0; i< RFcodesAmount; i++)
    {
        for(j=0; j < MirrorButtonsAmount; j++)
        {
            for(k=0; k < 2; k++)
            {
                Global.RemoteButtonRF[i][j][k] = 0x00;
            }
        }
    }
    SyncGlobalToEEPROM();
}

/******************************************************************************/
/* EEPROMinitialized
 *
 * The function returns true if the EEPROM has been initialized before,
 *  otherwise is returns false.
/******************************************************************************/
unsigned char EEPROMinitialized(void)
{
    if(Global.EEPROMinitFlag == EEPROMinitilized)
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
/* SyncEEPROMToGlobal
 *
 * The function syncs the EEPROM to the global data.
/******************************************************************************/
void SyncEEPROMToGlobal(void)
{
    unsigned char i,j,k;
    GBLdata Temp;

    GetEEPROM(&Temp);

    Global.BlueToothFlag            = Temp.BlueToothFlag;
    Global.SWNECcode                = Temp.SWNECcode;
    Global.EEPROMinitFlag           = Temp.EEPROMinitFlag;
    for(i=0; i< ButtonAmount; i++)
    {
        for(j=0; j < 2; j++)
        {
            Global.RemoteButtonNEC[i][j] = Temp.RemoteButtonNEC[i][j];
        }
    }
    Global.EEPROMinitFlag           = Temp.EEPROMinitFlag;
    for(i=0; i< RFcodesAmount; i++)
    {
        for(j=0; j < MirrorButtonsAmount; j++)
        {
            for(k=0; k < 2; k++)
            {
                Global.RemoteButtonRF[i][j][k] = Temp.RemoteButtonRF[i][j][k];
            }
        }
    }
}

/******************************************************************************/
/* GetMemoryLong
 *
 * The function returns the data saved in EEPROM.
/******************************************************************************/
unsigned long GetMemoryLong(unsigned char AddressFirst)
{
    unsigned char H;
    unsigned char MH;
    unsigned char ML;
    unsigned char L;
    unsigned long temp =0;

    H = ReadEEPROM_1Byte(AddressFirst);
    MH = ReadEEPROM_1Byte(AddressFirst + 1);
    ML = ReadEEPROM_1Byte(AddressFirst + 2);
    L = ReadEEPROM_1Byte(AddressFirst + 3);

    temp = (((unsigned long) H << 24) + ((unsigned long)MH << 16)
            + ((unsigned long)ML << 8) + (unsigned long)L);
    return temp;
}

/******************************************************************************/
/* SetMemoryLong
 *
 * The function burns the data to the EEPROM.
/******************************************************************************/
unsigned char SetMemoryLong(unsigned long Data, unsigned char AddressFirst)
{
    unsigned char H;
    unsigned char MH;
    unsigned char ML;
    unsigned char L;
    unsigned long Test;

    H = (unsigned char)  ((Data & 0XFF000000) >> 24);
    MH = (unsigned char) ((Data & 0X00FF0000) >> 16);
    ML = (unsigned char) ((Data & 0X0000FF00) >> 8);
    L = (unsigned char)  (Data & 0X000000FF);


    WriteEEPROM_1Byte(AddressFirst, H);
    WriteEEPROM_1Byte((AddressFirst + 1), MH);
    WriteEEPROM_1Byte((AddressFirst + 2), ML);
    WriteEEPROM_1Byte((AddressFirst + 3), L);

    // Test to see if the contents in memory are the same as we intended
    Test = GetMemoryLong(AddressFirst);

    if(Test == Data)
    {
        return PASS;
    }
    return FAIL;
}

/******************************************************************************/
/* GetMemoryInt
 *
 * The function returns the data saved in EEPROM.
/******************************************************************************/
unsigned long GetMemoryInt(unsigned char AddressFirst)
{
    unsigned char H;
    unsigned char L;
    unsigned int temp =0;

    H = ReadEEPROM_1Byte(AddressFirst);
    L = ReadEEPROM_1Byte(AddressFirst + 1);

    temp = (((unsigned int) H << 8) + (unsigned int)L);
    return temp;
}

/******************************************************************************/
/* SetMemoryInt
 *
 * The function burns the data to the EEPROM.
/******************************************************************************/
unsigned char SetMemoryInt(unsigned int Data, unsigned char AddressFirst)
{
    unsigned char H;
    unsigned char L;
    unsigned long Test;

    H = (unsigned char)  ((Data & 0xFF00) >> 8);
    L = (unsigned char)  (Data & 0x00FF);


    WriteEEPROM_1Byte(AddressFirst, H);
    WriteEEPROM_1Byte((AddressFirst + 1), L);

    // Test to see if the contents in memory are the same as we intended
    Test = GetMemoryInt(AddressFirst);

    if(Test == Data)
    {
        return PASS;
    }
    return FAIL;
}

/******************************************************************************/
/* GetMemoryChar
 *
 * The function returns the data saved in EEPROM.
/******************************************************************************/
unsigned long GetMemoryChar(unsigned char AddressFirst)
{
    unsigned char H;

    H = ReadEEPROM_1Byte(AddressFirst);

    return H;
}

/******************************************************************************/
/* SetMemoryChar
 *
 * The function burns the data to the EEPROM.
/******************************************************************************/
unsigned char SetMemoryChar(unsigned char Data, unsigned char AddressFirst)
{
    unsigned char Test;

    WriteEEPROM_1Byte(AddressFirst, Data);

    // Test to see if the contents in memory are the same as we intended
    Test = GetMemoryChar(AddressFirst);

    if(Test == Data)
    {
        return PASS;
    }
    return FAIL;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/