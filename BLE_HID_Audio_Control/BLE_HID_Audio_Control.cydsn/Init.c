/*******************************************************************************
* File Name: Init.c
*
* Version: 1.0
*
* Description:
* This file contains system initialization routine
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <BLE Interface.h>
#include <Configuration.h>
#include <Project.h>

/*******************************************************************************
* Function Name: InitializeSystem
********************************************************************************
*
* Summary:
*  Systm initialization function.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
void InitializeSystem(void)
{
    CyGlobalIntEnable;      /* Enable Global Interrupts*/
    
    BLE_Interface_Start();  /* Kick start the BLE HID GATT server interface */

#if (CONSOLE_LOG_ENABLED)
    Debug_Console_Start();  /* Consolle log interface */
#endif /* End of #if (CONSOLE_LOG_ENABLED) */
}

#if CONSOLE_LOG_ENABLED
/*******************************************************************************
* Function Name: _write
********************************************************************************
*
* Summary:
*  Initialize the printf stream to use the UART console instantiated in the 
*  project.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
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
#endif

/* [] END OF FILE */
