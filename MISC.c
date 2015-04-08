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
/* Contains Miscellaneous Functions
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

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "MISC.h"
#include "system.h"
#include "user.h"

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* DelayUs
 * Input the number in microseconds to be delayed.
 *
 * The function waists loops for the entered bumber of cycles.
 * The actual delay can be scewed when interrupts are enabled.
/******************************************************************************/
void delayUS(long US)
{
    long i;
    for(i=0; i<US; i += delayConst)
    {
        NOP();
    }
}

/******************************************************************************/
/* DelayNOP
 *
 * The function waists the number of cycles passed into the function.
/******************************************************************************/
void delayNOP(unsigned char NOPs)
{
    unsigned char i;
    for(i=0; i<NOPs; i++)
    {
        NOP();
    }
}

/******************************************************************************/
/* CalibrateDelay
 *
 * This function never returns. An oscilloscope is to be connected to the LED
 *  pin and the delayConst is to be corrected until the scope shows a 100Hz
 *  square wave.
/******************************************************************************/
void CalibrateDelay(void)
{
    INTCONbits.PEIE = OFF; //Peripheral interrupts
    INTCONbits.GIE  = OFF; //Global interrupts
    while (1)
    {
        IRLEDon();
        delayUS(5000);
        IRLEDoff();
        delayUS(5000);
    }
}

