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
/* Contains Timer functions.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "Timer.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
unsigned char Timer1_Postscaler = 0;
unsigned char Timer2_Postscaler = 0;
unsigned char Timer3_Postscaler = 0;

/******************************************************************************/
/* Functions
 *
/******************************************************************************/

/******************************************************************************/
/* InitTimers
 *
 * The function initializes timers 1, 2, and 3.
/******************************************************************************/
void InitTimers(void)
{
    InitTimer1(); // Used for LED timing
    EnableTimer1Int();
    InitTimer2(); // Used for RF
    EnableTimer2Int();
    InitTimer3();
    EnableTimer3Int();
}

/******************************************************************************/
/* Timer0Init
 *
 * The function initializes timer 0.
/******************************************************************************/
void InitTimer0(void)
{
    T0CONbits.T08BIT = FALSE;   // 16 bit timer
    T0CONbits.T0CS = FALSE;     // FOSC/4 is system clock
    T0CONbits.PSA = FALSE;      // Use Prescaler
    T0CONbits.T0PS = 0x4;      // Prescaler is 32
}

/******************************************************************************/
/* ResetTimer0
 *
 * The function resets the counter on Timer 0.
/******************************************************************************/
void ResetTimer0(void)
{
    /*
     * Reset count to 2 because when the TMR0 register is written to,
     * the increment is inhibited for the following two instruction cycles
     */
    TMR0H = 0;
    TMR0L = 2;
}

/******************************************************************************/
/* SetTimer0
 *
 * The function resets the counter on Timer 0.
/******************************************************************************/
void SetTimer0(unsigned int temp)
{
    /*
     * Reset count to 2 because when the TMR0 register is written to,
     * the increment is inhibited for the following two instruction cycles
     */
    TMR0H = (temp & 0xFF00)>> 8;;
    TMR0L = temp & 0xFF;
}

/******************************************************************************/
/* Timer1Init
 *
 * The function initializes timer 1.
/******************************************************************************/
void InitTimer1(void)
{
    T1CONbits.RD16      = TRUE;         // Write in one operation
    T1CONbits.T1RUN     = FALSE;        // FOSC/4 is system clock
    T1CONbits.T1CKPS    = 0x3;          // Prescaler is 8
    T1CONbits.T1OSCEN   = FALSE;        // Oscillator is off
    T1CONbits.TMR1CS    = FALSE;        // Clocked by FOSC/4
}

/******************************************************************************/
/* ResetTimer1
 *
 * The function resets the counter on Timer 1.
/******************************************************************************/
void ResetTimer1(void)
{
    /*
     * Reset count to 2 because when the TMR1 register is written to,
     * the increment is inhibited for the following two instruction cycles
     */
    TMR1H = 0;
    TMR1L = 2;
}

/******************************************************************************/
/* Timer2Init
 *
 * The function initializes timer 2.
/******************************************************************************/
void InitTimer2(void)
{
    T2CONbits.T2OUTPS = 0xE; // Postscaler is 15
    T2CONbits.T2CKPS = 0x3; // Prescaler is 16
}

/******************************************************************************/
/* LEDTimerON
 *
 * The function uses timer 2 to time the turning off of the LEDS.
/******************************************************************************/
void LEDTimerON(void)
{
    Timer1OFF();
    ResetTimer1();
    Timer1_Postscaler = 0;
    ResetTimer1();
    Timer1ON();
}

/******************************************************************************/
/* Timer3Init
 *
 * The function initializes timer 3.
/******************************************************************************/
void InitTimer3(void)
{
    T3CONbits.RD16      = TRUE;         // Write in one operation
    T3CONbits.T3CKPS    = 0x3;          // Prescaler is 8
    T3CONbits.TMR3CS    = FALSE;        // Clocked by FOSC/4
}

/******************************************************************************/
/* ResetTimer3
 *
 * The function resets the counter on Timer 3.
/******************************************************************************/
void ResetTimer3(void)
{
    /*
     * Reset count to 2 because when the TMR3 register is written to,
     * the increment is inhibited for the following two instruction cycles
     */
    TMR3H = 0;
    TMR3L = 2;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/