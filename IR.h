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
 * 04/09/15     1.0_DW0b    Fixed bugs.
 *                          Added features.
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

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define IR_LED_TIMEOUT 2500
#define IR_SIZE MaxScanEdgeChange

#define Old   0
#define Error   0
#define New     1
#define Repeat  2

#define IR 66

#define Start1  7
#define Start2  8
#define Data1   9
#define Data2   10
#define End1    11
#define End2    12
#define Finished    13
#define Repeat1    14
#define Repeat2    15
#define Repeat3    16
#define Repeat4    17
#define Repeat5    18
#define Receiving  19

/******************************************************************************/
/* IRtimeoutLoops
 *
 * This controls how many loops around the main program are allowed to pass
 *  before the IR receiver tiemsout and there can no longer be a repeat code.
/******************************************************************************/
#define IRtimeoutLoops 5000

/******************************************************************************/
/* MaxScanEdgeChange
 *
 * Max number to bit flip flops.
/******************************************************************************/
/* this is equal to (number of bits + header) *2*/
#define MaxScanEdgeChange 72

/******************************************************************************/
/* IRmodCalOn and IRmodCalOff
 *
 * These numbers are used to calibrate the IR modulation. The modulation carrier
 *  needs to be 38kHz.
 *
 * IRmodCalOn is the amount of counts that the IR led is on.
 * IRmodCalOff - IRmodCalOn is the amount of counts that the IR led is off.
/******************************************************************************/
#define IRmodCalOn  9
#define IRmodCalOff 17

/******************************************************************************/
/* NEC bit Timing for Receive at 64 MHz with a prescaler of 32                */
/******************************************************************************/
/* Starting bit High */
/* Nominal 9mS*/
#define StartbitHIGHnominal 4500
/* High Tolerance is 10mS */
#define StartbitHIGHupper 5000
/* Low Tolerance is 8mS */
#define StartbitHIGHlower 4000

/* Starting bit Low */
/* Nominal 4.5mS*/
#define StartbitLOWnominal 2250
/* High Tolerance is 5.5mS */
#define StartbitLOWupper 2750
/* Low Tolerance is 3.5mS */
#define StartbitLOWlower 1750

/* Logical 0 burst/Space and Logical 1 burst*/
/* Nominal 0.5625mS*/
#define DataShortnominal 281
/* High Tolerance is 1.2mS */
#define DataShortupper 600
/* Low Tolerance is 0.25mS */
#define DataShortlower 125

/* Logical 1 space */
/* Nominal 1.6875mS*/
#define DataLongnominal 844
/* High Tolerance is 1.9mS */
#define DataLongupper 950
/* Low Tolerance is 1.4mS */
#define DataLonglower 700

/* Pause */
/* Nominal 40mS*/
#define PauseBurstnominal 20000
/* High Tolerance is 42mS */
#define PauseBurstupper 21000
/* Low Tolerance is 38mS */
#define PauseBurstlower 19000

/* Nominal 2.25mS*/
#define PauseSpacenominal 1125
/* High Tolerance is 2.5mS */
#define PauseSpaceupper 1250
/* Low Tolerance is 1.85mS */
#define PauseSpacelower 925

/* Repeat 96mS*/
#define Repeatnominal 47500
/* High Tolerance is 100mS */
#define Repeatupper 50000
/* Low Tolerance is 90mS */
#define Repeatlower 45000

/******************************************************************************/
/* Macro Functions                                                            */
/******************************************************************************/

/******************************************************************************/
/* IRLEDmodON()
 *
 * The function turns on the IR 38kHz modulation.
/******************************************************************************/
#define IRLEDmodON() (IRmod = TRUE)

/******************************************************************************/
/* IRLEDmodOFF()
 *
 * The function turns off the IR 38kHz modulation.
/******************************************************************************/
#define IRLEDmodOFF() (IRmod = FALSE)

/******************************************************************************/
/* IRreceiverIntOn()
 *
 * The function turns on the port b interrupt associated with the ir receiver.
/******************************************************************************/
#define IRreceiverIntOn() (IOCBbits.IOCB4 = ON)

/******************************************************************************/
/* IRreceiverIntOff()
 *
 * The function turns off the port b interrupt associated with the ir receiver.
/******************************************************************************/
#define IRreceiverIntOff() (IOCBbits.IOCB4 = OFF)

/******************************************************************************/
/* NEC bit Timing for Receive at 64 MHz with a prescaler of 16 and postcaler
 *  of 15 * RF_IR_Postscaler                 
/******************************************************************************/
#define Scale_StartbitHIGHnominal 150 // RF_IR_Postscaler = 4
#define Scale_StartbitLOWnominal 75 // RF_IR_Postscaler = 4
#define Scale_DataShortnominal 38 // RF_IR_Postscaler = 1
#define Scale_DataLongnominal 113 // RF_IR_Postscaler = 1
#define Scale_PauseBurstnominal 242 // RF_IR_Postscaler = 11
#define Scale_PauseSpacenominal 149 // RF_IR_Postscaler = 1
#define Scale_Repeatnominal 246 // RF_IR_Postscaler = 26

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
extern unsigned char IRpinOLD;
extern unsigned int IRRawCode[IR_SIZE];
extern unsigned long IR_NEC;
extern unsigned char IRrawCodeNum;
extern unsigned char IR_New_Code;
extern unsigned char IRbit;
extern unsigned char IRrepeatflag;
extern unsigned char IRsendFlag;
extern unsigned char IRcodeBit;
extern unsigned char IRcodePlace;
extern unsigned long IRsendCode;
extern unsigned char IRrepeatAmount;
extern volatile unsigned char IRmod;
extern volatile unsigned char ReceivingIR;
extern volatile unsigned char IRbitPosition;
extern unsigned long IRtimeout;

/******************************************************************************/
/* Function prototypes                                                        */
/******************************************************************************/
void InitIR(void);
unsigned char ReadIRpin(void);
unsigned char IRrawToNEC(unsigned int* Raw, unsigned long* NEC, unsigned char Invert);
void UseIRCode(unsigned char* Code, unsigned long NEC);
unsigned char SendNEC_bytes(unsigned long code, unsigned char RepeatAmount);
void SendNEC_wait(unsigned long code, unsigned char RepeatAmount);
void CalibrateIR(void);
unsigned char CheckReceivingIR(void);
unsigned char DecodeNEC(unsigned long Nec, unsigned char* address, unsigned char* command);
unsigned long EncodeNEC(unsigned char address, unsigned char command);

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/