/*******************************************************************************
* File Name: BLE Connection.c
*
* Version: 1.0
*
* Description: This file contains routines that handle all the BLE interface  
*              connection establishment procedures and event handlers.
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <BLE Interface.h>
#include <Configuration.h>
#include <ConsumerControl.h>
#include <project.h>
#include <stdio.h>

/***************************************
*    Function declarations
***************************************/
void BLE_AppEventHandler(uint32 event, void* eventParam);
void BLE_HidCallBack (uint32 event, void *eventParam);
void BLE_BasCallBack (uint32 event, void *eventParam);

/***************************************
*    Global variables
***************************************/

/*******************************************************************************
* Function Name: BLE_Interface_Start
********************************************************************************
*
* Summary:
*  Initialization the BLE interface.
*
* Parameters:  
*  None
*
* Return: 
*  state returned by the BLE interface start API
*
*******************************************************************************/
CYBLE_API_RESULT_T BLE_Interface_Start(void)
{
    CYBLE_API_RESULT_T apiResult;
    
    CyGlobalIntEnable;
    
    /* Start the BLE component */
    apiResult = CyBle_Start(BLE_AppEventHandler);
    
    if(apiResult == CYBLE_ERROR_OK)
    {
        /* Register HID service callback routine */
        CyBle_HidsRegisterAttrCallback(BLE_HidCallBack);
        CyBle_BasRegisterAttrCallback(BLE_BasCallBack);
    }
    
    return apiResult;
}

/*******************************************************************************
* Function Name: BLE_Run
********************************************************************************
*
* Summary:
*  BLE interface processing engine. This API should be continuously called by 
*  the application in its main loop.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
void BLE_Run(void)
{
    CyBle_ProcessEvents();
}

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
    
    switch (event)
	{
        /**********************************************************
        *                       General Events
        ***********************************************************/
		case CYBLE_EVT_STACK_ON: /* This event is received when component is Started */
            /* Enter in to discoverable mode so that remote can search it. */
            (void) CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);   
            break;

        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        
        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
#if(CONSOLE_LOG_ENABLED)            
            printf("EVT_PASSKEY_DISPLAY_REQUEST %6.6ld \r\n", *(uint32 *)eventParam);
#endif
        break;
           
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            /* Put the device to discoverable mode so that remote can search it. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            
            if(apiResult != CYBLE_ERROR_OK)
            {
                CYASSERT(0);    
            }
            break;    
        /**********************************************************
        *                       GATT Events
        ***********************************************************/
            
        default:            
			break;
	}
}

/*******************************************************************************
* Function Name: BLE_HidCallBack
********************************************************************************
*
* Summary:
*   BLE HID service event handler for handling HID notification enable/disable 
*   and other HID events from the master.
*
* Parameters:  
*  event - event that triggered this callback
*  eventParam - parameters for the event.
*
* Return: 
*  None
*******************************************************************************/
void BLE_HidCallBack (uint32 event, void *eventParam)
{
    switch(event)
    {
        case CYBLE_EVT_HIDSS_NOTIFICATION_ENABLED:
#if CONSOLE_LOG_ENABLED        
            printf("EVT_HIDSS_NOTIFICATION_ENABLE: serv:%x, char: %x\r\n", ((CYBLE_HIDS_CHAR_VALUE_T *)eventParam)->serviceIndex,
                                                                           ((CYBLE_HIDS_CHAR_VALUE_T *)eventParam)->charIndex);
#endif            
            if(((CYBLE_HIDS_CHAR_VALUE_T *)eventParam)->serviceIndex == CONSUMER_SERVICE_INDEX)
            {
                SetConsumerNotification(NOTIFICATION_ENABLED);
            }
            break;
            
        case CYBLE_EVT_HIDSS_NOTIFICATION_DISABLED:
#if CONSOLE_LOG_ENABLED            
            printf("EVT_HIDSS_NOTIFICATION_DISABLE: %x \r\n", ((CYBLE_HIDS_CHAR_VALUE_T *)eventParam)->charIndex);
#endif            
            if(((CYBLE_HIDS_CHAR_VALUE_T *)eventParam)->serviceIndex == CONSUMER_SERVICE_INDEX)
            {
                SetConsumerNotification(NOTIFICATION_DISABLED);
            }
            break;
            
        case CYBLE_EVT_HIDSS_SUSPEND:
#if CONSOLE_LOG_ENABLED            
            printf("EVT_HIDSS_SUSPEND \r\n");
#endif            
            break;
        case CYBLE_EVT_HIDSS_EXIT_SUSPEND:
#if CONSOLE_LOG_ENABLED
            printf("EVT_HIDSS_EXIT_SUSPEND \r\n");
#endif            
            break;

		default:
            printf("OTHER event: %lx \r\n", event);
			break;
    }
}

/*******************************************************************************
* Function Name: BLE_BasCallBack
********************************************************************************
*
* Summary:
*   BLE battery service event handler for handling BAS notification enable/disable 
*   events from the master.
*
* Parameters:  
*  event - event that triggered this callback
*  eventParam - parameters for the event.
*
* Return: 
*  None
*******************************************************************************/
void BLE_BasCallBack (uint32 event, void *eventParam)
{
    switch(event)
    {
        case CYBLE_EVT_BASS_NOTIFICATION_ENABLED:
#if CONSOLE_LOG_ENABLED        
            printf("EVT_BASS_NOTIFICATION_ENABLE \r\n");
#endif            
            break;
                
        case CYBLE_EVT_BASS_NOTIFICATION_DISABLED:
#if CONSOLE_LOG_ENABLED        
            printf("EVT_BASS_NOTIFICATION_DISABLE \r\n");
#endif            
            break;

		default:
#if CONSOLE_LOG_ENABLED 
            printf("OTHER event: %lx \r\n", event);
#endif            
			break;
    }
}

/* [] END OF FILE */
