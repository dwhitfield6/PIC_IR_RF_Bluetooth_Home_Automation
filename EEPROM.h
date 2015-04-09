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
/* Defines                                                                    */
/******************************************************************************/
#define EEPROMinitilized 59

/******************************************************************************/
/* ButtonAmount                                                         
 * 
 * This parameter hold the amount of remote control buttons that are
 * able to be saved.
/******************************************************************************/
#define ButtonAmount 20

/******************************************************************************/
/* RFcodesAmount
 *
 * This parameter hold the amount of remote control buttons that are
 * able to be saved.
/******************************************************************************/
#define RFcodesAmount 20

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
/* the amount of data needed is 2 * ButtonAmount */
#define EE_RemoteButtonNEC 5

/* unsigned char EEPROMinitFlag */
#define EE_EEPROMinitFlag (2*ButtonAmount + 5)

/******************************/
/* RF associated with each NEC code
/******************************/
/* Stores the first button that will cause each RF code to get sent */
/* the amount of data needed 1 * RFcodesAmount */
#define EE_RemoteButton1RF (2*ButtonAmount + 7)
/* Stores the second button that will cause each RF code to get sent */
#define EE_RemoteButton2RF (2*ButtonAmount + 7 + RFcodesAmount*2)
/* Stores the third button that will cause each RF code to get sent */
#define EE_RemoteButton3RF (2*ButtonAmount + 7 + RFcodesAmount*4)
/* Stores the forth button that will cause each RF code to get sent */
#define EE_RemoteButton4RF (2*ButtonAmount + 7 + RFcodesAmount*6)
/* Stores the fifth button that will cause each RF code to get sent */
#define EE_RemoteButton5RF (2*ButtonAmount + 7 + RFcodesAmount*8)

/******************************************************************************/
/* Structures                                                                 */
/******************************************************************************/
typedef struct EEdata
{
    unsigned char BlueToothFlag;
    unsigned long SWNECcode;
    unsigned char RemoteButtonNEC[ButtonAmount][2];
    unsigned char EEPROMinitFlag;

    /*
     * RemoteButtonRF 0 is for config 1 channel D
     * RemoteButtonRF 1 is for config 1 channel E
     * RemoteButtonRF 2 is for config 1 channel F
     * RemoteButtonRF 3 is for config 2 channel B
     * RemoteButtonRF 4 is for config 2 channel D
     * RemoteButtonRF 5 is for config 2 channel H device 1
     * RemoteButtonRF 6 is for config 2 channel H device 2
     * RemoteButtonRF 7 is for config 2 channel H device 3
     */
    unsigned char RemoteButton1RF[RFcodesAmount][2];
    unsigned char RemoteButton2RF[RFcodesAmount][2];
    unsigned char RemoteButton3RF[RFcodesAmount][2];
    unsigned char RemoteButton4RF[RFcodesAmount][2];
    unsigned char RemoteButton5RF[RFcodesAmount][2];

}GBLdata;

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
GBLdata Global =0;

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
void SetEEPROMdefault(void);
unsigned char EEPROMinitialized(void);
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/