/*******************************************************************************
* File Name: BLE.c
*
* Description:
*  This file contains BLE related functions and event handlers
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
#include <MIDI.h>
#include <Project.h>

/***************************************
*        Global and Static variables
***************************************/
static BLE_CONNECTION_PARAM_INDEX connectionParamIndex = LOW_LATENCY_CONNECTION_PARAMETER_INDEX;
static BLE_CONNECTION_PARAM_STATE connectionParamState = CONNECTION_PARAMETER_IDLE_STATE;

static CYBLE_GAP_CONN_UPDATE_PARAM_T midiConnectionParam[] =
{
    /* Connection parameter low latency */
    {
        8,        /* Minimum connection interval of 11.25 ms */
        10,        /* Maximum connection interval of 11.25 ms */
        0,          /* Slave latency */
        100        /* Supervision timeout of 5 seconds */
    },
    /* Connection parameter mid latency */
    {
        11,        /* Minimum connection interval of 15 ms */
        13,        /* Maximum connection interval of 15 ms */
        0,          /* Slave latency */
        100        /* Supervision timeout of 1 seconds */
    },
};

/*******************************************************************************
* Function Name: BLE_Start
********************************************************************************
* Summary:
*        Start the BLE interface
*
* Parameters:
*  void
*
* Return:
*  uint8 - status of BLE interface initialization
*
*******************************************************************************/
uint8 BLE_Start(void)
{
    CYBLE_API_RESULT_T apiResult;

    apiResult = CyBle_Start(StackEventHandler);

    if(apiResult != CYBLE_ERROR_OK)
    {
        return 0;
    }
    
    BLE_StateInit();
    
    return 1;
}

/*******************************************************************************
* Function Name: BLE_StateInit
********************************************************************************
* Summary:
*        Start the BLE interface
*
* Parameters:
*  void
*
* Return:
*  uint8 - status of BLE interface initialization
*
*******************************************************************************/
static void BLE_StateInit(void)
{
    /* Start the BLE interface with a low latencey connection parameters */
    connectionParamIndex = LOW_LATENCY_CONNECTION_PARAMETER_INDEX;
    
    /* Only after the CCCD is enabled, send the connnection parameter update request */
    connectionParamState = CONNECTION_PARAMETER_IDLE_STATE;
}

/*******************************************************************************
* Function Name: BLE_Run
********************************************************************************
* Summary:
*        Continuous BLE event processing engine module
*
* Parameters:
*  void
*
* Return:
*  None
*
*******************************************************************************/
void BLE_Run(void)
{
    /* Single API call to service all the BLE stack events. Must be
     * called at least once in a BLE connection interval */
    CyBle_ProcessEvents();
    
    /* Setup BLE interface parameters as per MIDI transport requirements */
    UpdateMidiInterfaceConnParam(connectionParamIndex);

    /* Configure BLESS in DeepSleep mode  */
    CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
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
            
            /* Re-initialize the BLE interface */
            BLE_StateInit();
          break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            /* BLE link is established */
            Advertising_LED_Write(LED_OFF);
        break;

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
            
        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP:
            if(*(uint16 *)eventParam == CONNECTION_PARAMETER_ACCEPTED)
            {
                /* Previously sent connection parameter is accepted by the iOS/Mac device */
                connectionParamState = CONNECTION_PARAMETER_ACCEPTED_STATE;
            }
            else if (*(uint16 *)eventParam == CONNECTION_PARAMETER_REJECTED)
            {
                connectionParamIndex++;
                if(connectionParamIndex != TOTAL_NUMBER_OF_CONNECTION_PARAMETERS)
                {
                    connectionParamState = CONNECTION_PARAMETER_UPDATE_REQUIRED_STATE; 
                }
                else
                {
                    connectionParamState = CONNECTION_PARAMETER_REJECTED_STATE;
                }
            }
        break;
            
        case CYBLE_EVT_GATTS_WRITE_REQ:
            if(connectionParamState == CONNECTION_PARAMETER_IDLE_STATE)
            {
                connectionParamState = CONNECTION_PARAMETER_UPDATE_REQUIRED_STATE;    
            }
            
            MIDIEventHandler(event, eventParam);
        break;
            
        case CYBLE_EVT_GATTS_WRITE_CMD_REQ:    
            MIDIEventHandler(event, eventParam);
        break;

        default:
        break;
    }
}

/*******************************************************************************
* Function Name: UpdateMidiInterfaceConnParam
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
static void UpdateMidiInterfaceConnParam(BLE_CONNECTION_PARAM_INDEX connectionParamIndex)
{
    if(connectionParamState == CONNECTION_PARAMETER_UPDATE_REQUIRED_STATE && CyBle_GetState() == CYBLE_STATE_CONNECTED)
    {
        CYBLE_API_RESULT_T conResult;

        conResult = CyBle_L2capLeConnectionParamUpdateRequest(cyBle_connHandle.bdHandle, 
                                                            &midiConnectionParam[connectionParamIndex]);
        
        if(conResult != CYBLE_ERROR_OK)
        {
            connectionParamState = CONNECTION_PARAMETER_SENT_STATE;
        }
        else
        {
            connectionParamState = CONNECTION_PARAMETER_FAILURE_STATE;
        }
    }
}

/* [] END OF FILE */
