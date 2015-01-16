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
*  OLEG@CYPRESS.COM, KRIS@CYPRESS.COM
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
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <Project.h>


/***************************************
*        API Constants
***************************************/
#define NO_ALERT           (0u)
#define MILD_ALERT         (1u)
#define HIGH_ALERT         (2u)

#define LED_NO_ALERT       (0u)
#define LED_MILD_ALERT     (100u)
#define LED_HIGH_ALERT     (200u)

#define LED_ON             (0u)
#define LED_OFF            (1u)

/* KRIS_UPDATE - Extern and constants required for dynamic ADV payload update */
#define ENABLE_DYNAMIC_ADV          (1u)

#if ENABLE_DYNAMIC_ADV
extern CYBLE_GAPP_DISC_MODE_INFO_T cyBle_discoveryModeInfo;
#define BATTERY_SERVICE_DATA_INDEX  (11u)
#define LOOP_DELAY                  (1u)
#define advPayload                  (cyBle_discoveryModeInfo.advData->advData)

#endif

/***************************************
*        Function Prototypes
***************************************/
void StackEventHandler(uint32 event, void* eventParam);
void IasEventHandler(uint32 event, void* eventParam);

/***************************************
*        Global variables
***************************************/

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
    uint16 count = 0;
    uint8 batteryLevel = 100;

    CyGlobalIntEnable;

    PWM_Start();

    apiResult = CyBle_Start(StackEventHandler);

    if(apiResult != CYBLE_ERROR_OK)
    {
        /* BLE stack initialization failed, check your configuration */
        CYASSERT(0);
    }

    CyBle_IasRegisterAttrCallback(IasEventHandler);

    for(;;)
    {
        CYBLE_STATE_T bleState;

        /* Single API call to service all the BLE stack events. Must be
         * called at least once in a BLE connection interval */
        CyBle_ProcessEvents();

        bleState = CyBle_GetState();

        if(bleState != CYBLE_STATE_STOPPED &&
            bleState != CYBLE_STATE_INITIALIZING)
        {
            /* Configure BLESS in DeepSleep mode  */
            CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);

            /* Configure PSoC 4 BLE system in sleep mode */
            CySysPmSleep();

            /* BLE link layer timing interrupt will wake up the system */
        }
        
        /* KRIS_UPDATE - Dynamic advertisement payload update code */
        /*-----------------------------------------------------------------*/
#if ENABLE_DYNAMIC_ADV        
        count++; /* Loop counter */
        
        /* Make sure the BLE sub-system is not in connected state. Once the system enters sleep mode during
         * advertisement, the source of wake-up is the advertisement timeout timer which has a wakeup interval
         * of 1 advertisement (ADV) interval (25ms). LOOP_DELAY * ADV interval is the interval after which ADV 
         * data is updated in this firmware.*/
        if(bleState != CYBLE_STATE_CONNECTED && 
            count >= LOOP_DELAY && 
            CyBle_GetBleSsState() == CYBLE_BLESS_STATE_EVENT_CLOSE)
        {
            advPayload[BATTERY_SERVICE_DATA_INDEX] = batteryLevel--;
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            /* Handle erros condition for start advertisement here */
            
            count = 0;
            
            if(batteryLevel == 0)
            {
                batteryLevel = 100;
            }
            
            CyBle_GapUpdateAdvData(cyBle_discoveryModeInfo.advData, cyBle_discoveryModeInfo.scanRspData);
        }
#endif        
        /*-----------------------------------------------------------------*/
    }
}


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
            Advertising_LED_Write(LED_ON);
            PWM_WriteCompare(LED_NO_ALERT);
          break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            /* BLE link is established */
            Advertising_LED_Write(LED_OFF);
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

        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
            {
                /* Advertisement event timed out, go to low power
                 * mode (Hibernate mode) and wait for an external
                 * user event to wake up the device again */
                Advertising_LED_Write(LED_OFF);
                Hibernate_LED_Write(LED_ON);
                PWM_Stop();
                Wakeup_SW_ClearInterrupt();
                Wakeup_Interrupt_ClearPending();
                Wakeup_Interrupt_Start();
                CySysPmHibernate();
            }
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
* Function Name: IasEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component,
*  which are specific to Immediate Alert Service.
*
* Parameters:
*  uint8 event:       Write Command event from the CYBLE component.
*  void* eventParams: A structure instance of CYBLE_GATT_HANDLE_VALUE_PAIR_T
*                     type.
*
* Return:
*  None
*
*******************************************************************************/
void IasEventHandler(uint32 event, void *eventParam)
{
    /* Alert Level Characteristic write event */
    if(event == CYBLE_EVT_IASS_WRITE_CHAR_CMD)
    {
        uint8 alertLevel;

        /* Data structure that is returned as eventParam */
        CYBLE_IAS_CHAR_VALUE_T *charValue = (CYBLE_IAS_CHAR_VALUE_T *)eventParam;

        /* Extract Alert Level value from the data structure */
        alertLevel = *((charValue->value->val));

        switch(alertLevel)
        {
            case NO_ALERT:
            PWM_WriteCompare(LED_NO_ALERT);
            break;

            case MILD_ALERT:
            PWM_WriteCompare(LED_MILD_ALERT);
            break;

            case HIGH_ALERT:
            PWM_WriteCompare(LED_HIGH_ALERT);
            break;
        }
    }
}


/* [] END OF FILE */
