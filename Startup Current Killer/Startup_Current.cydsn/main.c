/*******************************************************************************
* File Name: main.c
* 
* Version 1.0
*
* Author - kris@cypress.com
* 
* Description:
*  main code for Beaocn application.
*
********************************************************************************
* Copyright 2010-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#include <project.h>

#define TIMING_DEBUG_ENABLE                         (1u) /* Enable firmware debugging on state of GPIO */
#define USE_WCO_FOR_TIMING                          (1u) /* Enable this to configure the device in DeepSleep mode 
                                                          * when WCO is starting up */

#if USE_WCO_FOR_TIMING
#define SOURCE_COUNTER                              (0u)  
#define COUNTER_ENABLE                              (1u)    
#define COUNT_PERIOD                                ((uint32)32767/2)
#define WDT_INTERRUPT_SOURCE                        CY_SYS_WDT_COUNTER0_INT    
#endif    

/*******************************************************************************
* Function Name: BLE_AppEventHandler
********************************************************************************
*
* Summary:
*   BLE stack generic event handler routine for handling connection, discovery, 
*   security etc. events.
*
* Parameters:  
*  event - event that triggered this callback
*  eventParam - parameters for the event.
*
* Return: 
*  None
*******************************************************************************/
void BLE_AppEventHandler(uint32 event, void* eventParam)
{
    CYBLE_API_RESULT_T apiResult;
    
    (void)eventParam;

    switch (event)
	{
        /**********************************************************
        *                       General Events
        ***********************************************************/
		case CYBLE_EVT_STACK_ON: /* This event is received when component is Started */
            /* Enter in to discoverable mode so that remote can search it. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            
            if(apiResult != CYBLE_ERROR_OK)
            {
                CYASSERT(0);
            }
        break;
            
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
            {
                /* On advertisement timeout, restart advertisement */
                apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
                if(apiResult != CYBLE_ERROR_OK)
                {
                    CYASSERT(0);
                }
            }
        break;
            
		default:
        break;
	}
}

#if USE_WCO_FOR_TIMING
/*******************************************************************************
* Function Name: WDT_Handler
********************************************************************************
*
* Summary:
*  WDT interrupt handler routine. WDT is only used to wakeup the device after
*  WCO has started  
*
* Parameters:  
*  None
*
* Return: 
*  None
*******************************************************************************/    
CY_ISR(WDT_Handler)
{
    if(CySysWdtGetInterruptSource() & WDT_INTERRUPT_SOURCE)
    {
        CySysWdtClearInterrupt(WDT_INTERRUPT_SOURCE);
    }
}
#endif

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*        MyBeacon entry point. This calls the BLE and other peripheral Component
* APIs for achieving the desired system behaviour
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main()
{
    CyGlobalIntEnable;
    
    /* Set the divider for ECO, ECO will be used as source when IMO is switched off to save power, to drive the HFCLK */
    CySysClkWriteEcoDiv(CY_SYS_CLK_ECO_DIV8);
    
    /* If USE_WCO_FOR_TIMING is set, then do the following:
     * 1. Shut down the ECO (to reduce power consumption while WCO is starting)
     * 2. Enable WDT to wakeup the system after 500ms (WCO startup time). 
     * 3. Configure PSoC 4 BLE device in DeepSleep mode for the 500ms WCO startup time
     * 4. After WCO is enabled, restart the ECO so that BLESS interface can function */
#if USE_WCO_FOR_TIMING
    CySysClkEcoStop();
    
    WDT_Interrupt_StartEx(WDT_Handler);
    
    CySysClkWcoStart();
    
    CySysWdtUnlock(); /* Unlock the WDT registers for modification */
    
    CySysWdtWriteMode(SOURCE_COUNTER, CY_SYS_WDT_MODE_INT);
    
    CySysWdtWriteClearOnMatch(SOURCE_COUNTER, COUNTER_ENABLE);
    
    CySysWdtWriteMatch(SOURCE_COUNTER, COUNT_PERIOD);
    
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
    
    CySysWdtLock();
    
#if TIMING_DEBUG_ENABLE                    
    DeepSleep_Write(1);
#endif  

    CySysPmDeepSleep(); /* Wait for the WDT interrupt to wake up the device */
    
#if TIMING_DEBUG_ENABLE                    
    DeepSleep_Write(0);
#endif

    (void)CySysClkEcoStart(2000u);
    CyDelayUs(500u);

    (void)CySysClkWcoSetPowerMode(CY_SYS_CLK_WCO_LPM);    /* Switch to the low power mode */

    CySysClkSetLfclkSource(CY_SYS_CLK_LFCLK_SRC_WCO);

    CySysWdtUnlock();
    
    CySysWdtDisable(CY_SYS_WDT_COUNTER0_MASK);
    
    CySysWdtLock();
#endif
    
    CyBle_Start(BLE_AppEventHandler);
    
    for(;;)
    {
        CYBLE_LP_MODE_T pwrState;
        CYBLE_BLESS_STATE_T blessState;
        uint8 intStatus = 0;
        
        CyBle_ProcessEvents(); /* BLE stack processing state machine interface */
        
        pwrState  = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP); /* Configure BLESS in Deep-Sleep mode */

        intStatus = CyEnterCriticalSection(); /* No interrupts allowed while entering system low power modes */
        
        blessState = CyBle_GetBleSsState();

        if(pwrState == CYBLE_BLESS_DEEPSLEEP) /* Make sure BLESS is in Deep-Sleep before configuring system in Deep-Sleep */
        {
            if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
            {
#if TIMING_DEBUG_ENABLE                    
                DeepSleep_Write(1);
#endif                

                CySysPmDeepSleep(); /* System Deep-Sleep. 1.3uA mode */
                
#if TIMING_DEBUG_ENABLE                    
                DeepSleep_Write(0);
#endif                 
            }
        }
        else if (blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
        {
             /* Change HF clock source from IMO to ECO, as IMO can be stopped to save power */
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_ECO); 
            
            /* Stop IMO for reducing power consumption */
            CySysClkImoStop(); 
            
#if TIMING_DEBUG_ENABLE            
            Sleep_Write(1);
#endif            
            /* Put the CPU to Sleep. 1.1mA mode */
            CySysPmSleep();
            
#if TIMING_DEBUG_ENABLE            
            Sleep_Write(0);
#endif            
            
            /* Starts execution after waking up, start IMO */
            CySysClkImoStart();
            
            /* Change HF clock source back to IMO */
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_IMO);
        }
        
        CyExitCriticalSection(intStatus);
    }
}

/* [] END OF FILE */
