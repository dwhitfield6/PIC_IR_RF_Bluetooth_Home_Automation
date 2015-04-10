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
/* Contains functions for IR receiver.
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

#include "user.h"          /* User funct/params, such as InitApp */
#include "IR.h"          /* User funct/params, such as InitApp */
#include "Timer.h"          /* User funct/params, such as InitApp */
#include "MISC.h"          /* User funct/params, such as InitApp */
#include "UART.h"          /* User funct/params, such as InitApp */
#include "Button.h"          /* User funct/params, such as InitApp */
#include "EEPROM.h"          /* User funct/params, such as InitApp */
#include "RF.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
unsigned char IRpinOLD;
unsigned int IRRawCode[IR_SIZE];
unsigned long IR_NEC;
unsigned char IRrawCodeNum = 0;
unsigned char IR_New_Code  = 0;
unsigned char IRsendFlag = 0;
unsigned char IRcodeBit = 0;
unsigned char IRcodePlace = 1;
unsigned long IRsendCode = 0;
unsigned char IRbit = 0;
unsigned char IRrepeatflag = FALSE;
unsigned char IRrepeatAmount = 0;
volatile unsigned char IRmod = FALSE;
volatile unsigned char ReceivingIR = Finished;
volatile unsigned char IRbitPosition = 32;
unsigned char IRaddress = 0;
unsigned char IRcommand = 0;
unsigned long IRtimeout = IRtimeoutLoops + 1;

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* InitIR
 *
 * The function initializes IR receiver.
/******************************************************************************/
void InitIR(void)
{
    /* Turn on interrupt on pin RB4 */
    IRreceiverIntOn();
    /* Initialize compare value of IR pin */
    IRpinOLD = ReadIRpin();
    /* Timer 0 is used for IR timing */
    InitTimer0();
    /* Clean the code buffers */
    cleanBuffer16bit(IRRawCode, IR_SIZE);
    IR_NEC = 0;
    Timer0OFF();
    /* Reset the timer */
    ResetTimer0();
    /*Clear flag and disable interrupts */
    DisableTimer0Int();
    INTCONbits.TMR0IF = 0;
    /* Enable RB port change on interrupt */
    INTCONbits.RBIF = FALSE;
    INTCONbits.RBIE = TRUE;
}

/******************************************************************************/
/* ReadIRpin
 *
 * The function reads the IR pin and return a 0 if low and 1 if High.
/******************************************************************************/
unsigned char ReadIRpin(void)
{
    //READ RB4
    if((PORTB & IRreceiver) == IRreceiver)
    {
        return (1);
    }
    else
    {
        return 0;
    }
}

/******************************************************************************/
/* IRrawToNEC
 *
 * The function converts from raw counts to NEC code.
/******************************************************************************/
unsigned char IRrawToNEC(unsigned int* Raw, unsigned long* NEC, unsigned char Invert)
{
    /* the Protocol is as follows
     *
     * 1. A 9ms leading pulse burst (16 times the pulse burst length used for
     *   a logical data bit)
     *
     * 2. A 4.5ms space
     *
     * 3. The 8-bit address for the receiving device
     *
     * 4. The 8-bit logical inverse of the address
     *
     * 5. The 8-bit command
     *
     * 6. The 8-bit logical inverse of the command
     *
     * 7. A final 562.5탎 pulse burst to signify the end of message transmission.
     *
     * 8. A 40 mS pause signifies the end of entire transmission
     *
     * 9. A 9ms leading pulse burst
     *
     * 10. A 2.25ms space
     *
     * 11.A 562.5탎 pulse burst to mark the end of the space
     *  (and hence end of the transmitted repeat code).
     */
    unsigned char i;
    unsigned char first =0;
    unsigned char StartBit=IR_SIZE;
    unsigned long NECtemp = 0;

    /* Start Bit begin*/
    for (i =0; i < IR_SIZE; i++)
    {
        if(Raw[i] >= Repeatlower && Raw[i] <= Repeatupper )
        {
            /* This is the Repeat bit*/
            return Repeat;
        }
        else if(Raw[i] >= StartbitHIGHlower && Raw[i] <= StartbitHIGHupper )
        {
            /* This is the Start bit*/
            StartBit = i;
            break;
        }
    }
    if(IR_SIZE - StartBit < MaxScanEdgeChange)
    {
        /* No start bit found  or found too late in buffer */
        return Error;
    }

    StartBit++;
    if(Raw[StartBit] >= PauseSpacelower && Raw[StartBit] <= PauseSpaceupper )
    {
        /* This is a repeat code most likely */
        if(Raw[StartBit] >= DataShortlower && Raw[StartBit] <= DataShortupper )
        {
            /* This is a repeat code most likely */
            if(Raw[StartBit] >= DataShortlower && Raw[StartBit] <= DataShortupper )
            {
                /* This is a repeat code most likely */
                if(Raw[StartBit] >= Repeatlower && Raw[StartBit] <= Repeatupper)
                {
                    /* This is a repeat code */
                    return Repeat;
                }
            }
        }
        
    }
    else if(Raw[StartBit] < StartbitLOWlower || Raw[StartBit] > StartbitLOWupper )
    {
        /* Start bit is wrong */
        return Error;
    }
    
    StartBit++;
    for(i = StartBit; i < IR_SIZE;i++)
    {
        if(Raw[StartBit] >= DataShortlower && Raw[StartBit] <= DataShortupper )
        {
            if(!first)
            {
                /* This is the beginning of the bit*/
                first = 1;
            }
            else
            {
                /* We just got 2 562.5 uS Pulses in a row */
                first = 0;
                NECtemp <<= 1;
                if(!Invert)
                {
                    NECtemp++;
                }
            }
        }
        else if(Raw[StartBit] >= DataLonglower && Raw[StartBit] <= DataLongupper)
        {
            if(first)
            {
                /* This is a logical Zero */
                NECtemp <<= 1;
                if(Invert)
                {
                    NECtemp++;
                }
                first = 0;
            }
            else
            {
                /* we just got 2 longs in a row which should never happen*/
                return Error;
            }
        }
        else if(Raw[StartBit] == 0 && first == 1)
        {
            /*
             * We got the final 562.5탎 pulse burst to signify the end of
             *  message transmission and the rest of the buffer is empty.
             */
            *NEC = NECtemp;
            return New;
        }
        else if(Raw[StartBit] >= PauseBurstlower && Raw[StartBit] <= PauseBurstupper)
        {
            /* Pause bit */
            *NEC = NECtemp;
            return New;
        }
        else if(Raw[StartBit] >= Repeatlower && Raw[StartBit] <= Repeatupper)
        {
            /* repeat character */
            return Repeat;
        }
        else
        {
            return Error;
        }
        StartBit++;
    }
    /* Cant get here but do this to make compiler happy */
    return Error;
}

