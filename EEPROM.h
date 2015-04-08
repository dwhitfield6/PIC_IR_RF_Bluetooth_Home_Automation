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

#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Structures                                                                 */
/******************************************************************************/
typedef struct EEdata
{
    unsigned char BlueToothFlag;
    unsigned long SWNECcode;
    unsigned char RemoteButton1NECaddr;
    unsigned char RemoteButton1NECcom;
    unsigned char RemoteButton2NECaddr;
    unsigned char RemoteButton2NECcom;
    unsigned char RemoteButton3NECaddr;
    unsigned char RemoteButton3NECcom;
    unsigned char RemoteButton4NECaddr;
    unsigned char RemoteButton4NECcom;
    unsigned char RemoteButton5NECaddr;
    unsigned char RemoteButton5NECcom;
    unsigned char RemoteButton6NECaddr;
    unsigned char RemoteButton6NECcom;
    unsigned char RemoteButton7NECaddr;
    unsigned char RemoteButton7NECcom;
    unsigned char RemoteButton8NECaddr;
    unsigned char RemoteButton8NECcom;
    unsigned char RemoteButton9NECaddr;
    unsigned char RemoteButton9NECcom;
    unsigned char RemoteButton10NECaddr;
    unsigned char RemoteButton10NECcom;
    unsigned char RemoteButton11NECaddr;
    unsigned char RemoteButton11NECcom;
    unsigned char RemoteButton12NECaddr;
    unsigned char RemoteButton12NECcom;
    unsigned char RemoteButton13NECaddr;
    unsigned char RemoteButton13NECcom;
    unsigned char RemoteButton14NECaddr;
    unsigned char RemoteButton14NECcom;
    unsigned char RemoteButton15NECaddr;
    unsigned char RemoteButton15NECcom;
    unsigned char RemoteButton16NECaddr;
    unsigned char RemoteButton16NECcom;
    unsigned char RemoteButton17NECaddr;
    unsigned char RemoteButton17NECcom;
    unsigned char RemoteButton18NECaddr;
    unsigned char RemoteButton18NECcom;
    unsigned char RemoteButton19NECaddr;
    unsigned char RemoteButton19NECcom;
    unsigned char RemoteButton20NECaddr;
    unsigned char RemoteButton20NECcom;

}GBLdata;

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

GBLdata Global =0;

/******************************************************************************/
/* EEPROM Data Addresses                                                      */
/******************************************************************************/
/* Unsigned char BlueToothFlag */
#define EE_BlueToothFlag        0

/* Unsigned long NECcode */
#define EE_NECcodeBYTE1         1
#define EE_NECcodeBYTE2         2
#define EE_NECcodeBYTE3         3
#define EE_NECcodeBYTE4         4

/******************************/
/* NEC button data
/******************************/
/* Remote Button 1 storage of NEC code */
/* Unsigned char RemoteButton1NECaddr */
#define EE_RemoteButton1NECaddr 5
/* Unsigned char RemoteButton1NECcom */
#define EE_RemoteButton1NECcom  6

/* Remote Button 2 storage of NEC code */
/* Unsigned char RemoteButton2NECaddr */
#define EE_RemoteButton2NECaddr 7
/* Unsigned char RemoteButton2NECcom */
#define EE_RemoteButton2NECcom  8

/* Remote Button 3 storage of NEC code */
/* Unsigned char RemoteButton3NECaddr */
#define EE_RemoteButton3NECaddr 9
/* Unsigned char RemoteButton3NECcom */
#define EE_RemoteButton3NECcom  10

/* Remote Button 4 storage of NEC code */
/* Unsigned char RemoteButton4NECaddr */
#define EE_RemoteButton4NECaddr 11
/* Unsigned char RemoteButton4NECcom */
#define EE_RemoteButton4NECcom  12

/* Remote Button 5 storage of NEC code */
/* Unsigned char RemoteButton5NECaddr */
#define EE_RemoteButton5NECaddr 13
/* Unsigned char RemoteButton5NECcom */
#define EE_RemoteButton5NECcom  14

/* Remote Button 6 storage of NEC code */
/* Unsigned char RemoteButton6NECaddr */
#define EE_RemoteButton6NECaddr 15
/* Unsigned char RemoteButton6NECcom */
#define EE_RemoteButton6NECcom  16

