/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This is source code for the PSoC 4 BLE Find Me Profile Target example project.
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
*  1. PSoC Creator 3.1
*  2. ARM CM3-RVDS
*  3. ARM CM3-GCC
*  4. ARM CM3-MDK
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include "ias.h"
#include "common.h"

#define REUSE_BLE_COMPONENT_SRAM            (1u)
#define API_TIME_MEASURE                    (1u)

#if REUSE_BLE_COMPONENT_SRAM
    
/* Custom linker script provides starting and end locations of the .bss area used by BLE stack. 
 * See cmogcc.ld file in Workspace explorer (under Source Files folder) for details */
extern void __custom_data_start(void);
extern void __custom_data_end(void);

#define DATA_POINTER_START                  (&__custom_data_start)    
#define DATA_POINTER_END                    (&__custom_data_end)
#define MEMORY_SIZE                         (DATA_POINTER_END - DATA_POINTER_START)
    
/* Allocate RAM memory for stack. This memory can be used by the application before initializing the stack */
CYBLE_CYALIGNED static uint8 cyBle_stackMemoryRam[CYBLE_STACK_RAM_SIZE];
uint8 *dataPointer;


/*******************************************************************************
* Function Name: CyBle_Start_Custom
********************************************************************************
*
* Summary:
*  Our custom BLE_Start routine that uses the memory allocated by the application
*  layer for its internal heap
*
* Parameters:
* callbackFunc - Application callback function that the stack should call to notify
* pending events or data.
*
* Return:
*  Status of the BLE stack initialization
*
*******************************************************************************/
CYBLE_API_RESULT_T CyBle_Start_Custom(CYBLE_CALLBACK_T callbackFunc)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    
    /* If not initialized then initialize all the required hardware and software */
    if(cyBle_initVar == 0u)
    {
        CyBle_Init();
        cyBle_initVar = 1u;
    }
    
    if(NULL != callbackFunc)
    {
        CyBle_ApplCallback = callbackFunc;

        apiResult = CyBle_StackInit(&CyBle_EventHandler, cyBle_stackMemoryRam, CYBLE_GATT_MTU);

        if(apiResult == CYBLE_ERROR_OK)
        {
            CyBle_SetState(CYBLE_STATE_INITIALIZING);
            
        #if (CYBLE_GATT_ROLE_CLIENT)
            CyBle_SetClientState(CYBLE_CLIENT_STATE_DISCONNECTED);
        #endif /* CYBLE_GATT_ROLE_CLIENT */
            
        #if(CYBLE_GATT_ROLE_SERVER)
            apiResult = CyBle_GattsDbRegister(cyBle_gattDB, CYBLE_GATT_DB_INDEX_COUNT, CYBLE_GATT_DB_MAX_VALUE_LEN);
        #endif /* CYBLE_GATT_ROLE_SERVER */
        }
    }

    return (apiResult);
}    
    
#endif    


/*******************************************************************************
* Function Name: StackEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  uint8 event:       Event from the CYBLE component
*  void* eventParams: A structure instance for corresponding event type. The
*                     list of event structure is described in the component
*                     datasheet.
*
* Return:
*  None
*
*******************************************************************************/
void StackEventHandler(uint32 event, void *eventParam)
{
    switch(event)
    {
        /* Mandatory events to be handled by Find Me Target design */
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            /* Start BLE advertisement for 30 seconds and update link
             * status on LEDs */
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
#if API_TIME_MEASURE
            Debug_Write(0);
#endif    
            Advertising_LED_Write(LED_ON);
            alertLevel = NO_ALERT;
        break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            /* BLE link is established */
            Advertising_LED_Write(LED_OFF);
            Disconnect_LED_Write(LED_OFF);
        break;

        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
            {
                /* Advertisement event timed out, go to low power
                 * mode (Stop mode) and wait for device reset
                 * event to wake up the device again */
                Advertising_LED_Write(LED_OFF);
                Disconnect_LED_Write(LED_ON);
                CySysPmSetWakeupPolarity(CY_PM_STOP_WAKEUP_ACTIVE_HIGH);
                CySysPmStop();
               
                /* Code execution will not reach here */
            }
        break;

        /* Other events that are generated by the BLE Component that
         * are not required for functioning of this design */
        /**********************************************************
        *                       General Events
        ***********************************************************/
        case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
            break;

        case CYBLE_EVT_HCI_STATUS:
            break;

        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        case CYBLE_EVT_GAP_AUTH_REQ:
            break;

        case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            break;

        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            break;

        case CYBLE_EVT_GAP_AUTH_COMPLETE:

            break;
        case CYBLE_EVT_GAP_AUTH_FAILED:

            break;

        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
            break;

        case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
            break;

        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
            break;

        case CYBLE_EVT_GATT_DISCONNECT_IND:
            break;

        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
        break;

        default:
        break;
    }
}


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
    CYBLE_API_RESULT_T apiResult;
    
