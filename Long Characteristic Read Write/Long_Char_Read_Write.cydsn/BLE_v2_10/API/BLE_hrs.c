/*******************************************************************************
File Name: CYBLE_hrs.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the source code for the Heart Rate Service of 
 the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_HrsApplCallback;

#ifdef CYBLE_HRS_SERVER

`$HrsServer`

#endif /* CYBLE_HRS_SERVER */

#ifdef CYBLE_HRS_CLIENT
    
/* Server's Heart Rate Service characteristics GATT DB handles structure */
CYBLE_HRSC_T cyBle_hrsc;

/* Internal storage for last request handle to check the response */
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_hrscReqHandle;
#endif /* CYBLE_HRS_CLIENT */


/****************************************************************************** 
##Function Name: CyBle_HrsInit
*******************************************************************************

Summary:
 Initializes the profile internals.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_HrsInit(void)
{
#ifdef CYBLE_HRS_CLIENT
    (void)memset(&cyBle_hrsc, 0, sizeof(cyBle_hrsc));
    cyBle_hrscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
#endif /* CYBLE_GATT_ROLE_CLIENT */
}


/****************************************************************************** 
##Function Name: CyBle_HrsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc: An application layer event callback function to receive 
                   events from the BLE Component. The definition of 
                   CYBLE_CALLBACK_T for HRS Service is,
            
                   typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                      void *eventParam)

                   * eventCode indicates the event that triggered this 
                     callback (e.g. CYBLE_EVT_HRSS_NOTIFICATION_ENABLED).
                   * eventParam contains the parameters corresponding to the 
                     current event. (e.g. pointer to CYBLE_HRS_CHAR_VALUE_T
                     structure that contains details of the characteristic 
                     for which notification enabled event was triggered).

Return:
 None

******************************************************************************/
void CyBle_HrsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_HrsApplCallback = callbackFunc;
}


#ifdef CYBLE_HRS_SERVER


