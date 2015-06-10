/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This is source code for the PSoC 4 BLE MIDI for iOS/Mac example project.
*
* Note:
*
* Owners:
*  KRIS@CYPRESS.COM
*
* Related Document:
*
* Hardware Dependency:
*  1. PSoC 4 BLE device
*  2. CY8CKIT-042 BLE Pioneer Kit
*
* Code Tested With:
*  1. PSoC Creator 3.1 CP1
*  2. ARM CM3-GCC
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <BLE_Interface.h>
#include <Configuration.h>
#include <Init.h>
#include <Logger.h>
#include <MIDI.h>
#include <Project.h>
#include <UI.h>

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Main function.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main()
{
    uint8 status;
    
    status = InitializeSystem();
    
    if(status == 0)
    {
        CYASSERT(0);    /* Something went wrong with BLE initialization, check */
    }

    while(1)
    {
        uint16 uiChangeStatus;

        BLE_Run();
        
        uiChangeStatus = UIInput_Run();
        
        Midi_Run(uiChangeStatus);
        
        UIOutput_Run();
        
        if(cyBle_pendingFlashWrite != 0u)
        {
            /* Store bounding data to flash only when all debug information has been sent */
            CyBle_StoreBondingData(0u);
        }
    }
}

/* [] END OF FILE */
