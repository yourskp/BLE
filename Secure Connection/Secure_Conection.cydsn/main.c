/*******************************************************************************
* File Name: main.c
*
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This project demonstrates the operation of establishing a secure BLE connection
*  using Heart Rate Profile in Server (Peripheral) role as an example.
*
* Note:
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "hrss.h"

volatile uint32 mainTimer = 0;
uint8 advState = ADV_STATE_GENERAL;
CYBLE_API_RESULT_T apiResult;


/*******************************************************************************
* Function Name: AppCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
*******************************************************************************/
void AppCallBack(uint32 event, void* eventParam)
{

#ifdef DEBUG_OUT    
    DebugOut(event, eventParam);
#endif

    switch(event)
    {
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            Disconnect_LED_Write(LED_ON);
            mainTimer = 0;
        case CYBLE_EVT_STACK_ON:
            heartRateSimulation = DISABLED;
            /* Put the device into discoverable mode so that remote can search it. */
            StartAdvertisement(ADV_STATE_GENERAL);
            Advertising_LED_Write(LED_ON);
            advState = ADV_STATE_GENERAL;
            break;
            
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED) /* ADV timed out */
            {
                if(advState == ADV_STATE_GENERAL)
                {
                    Advertising_LED_Write(LED_OFF);
                    CyDelay(100);
                    StartAdvertisement(ADV_STATE_WHITELIST);
                    Advertising_LED_Write(LED_ON);
                    advState = ADV_STATE_WHITELIST;
                }
                else
                {
                    /* No connectionestablished, shut down ADV */
                    Advertising_LED_Write(LED_OFF);
                }
            }
            break;    
            
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            Disconnect_LED_Write(LED_OFF);
            Advertising_LED_Write(LED_OFF);
            break;

        default:
            break;
    }
}

int main()
{
    CYBLE_LP_MODE_T lpMode;
    CYBLE_BLESS_STATE_T blessState;

    CyGlobalIntEnable;
    
    UART_DEB_Start();               /* Start communication component */
    printf("BLE Secure Connection Example Project \r\n");
    
    Disconnect_LED_Write(LED_OFF);
    Advertising_LED_Write(LED_OFF);

    /* Start CYBLE component and register generic event handler */
    apiResult = CyBle_Start(AppCallBack);
    if(apiResult != CYBLE_ERROR_OK)
    {
        printf("CyBle_Start API Error: %x \r\n", apiResult);
    }
    
    /* Services initialization */
    HrsInit();
    
    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1)
    {
        if(CyBle_GetState() != CYBLE_STATE_INITIALIZING)
        {
            /* Enter DeepSleep mode between connection intervals */
            lpMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
            CyGlobalIntDisable;
            blessState = CyBle_GetBleSsState();

            if(lpMode == CYBLE_BLESS_DEEPSLEEP) 
            {   
                if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
                {
                    /* Put the device into the Deep Sleep mode only when all debug information has been sent */
                    if((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u)
                    {
                        CySysPmDeepSleep();
                    }
                    else
                    {
                        CySysPmSleep();
                    }
                }
            }
            else
            {
                if(blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
                {
                    CySysPmSleep();
                }
            }
            CyGlobalIntEnable;
        }
        
        
        /***********************************************************************
        * Wait for connection established with Central device
        ***********************************************************************/
        if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
        {
            /*******************************************************************
            *  Periodically simulates heart beat and sends the results to the Client
            *******************************************************************/
            mainTimer++;
            
            if(mainTimer == MAIN_LOOP_SIMULATION_THRESHOLD)
            {
                mainTimer = 0u;
                if(heartRateSimulation == ENABLED)
                {
                    SimulateHeartRate();
                    CyBle_ProcessEvents();
                }
            }
            else if((cyBle_pendingFlashWrite != 0u) &&
                    ((UART_DEB_SpiUartGetTxBufferSize() + UART_DEB_GET_TX_FIFO_SR_VALID) == 0u))
            {
                apiResult = CyBle_StoreBondingData(0u);
                printf("Store bonding data, status: %x \r\n", apiResult);
            }
            else
            {
                /* nothing else */ 
            }
        }
        
        /*******************************************************************
        *  Process all pending BLE events in the stack
        *******************************************************************/
        CyBle_ProcessEvents();
    }
}

/* [] END OF FILE */
