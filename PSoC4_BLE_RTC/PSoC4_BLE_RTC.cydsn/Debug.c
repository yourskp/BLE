/*******************************************************************************
* File Name: Debug.c
*
* Version: 1.0
*
* Description:
*  Debug code for Printf stdout mapping to UART interfaces
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <Configuration.h>
#include <project.h>
#include <stdio.h>

/* For GCC compiler revise _write() function for printf functionality */
int _write(int file, char *ptr, int len)
{
    int i;
    file = file;
    for (i = 0; i < len; i++)
    {
        Debug_Console_UartPutChar(*ptr++);
    }
    
    return len;
}

/* [] END OF FILE */
