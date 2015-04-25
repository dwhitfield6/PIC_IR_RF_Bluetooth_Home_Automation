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
/* Files to Include                                                           */
/******************************************************************************/
#ifndef Flash_H
#define	Flash_H

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
#define ERASE   1
#define WRITE   2
#define READ    4
#define PROGRAM    8
#define CONFIG    16
/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void FLASH_Row_Erase(unsigned long address, unsigned char Prog__nConfig);
void FLASH_Row_Write(unsigned long address, unsigned char* data64, unsigned char Prog__nConfig);
void FLASH_Row_Read(unsigned long address, unsigned char* data64, unsigned char Prog__nConfig);
void FLASH_UNLOCK(void);
void Write_HEX_Row(void);

#endif	/* Flash_H */

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/