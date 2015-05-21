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
/* Files to Include                                                           */
/******************************************************************************/
#ifndef EEPROM_H
#define	EEPROM_H

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

#include "user.h"          /* User funct/params, such as InitApp */
#include "RF.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define EEPROMinitilized 59

/******************************************************************************/
/* ButtonAmount                                                         
 * 
 * This parameter hold the amount of remote control buttons that are
 * able to be saved.
/******************************************************************************/
#define ButtonAmount 12

/******************************************************************************/
/* RFcodesAmount
 *
 * This parameter holds the amount of remote control buttons that can control
 *  the same RF switch code.
 *
 * example: if this is 5 it means that 5 different Remote buttons will cause
 *  the rf channel to be sent
/******************************************************************************/
#define MirrorButtonsAmount 1

/******************************************************************************/
/* EEPROM Data Addresses                                                      */
/******************************************************************************/
/* unsigned char BlueToothFlag */
#define EE_BlueToothFlag        0

/* unsigned long NECcode */
#define EE_NECcodeBYTE1         1
#define EE_NECcodeBYTE2         2
#define EE_NECcodeBYTE3         3
#define EE_NECcodeBYTE4         4

/******************************/
/* NEC button data
/******************************/
/* Remote Button storage of NEC codes */
/* the amount of data needed is (2 * ButtonAmount) */
#define EE_RemoteButtonNEC 5

/* unsigned char EEPROMinitFlag */
#define EE_EEPROMinitFlag (2*ButtonAmount + 5)

/***********************************/
/* RF associated with each NEC code
/***********************************/
/* Stores the first button that will cause each RF code to get sent */
/* the amount of data needed (2 * MirrorButtonsAmount * RFnumberOfSavedCodes) */
#define EE_RemoteButtonRF (2*ButtonAmount + 6)

/******************************/
/* Serial Number
/******************************/
/* unsigned long SerialNumber */
#define EE_SerialNumberBYTE1 (2*ButtonAmount + 2*MirrorButtonsAmount*RFnumberOfSavedCodes + 6)
#define EE_SerialNumberBYTE2 (2*ButtonAmount + 2*MirrorButtonsAmount*RFnumberOfSavedCodes + 7)
#define EE_SerialNumberBYTE3 (2*ButtonAmount + 2*MirrorButtonsAmount*RFnumberOfSavedCodes + 8)
#define EE_SerialNumberBYTE4 (2*ButtonAmount + 2*MirrorButtonsAmount*RFnumberOfSavedCodes + 9)

/******************************************************************************/
/* Structures                                                                 */
/******************************************************************************/
typedef struct EEdata
{
    unsigned char BlueToothFlag;
    unsigned long SWNECcode;

    unsigned char RemoteButtonNEC[ButtonAmount][2];
    /* RemoteButtonNEC[amount of availabe buttons][Address,Command] */

    unsigned char EEPROMinitFlag;
}GBLdata1;

typedef struct EEdata2
{
    /*
     * RemoteButtonRF[rfnum][Nec:Address Nec:Command];
     *
     * rfnum 0 is for config 1 channel D
     * rfnum 1 is for config 1 channel E
     * rfnum 2 is for config 1 channel F
     * rfnum 3 is for config 2 channel B
     * rfnum 4 is for config 2 channel D
     * rfnum 5 is for config 2 channel H device 1
     * rfnum 6 is for config 2 channel H device 2
     * rfnum 7 is for config 2 channel H device 3
     */
    unsigned char RemoteButtonRF[RFnumberOfSavedCodes][MirrorButtonsAmount][2];
    /* 
     * RemoteButtonRF[amount of Rf codes]
     * [amount of buttons that can control that code][Address,Command]
     */

    unsigned long SerialNumber;
}GBLdata2;

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
GBLdata1    Global1 = 0;
GBLdata2    Global2 = 0;

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

unsigned int ReadEEPROM_1Byte(unsigned int address);
void EEPROM_UNLOCK(void);
void WriteEEPROM_1Byte(unsigned int address, unsigned char data);
void GetEEPROM1(GBLdata1 *Temp);
unsigned long SetEEPROM1(unsigned long burn);
void GetEEPROM2(GBLdata2 *Temp);
unsigned long SetEEPROM2(unsigned long burn);
unsigned long GetMemoryLong(unsigned int AddressFirst);
unsigned char SetMemoryLong(unsigned long Data, unsigned int AddressFirst);
unsigned long GetMemoryInt(unsigned int AddressFirst);
unsigned char SetMemoryInt(unsigned int Data, unsigned int AddressFirst);
unsigned long GetMemoryChar(unsigned int AddressFirst);
unsigned char SetMemoryChar(unsigned char Data, unsigned int AddressFirst);
unsigned char SyncGlobalToEEPROM(void);
void SyncEEPROMToGlobal(void);
void SetEEPROMdefault(void);
unsigned char EEPROMinitialized(void);
void SyncEEPROMToGlobal1(void);
void SyncEEPROMToGlobal2(void);

#endif	/* EEPROM_H */

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
