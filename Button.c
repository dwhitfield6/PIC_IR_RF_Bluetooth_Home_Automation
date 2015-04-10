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
/* Contains functions for the pushbotton switch.
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
#include <stdio.h>       /* For true/false definition */

#endif

#include "user.h"
#include "MISC.h"

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* ReadPushButton
 *
 * The function reads the pushbutton.
/******************************************************************************/
unsigned char ReadPushButton(void)
{
    //READ RC0
    if((PORTC & Pushbutton) != Pushbutton)
    {
        return ON;
    }
    return OFF;
}

/******************************************************************************/
/* ReadCodeButtons
 *
 * The function reads the dip switch that is associated with the code.
/******************************************************************************/
unsigned char ReadCodeButtons(void)
{
    unsigned char temp = 0;
    
    SWcodePwrON();
    delayUS(5); // let the switch voltage settle in order to take a good reading
    
    //READ RB0 - RB3
    if((PORTB & Swcode4) != Swcode4)
    {
        temp +=8;
    }
    if((PORTB & Swcode3) != Swcode3)
    {
        temp +=4;
    }
    if((PORTB & Swcode2) != Swcode2)
    {
        temp +=2;
    }
    if((PORTB & Swcode1) != Swcode1)
    {
        temp +=1;
    }
    SWcodePwrOFF();
    return temp;
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/