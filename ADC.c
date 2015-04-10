/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/02/15     1.0_DW0a    Initial project make.
 *                          Derived from project 'PIC_PS2_to_UART'.
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
/******************************************************************************/

/******************************************************************************/
/* Contains functions that control the internal ADC.
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

#include "ADC.h"        
#include "user.h"          
#include "MISC.h"          

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* ReadVoltage
 *
 * The function returns the voltage that powers the unit. There is a voltage
 * divider that is turned on (DivON) and the divided voltage is read.
/******************************************************************************/
double ReadVoltage(void)
{
    unsigned int temp;
    double voltage;

    VoltageDividerON();
    delayUS(5); // wait for the voltage divider to settle
    if(InternalADC_Read(0, &temp))
    {
        voltage = ((double)temp / 1024.0) * ratio * NominalVDD;
    }
    else
    {
        voltage = 0.0;
    }
    VoltageDividerOFF();
    return voltage;
}

/******************************************************************************/
/* InternalADC_Read
 *
 * The function returns the digital adc counts of the channel.
/******************************************************************************/
unsigned char InternalADC_Read(unsigned char channel, unsigned int *ADCcounts)
{
    unsigned char AnaChanSel = 1;
    if(channel > 0b1100)
    {
        *ADCcounts = ERROR;
        return ERROR;
    }
    if(channel > 1)
    {
        AnaChanSel <= (channel - 1);
    }
    else if (channel == 0)
    {
        AnaChanSel = 0;
    }

    ADCON0 |= (AnaChanSel << 2); // select channel

    ADCON1bits.VCFG0 = 0; //Positive voltage is VDD
    ADCON1bits.VCFG1 = 0; //Negative voltage is VSS

    EnableADC();
    delayUS(ACQtime);

    ADCON0bits.GO = TRUE;
    while(ADCON0bits.GO);
    DisableADC();

    *ADCcounts = ((ADRESH << 8) + ADRESL);
    return OK;
}

/******************************************************************************/
/* ResetADC
 *
 * The function shutsdown the ADC module and sets all registers to their
 *   preinitialized values.
/******************************************************************************/
void ResetADC(void)
{
    ADCON0 =0;
    ADCON1 =0;
    ADCON2 =0;
}

/******************************************************************************/
/* InitADC
 *
 * This function initializes the ADC.
/******************************************************************************/
void InitADC(void)
{
    ResetADC();
    ADCON2bits.ADFM = 1;    // Right justified
    ADCON2bits.ACQT = 0x7;  // sample and hold for 20 TAD
    ADCON2bits.ADCS = 0x4;  // AD conversion clock if FOSC/4
}

/******************************************************************************/
/* VoltageDividerON()
 *
 * The function turns on the voltage divider to take a voltage reading.
/******************************************************************************/
void VoltageDividerON(void)
{
    VoltageDividerTris  = OUTPUT;
    LATB &= ~VoltageDivider;
}

/******************************************************************************/
/* VoltageDividerOFF()
 *
 * The function turns off the voltage divider to take a voltage reading.
/******************************************************************************/
void VoltageDividerOFF(void)
{
    VoltageDividerTris  = INPUT;
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/