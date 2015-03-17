/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This project demonstrates how to read and write user SFLASH locations from
*  firmware.
*
* Owner:
*  kris@cypress.com
*
* Hardware Dependency:
*  1. PSoC 4 BLE device
*
* Code Tested With:
*  1. PSoC Creator 3.1 SP1
*  2. ARM GCC 4.8.4
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <WriteUserSFlash.h>

char HexToAscii(uint8 value, uint8 nibble);

#define SWITCH_PRESSED                  (0u)
#define USER_SFLASH_ROW_SIZE            (128u)
#define SFLASH_STARTING_VALUE           (0x80)
#define USER_SFLASH_ROWS                (4u)
#define USER_SFLASH_BASE_ADDRESS        (0x0ffff200u)

int main()
{
    uint32 data[USER_SFLASH_ROW_SIZE/4];
    uint8 *sflashPtr;
    uint8 rowIndex;
    uint32 dataIndex;
            
    CyGlobalIntEnable;
    
    UART_Console_Start();
    
    if(Write_Switch_Read() == SWITCH_PRESSED)
    {
        for(dataIndex = 0; dataIndex < (USER_SFLASH_ROW_SIZE/4); dataIndex++)
        {
            data[dataIndex] = SFLASH_STARTING_VALUE + dataIndex;  
        }
        
        for(rowIndex = 0; rowIndex < USER_SFLASH_ROWS; rowIndex++)
        {
            uint32 status;
            
            UART_Console_UartPutString("\r\nWriting user SFLASH row");
            UART_Console_UartPutChar('0' + rowIndex);
            status  = WriteUserSFlashRow(rowIndex, &data[0]);
            
            if(status == USER_SFLASH_WRITE_SUCCESSFUL)
            {
                UART_Console_UartPutString(" successful");
            }
            else
            {
                UART_Console_UartPutString(" failed - ");
                UART_Console_UartPutChar(HexToAscii(HI8(HI16(status)),1));
                UART_Console_UartPutChar(HexToAscii(HI8(HI16(status)),0));
                UART_Console_UartPutChar(HexToAscii(LO8(HI16(status)),1));
                UART_Console_UartPutChar(HexToAscii(LO8(HI16(status)),0));
            }
        }
        UART_Console_UartPutString("\r\nUser SFLASH write complete\r\n");
    }
    
    sflashPtr = (uint8 *)USER_SFLASH_BASE_ADDRESS;

    /* Read all the 512 bytes of user configurable SFLASH content and display on UART console */
    for(rowIndex = 0; rowIndex < USER_SFLASH_ROWS; rowIndex++)
    {
        UART_Console_UartPutString("\r\n\nUser SFLASH row ");
        UART_Console_UartPutChar('0' + rowIndex);
        UART_Console_UartPutString(" data:\r\n");
        
        for(dataIndex = 0; dataIndex < USER_SFLASH_ROW_SIZE; dataIndex++)
        {
            uint8 readData;
            
            readData = *sflashPtr++;
            
            UART_Console_UartPutChar(HexToAscii(readData,1));
            UART_Console_UartPutChar(HexToAscii(readData,0));
            UART_Console_UartPutChar(' ');
        }
    }

    while(1);
}

char HexToAscii(uint8 value, uint8 nibble)
{
    if(nibble == 1)
    {
        value = value & 0xf0;
        value = value >> 4;
        
        /* bit-shift the result to the right by four bits (i.e. quickly divides by 16)*/
        if (value >9)
        {
            value = value - 10 + 'A';
        }
        else
        {
            value = value + '0';
        }
    }
    else if (nibble == 0)
    {
        /* means use a bitwise AND to take the bottom four bits from the byte,
        0x0F is 00001111 in binary*/
        value = value & 0x0F;
        if (value >9)
        {
            value = value - 10 + 'A';
        }
        else
        {
            value = value + '0';
        }
    }
    else
    {
        value = ' ';    
    }
    
    return value;
}

/* [] END OF FILE */