/****************************************************************************** 
##Function Name: CyBle_HrssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets local characteristic value of the specified Heart Rate Service 
 characteristic.

Parameters:
 charIndex: The index of a service characteristic.
 attrSize: The size of the characteristic value attribute. The Heart Rate
            Measurement characteristic has a 20 byte length (by default).
            The Body Sensor Location and Control Point characteristic
            both have 1 byte length.
 *attrValue: The pointer to the characteristic value data that should be
                  stored in the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent

******************************************************************************/
CYBLE_API_RESULT_T CyBle_HrssSetCharacteristicValue(CYBLE_HRS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex >= CYBLE_HRS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if((charIndex == CYBLE_HRS_BSL) && (!CYBLE_HRS_IS_BSL_SUPPORTED))
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        /* Store characteristic value into GATT database */
        locHandleValuePair.attrHandle = cyBle_hrss.charHandle[charIndex];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        
        if(CYBLE_GATT_ERR_NONE != CyBle_GattsWriteAttributeValue(&locHandleValuePair,
                                                0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
    }

    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_HrssGetCharacteristicValue
*******************************************************************************

Summary:
 Gets the local characteristic value of specified Heart Rate Service 
 characteristic.

Parameters:
 charIndex:  The index of a service characteristic.
 attrSize:   The size of the characteristic value attribute. The Heart Rate
             Measurement characteristic has a 20 byte length (by default).
             The Body Sensor Location and Control Point characteristic
             both have 1 byte length.
 *attrValue: The pointer to the location where characteristic value data 
             should be stored. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent

******************************************************************************/
CYBLE_API_RESULT_T CyBle_HrssGetCharacteristicValue(CYBLE_HRS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex >= CYBLE_HRS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if((charIndex == CYBLE_HRS_BSL) && (!CYBLE_HRS_IS_BSL_SUPPORTED))
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        /* Get characteristic value from GATT database */
        locHandleValuePair.attrHandle = cyBle_hrss.charHandle[charIndex];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        
        if(CYBLE_GATT_ERR_NONE != CyBle_GattsReadAttributeValue(&locHandleValuePair, 0u,
                                    CYBLE_GATT_DB_READ | CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }  
    }

    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_HrssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets the local characteristic descriptor of the specified Heart Rate 
 Service characteristic.

Parameters:
 charIndex:  The index of the characteristic.
 descrIndex: The index of the descriptor.
 attrSize:   The size of the descriptor value attribute. The Heart Rate
             Measurement characteristic client configuration descriptor has
             2 bytes length.
 *attrValue: The pointer to the location where characteristic descriptor 
             value data should be stored. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional descriptor is absent

******************************************************************************/
CYBLE_API_RESULT_T CyBle_HrssGetCharacteristicDescriptor(CYBLE_HRS_CHAR_INDEX_T charIndex, 
                                                         CYBLE_HRS_DESCR_INDEX_T descrIndex,
                                                         uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((charIndex >= CYBLE_HRS_CHAR_COUNT) || (descrIndex >= CYBLE_HRS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if((charIndex != CYBLE_HRS_HRM) || (descrIndex != CYBLE_HRS_HRM_CCCD))
	{
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
	else
	{
		/* Get data from database */
		locHandleValuePair.attrHandle = cyBle_hrss.hrmCccdHandle;
		locHandleValuePair.value.len = attrSize;
		locHandleValuePair.value.val = attrValue;

		if(CYBLE_GATT_ERR_NONE != CyBle_GattsReadAttributeValue(&locHandleValuePair, 0u,
						CYBLE_GATT_DB_READ | CYBLE_GATT_DB_LOCALLY_INITIATED))
		{
			apiResult = CYBLE_ERROR_INVALID_PARAMETER;
		}
		else
		{
			apiResult = CYBLE_ERROR_OK;
		}
	}
    

    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_HrssSendNotification
*******************************************************************************

Summary:
 Sends notification of a specified Heart Rate Service characteristic value 
 to the Client device. No response is expected.
 
 The CYBLE_EVT_HRSC_NOTIFICATION event is received by the peer device, on 
 invoking this function.

Parameters:
 connHandle: The connection handle which consist of the device ID and ATT
             connection ID.
 charIndex:  The index of a service characteristic.
 attrSize:   The size of the characteristic value attribute. The Heart Rate
             Measurement characteristic has a 20 byte length (by default).
             The Body Sensor Location and Control Point characteristic
             both have 1 byte length.
 *attrValue: The pointer to the characteristic value data that should be 
             sent to the client device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed. 
  * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the client.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_HrssSendNotification(CYBLE_CONN_HANDLE_T connHandle, CYBLE_HRS_CHAR_INDEX_T charIndex,
                                              uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;
    
    /* Send Notification if it is enabled and connected */
    if(CYBLE_STATE_CONNECTED != CyBle_GetState())
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_HRS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if((cyBle_hrss.hrmCccdHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
                || (!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_hrss.hrmCccdHandle)))
    {
        apiResult = CYBLE_ERROR_NTF_DISABLED;
    }
    else
    {
        /* Fill all fields of write request structure ... */
        ntfReqParam.attrHandle = cyBle_hrss.charHandle[charIndex]; 
        ntfReqParam.value.val = attrValue;
        ntfReqParam.value.len = attrSize;
        
        /* Send notification to client using previously filled structure */
        apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
    }

    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_HrssWriteEventHandler
*******************************************************************************

Summary:
 Handles the Heart Rate Measurement Client Configuration Characteristic
 Descriptor Write Event or Control Point Characteristic Write Event.

Parameters:
 void *eventParam: The pointer to the data structure specified by the event.

Return:
 None

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_HrssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    uint32 eventCode = 0u;
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    CYBLE_HRS_CHAR_VALUE_T locCharIndex;

    locCharIndex.connHandle = eventParam->connHandle;
    locCharIndex.value = NULL;
    
    if(eventParam->handleValPair.attrHandle == cyBle_hrss.hrmCccdHandle)
    {
        locCharIndex.charIndex = CYBLE_HRS_HRM;
        /* Heart Rate Measurement characteristic descriptor write request */
        if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
        {
            eventCode = (uint32)CYBLE_EVT_HRSS_NOTIFICATION_ENABLED;
        }
        else
        {
            eventCode = (uint32)CYBLE_EVT_HRSS_NOTIFICATION_DISABLED;
        }
    #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
        /* Set flag to store bonding data to flash */
        cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
    #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
        
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
    else if(eventParam->handleValPair.attrHandle == cyBle_hrss.charHandle[CYBLE_HRS_CPT])
    {
        locCharIndex.charIndex = CYBLE_HRS_CPT;
        /* Heart Rate Control Point characteristic write request */
        if(CYBLE_HRS_RESET_ENERGY_EXPENDED == eventParam->handleValPair.value.val[0u])
        {
            eventCode = (uint32)CYBLE_EVT_HRSS_ENERGY_EXPENDED_RESET;
        }
        else
        {
            gattErr = CYBLE_GATT_ERR_HEART_RATE_CONTROL_POINT_NOT_SUPPORTED;
        }
        
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
    else
    {
        /* Heart Rate Service doesn't support any other write requests */
    }

    if(0u != eventCode)
    {
        gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                    &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
        
        if(CYBLE_GATT_ERR_NONE == gattErr)
        {
            if(NULL != CyBle_HrsApplCallback)
            {
                CyBle_HrsApplCallback(eventCode, &locCharIndex);
            }
            else
            {
                CyBle_ApplCallback(CYBLE_EVT_GATTS_WRITE_REQ, eventParam);
            }
        }
    }

    return (gattErr);
}

#endif /* CYBLE_HRS_SERVER */

#ifdef CYBLE_HRS_CLIENT


/****************************************************************************** 
##Function Name: CyBle_HrscSetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to write the characteristic value attribute 
 (identified by charIndex) to the server. The Write Response just confirms 
 the operation success.
 
 This function call can result in generation of the following events based on
 the response from the server device:
 * CYBLE_EVT_HRSC_WRITE_CHAR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

Parameters:
 connHandle: The connection handle.
 charIndex: The index of a service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be
              sent to the server device. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                    characteristic

******************************************************************************/
CYBLE_API_RESULT_T CyBle_HrscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_HRS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_HRS_CHAR_COUNT) || ((charIndex == CYBLE_HRS_CPT) && (attrSize > CYBLE_HRS_CPT_CHAR_LEN)))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_hrsc.charInfo[charIndex].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_WRITE & cyBle_hrsc.charInfo[charIndex].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
        writeReqParam.attrHandle = cyBle_hrsc.charInfo[charIndex].valueHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;
        apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &writeReqParam);

        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_hrscReqHandle = cyBle_hrsc.charInfo[charIndex].valueHandle;
        }
    }

    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_HrscGetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to read the characteristic Value from a server
 which is identified by charIndex.

 The Read Response returns the characteristic Value in the Attribute Value
 parameter.

 The Read Response only contains the characteristic Value that is less than or
 equal to (MTU - 1) octets in length. If the characteristic Value is greater
 than (MTU - 1) octets in length, the Read Long Characteristic Value procedure
 may be used if the rest of the characteristic Value is required.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The read request was sent successfully  
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                    characteristic

******************************************************************************/
CYBLE_API_RESULT_T CyBle_HrscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_HRS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_HRS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_hrsc.charInfo[charIndex].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_READ & cyBle_hrsc.charInfo[charIndex].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_hrsc.charInfo[charIndex].valueHandle);
        
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_hrscReqHandle = cyBle_hrsc.charInfo[charIndex].valueHandle;
        }
    }

    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_HrscSetCharacteristicDescriptor
*******************************************************************************

Summary:
 This function is used to write the characteristic Value to the server, which
 is identified by charIndex.

 This function call can result in generation of the following events based on
 the response from the server device:
 * CYBLE_EVT_HRSC_WRITE_DESCR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

 One of the following events is received by the peer device, on invoking 
 this function:
 * CYBLE_EVT_HRSS_NOTIFICATION_ENABLED
 * CYBLE_EVT_HRSS_NOTIFICATION_DISABLED

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.
 descrIndex: The index of the service characteristic descriptor.
 attrSize: The size of the characteristic descriptor value attribute.
 *attrValue: The pointer to the characteristic descriptor value data that
              should be sent to the server device. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_INVALID_STATE - The state is not valid
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
                                    the specified attribute

******************************************************************************/
CYBLE_API_RESULT_T CyBle_HrscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, 
    CYBLE_HRS_CHAR_INDEX_T charIndex, CYBLE_HRS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_HRS_CHAR_COUNT)
         || (descrIndex >= CYBLE_HRS_DESCR_COUNT)
         || (attrSize != CYBLE_CCCD_LEN))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(charIndex != CYBLE_HRS_HRM)
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_hrsc.hrmCccdHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else
    {
        /* Fill all fields of write request structure ... */
        writeReqParam.attrHandle = cyBle_hrsc.hrmCccdHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = CYBLE_CCCD_LEN;

        /* ... and send request to server device. */
        apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
        
        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_hrscReqHandle = cyBle_hrsc.hrmCccdHandle;
        }
    }

    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_HrscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets a characteristic descriptor of a specified characteristic of the service.
 
 This function call can result in generation of the following events based on
 the response from the server device:
 * CYBLE_EVT_HRSC_READ_DESCR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.
 descrIndex: The index of the service characteristic descriptor.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_INVALID_STATE - The state is not valid
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular descriptor
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
                                    the specified attribute

