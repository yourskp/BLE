/*****************************************************************************
* File Name: BleApplications.c
*
* Version: 1.0
*
* Description:
*   This file implements the BLE capability in the PSoC 4 Level Sensor.
*
* Hardware Dependency:
*   Chemical Tank Rev A PCB
*
******************************************************************************
* Copyright (2016), Intermountain Water Softeners
******************************************************************************
* This software is owned by Intermountain Water Softeners (Intermountain) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of (Intermountain).
*
*****************************************************************************/

/*****************************************************************************
* Included headers
*****************************************************************************/
#include <main.h>
#include <BLEApplications.h>


/*****************************************************************************
* Static variables 
*****************************************************************************/

/*This flag is set when the Central device writes to CCC (Client Characteristic 
* Configuration) of the CapSense slider Characteristic to enable notifications */
uint8 sendCapSenseSliderNotifications = FALSE;	

#if KRIS_CHANGES
uint8 sendTemperatureNotifications  = FALSE;
uint8 sendBatteryNotifications  = FALSE;
#endif

/* This flag is used by application to know whether a Central 
* device has been connected. This is updated in BLE event callback 
* function*/
uint8 deviceConnected = FALSE;


/*******************************************************************************
* Function Name: CustomEventHandler
********************************************************************************
* Summary:
* Call back event function to handle various events from BLE stack
*
* Parameters:
*  event:		event returned
*  eventParam:	link to value of the events returned
*
* Return:
*  void
*
*******************************************************************************/
void CustomEventHandler(uint32 event, void * eventParam)
{
    /* Local variable to strore the write request parameters */
	CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    
    /* Local variable to store the current CCCD value */
	uint8 CapSenseCCCDvalue[2];
	
	/* Handle value to update the CCCD */
	CYBLE_GATT_HANDLE_VALUE_PAIR_T CapSenseNotificationCCCDhandle;
   
    switch(event)
    {
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
			/* Start Advertisement and enter Discoverable mode*/
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
			break;
			
		case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
			/* Set the BLE state variable to control LED status */
            if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
            {
                /* Start Advertisement and enter Discoverable mode*/
                CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            }
			break;
			
        case CYBLE_EVT_GATT_CONNECT_IND:
			/* This flag is used in application to check connection status */
			deviceConnected = TRUE;
			break;
        
        case CYBLE_EVT_GATT_DISCONNECT_IND:
			/* Update deviceConnected flag*/
			deviceConnected = FALSE;
			
			/* Reset CapSense notification flag to prevent further notifications
			 * being sent to Central device after next connection. */
			sendCapSenseSliderNotifications = FALSE;
			
			break;
        
            
        case CYBLE_EVT_GATTS_WRITE_REQ: 							
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            
            /* if (attributeHandle == CapSense Slider Characteristics Handle) */
            if(wrReqParam->handleValPair.attrHandle == cyBle_customs[CAPSENSE_SERVICE_INDEX].
			            customServiceInfo[CAPSENSE_SLIDER_CHAR_INDEX].
                        customServiceCharDescriptors[CAPSENSE_SLIDER_CCC_INDEX])
            {
                sendCapSenseSliderNotifications = wrReqParam->handleValPair.value.val[CCC_DATA_INDEX];
				
                /* When the Client Characteristic Configuration descriptor (CCCD) is
                * written by the Central device for enabling/disabling notifications, 
                * then the same descriptor value has to be explicitly updated in 
                * application so that it reflects the correct value when the 
                * descriptor is read. 
                */
                {
                    /* Write the present CapSense notification status to the local variable */
            		CapSenseCCCDvalue[0] = sendCapSenseSliderNotifications;
            		CapSenseCCCDvalue[1] = 0x00;
            		
            		/* Update CCCD handle with notification status data*/
            		CapSenseNotificationCCCDhandle.attrHandle = CAPSENSE_CCC_HANDLE;
            		CapSenseNotificationCCCDhandle.value.val = CapSenseCCCDvalue;
            		CapSenseNotificationCCCDhandle.value.len = sizeof(CapSenseCCCDvalue);
            		
            		/* Report data to BLE component for sending data when read by Central device */
            		CyBle_GattsWriteAttributeValue(&CapSenseNotificationCCCDhandle, ZERO, &cyBle_connHandle, \
                                                                                CYBLE_GATT_DB_LOCALLY_INITIATED);
                }
           }
            
#if KRIS_CHANGES            
            /* Check for notification enable write command for battery and temperature Characteristics */
            else if (wrReqParam->handleValPair.attrHandle == 
                CYBLE_TEMPERATURE_TEMPERATURE_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
            {
                sendTemperatureNotifications = wrReqParam->handleValPair.value.val[CCC_DATA_INDEX];
				
                /* When the Client Characteristic Configuration descriptor (CCCD) is
                * written by the Central device for enabling/disabling notifications, 
                * then the same descriptor value has to be explicitly updated in 
                * application so that it reflects the correct value when the 
                * descriptor is read. 
                */
                {
                    /* Write the present CapSense notification status to the local variable */
            		CapSenseCCCDvalue[0] = sendTemperatureNotifications;
            		CapSenseCCCDvalue[1] = 0x00;
            		
            		/* Update CCCD handle with notification status data*/
            		CapSenseNotificationCCCDhandle.attrHandle = CYBLE_TEMPERATURE_TEMPERATURE_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
            		CapSenseNotificationCCCDhandle.value.val = CapSenseCCCDvalue;
            		CapSenseNotificationCCCDhandle.value.len = sizeof(CapSenseCCCDvalue);
            		
            		/* Report data to BLE component for sending data when read by Central device */
            		CyBle_GattsWriteAttributeValue(&CapSenseNotificationCCCDhandle, ZERO, &cyBle_connHandle, \
                                                                                CYBLE_GATT_DB_LOCALLY_INITIATED);
                }
            }
            else if (wrReqParam->handleValPair.attrHandle == 
                CYBLE_BATTERY_VOLTAGE_BATTERY_VOLTAGE_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
            {
                sendBatteryNotifications = wrReqParam->handleValPair.value.val[CCC_DATA_INDEX];
				
                /* When the Client Characteristic Configuration descriptor (CCCD) is
                * written by the Central device for enabling/disabling notifications, 
                * then the same descriptor value has to be explicitly updated in 
                * application so that it reflects the correct value when the 
                * descriptor is read. 
                */
                {
                    /* Write the present CapSense notification status to the local variable */
            		CapSenseCCCDvalue[0] = sendBatteryNotifications;
            		CapSenseCCCDvalue[1] = 0x00;
            		
            		/* Update CCCD handle with notification status data*/
            		CapSenseNotificationCCCDhandle.attrHandle = CYBLE_BATTERY_VOLTAGE_BATTERY_VOLTAGE_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
            		CapSenseNotificationCCCDhandle.value.val = CapSenseCCCDvalue;
            		CapSenseNotificationCCCDhandle.value.len = sizeof(CapSenseCCCDvalue);
            		
            		/* Report data to BLE component for sending data when read by Central device */
            		CyBle_GattsWriteAttributeValue(&CapSenseNotificationCCCDhandle, ZERO, &cyBle_connHandle, \
                                                                                CYBLE_GATT_DB_LOCALLY_INITIATED);
                }
           }
			
#endif    
			/* Send the response to the write request received. */
			CyBle_GattsWriteRsp(cyBle_connHandle);
			
			break;

        default:

       	 	break;
    }
}


/*******************************************************************************
* Function Name: SendCapSenseNotification
********************************************************************************
* Summary:
* Send CapSense Slider data as BLE Notifications. This function updates
* the notification handle with data and triggers the BLE component to send 
* notification
*
* Parameters:
*  CapSenseSliderData:	CapSense slider value	
*
* Return:
*  void
*
*******************************************************************************/
void SendCapSenseNotification(uint8 CapSenseSliderData)
{
	/* 'CapSensenotificationHandle' stores CapSense notification data parameters */
	CYBLE_GATTS_HANDLE_VALUE_NTF_T		CapSensenotificationHandle;	
	
	/* Update notification handle with CapSense slider data*/
	CapSensenotificationHandle.attrHandle = CYBLE_CAPSENSE_SERVICE_CAPSENSE_SLIDER_CHARACTERISTIC_CHAR_HANDLE;				
	CapSensenotificationHandle.value.val = &CapSenseSliderData;
	CapSensenotificationHandle.value.len = sizeof(CapSenseSliderData);
	
	/* Send notifications. */
	CyBle_GattsNotification(cyBle_connHandle, &CapSensenotificationHandle);
}

#if KRIS_CHANGES            
void SendTemperatureNotification(uint8 temperatureData)
{
	/* 'temperatureNotificationHandle' stores temperature notification data parameters */
	CYBLE_GATTS_HANDLE_VALUE_NTF_T		temperatureNotificationHandle;	
	
	/* Update notification handle with temperature data*/
	temperatureNotificationHandle.attrHandle = CYBLE_TEMPERATURE_TEMPERATURE_CHARACTERISTIC_CHAR_HANDLE;				
	temperatureNotificationHandle.value.val = &temperatureData;
	temperatureNotificationHandle.value.len = sizeof(temperatureData);
	
	/* Send notifications. */
	CyBle_GattsNotification(cyBle_connHandle, &temperatureNotificationHandle);
}

void SendBatteryNotification(uint16 batteryData)
{
	/* 'batteryNotificationHandle' stores battery notification data parameters */
	CYBLE_GATTS_HANDLE_VALUE_NTF_T		batteryNotificationHandle;	
	
	/* Update notification handle with battery data*/
	batteryNotificationHandle.attrHandle = CYBLE_BATTERY_VOLTAGE_BATTERY_VOLTAGE_CHARACTERISTIC_CHAR_HANDLE;				
	batteryNotificationHandle.value.val = (uint8 *)&batteryData;
	batteryNotificationHandle.value.len = sizeof(batteryData);
	
	/* Send notifications. */
	CyBle_GattsNotification(cyBle_connHandle, &batteryNotificationHandle);
}

#endif    

/* [] END OF FILE */