/* Remote Button 7 storage of NEC code */
/* Unsigned char RemoteButton7NECaddr */
#define EE_RemoteButton7NECaddr 17
/* Unsigned char RemoteButton7NECcom */
#define EE_RemoteButton7NECcom  18

/* Remote Button 8 storage of NEC code */
/* Unsigned char RemoteButton8NECaddr */
#define EE_RemoteButton8NECaddr 19
/* Unsigned char RemoteButton8NECcom */
#define EE_RemoteButton8NECcom  20

/* Remote Button 9 storage of NEC code */
/* Unsigned char RemoteButton9NECaddr */
#define EE_RemoteButton9NECaddr 21
/* Unsigned char RemoteButton9NECcom */
#define EE_RemoteButton9NECcom  22

/* Remote Button 10 storage of NEC code */
/* Unsigned char RemoteButton10NECaddr */
#define EE_RemoteButton10NECaddr 23
/* Unsigned char RemoteButton10NECcom */
#define EE_RemoteButton10NECcom  24

/* Remote Button 11 storage of NEC code */
/* Unsigned char RemoteButton11NECaddr */
#define EE_RemoteButton11NECaddr 25
/* Unsigned char RemoteButton11NECcom */
#define EE_RemoteButton11NECcom  26

/* Remote Button 12 storage of NEC code */
/* Unsigned char RemoteButton12NECaddr */
#define EE_RemoteButton12NECaddr 27
/* Unsigned char RemoteButton12NECcom */
#define EE_RemoteButton12NECcom  28

/* Remote Button 13 storage of NEC code */
/* Unsigned char RemoteButton13NECaddr */
#define EE_RemoteButton13NECaddr 29
/* Unsigned char RemoteButton13NECcom */
#define EE_RemoteButton13NECcom  30

/* Remote Button 14 storage of NEC code */
/* Unsigned char RemoteButton14NECaddr */
#define EE_RemoteButton14NECaddr 31
/* Unsigned char RemoteButton14NECcom */
#define EE_RemoteButton14NECcom  32

/* Remote Button 15 storage of NEC code */
/* Unsigned char RemoteButton15NECaddr */
#define EE_RemoteButton15NECaddr 33
/* Unsigned char RemoteButton15NECcom */
#define EE_RemoteButton15NECcom  34

/* Remote Button 16 storage of NEC code */
/* Unsigned char RemoteButton16NECaddr */
#define EE_RemoteButton16NECaddr 35
/* Unsigned char RemoteButton16NECcom */
#define EE_RemoteButton16NECcom  36

/* Remote Button 17 storage of NEC code */
/* Unsigned char RemoteButton17NECaddr */
#define EE_RemoteButton17NECaddr 37
/* Unsigned char RemoteButton17NECcom */
#define EE_RemoteButton17NECcom  38

/* Remote Button 18 storage of NEC code */
/* Unsigned char RemoteButton18NECaddr */
#define EE_RemoteButton18NECaddr 39
/* Unsigned char RemoteButton18NECcom */
#define EE_RemoteButton18NECcom  40

/* Remote Button 19 storage of NEC code */
/* Unsigned char RemoteButton19NECaddr */
#define EE_RemoteButton19NECaddr 41
/* Unsigned char RemoteButton19NECcom */
#define EE_RemoteButton19NECcom  42

/* Remote Button 20 storage of NEC code */
/* Unsigned char RemoteButton20NECaddr */
#define EE_RemoteButton20NECaddr 43
/* Unsigned char RemoteButton20NECcom */
#define EE_RemoteButton20NECcom  44

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/

unsigned int ReadEEPROM_1Byte(unsigned int address);
void EEPROM_UNLOCK(void);
void WriteEEPROM_1Byte(unsigned int address, unsigned char data);
GBLdata GetEEPROM(void);
unsigned long SetEEPROM(GBLdata Temp,unsigned long burn);
unsigned long GetMemoryLong(unsigned char AddressFirst);
unsigned char SetMemoryLong(unsigned long Data, unsigned char AddressFirst);
unsigned long GetMemoryInt(unsigned char AddressFirst);
unsigned char SetMemoryInt(unsigned int Data, unsigned char AddressFirst);
unsigned long GetMemoryChar(unsigned char AddressFirst);
unsigned char SetMemoryChar(unsigned char Data, unsigned char AddressFirst);
unsigned char SyncGlobalToEEPROM(void);
void SyncEEPROMToGlobal(void);
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/