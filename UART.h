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
/* Files to Include                                                           */
/******************************************************************************/
#ifndef UART_H
#define	UART_H

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

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "RF.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* TimeBetweenChars
 *
 * MicroSeconds between printing characters.
/******************************************************************************/
#define TimeBetweenChars 10

/******************************************************************************/
/* MaxCommandLen
 *
 * This is the Maximum commmand length.
/******************************************************************************/
#define MaxCommandLen   30

/******************************************************************************/
/* MaxDescLen
 *
 * This is the Maximum commmand description length.
/******************************************************************************/
#define MaxDescLen   70

/******************************************************************************/
/* Uart character and word spacing
 *
 * This parameter modifies the amount of time waited between characters and
 *   words when printing over the UART.
/******************************************************************************/

#define Character_Spacing   5000L

/*
 * A typical Character_Spacing value for fast printing is 5000. For slow
 *   printing 200,000 is common.
 */

#define Word_Spacing        15000L

/******************************************************************************/
/* RXbufsize
 *
 * This parameter is the size of the received array. Must be small so that the
 *  program gets built correctly.
/******************************************************************************/
#define RXbufsize 100

/******************************************************************************/
/* RXCommandsize
 *
 * This parameter is the size of the received array. Must be less than 256
/******************************************************************************/
#define RXCommandsize 100

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define Local_USART_TX_INT_ON       0b10000000  // Transmit interrupt on
#define Local_USART_TX_INT_OFF      0b00000000  // Transmit interrupt off
#define Local_USART_TX_INT_MASK     (~Local_USART_TX_INT_ON)	//Mask Trnasmit Interrupt select bit
#define Local_USART_RX_INT_ON       0b01000000  // Receive interrupt on
#define Local_USART_RX_INT_OFF      0b00000000  // Receive interrupt off
#define Local_USART_RX_INT_MASK     (~Local_USART_RX_INT_ON)	//Mask Receive Interrupt select bit
#define Local_USART_ADDEN_ON        0b00100000  // Enables address detection
#define Local_USART_ADDEN_OFF       0b00000000  // Disables address detection
#define Local_USART_ADDEN_MASK      (~Local_USART_ADDEN_ON)	//Mask address detection select bit
#define Local_USART_BRGH_HIGH       0b00010000  // High baud rate
#define Local_USART_BRGH_LOW        0b00000000  // Low baud rate
#define Local_USART_BRGH_MASK       (~Local_USART_BRGH_HIGH)	//Mask baud rate select bit
#define Local_USART_CONT_RX         0b00001000  // Continuous reception
#define Local_USART_SINGLE_RX       0b00000000  // Single reception
#define Local_USART_CONT_RX_MASK    (~Local_USART_CONT_RX) 	//Mask Continuous Reception select bit
#define Local_USART_SYNC_MASTER     0b00000100  // Synchrounous master mode
#define Local_USART_SYNC_SLAVE      0b00000000  // Synchrounous slave mode
#define Local_USART_SYNC_MASK       (~Local_USART_SYNC_MASTER)	//Mask usart mode select bit
#define Local_USART_NINE_BIT        0b00000010  // 9-bit data
#define Local_USART_EIGHT_BIT       0b00000000  // 8-bit data
#define Local_USART_BIT_MASK        (~Local_USART_NINE_BIT)		//Mask 9 bit transmit  select bit
#define Local_USART_SYNCH_MODE      0b00000001  // Synchronous mode
#define Local_USART_ASYNCH_MODE     0b00000000  // Asynchronous mode
#define Local_USART_MODE_MASK       (~Local_USART_SYNCH_MODE) 	//Mask sync/async mode select bit
#define USART_BRG16                 0b00001000  // Synchronous mode

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
extern volatile unsigned char ReceivedString[RXbufsize];
extern volatile unsigned char ReceivedStringPos;
extern volatile unsigned char CommandString[RXCommandsize];
extern volatile unsigned char CommandStringPos;
extern volatile unsigned char NewReceivedString;
const unsigned char SYNTAX_ERR[] = "Error: Syntax";
const unsigned char CRLN[] = "\r\n";
unsigned char BufferOverflow = FALSE;

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void Local_CloseUSART(void );
void InitUART(unsigned long Baud);
void OpenUSART( unsigned char config, unsigned int spbrg);
void UARTchar(unsigned char data);
void UARTchar_CONST(const unsigned char data);
void UARTstring(unsigned char *data);
void UARTstring_CONST(const unsigned char *data);
unsigned char ReadUSART(void);
void UART_send_break(void);
void UARTstringCRLN(unsigned char *data);
void UARTstringCRLN_CONST(const unsigned char *data);
void UARTcommandMenu(const unsigned char *Command,const unsigned char *Desc);
void EraseScreen(unsigned char characters);
void ClearUSART(void);
void PrintHeader(const unsigned char* Title);

#endif	/* UART_H */

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/