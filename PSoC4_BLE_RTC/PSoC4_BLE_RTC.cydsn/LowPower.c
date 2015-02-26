/*******************************************************************************
* File Name: LowPower.c
*
* Version: 1.0
*
* Description:
* This file contains routines for configuring the device in low power mode.
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <Configuration.h>
#include <Project.h>

/*******************************************************************************
* Function Name: HandleLowPowerMode
********************************************************************************
*
* Summary:
*  configures the peripherals used in the design and the device into possible
*  low power modes based on the state of the system.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
void HandleLowPowerMode (void)
{
    CYBLE_LP_MODE_T lpMode;
    CYBLE_BLESS_STATE_T blessState;
    uint32 intStatus;
    
    lpMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP); /* try configuring BLESS in DeepSleep mode */
        
    intStatus = CyEnterCriticalSection();
    
    blessState = CyBle_GetBleSsState(); /* Check the current state of the BLESS */

    if(lpMode == CYBLE_BLESS_DEEPSLEEP) /* BLESS had entered low power mode */
    {   
        /* and it continues to be in a state where BLESS can enter DeepSleep mode */
        if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
        {
            #if(CONSOLE_LOG_ENABLED)
                /* Put the device into the Dee Sleep mode only when all debug information has been sent */
            if((Debug_Console_SpiUartGetTxBufferSize() + Debug_Console_GET_TX_FIFO_SR_VALID) == 0u)
            {
                CySysPmDeepSleep();
            }
            else
            {
                /* Wait for UART interface to finish data transfer */
                CySysPmSleep();
            }
            #else
            CySysPmDeepSleep();    
            #endif    
        }
    }
    else
    {
        /* BLESS can't enter DeepSleep as next BLE connection interval is close by, go to Sleep for now */
        if(blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
        {
            CySysPmSleep();
        }
    }
    
    CyExitCriticalSection(intStatus);
}

/* [] END OF FILE */
