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
/* Contains UART functions.
 *
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#define USE_OR_MASKS
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
#include <stdio.h>         /* For sprintf definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "UART.h"          /* User funct/params, such as InitApp */
#include "MISC.h"          /* User funct/params, such as InitApp */
#include "FLASH.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
unsigned char ReceivedString[RXbufsize];
unsigned char ReceivedStringPos = 0;
unsigned char CommandString[RXCommandsize];
unsigned char CommandStringPos = 0;
volatile unsigned char NewReceivedString = FALSE;

/******************************************************************************/
/* Functions
/******************************************************************************/

/******************************************************************************/
/* Local_CloseUSART
 *
 * The function disables the UART.
/******************************************************************************/
void Local_CloseUSART(void )
{
    PIE1bits.RCIE   = OFF;
    PIE1bits.TXIE   = OFF;
    RCSTAbits.SPEN  = OFF;
    RCSTAbits.CREN  = OFF;
    RCSTAbits.FERR  = FALSE;
    TXSTAbits.TXEN = OFF;
}

/******************************************************************************/
/* InitUART
 *
 * The function initializes the UART. The Baud is set and the Parity setting is
 *   also set.
/******************************************************************************/
void InitUART(unsigned long Baud)
{
    unsigned long temp;
    unsigned char config = 0;
    unsigned int spbrg;

    Local_CloseUSART(); //turn off usart if was previously on

    //-----configure USART -----
    config |= Local_USART_TX_INT_OFF; // Tx interrupts off
    config |= Local_USART_ASYNCH_MODE;// Asynchronous mode
    config |= Local_USART_CONT_RX;    // Enable receive
    config |= Local_USART_BRGH_HIGH;  // High speed
    config &= ~Local_USART_NINE_BIT;  // eight bits

    //calculate the spbrg from the baud rate.
    temp = (SYS_FREQ / Baud) - 1;
    temp = (temp >> 2) - 1;
    spbrg = (unsigned int) Round(temp);

    BAUDCONbits.BRG16 = 1; // 16 bit baud
    
    OpenUSART(config, spbrg);

    PIR1bits.RCIF   = 0;   // reset RX pin flag
    PIE1bits.RCIE   = 1;   // enable RX interrupts
    INTCONbits.PEIE = 1; //Enable pheripheral interrupt
}

/******************************************************************************/
/* OpenUSART
 *
 * The function sets up the UART transmitter and receiver.
/******************************************************************************/
void OpenUSART( unsigned char config, unsigned int spbrg)
{
    TXSTA = 0;           // Reset USART registers to POR state
    RCSTA = 0;

    if(config&0x01)      // Sync or async operation
    {
        TXSTAbits.SYNC = 1;
    }

    if(config&0x02)      // 8- or 9-bit mode
    {
        TXSTAbits.TX9 = 1;
        RCSTAbits.RX9 = 1;
    }

    if(config&0x04)      // Master or Slave (sync only)
    {
        TXSTAbits.CSRC = 1;
    }

    if(config&0x08)      // Continuous or single reception
    {
        RCSTAbits.CREN = 1;
    }
    else
    {
        RCSTAbits.SREN = 1;
    }

    if(config&0x10)      // Baud rate select (asychronous mode only)
    {
        TXSTAbits.BRGH = 1;
    }

    if(config&0x20)  // Address Detect Enable
    {
         RCSTAbits.ADEN = 1;
    }

    if(config&0x40)      // Interrupt on receipt
    {
        PIR1bits.RCIF = 0;
        PIE1bits.RCIE = 1;

    }
    else
    {
        PIE1bits.RCIE = 0;
    }

    if(config&0x80)      // Interrupt on transmission
    {
        PIR1bits.TXIF = 0;
        PIE1bits.TXIE = 1;
    }
    else
    {
        PIE1bits.TXIE = 0;
    }

    SPBRG = spbrg;       // Write baudrate to SPBRG1
    SPBRGH = spbrg >> 8; // For 16-bit baud rate generation

    TXSTAbits.TXEN = 1;  // Enable transmitter
    RCSTAbits.SPEN = 1;  // Enable receiver
}

/******************************************************************************/
/* UARTchar
 *
 * The function sends one character over the UART.
/******************************************************************************/
void UARTchar(unsigned char data)
{
    if(data != 0)
    {
        TXREG = data;      // Write the data byte to the USART
        while(!TXSTAbits.TRMT); //Wait for previous character to be output
    }
}

/******************************************************************************/
/* UARTstring
 *
 * The function sends a group of characters over the UART.
/******************************************************************************/
void UARTstring(unsigned char *data)
{
    while(*data != 0)
    {
        UARTchar(*data); // Transmit a byte
        *data++;
    }    
}

/******************************************************************************/
/* ReadUSART
 *
 * The function reads the UART and returns the data read.
/******************************************************************************/
unsigned char ReadUSART(void)
{
    unsigned char data;   // Holds received data
    
    if(RCSTAbits.OERR)
    {
        /* If an overrun error occured clear error */
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
        return FALSE;
    }

    data = RCREG;                      // Read data
    return (data);                     // Return the received data
}

/******************************************************************************/
/* UART_send_break
 *
 * The function sends the break character over the UART. This break is always
 *   12 bits of 0.
/******************************************************************************/
void UART_send_break(void)
{
    TXSTAbits.SENDB = TRUE;
    TXREG = 'd';                // arbitrary
    while(TXSTAbits.SENDB);    // Break sent successfully
}

/******************************************************************************/
/* UARTstringCRLN
 *
 * The function sends a group of characters over the UART. There is a carriage
 *  retun and line feed after the string.
/******************************************************************************/
void UARTstringCRLN(unsigned char *data)
{
    while(*data != 0)
    {
        UARTchar(*data); // Transmit a byte
        *data++;
    }
    UARTstring((unsigned char*)"\r\n");
}
/******************************************************************************/
/* UARTcommandMenu
 *
 * The function sends the Command string followed by a bunch of dashes followed
 *  by the description string used for the command menu.
/******************************************************************************/
void UARTcommandMenu(unsigned char *Command, unsigned char *Desc)
{
    unsigned char place = 0;
    unsigned char i;

    while((*Command != 0) && (place < MaxCommandLen))
    {
        UARTchar(*Command); // Transmit a byte
        *Command++;
        place++;
    }
    for(i =0; i < ((MaxCommandLen - place) + 1); i++)
    {
        UARTchar('-');
    }
    place = 0;
    while((*Desc != 0) && (place < MaxDescLen))
    {
        UARTchar(*Desc); // Transmit a byte
        *Desc++;
        place++;
    }
    UARTstring((unsigned char*)"\r\n");
}
/******************************************************************************/
/* EraseScreen
 *
 * The function does a carriage return and rints all spaces (blank character)
 *  then does anoth carriage return. This is used to clear the screen on a
 *  terminal software.
/******************************************************************************/
void EraseScreen(unsigned char characters)
{
    unsigned char i;
    
    UARTchar('\r');
    for (i = 0; i < characters; i++)
    {
        UARTchar(' ');
    }
    UARTchar('\r');
}
/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/