#if REUSE_BLE_COMPONENT_SRAM
    uint32 index;
    
    dataPointer = (uint8 *)DATA_POINTER_START; /* Pointer to another 4K of uninitialized memory that the BLE stack uses */
    
    for(index = 0; index < sizeof(cyBle_stackMemoryRam); index++)
    {
        /* Using stack memory for some other user operation */
        cyBle_stackMemoryRam[index] = index%256;
    }
    
    /* use the BLE stack .bss area memory for application usage before calling the BLE_Start routine */
    for(index = 0; index < MEMORY_SIZE; index++)
    {
        dataPointer[index] = index%256;
    }
    
    /* Reset the memory before calling BLE Start API */
    memset(&cyBle_stackMemoryRam[0], 0, sizeof(cyBle_stackMemoryRam));
    memset(dataPointer, 0, MEMORY_SIZE);
        
    CyGlobalIntEnable;

#if API_TIME_MEASURE
    Debug_Write(1);
#endif    
    apiResult = CyBle_Start_Custom(StackEventHandler);
#if API_TIME_MEASURE
    CyBle_ProcessEvents();
#endif     
#else
    CyGlobalIntEnable;                
    apiResult = CyBle_Start(StackEventHandler);
#endif

    if(apiResult != CYBLE_ERROR_OK)
    {
        /* BLE stack initialization failed, check your configuration */
        CYASSERT(0);
    }

    CyBle_IasRegisterAttrCallback(IasEventHandler);

    for(;;)
    {
        static uint8 toggleTimeout = 0;
        CYBLE_BLESS_STATE_T blessState;
        uint8 intrStatus;

        /* Single API call to service all the BLE stack events. Must be
         * called at least once in a BLE connection interval */
        CyBle_ProcessEvents();
        
        /* Update Alert Level value on the blue LED */
        switch(alertLevel)
        {
            case NO_ALERT:
            Alert_LED_Write(LED_OFF);
            break;

            case MILD_ALERT:
            toggleTimeout++;
            if(toggleTimeout == LED_TOGGLE_TIMEOUT)
            {
                /* Toggle alert LED after timeout */
                Alert_LED_Write(Alert_LED_Read() ^ 0x01);
                toggleTimeout = 0;
            }
            break;

            case HIGH_ALERT:
            Alert_LED_Write(LED_ON);
            break;
        }

        /* Configure BLESS in Deep-Sleep mode */
        CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
        
        /* Prevent interrupts while entering system low power modes */
        intrStatus = CyEnterCriticalSection();
        
        /* Get the current state of BLESS block */
        blessState = CyBle_GetBleSsState();
        
        /* If BLESS is in Deep-Sleep mode or the XTAL oscillator is turning on, 
         * then PSoC 4 BLE can enter Deep-Sleep mode (1.3uA current consumption) */
        if(blessState == CYBLE_BLESS_STATE_ECO_ON || 
            blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
        {
            CySysPmDeepSleep();
        }
        else if(blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
        {
            /* If BLESS is active, then configure PSoC 4 BLE system in 
             * Sleep mode (~1.6mA current consumption) */
            CySysPmSleep();
        }
        else
        {
            /* Keep trying to enter either Sleep or Deep-Sleep mode */    
        }
        CyExitCriticalSection(intrStatus);
        
        /* BLE link layer timing interrupt will wake up the system from Sleep 
         * and Deep-Sleep modes */
    }
}

/* [] END OF FILE */
