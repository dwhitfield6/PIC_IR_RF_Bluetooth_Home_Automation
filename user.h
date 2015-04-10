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
#ifndef USER_H
#define	USER_H

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

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/******************************************************************************/
/* Pick Which System!!!
 * 
 * Choose if the system has a bluetooth module or not.
 *
/******************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#define BLUETOOTH

/******************************************************************************/
/* Product Name
 *
 * This is the name that is brodcasted over bluetooth.
/******************************************************************************/
unsigned char ProductName[50] = {"Home Automation"};

/******************************************************************************/
/* Serial Number
 *
 * This is the product serial number.
 * The serial number mush be less than 1000
/******************************************************************************/
unsigned long SN = 1;

/******************************************************************************/
/* LEDPostscaler
 *
 * This is parameter changes how long it takes to turn off an LED once it is
 *  turned on.
/******************************************************************************/
#define LEDPostscaler 3

/******************************************************************************/
/* PCB board
 *
 * This code is used with boards:
 * 1. IR_to_RF_w_bluetooth_revA
/******************************************************************************/

/******************************************************************************/
/* Important parameters.
 *
 * NominalVDD is the voltage of the ldo powering the MCU. This is ued for the
 *  ADC reference voltage.
 *
 * VoltageLow is the low voltage threshold. 
 *
 * VoltageHigh is the high voltage threshold.
 *
/******************************************************************************/
#define NominalVDD 3.3
#define VoltageLow 3.7
#define VoltageHigh 30.0

/******************************************************************************/
/* BAUD
 *
 * This define the BAUD rate used by the system.
/******************************************************************************/
#define BAUD 115200

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define ON 1
#define OFF 0
#define TRUE 1
#define FALSE 0
#define INPUT 1
#define OUTPUT 0
#define PASS 1
#define FAIL 0
#define YES 1
#define NO 0

/******************************************************************************/
/* Pin Defines                                                                */
/******************************************************************************/
/* Connected to voltage divider and input of the power supply */
#define InputVoltageTris	TRISAbits.TRISA0
#define InputVoltage 0x01 //RA0

/* Connected to the bluetooth modules PIO2: 'Connected' */
#define BLUE_ConnectedTris	TRISAbits.TRISA1
#define BLUE_Connected 0x02 //RA1

/* Connected to the bluetooth modules reset pin */
#define BLUE_ResetTris	TRISAbits.TRISA2
#define BLUE_Reset 0x04 //RA2

/* Connected to the bluetooth modules PIO3: 'AutoDiscovery' */
#define BLUE_AutoDiscoveryTris	TRISAbits.TRISA3
#define BLUE_AutoDiscovery 0x08 //RA3

/* Connected to the bluetooth modules PIO6: 'AutoMaster' */
#define BLUE_AutoMasterTris	TRISAbits.TRISA4
#define BLUE_AutoMaster 0x10 //RA4

/* Connected to the bluetooth modules PIO4: 'Factory reset' */
#define BLUE_FactoryTris	TRISAbits.TRISA5
#define BLUE_Factory 0x10 //RA5

/* Connected to the pushbutton switch */
#define PushbuttonTris	TRISCbits.TRISC0
#define Pushbutton 0x01 //RC0

/* Connected to the Red BiColor Led  */
#define BiRedTris	TRISCbits.TRISC1
#define BiRed 0x02 //RC1

/* Connected to the Green BiColor Led  */
#define BiGreenTris	TRISCbits.TRISC2
#define BiGreen 0x04 //RC2

/* Connected to the transistor powering the IR Led  */
#define IRLEDTris	TRISCbits.TRISC3
#define IRLED 0x08 //RC3

/* Connected to the code dip switchs to control power */
#define SWcodePwrTris	TRISCbits.TRISC4
#define SWcodePwr 0x10 //RC4

/* Connected to the RF transmitter */
#define RFtransTris	TRISCbits.TRISC5
#define RFtrans 0x20 //RC5

/* Connected to the bluetooth modules TX */
#define BLUE_TxTris	TRISCbits.TRISC6
#define BLUE_Tx 0x40 //RC6

/* Connected to the bluetooth modules RX */
#define BLUE_RxTris	TRISCbits.TRISC7
#define BLUE_Rx 0x80 //RC7

/* Connected to the number 4 code switch */
#define Swcode4Tris	TRISBbits.TRISB0
#define Swcode4 0x01//RB0

/* Connected to the number 3 code switch */
#define Swcode3Tris	TRISBbits.TRISB1
#define Swcode3 0x02//RB1

/* Connected to the number 2 code switch */
#define Swcode2Tris	TRISBbits.TRISB2
#define Swcode2 0x04//RB2

/* Connected to the number 1 code switch */
#define Swcode1Tris	TRISBbits.TRISB3
#define Swcode1 0x08//RB3

/* Connected to the IR receiver */
#define IRreceiverTris	TRISBbits.TRISB4
#define IRreceiver 0x10//RB4

/* Connected to the Voltage divider */
#define VoltageDividerTris	TRISBbits.TRISB5
#define VoltageDivider 0x20//RB5

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* SWcodePwrON()
 *
 * The function turns on the code switches to take a code reading.
/******************************************************************************/
#define SWcodePwrON()  (LATC &= ~SWcodePwr)

/******************************************************************************/
/* SWcodePwrOFF()
 *
 * The function turns off the code switches.
/******************************************************************************/
#define SWcodePwrOFF()  (LATC |= SWcodePwr)

/******************************************************************************/
/* RedLEDON()
 *
 * The function turns on the Red LED.
/******************************************************************************/
#define RedLEDON()  (LATC |= BiRed)

/******************************************************************************/
/* RedLEDOFF()
 *
 * The function turns off the Red LED.
/******************************************************************************/
#define RedLEDOFF() (LATC &= ~BiRed)

/******************************************************************************/
/* RedLEDTOGGLE()
 *
 * The function turns off the Red LED.
/******************************************************************************/
#define RedLEDTOGGLE() (LATC ^= BiRed)

/******************************************************************************/
/* GreenLEDON()
 *
 * The function turns on the Green LED.
/******************************************************************************/
#define GreenLEDON()  (LATC |= BiGreen)

/******************************************************************************/
/* GreenLEDOFF()
 *
 * The function turns off the GreenLED.
/******************************************************************************/
#define GreenLEDOFF() (LATC &= ~BiGreen)

/******************************************************************************/
/* GreenLEDTOGGLE()
 *
 * The function turns off the Green LED.
/******************************************************************************/
#define GreenLEDTOGGLE() (LATC ^= BiGreen)

/******************************************************************************/
/* RFon()
 *
 * The function turns on the Rf transmitter.
/******************************************************************************/
#define RFon() (LATC |= RFtrans)

/******************************************************************************/
/* RFoff()
 *
 * The function turns off the Rf transmitter.
/******************************************************************************/
#define RFoff() (LATC &= ~RFtrans)

/******************************************************************************/
/* IRLEDon()
 *
 * The function turns on the IR LED.
/******************************************************************************/
#define IRLEDon() (LATC |= IRLED)

/******************************************************************************/
/* IRLEDoff()
 *
 * The function turns off the IR LED.
/******************************************************************************/
#define IRLEDoff() (LATC &= ~IRLED)

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void InitApp(void);        
void Init_System (void);

#endif	/* USER_H */