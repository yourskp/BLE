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

#include <BLEConnection.h>
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
    
    BLE_Engine_Start();     /* Kick start the BLE GATT server and client interface */

#if (CONSOLE_LOG_ENABLED)
    Debug_Console_Start();  /* Consolle log interface */
#endif /* End of #if (CONSOLE_LOG_ENABLED) */
}
/* [] END OF FILE */
