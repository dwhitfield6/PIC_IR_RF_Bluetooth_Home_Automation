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
GBLdata GetEEPROM(void)
{
    GBLdata Temp;
    Temp.BlueToothFlag = GetMemoryChar(EE_BlueToothFlag);
    Temp.NECcode = GetMemoryLong(EE_NECcodeBYTE1);
    return Temp;
}

/******************************************************************************/
/* SetEEPROM
 *
 * The function burns the EEdata struct to memory.
/******************************************************************************/
unsigned long SetEEPROM(GBLdata Temp,unsigned long burn)
{
    unsigned long fail = 0;
    if(burn & 0x00000001)
    {
        if(!SetMemoryChar(Temp.BlueToothFlag,EE_BlueToothFlag))
        {
            /* Failed to burn BlueToothFlag */
            fail += 0x00000001;
        }
    }
    if(burn & 0x00000002)
    {
        if(!SetMemoryLong(Temp.NECcode,EE_NECcodeBYTE1))
        {
            /* Failed to burn NECcode */
            fail += 0x00000002;
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
    if(!SetEEPROM(Global,0x00000003))
    {
        return PASS;
    }
    return FAIL;
}

/******************************************************************************/
/* SyncEEPROMToGlobal
 *
 * The function syncs the EEPROM to the global data.
/******************************************************************************/
void SyncEEPROMToGlobal(void)
{
    GBLdata Temp;

    Temp = GetEEPROM();

    Global.BlueToothFlag    = Temp.BlueToothFlag;
    Global.NECcode          = Temp.NECcode;
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