/******************************************************************************/
/* UseIRCode
 *
 * The function is called after the IR receiver either gets a repeat code or
 *  a new code.
/******************************************************************************/
void UseIRCode(unsigned char* Code, unsigned long NEC)
{
    unsigned char   i,j;
    unsigned char found = FALSE;

    LEDTimerON();
    if(*Code == 2 || *Code == 1)
    {
        DecodeNEC(NEC, &IRaddress, &IRcommand);
        if(ReadPushButton())
        {
            Global.SWNECcode = NEC;
            if(SyncGlobalToEEPROM())
            {
                /* Successful EEPROM burn */
                for(i =0;i<10;i++)
                {
                   GreenLEDON();
                   delayUS(50000);
                   GreenLEDOFF();
                   delayUS(50000);
                }
            }
            else
            {
                /* Unsuccessful EEPROM burn */
                for(i =0;i<10;i++)
                {
                   RedLEDON();
                   delayUS(50000);
                   RedLEDOFF();
                   delayUS(50000);
                }
            }            
        }
        else
        {
            if(NEC == Global.SWNECcode)
            {
                if(*Code == 2)
                {
                    RedLEDON();
                }
                GreenLEDON();
                SendRF_Channel(ReadCodeButtons());
                found = TRUE;
            }
            else
            {
                for(j=0; j < MirrorButtonsAmount; j++)
                {
                    for(i=0; i < RFcodesAmount; i++)
                    {

                        if(IRaddress == Global.RemoteButtonRF[i][j][0])
                        {
                            if(IRcommand == Global.RemoteButtonRF[i][j][1])
                            {
                                if(*Code == 2)
                                {
                                    RedLEDON();
                                }
                                SendRF_Channel(i);
                                found = TRUE;
                            }
                        }
                    }
                }
            }

            if(found == FALSE)
            {
                RedLEDON();                
            }
        }
    }
    *Code = 0;
    IRpinOLD = ReadIRpin();
    INTCONbits.RBIF = FALSE;
    IRreceiverIntOn();
    INTCONbits.RBIE = TRUE;
}
/******************************************************************************/
/* SendNEC_bytes
 *
 * The function sends the following NEC code to the IR LED (unmodulated).
/******************************************************************************/
unsigned char SendNEC_bytes(unsigned long code, unsigned char RepeatAmount)
{
     /* the Protocol is as follows
     *
     * 1. A 9ms leading pulse burst (16 times the pulse burst length used for
     *   a logical data bit)
     *
     * 2. A 4.5ms space
     *
     * 3. The 8-bit address for the receiving device
     *
     * 4. The 8-bit logical inverse of the address
     *
     * 5. The 8-bit command
     *
     * 6. The 8-bit logical inverse of the command
     *
     * 7. A final 562.5탎 pulse burst to signify the end of message transmission.
     *
     * 8. A 40 mS pause signifies the end of entire transmission
     *
     * 9. A 9ms leading pulse burst
     *
     * 10. A 2.25ms space
     *
     * 11.A 562.5탎 pulse burst to mark the end of the space
     *  (and hence end of the transmitted repeat code).
     */

    if(Sent)
    {
        /* Previous send finished */
        if(!code)
        {
            return FAIL;
        }
        RF_IR = IR;
        Sent = NO;
        IRsendFlag = 1;
        IRcodeBit = 0;
        IRcodePlace = Start1;
        IRsendCode = Reverse_4Byte(code);
        if(RepeatAmount)
        {
            IRrepeatflag = TRUE;
        }
        IRrepeatAmount = RepeatAmount;
        ResetTimer2();
        SetTimer2(Scale_StartbitHIGHnominal);
        RF_IR_Postscaler = 4;
        IRLEDmodON();
        Timer2ON();
        return PASS;
    }
    return SENDING;
}

