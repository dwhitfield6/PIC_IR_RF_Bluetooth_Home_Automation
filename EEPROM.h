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
    unsigned long NECcode;
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