/******************************************************************************/
/* IsLetter
 *
 * The function returns true if the data is a Letter.
/******************************************************************************/
unsigned char IsLetter(unsigned char data)
{
    if(data >= 65 && data <= 90)
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
/* IsCharacter
 *
 * The function returns true if the data is a printable character.
/******************************************************************************/
unsigned char IsCharacter(unsigned char data)
{
    if(data >= 32 && data <= 126)
    {
        return TRUE;
    }
    return FALSE;
}

/******************************************************************************/
/* cleanBuffer
 *
 * This function sets an amount of data in the array as 0.
/******************************************************************************/
void cleanBuffer(unsigned char* data, int count)
{
    unsigned char i=0;
    for(i=0; i<count;i++)
    {
        data[i]= 0;
    }
}

/******************************************************************************/
/* cleanBuffer16bit
 *
 * This function sets an amount of data in the array as 0.
/******************************************************************************/
void cleanBuffer16bit(unsigned int* data, int count)
{
    unsigned char i=0;
    for(i=0; i<count;i++)
    {
        data[i]= 0;
    }
}

/******************************************************************************/
/* round
 *
 * This function rounds to the nearest whole number.
/******************************************************************************/
double Round(double input)
{
    long temp = (long)(input + 0.5);

    return temp;
}

/******************************************************************************/
/* BufferCopy
 *
 * This function copies the 'from' array to the 'to' array.
/******************************************************************************/
void BufferCopy(unsigned char* from,unsigned char* to, unsigned char count, unsigned char shift)
{
    unsigned char i=0;
    cleanBuffer(to,count);
    for(i = shift; i>0;i--)
    {
        *to = ' ';
        to++;
    }
    while(*from != 0 && count >0)
    {
        *to = *from;
        from++;
        to++;
        count--;
    }
}

/******************************************************************************/
/* StringMatch
 *
 * This function returns TRUE if the array 'This' matches the array 'That'.
/******************************************************************************/
unsigned char StringMatch(unsigned char* This,const unsigned char* That)
{
    while(*This != 0)
    {
       if(*This != *That || *That == 0)
       {
           return FALSE;
       }
       This++;
       That++;
    }
    if(*That == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/******************************************************************************/
/* StringMatchCaseInsensitive
 *
 * This function returns TRUE if the array 'This' matches the array 'That' and
 *  it is not case sensitive.
/******************************************************************************/
unsigned char StringMatchCaseInsensitive(unsigned char* This,const unsigned char* That)
{
    unsigned char   tempThis,
                    tempThat;

    while(*This != 0)
    {
        tempThis = *This;
        tempThat = *That;
        lowercaseChar(&tempThis);
        lowercaseChar(&tempThat);

       if((tempThis != tempThat) || (tempThat == 0))
       {
           return FALSE;
       }
       This++;
       That++;
    }
    if(*That == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/******************************************************************************/
/* StringContains
 *
 * This function returns TRUE if the array 'That' is contained in the array
 *   'This'.
/******************************************************************************/
unsigned char StringContains(unsigned char* This, const unsigned char* That)
{
    unsigned char begin = FALSE;

    while(*This != 0)
    {
       if(*This == *That)
       {
           begin = TRUE;
           That++;
           if(*That == 0)
           {
               return TRUE;
           }
           if(*This == 0)
           {
               return FALSE;
           }
       }
       else
       {
           if(begin == TRUE)
           {
               return FALSE;
           }
       }
       This++;
    }
    return FALSE;
}

/******************************************************************************/
/* StringContainsCaseInsensitive
 *
 * This function returns TRUE if the array 'That' is contained in the array
 *   'This' and it is not case sensitive.
/******************************************************************************/
unsigned char StringContainsCaseInsensitive(unsigned char* This, const unsigned char* That)
{
    unsigned char   tempThis,
                    tempThat;
    unsigned char   begin = FALSE;

        while(*This != 0)
        {
            tempThis = *This;
            tempThat = *That;
            lowercaseChar(&tempThis);
            lowercaseChar(&tempThat);

            if(tempThis == tempThat)
            {
               begin = TRUE;
               That++;
               if(*That == 0)
               {
                   return TRUE;
               }
               if(*This == 0)
               {
                   return FALSE;
               }
            }
            else
            {
                if(begin == TRUE)
                {
                    return FALSE;
                }
            }
            This++;
    }
    return FALSE;
}

/******************************************************************************/
/* lowercaseString
 *
 * This function turns all characters to in the string to lowercase.
/******************************************************************************/
void lowercaseString(unsigned char* Input)
{
    while((*Input != 0))
    {
        if((*Input >= 65) && (*Input <= 90 ))
        {
            *Input += 32;
        }
        Input++;
    }
}

/******************************************************************************/
/* lowercaseChar
 *
 * This function turns the character to lowercase.
/******************************************************************************/
void lowercaseChar(unsigned char* Input)
{
    if((*Input >= 65) && (*Input <= 90 ))
    {
        *Input += 32;
    }
}

/******************************************************************************/
/* Reverse_Byte
 *
 * The function reads the value of 'This' and returns the reverse of the data.
/******************************************************************************/
unsigned char Reverse_Byte(unsigned char This)
{
    unsigned char temp=0;

    temp += (This & 0x01) << 7;
    temp += (This & 0x02) << 5;
    temp += (This & 0x04) << 3;
    temp += (This & 0x08) << 1;
    temp += (This & 0x10) >> 1;
    temp += (This & 0x20) >> 3;
    temp += (This & 0x40) >> 5;
    temp += (This & 0x80) >> 7;

    return temp;
}

/******************************************************************************/
/* Reverse_2Byte
 *
 * The function reads the value of 'This' and returns the reverse of the data.
/******************************************************************************/
unsigned int Reverse_2Byte(unsigned int This)
{
    unsigned int temp=0;

    temp += (This & 0x0001) << 15;
    temp += (This & 0x0002) << 13;
    temp += (This & 0x0004) << 11;
    temp += (This & 0x0008) << 9;
    temp += (This & 0x0010) << 7;
    temp += (This & 0x0020) << 5;
    temp += (This & 0x0040) << 3;
    temp += (This & 0x0080) << 1;
    temp += (This & 0x0100) >> 1;
    temp += (This & 0x0200) >> 3;
    temp += (This & 0x0400) >> 5;
    temp += (This & 0x0800) >> 7;
    temp += (This & 0x1000) >> 9;
    temp += (This & 0x2000) >> 11;
    temp += (This & 0x4000) >> 13;
    temp += (This & 0x8000) >> 15;

    return temp;
}

/******************************************************************************/
/* Reverse_4Byte
 *
 * The function reads the value of 'This' and returns the reverse of the data.
/******************************************************************************/
unsigned long Reverse_4Byte(unsigned long This)
{
    unsigned long temp=0;

    temp += (This & 0x00000001) << 31;
    temp += (This & 0x00000002) << 29;
    temp += (This & 0x00000004) << 27;
    temp += (This & 0x00000008) << 25;
    temp += (This & 0x00000010) << 23;
    temp += (This & 0x00000020) << 21;
    temp += (This & 0x00000040) << 19;
    temp += (This & 0x00000080) << 17;
    temp += (This & 0x00000100) << 15;
    temp += (This & 0x00000200) << 13;
    temp += (This & 0x00000400) << 11;
    temp += (This & 0x00000800) << 9;
    temp += (This & 0x00001000) << 7;
    temp += (This & 0x00002000) << 5;
    temp += (This & 0x00004000) << 3;
    temp += (This & 0x00008000) << 1;
    temp += (This & 0x00010000) >> 1;
    temp += (This & 0x00020000) >> 3;
    temp += (This & 0x00040000) >> 5;
    temp += (This & 0x00080000) >> 7;
    temp += (This & 0x00100000) >> 9;
    temp += (This & 0x00200000) >> 11;
    temp += (This & 0x00400000) >> 13;
    temp += (This & 0x00800000) >> 15;
    temp += (This & 0x01000000) >> 17;
    temp += (This & 0x02000000) >> 19;
    temp += (This & 0x04000000) >> 21;
    temp += (This & 0x08000000) >> 23;
    temp += (This & 0x10000000) >> 25;
    temp += (This & 0x20000000) >> 27;
    temp += (This & 0x40000000) >> 29;
    temp += (This & 0x80000000) >> 31;

    return temp;
}

/******************************************************************************/
/* GetNumber
 *
 * This function resembles scanf. Use 0x...for hex number.
/******************************************************************************/
unsigned char GetNumber(unsigned char* This, unsigned char CommaNumber, unsigned long* result)
{
    unsigned char i =0;
    long temp =0;
    unsigned char negative =0;
    unsigned char tempCommaNumber = 1;
    unsigned char count =0;
    unsigned char Hex = FALSE;
    unsigned char tempThis;

    while(*This != 0)
    {
        if(*This == '0')
        {
            count++;
            This++;
            if(*This == 'x' || *This == 'X')
            {
                Hex = TRUE;
            }
        }
        count++;
        This++;
    }
    This-=count;

    SCAN:while(*This != '=' && *This != ',' && *This != 0)
    {
        /* go all the way to the first comma */
        This++;
    }
    if(*This == 0)
    {
        return NOCOMMA; // there is no equal
    }
    else if(*This != '=' || *This != ',')
    {
        if(tempCommaNumber < CommaNumber)
        {
            tempCommaNumber++;
            This++;
            goto SCAN;
        }
    }
    if(*This == 0)
    {
        return NOCOMMA; // there is no equal
    }
    This++;
    if(*This == ' ')
    {
      This++;
      //move past the space
    }
    else if (*This == '=')
    {
         return DOUBLECOMMA; //double equal
    }
    else if(*This == '-')
    {
      negative = 1;
      This++;
      //move past the space
    }
    else if(*This == 0)
    {
        return NOVALUE; // there is no value after the equal
    }
    if(!Hex)
    {
        while(*This >=48 && *This <= 57)
        {
            temp = temp * 10;
            temp += *This - 48;
            i++;
            if(i>32)
            {
                return TOOBIG;//number too big
            }
            This++;
        }
    }
    else
    {
        negative = FALSE;
        while(*This != 'x' && *This != 'X')
        {
            This++;
        }
        This++;
        while((*This >=48 && *This <= 57) || (*This >=65 && *This <= 70) || (*This >= 97 && *This <= 102))
        {
            temp <<= 4;
            tempThis = *This;
            if(IsLetter(tempThis))
            {
                lowercaseChar(&tempThis);
                temp += (tempThis - 87);
            }
            else
            {
                temp += (*This - 48);
            }
            i++;
            if(i>32)
            {
                return TOOBIG;//number too big
            }
            This++;
        }
    }
    if(negative)
    {
        temp = (temp * -1);
    }
    *result = temp;
    return NoError;
}

/******************************************************************************/
/* StringAddEqual
 *
 * This function puts an equal sign between letters and a number value.
/******************************************************************************/
unsigned char StringAddEqual(unsigned char* Input)
{
    unsigned char i =0;
    unsigned char j =0;
    unsigned char firstnumber = 200;
    unsigned char temp[100];
    cleanBuffer(temp,100);
    while(Input[i] != 0)
    {
            if((Input[i] >= 48) && (Input[i] <= 57) || (Input[i] == '-'))
            {
                firstnumber = i;
                break;
            }
            i++;
    }
    if(firstnumber > 99)
    {
        /* there is no number*/
        return FALSE;
    }
    for(j=0;j<firstnumber;j++)
    {
           temp[j] = Input[j];
    }
    temp[firstnumber] = '=';
    for(j = (firstnumber);j<99;j++)
    {
        temp[j+1] = Input[j];
        if(Input[j] ==0)
        {
            break;
        }
    }
    BufferCopy(temp,Input,100, 0);
    return TRUE;
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/