/******************************************************************************/
/* SendRF_wait
 *
 * Sends the code and waits here until finished. Send the 'amount' of repeats
 *  passed in as amount. The modulation is done here without interrupts.
/******************************************************************************/
void SendNEC_wait(unsigned long code, unsigned char RepeatAmount)
{
    unsigned char toggle = 0;
    while(CheckReceivingIR()); // Wait for IR to get received.

    IRreceiverIntOff();
    INTCONbits.RBIE = OFF;
    INTCONbits.PEIE = OFF;

    SendNEC_bytes(code, RepeatAmount);
    while(!Sent)
    {
        if(IRmod)
        {
            /* This needs to oscilate at 38kHz */            
            if(toggle < IRmodCalOn)
            {
                IRLEDon();
            }
            else if(toggle < IRmodCalOff)
            {
                IRLEDoff();
            }
            else
            {
                toggle = 0;
            }
            toggle++;
        }
        else
        {
            IRLEDoff();
            toggle = 0;
        }
    }
    
    if(IRbitPosition)
    {
        /* this is means that the ir code did not completely send */
        NOP();
    }

    INTCONbits.PEIE = ON;
    delayUS(8000);
    IRpinOLD = ReadIRpin();
    INTCONbits.RBIF = FALSE;
    IRreceiverIntOn();
    INTCONbits.RBIE = ON;
}

/******************************************************************************/
/* CalibrateIR
 *
 * This function is used to calibrate the IR sending. This function does not
 *  return.
/******************************************************************************/
void CalibrateIR(void)
{
    unsigned char toggle = 0;
    INTCONbits.GIE  = OFF;
    INTCONbits.RBIE = OFF;
    IRreceiverIntOff();
    INTCONbits.PEIE = OFF;

    while(1)
    {
            /* This needs to oscilate at 38kHz */
            if(toggle < IRmodCalOn)
            {
                IRLEDon();
            }
            else if(toggle < IRmodCalOff)
            {
                IRLEDoff();
            }
            else
            {
                toggle = 0;
            }
            toggle++;
    }
}

/******************************************************************************/
/* CheckReceivingIR
 *
 * This function return true if we are in the process of receiving an IR code.
 *  Otherwise it returns false.
/******************************************************************************/
unsigned char CheckReceivingIR(void)
{
    if(ReceivingIR == Finished)
    {
        return FALSE;
    }
    return TRUE;
}

/******************************************************************************/
/* EncodeNEC
 *
 * This function takes in a command and address and makes the NEC code.
/******************************************************************************/
unsigned long EncodeNEC(unsigned char address, unsigned char command)
{
    unsigned long   temp1,
                    temp2,
                    temp3,
                    temp4;

    /*
     * The encoding scheme is as follows:
     * the address and command are both least significant bit first
     * and are both 1 byte.
     * |Address|inverted address|command|inverted command|
     */
    temp1 = (unsigned long) Reverse_Byte(address);
    temp2 = (unsigned long) Reverse_Byte(~address);
    temp3 = (unsigned long) Reverse_Byte(command);
    temp4 = (unsigned long) Reverse_Byte(~command);

    return ((temp1 << 24) + (temp2 << 16) + (temp3 << 8) + temp4);
}

/******************************************************************************/
/* DecodeNEC
 *
 * This function takes in the NEC code and calculates the address adn command.
/******************************************************************************/
unsigned char DecodeNEC(unsigned long Nec, unsigned char* address, unsigned char* command)
{
    unsigned char   temp1,
                    temp2,
                    temp3;

    temp1 = ~Reverse_Byte((unsigned char) (Nec & 0x00FF));
    temp2 = Reverse_Byte((unsigned char) ((Nec & 0xFF00)>> 8));
    if(temp1 != temp2)
    {
        /* the logic inverse version doe not match */
        return FAIL;
    }
    temp1 = ~Reverse_Byte((unsigned char) ((Nec & 0x00FF0000) >> 16));
    temp3 = Reverse_Byte((unsigned char) ((Nec & 0xFF000000) >> 24));
    if(temp1 != temp3)
    {
        /* the logic inverse version doe not match */
        return FAIL;
    }
    *command = temp2;
    *address = temp3;
    return PASS;
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/