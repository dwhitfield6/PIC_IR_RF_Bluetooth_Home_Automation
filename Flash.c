/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/24/15     1.0_DW0d    Initial Coding.
/******************************************************************************/

/******************************************************************************/
/* Contains functions that control the burning and reading of Flash memory.
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

#include "Flash.h"
#include "user.h"          

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* FLASH_Row_Erase
 *
 * The function erases a row (64 bytes)
/******************************************************************************/
void FLASH_Row_Erase(unsigned long address, unsigned char Prog__nConfig)
{
    unsigned char ADDR_UPPER,ADDR_HIGH, ADDR_LOW;
    unsigned char GIEstatus = INTCONbits.GIE;//read GIE setting
    unsigned char PEIEstatus = INTCONbits.PEIE;//read GIE setting

    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;
    
    ADDR_UPPER  = (unsigned char) ((address & 0xFF0000) >> 16);
    ADDR_HIGH   = (unsigned char) ((address & 0x00FF00) >> 8);
    ADDR_LOW    = (unsigned char) (address & 0x0000FF);
    TBLPTRU = ADDR_UPPER;
    TBLPTRH = ADDR_HIGH;
    TBLPTRL = ADDR_LOW;
    EECON1bits.EEPGD = 1; // Flash memory

    if(Prog__nConfig == PROGRAM)
    {
        EECON1bits.CFGS = 0;  // Flash
    }
    else
    {
        EECON1bits.CFGS = 1;  // Configuration
    }

    EECON1bits.WREN = TRUE; //Enable writes
    EECON1bits.FREE = TRUE; //Erase

    FLASH_UNLOCK();

    EECON1bits.WR = TRUE; //Block Erase
    while(EECON1bits.WR);

    EECON1bits.WREN = FALSE; //Enable writes
    if(GIEstatus)
    {
        INTCONbits.GIE = TRUE;//enable global interrupts
    }
    if(PEIEstatus)
    {
        INTCONbits.PEIE = TRUE;//enable low priority interrupts
    }
}

/******************************************************************************/
/* FLASH_Row_Write
 *
 * The function Writes a row (64 bytes)
/******************************************************************************/
void FLASH_Row_Write(unsigned long address, unsigned char* data64, unsigned char Prog__nConfig)
{
    unsigned char i;
    unsigned char ADDR_UPPER,ADDR_HIGH;
    unsigned char GIEstatus = INTCONbits.GIE;//read GIE setting
    unsigned char PEIEstatus = INTCONbits.PEIE;//read GIE setting

    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;
    
    FLASH_Row_Erase(address,Prog__nConfig);
    
    ADDR_UPPER  = (unsigned char) ((address & 0xFF0000) >> 16);
    ADDR_HIGH   = (unsigned char) ((address & 0x00FF00) >> 8);    
    
    for(i = 0; i <64; i++)
    {
        TBLPTRU = ADDR_UPPER;
        TBLPTRH = ADDR_HIGH;
        TBLPTRL = i;
        TABLAT = data64[i];
        EECON1bits.EEPGD = 1; // Flash memory
        if(Prog__nConfig == PROGRAM)
        {
            EECON1bits.CFGS = 0;  // Flash
        }
        else
        {
            EECON1bits.CFGS = 1;  // Configuration
        }
        EECON1bits.WREN = TRUE; //Enable writes
        FLASH_UNLOCK();
        EECON1bits.WR = TRUE; //Block Erase
        while(EECON1bits.WR);
    }
    
    EECON1bits.WREN = FALSE; //Enable writes
    if(GIEstatus)
    {
        INTCONbits.GIE = TRUE;//enable global interrupts
    }
    if(PEIEstatus)
    {
        INTCONbits.PEIE = TRUE;//enable low priority interrupts
    }
}

/******************************************************************************/
/* FLASH_Row_Read
 *
 * The function Reads a row (64 bytes)
/******************************************************************************/
void FLASH_Row_Read(unsigned long address, unsigned char* data64, unsigned char Prog__nConfig)
{
    unsigned char i;
    unsigned char ADDR_UPPER,ADDR_HIGH, ADDR_LOW;
    unsigned char GIEstatus = INTCONbits.GIE;//read GIE setting
    unsigned char PEIEstatus = INTCONbits.PEIE;//read GIE setting

    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;

    ADDR_UPPER  = (unsigned char) ((address & 0xFF0000) >> 16);
    ADDR_HIGH   = (unsigned char) ((address & 0x00FF00) >> 8);

    for(i = 0; i <64; i++)
    {
        TBLPTRU = ADDR_UPPER;
        TBLPTRH = ADDR_HIGH;
        TBLPTRL = i;
        data64[i] = TABLAT;
        EECON1bits.EEPGD = 1; // Flash memory
        if(Prog__nConfig == PROGRAM)
        {
            EECON1bits.CFGS = 0;  // Flash
        }
        else
        {
            EECON1bits.CFGS = 1;  // Configuration
        }
    }

    if(GIEstatus)
    {
        INTCONbits.GIE = TRUE;//enable global interrupts
    }
    if(PEIEstatus)
    {
        INTCONbits.PEIE = TRUE;//enable low priority interrupts
    }
}

/******************************************************************************/
/* FLASH_UNLOCK
 *
 * The function sends the unlock sequence required for modifying the internal
 *  EEPROM.
/******************************************************************************/
void FLASH_UNLOCK(void)
{
    EECON2 = 0x55;
    EECON2 = 0xAA;
}

/******************************************************************************/
/* Write_HEX_Row
 *
 * The function sends erases and writes a line of the hex file to the buffer
/******************************************************************************/
void Write_HEX_Row(void)
{
    NOP();
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/