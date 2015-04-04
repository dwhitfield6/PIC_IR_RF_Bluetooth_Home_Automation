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
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#ifndef TIMER_H
#define	TIMER_H

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
extern unsigned char Timer1_Postscaler;
extern unsigned char Timer2_Postscaler;
extern unsigned char Timer3_Postscaler;

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* Timer0ON()
 *
 * The function turns on timer 0.
/******************************************************************************/
#define Timer0ON() (T0CONbits.TMR0ON = TRUE)

/******************************************************************************/
/* Timer0OFF()
 *
 * The function turns off timer 0.
/******************************************************************************/
#define Timer0OFF() (T0CONbits.TMR0ON = FALSE)

/******************************************************************************/
/* EnableTimer0Int()
 *
 * The function turns on the timer 0 overflow interrupt.
/******************************************************************************/
#define EnableTimer0Int() (INTCONbits.TMR0IE = TRUE)

/******************************************************************************/
/* DisableTimer0Int()
 *
 * The function turns off the timer 0 overflow interrupt.
/******************************************************************************/
#define DisableTimer0Int() (INTCONbits.TMR0IE = FALSE)

/******************************************************************************/
/* Timer1ON()
 *
 * The function turns on timer 1.
/******************************************************************************/
#define Timer1ON() (T1CONbits.TMR1ON = TRUE)

/******************************************************************************/
/* Timer1OFF()
 *
 * The function turns off timer 1.
/******************************************************************************/
#define Timer1OFF() (T1CONbits.TMR1ON = FALSE)

/******************************************************************************/
/* EnableTimer1Int()
 *
 * The function turns on the timer 1 overflow interrupt.
/******************************************************************************/
#define EnableTimer1Int() (PIE1bits.TMR1IE = TRUE)

/******************************************************************************/
/* DisableTimer1Int()
 *
 * The function turns off the timer 1 overflow interrupt.
/******************************************************************************/
#define DisableTimer1Int() (PIE1bits.TMR1IE = FALSE)

/******************************************************************************/
/* Timer2ON()
 *
 * The function turns on timer 2.
/******************************************************************************/
#define Timer2ON() (T2CONbits.TMR2ON = TRUE)

/******************************************************************************/
/* Timer2OFF()
 *
 * The function turns off timer 2.
/******************************************************************************/
#define Timer2OFF() (T2CONbits.TMR2ON = FALSE)

/******************************************************************************/
/* SetTimer2
 *
 * The function sets PR2 for the interrupt on TMR2-to-PR2 match.
/******************************************************************************/
#define SetTimer2(time) (PR2 = time)

/******************************************************************************/
/* ResetTimer2
 *
 * The function resets the counter on Timer 2.
/******************************************************************************/
#define ResetTimer2() (TMR2 = 0)

/******************************************************************************/
/* EnableTimer2Int()
 *
 * The function turns on the timer 2 compare interrupt.
/******************************************************************************/
#define EnableTimer2Int() (PIE1bits.TMR2IE = TRUE)

/******************************************************************************/
/* DisableTimer2Int()
 *
 * The function turns off the timer 2 compare interrupt.
/******************************************************************************/
#define DisableTimer2Int() (PIE1bits.TMR2IE = FALSE)

/******************************************************************************/
/* Timer3ON()
 *
 * The function turns on timer 3.
/******************************************************************************/
#define Timer3ON() (T3CONbits.TMR3ON = TRUE)

/******************************************************************************/
/* Timer3OFF()
 *
 * The function turns off timer 3.
/******************************************************************************/
#define Timer3OFF() (T3CONbits.TMR3ON = FALSE)

/******************************************************************************/
/* EnableTimer3Int()
 *
 * The function turns on the timer 3 compare interrupt.
/******************************************************************************/
#define EnableTimer3Int() (PIE2bits.TMR3IE = TRUE)

/******************************************************************************/
/* DisableTimer3Int()
 *
 * The function turns off the timer 3 compare interrupt.
/******************************************************************************/
#define DisableTimer3Int() (PIE2bits.TMR3IE = FALSE)

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void InitTimer0(void);
void ResetTimer0(void);
void InitTimer1(void);
void ResetTimer1(void);
void InitTimer2(void);
void LEDTimerON(void);
void InitTimer3(void);
void ResetTimer3(void);
void InitTimers(void);

#endif	/* TIMER_H */