******************************************************************************/
CYBLE_API_RESULT_T CyBle_HrscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                       CYBLE_HRS_CHAR_INDEX_T charIndex, CYBLE_HRS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_HRS_CHAR_COUNT) || (descrIndex >= CYBLE_HRS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(charIndex != CYBLE_HRS_HRM)
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_hrsc.hrmCccdHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else
    {
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, cyBle_hrsc.hrmCccdHandle);
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_hrscReqHandle = cyBle_hrsc.hrmCccdHandle;
        }
    }
    
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_HrscDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 *discCharInfo: The pointer to a characteristic information structure.

Return:
 None

******************************************************************************/
void CyBle_HrscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    /* Heart Rate Service characteristics UUIDs */
    static const CYBLE_UUID16 cyBle_hrscCharUuid[CYBLE_HRS_CHAR_COUNT] =
    {
        CYBLE_UUID_CHAR_HR_MSRMT,
        CYBLE_UUID_CHAR_BODY_SENSOR_LOC,
        CYBLE_UUID_CHAR_HR_CNTRL_POINT
    };
    uint8 i;
    
    for(i = 0u; i < (uint8) CYBLE_HRS_CHAR_COUNT; i++)
    {
        if(cyBle_hrscCharUuid[i] == discCharInfo->uuid.uuid16)
        {
            if(cyBle_hrsc.charInfo[i].valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                cyBle_hrsc.charInfo[i].valueHandle = discCharInfo->valueHandle;
                cyBle_hrsc.charInfo[i].properties = discCharInfo->properties;
            }
            else
            {
                CyBle_ApplCallback(CYBLE_EVT_GATTC_CHAR_DUPLICATION, &discCharInfo->uuid);
            }
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_HrscDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 *discCharInfo: The pointer to a characteristic information structure.

Return:
 None

******************************************************************************/
void CyBle_HrscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo)
{
    if(CYBLE_UUID_CHAR_CLIENT_CONFIG == discDescrInfo->uuid.uuid16)
    {
        if(cyBle_hrsc.hrmCccdHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_hrsc.hrmCccdHandle = discDescrInfo->descrHandle;
        }
        else    /* Duplication of descriptor */
        {
            CyBle_ApplCallback(CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_HrscNotificationEventHandler
*******************************************************************************

Summary:
 Handles the Notification Event.

Parameters:
 *eventParam: The pointer to a data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_HrscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    if(cyBle_hrsc.charInfo[CYBLE_HRS_HRM].valueHandle == eventParam->handleValPair.attrHandle)
    {
        if(NULL != CyBle_HrsApplCallback)
        {
            CYBLE_HRS_CHAR_VALUE_T locCharValue;
            
            locCharValue.connHandle = eventParam->connHandle;
            locCharValue.charIndex = CYBLE_HRS_HRM;
            locCharValue.value = &eventParam->handleValPair.value;

            CyBle_HrsApplCallback(CYBLE_EVT_HRSC_NOTIFICATION, &locCharValue);
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_HrscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles the Read Response Event.

Parameters:
 CYBLE_GATTC_READ_RSP_PARAM_T *eventParam: The pointer to the data structure.

Return:
 None

******************************************************************************/
void CyBle_HrscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    if((NULL != CyBle_HrsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_hrscReqHandle))
    {
        if(cyBle_hrsc.charInfo[CYBLE_HRS_BSL].valueHandle == cyBle_hrscReqHandle)
        {
            CYBLE_HRS_CHAR_VALUE_T locCharValue;
            
            locCharValue.connHandle = eventParam->connHandle;
            locCharValue.charIndex = CYBLE_HRS_BSL;
            locCharValue.value = &eventParam->value;
            
            cyBle_hrscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            CyBle_HrsApplCallback((uint32)CYBLE_EVT_HRSC_READ_CHAR_RESPONSE, &locCharValue);
        }
        else if(cyBle_hrsc.hrmCccdHandle == cyBle_hrscReqHandle)
        {
            CYBLE_HRS_DESCR_VALUE_T locDescrValue;
            
            locDescrValue.connHandle = eventParam->connHandle;
            locDescrValue.charIndex = CYBLE_HRS_HRM;
            locDescrValue.descrIndex = CYBLE_HRS_HRM_CCCD;
            locDescrValue.value = &eventParam->value;

            cyBle_hrscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            CyBle_HrsApplCallback((uint32)CYBLE_EVT_HRSC_READ_DESCR_RESPONSE, &locDescrValue);
        }
        else
        {
            /* No any more read response handles */
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_HrscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles the Write Response Event.

Parameters:
 *eventParam: The pointer to a data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_HrscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    if((NULL != CyBle_HrsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_hrscReqHandle))
    {
        if(cyBle_hrsc.charInfo[CYBLE_HRS_CPT].valueHandle == cyBle_hrscReqHandle)
        {
            CYBLE_HRS_CHAR_VALUE_T locCharIndex;
            
            locCharIndex.connHandle = *eventParam;
            locCharIndex.charIndex = CYBLE_HRS_CPT;
            locCharIndex.value = NULL;
            
            cyBle_hrscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            CyBle_HrsApplCallback((uint32)CYBLE_EVT_HRSC_WRITE_CHAR_RESPONSE, &locCharIndex);
        }
        if(cyBle_hrsc.hrmCccdHandle == cyBle_hrscReqHandle)
        {
            CYBLE_HRS_DESCR_VALUE_T locDescIndex;
            
            locDescIndex.connHandle = *eventParam;
            locDescIndex.charIndex = CYBLE_HRS_HRM;
            locDescIndex.descrIndex = CYBLE_HRS_HRM_CCCD;
            locDescIndex.value = NULL;

            cyBle_hrscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            CyBle_HrsApplCallback((uint32)CYBLE_EVT_HRSC_WRITE_DESCR_RESPONSE, &locDescIndex);
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_HrscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles the Error Response Event.

Parameters:
 *eventParam: Pointer to the data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_HrscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((NULL != eventParam) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_hrscReqHandle))
    {
        cyBle_hrscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


#endif /* CYBLE_HRS_CLIENT */


/* [] END OF FILE */
