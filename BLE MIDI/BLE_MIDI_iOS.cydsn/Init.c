/*******************************************************************************
* File Name: Init.c
*
* Description:
*  This file contains system initialization routines
* 
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <BLE_Interface.h>
#include <Configuration.h>
#include <Logger.h>
#include <Project.h>
#include <UI.h>

/*******************************************************************************
* Function Name: InitializeSystem
********************************************************************************
* Summary:
*        Start the components and initialize the system 
*
* Parameters:
*  void
*
* Return:
*  uint8 - status of the initialization
*
*******************************************************************************/
uint8 InitializeSystem(void)
{
    uint8 apiReturn = 1;

    CyGlobalIntEnable;

    PWM_Start();

    UI_Start();
    
    Midi_Start();

    apiReturn = BLE_Start();
    
#if LOGGING_ENABLED
    Logger_Start();
#endif
    
    return apiReturn;
}

/* [] END OF FILE */
