/*******************************************************************************
File Name: CYBLE_pass.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the source code for
 the Phone Alert Status Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_PassApplCallback;

#ifdef CYBLE_PASS_SERVER
`$PassServer`
    
    uint8 cyBle_passFlag;
#endif /* CYBLE_PASS_SERVER */

#ifdef CYBLE_PASS_CLIENT
    /* Server's Phone Alert Status Service characteristics GATT DB handles structure */
    CYBLE_PASSC_T cyBle_passc;

    /* Internal storage for last request handle to check the response */
    static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_passcReqHandle;
#endif /* CYBLE_PASS_CLIENT */


/******************************************************************************
##Function Name: CyBle_PassInit
*******************************************************************************

Summary:
 Initializes the profile internals.

Parameters:
 None

Return:
 None.

******************************************************************************/
void CyBle_PassInit(void)
{
#ifdef CYBLE_PASS_CLIENT
    (void)memset(&cyBle_passc, 0, sizeof(cyBle_passc));
    cyBle_passcReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
#endif /* CYBLE_GATT_ROLE_CLIENT */
}


/****************************************************************************** 
##Function Name: CyBle_PassRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc:  An application layer event callback function to receive 
                events from the BLE Component. The definition of 
                CYBLE_CALLBACK_T for PASS is,
        
                typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                  void *eventParam)

                * eventCode indicates the event that triggered this 
                  callback.
                * eventParam contains the parameters corresponding to the 
                  current event.

Return:
 None.

******************************************************************************/
void CyBle_PassRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_PassApplCallback = callbackFunc;
}


#ifdef CYBLE_PASS_SERVER


/******************************************************************************
##Function Name: CyBle_PasssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets the value of a characteristic which is identified by charIndex.

Parameters:
 charIndex:       the index of a service characteristic.
 attrSize:        the size of the characteristic value attribute.
 *attrValue:      the pointer to the characteristic value data that should be
                  stored to the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
    
******************************************************************************/
CYBLE_API_RESULT_T CyBle_PasssSetCharacteristicValue(CYBLE_PASS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(charIndex >= CYBLE_PASS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_passs.charInfo[charIndex].charHandle)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Store characteristic value into GATT database */
        locHandleValuePair.attrHandle = cyBle_passs.charInfo[charIndex].charHandle;
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        
        if(CYBLE_GATT_ERR_NONE != CyBle_GattsWriteAttributeValue(&locHandleValuePair,
                                    0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
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
##Function Name: CyBle_PasssGetCharacteristicValue
*******************************************************************************

Summary:
 Gets the value of a characteristic which is identified by charIndex.

Parameters:
 charIndex:       the index of a service characteristic.
 attrSize:        the size of the characteristic value attribute.
 *attrValue:      The pointer to the location where characteristic value 
                  data should be stored. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional descriptor is absent

******************************************************************************/
CYBLE_API_RESULT_T CyBle_PasssGetCharacteristicValue(CYBLE_PASS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(charIndex >= CYBLE_PASS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_passs.charInfo[charIndex].charHandle)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Get characteristic value from GATT database */
        locHandleValuePair.attrHandle = cyBle_passs.charInfo[charIndex].charHandle;
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        
        if(CYBLE_GATT_ERR_NONE != CyBle_GattsReadAttributeValue(&locHandleValuePair,
                                    0u, CYBLE_GATT_DB_LOCALLY_INITIATED))
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
##Function Name: CyBle_PasssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets a characteristic descriptor of a specified characteristic of the service.

Parameters:
 charIndex:       The index of the characteristic.
 descrIndex:      The index of the descriptor.
 attrSize:        The size of the descriptor value attribute.
 *attrValue:      The pointer to the descriptor value data that should be
                  stored to the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional descriptor is absent

******************************************************************************/
CYBLE_API_RESULT_T CyBle_PasssGetCharacteristicDescriptor(CYBLE_PASS_CHAR_INDEX_T charIndex, 
                                                         CYBLE_PASS_DESCR_INDEX_T descrIndex,
                                                         uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if((charIndex >= CYBLE_PASS_CHAR_COUNT) || (descrIndex >= CYBLE_PASS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_passs.charInfo[charIndex].descrHandle[descrIndex])
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Get data from database */
        locHandleValuePair.attrHandle = cyBle_passs.charInfo[charIndex].descrHandle[descrIndex];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;

        if(CYBLE_GATT_ERR_NONE != CyBle_GattsReadAttributeValue(&locHandleValuePair,
                                    0u, CYBLE_GATT_DB_LOCALLY_INITIATED))
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
##Function Name: CyBle_PasssSendNotification
*******************************************************************************

Summary:
 Sends a notification of the specified by the charIndex characteristic value.

Parameters:
 connHandle:      The connection handle
                  which consists of the device ID and ATT connection ID.
 charIndex:       The index of a service characteristic.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the characteristic value data that should 
                  be sent to the client device. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed 
  * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the Client

******************************************************************************/
CYBLE_API_RESULT_T CyBle_PasssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
                    CYBLE_PASS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Send Notification if it is enabled and connected */
    if(CYBLE_STATE_CONNECTED != CyBle_GetState())
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_PASS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(charIndex > CYBLE_PASS_RS)
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else if(!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_passs.charInfo[charIndex].descrHandle[CYBLE_PASS_CCCD]))
    {
        apiResult = CYBLE_ERROR_NTF_DISABLED;
    }
    else
    {
        CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfParam;
        
        /* Fill all fields of write request structure ... */
        ntfParam.attrHandle = cyBle_passs.charInfo[charIndex].charHandle; 
        ntfParam.value.val = attrValue;
        ntfParam.value.len = attrSize;
        
        /* Send notification to the Client using previously filled structure */
        apiResult = CyBle_GattsNotification(connHandle, &ntfParam);
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_PasssWriteEventHandler
*******************************************************************************

Summary:
 Handles the Write Request Event.

Parameters:
 *eventParam:  The pointer to the data structure specified by the event.

Return:
 CYBLE_GATT_ERR_CODE_T - an API result state if the API succeeded (CYBLE_GATT_ERR_NONE) or failed with error codes:
  * CYBLE_GATT_ERR_PROCEDURE_ALREADY_IN_PROGRESS
  * CYBLE_GATT_ERR_CCCD_IMPROPERLY_CONFIGURED

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_PasssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    
    if(NULL != CyBle_PassApplCallback)
    {
        CYBLE_PASS_CHAR_VALUE_T locCharIndex;
    
        for(locCharIndex.charIndex = CYBLE_PASS_AS;
                locCharIndex.charIndex < CYBLE_PASS_CP; locCharIndex.charIndex++)
        {
            if(eventParam->handleValPair.attrHandle == cyBle_passs.charInfo[locCharIndex.charIndex].descrHandle[CYBLE_PASS_CCCD])
            {
                locCharIndex.connHandle = eventParam->connHandle;
                locCharIndex.value = NULL;
                
                gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair,
                             0u, &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                
                if(CYBLE_GATT_ERR_NONE == gattErr)
                {
                    if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                    {
                        CyBle_PassApplCallback((uint32)CYBLE_EVT_PASSS_NOTIFICATION_ENABLED, &locCharIndex);
                    }
                    else
                    {
                        CyBle_PassApplCallback((uint32)CYBLE_EVT_PASSS_NOTIFICATION_DISABLED, &locCharIndex);
                    }
                }
            #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
                (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
                /* Set flag to store bonding data to flash */
                cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
            #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
				
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                break;
			}
        }
    }

    return (gattErr);
}


/******************************************************************************
##Function Name: CyBle_PasssWriteCmdEventHandler
*******************************************************************************

Summary:
 Handles the Write Without Response Request Event.

Parameters:
 *eventParam:  The pointer to the data structure specified by the event.

Return:
 None.

******************************************************************************/
void CyBle_PasssWriteCmdEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam) 
{
    if((NULL != CyBle_PassApplCallback) && 
            (eventParam->handleValPair.attrHandle == cyBle_passs.charInfo[CYBLE_PASS_CP].charHandle))
    {
        CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
        CYBLE_PASS_CHAR_VALUE_T locCharValue;
        
        locCharValue.charIndex = CYBLE_PASS_CP;
        locCharValue.value = &eventParam->handleValPair.value;
        gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 
                      0u, &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
        if(CYBLE_GATT_ERR_NONE == gattErr)
        {
            CyBle_PassApplCallback((uint32)CYBLE_EVT_PASSS_WRITE_CHAR, &locCharValue);
        }
        
    	cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
}


#endif /* CYBLE_PASS_SERVER */

#ifdef CYBLE_PASS_CLIENT


/******************************************************************************
##Function Name: CyBle_PasscSetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to write the characteristic (which is identified by
 charIndex) value attribute to the Server.

 The Write Response just confirms the operation success.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the characteristic value data that
                  should be sent to the server device.

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
CYBLE_API_RESULT_T CyBle_PasscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_PASS_CHAR_INDEX_T charIndex,
                                                            uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_PASS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_passc.charInfo[charIndex].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_WRITE_WITHOUT_RSP & cyBle_passc.charInfo[charIndex].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
        CYBLE_GATTC_WRITE_REQ_T writeReqParam;
        
        writeReqParam.attrHandle = cyBle_passc.charInfo[charIndex].valueHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        apiResult = CyBle_GattcWriteWithoutResponse(connHandle, &writeReqParam);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_passcReqHandle = cyBle_passc.charInfo[charIndex].valueHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_PasscGetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to read the characteristic Value from a Server
 which is identified by the charIndex.

 The Read Response returns the characteristic Value in the Attribute Value
 parameter.

 The Read Response only contains the characteristic Value that is less than or
 equal to (MTU - 1) octets in length. If the characteristic Value is greater
 than (MTU - 1) octets in length, the Read Long Characteristic Value procedure
 may be used if the rest of the characteristic Value is required.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The read request was sent successfully.  
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_STATE - Connection with the Server is not established.
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                    characteristic.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_PasscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_PASS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_PASS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_passc.charInfo[charIndex].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_READ & cyBle_passc.charInfo[charIndex].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_passc.charInfo[charIndex].valueHandle);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_passcReqHandle = cyBle_passc.charInfo[charIndex].valueHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_PasscSetCharacteristicDescriptor
*******************************************************************************

Summary:
 This function is used to write the characteristic Value to the server
 which is identified by the charIndex.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic.
 descrIndex:      The index of a service characteristic descriptor.
 attrSize:        The size of the characteristic descriptor value attribute.
 *attrValue:      The pointer to the characteristic descriptor value data that
                  should be sent to the server device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_INVALID_STATE - The state is not valid.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic.
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
                                    the specified attribute.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_PasscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, 
    CYBLE_PASS_CHAR_INDEX_T charIndex, CYBLE_PASS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_PASS_CHAR_COUNT)
         || (descrIndex >= CYBLE_PASS_DESCR_COUNT)
         || (attrSize != CYBLE_CCCD_LEN))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_passc.charInfo[charIndex].descrHandle[descrIndex])
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else
    {
        CYBLE_GATTC_WRITE_REQ_T writeReqParam;
        
        /* Fill all fields of write request structure ... */
        writeReqParam.attrHandle = cyBle_passc.charInfo[charIndex].descrHandle[descrIndex];
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = CYBLE_CCCD_LEN;

        /* ... and send request to Server device. */
        apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
        if(CYBLE_ERROR_OK == apiResult)
        {
            /* Save handle to support service specific read response from device */
            cyBle_passcReqHandle = writeReqParam.attrHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_PasscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets a characteristic descriptor of a specified characteristic of the service.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic.
 descrIndex:      The index of a service characteristic descriptor.

Return:
Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_INVALID_STATE - The state is not valid.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular descriptor.
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
                                    the specified attribute.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_PasscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                       CYBLE_PASS_CHAR_INDEX_T charIndex, CYBLE_PASS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_PASS_CHAR_COUNT) || (descrIndex >= CYBLE_PASS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_passc.charInfo[charIndex].descrHandle[descrIndex])
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else
    {
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle,
                        cyBle_passc.charInfo[charIndex].descrHandle[descrIndex]);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_passcReqHandle = cyBle_passc.charInfo[charIndex].descrHandle[descrIndex];
        }
    }
    
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_PasscDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a "CYBLE_EVT_GATTC_READ_BY_TYPE_RSP"
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 *discCharInfo:  the pointer to a characteristic information structure.

Return:
 None

******************************************************************************/
void CyBle_PasscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    /* Phone Alert Status Service characteristics UUIDs */
    static const CYBLE_UUID16 cyBle_passcCharUuid[CYBLE_PASS_CHAR_COUNT] =
    {
        CYBLE_UUID_CHAR_ALERT_STATUS,
        CYBLE_UUID_CHAR_RINGER_SETTING,
        CYBLE_UUID_CHAR_RINGER_CONTROL_POINT
    };
    static CYBLE_GATT_DB_ATTR_HANDLE_T *passLastEndHandle = NULL;
    uint8 i;
    
    /* Update last characteristic endHandle to declaration handle of this characteristic */
    if(passLastEndHandle != NULL)
    {
        *passLastEndHandle = discCharInfo->charDeclHandle - 1u;
        passLastEndHandle = NULL;
    }
    
    for(i = 0u; i < (uint8) CYBLE_PASS_CHAR_COUNT; i++)
    {
        if(cyBle_passcCharUuid[i] == discCharInfo->uuid.uuid16)
        {
            if(cyBle_passc.charInfo[i].valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                cyBle_passc.charInfo[i].valueHandle = discCharInfo->valueHandle;
                cyBle_passc.charInfo[i].properties = discCharInfo->properties;
                passLastEndHandle = &cyBle_passc.charInfo[i].endHandle;
            }
            else
            {
                CyBle_ApplCallback(CYBLE_EVT_GATTC_CHAR_DUPLICATION, &discCharInfo->uuid);
            }
        }
    }
    
    /* Init characteristic endHandle to Service endHandle.
       Characteristic endHandle will be updated to the declaration
       Handler of the following characteristic,
       in the following characteristic discovery procedure. */
    if(passLastEndHandle != NULL)
    {
        *passLastEndHandle = cyBle_serverInfo[CYBLE_SRVI_PASS].range.endHandle;
    }
}


/******************************************************************************
##Function Name: CyBle_PasscDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a "CYBLE_EVT_GATTC_READ_BY_TYPE_RSP"
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 *discCharInfo:  The pointer to a characteristic information structure.

Return:
 None

******************************************************************************/
void CyBle_PasscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo)
{
    if(CYBLE_UUID_CHAR_CLIENT_CONFIG == discDescrInfo->uuid.uuid16)
    {
        if(cyBle_passc.charInfo[cyBle_disCount - CYBLE_SCDI_PASS_AS].descrHandle[CYBLE_PASS_CCCD] == 
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_passc.charInfo[cyBle_disCount - CYBLE_SCDI_PASS_AS].descrHandle[CYBLE_PASS_CCCD] = 
                discDescrInfo->descrHandle;
        }
        else    /* Duplication of descriptor */
        {
            CyBle_ApplCallback(CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_PasscNotificationEventHandler
*******************************************************************************

Summary:
 Handles the Notification Event.

Parameters:
 *eventParam:  The pointer to a data structure specified by the event.

Return:
 None.

******************************************************************************/
void CyBle_PasscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    if(NULL != CyBle_PassApplCallback)
    {
        CYBLE_PASS_CHAR_VALUE_T locCharValue;
        
        for(locCharValue.charIndex = CYBLE_PASS_AS; locCharValue.charIndex < CYBLE_PASS_CP; locCharValue.charIndex++)
        {
            if(cyBle_passc.charInfo[locCharValue.charIndex].valueHandle == eventParam->handleValPair.attrHandle)
            {
                cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
                break;
            }
        }
        
        if(0u == (cyBle_eventHandlerFlag & CYBLE_CALLBACK))
        {
            locCharValue.connHandle = eventParam->connHandle;
            
            locCharValue.value = &eventParam->handleValPair.value;

            CyBle_PassApplCallback(CYBLE_EVT_PASSC_NOTIFICATION, &locCharValue);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_PasscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles the Read Response Event.

Parameters:
 *eventParam:  The pointer to the data structure.

Return:
 None.

******************************************************************************/
void CyBle_PasscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    if((NULL != CyBle_PassApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_passcReqHandle))
    {
        CYBLE_PASS_CHAR_INDEX_T locCharIndex;
        
        for(locCharIndex = CYBLE_PASS_AS; locCharIndex < CYBLE_PASS_CP; locCharIndex++)
        {
            if(cyBle_passc.charInfo[locCharIndex].valueHandle == cyBle_passcReqHandle)
            {
                CYBLE_PASS_CHAR_VALUE_T locCharValue;
                
                locCharValue.connHandle = eventParam->connHandle;
                locCharValue.charIndex = locCharIndex;
                locCharValue.value = &eventParam->value;
                
                cyBle_passcReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
                CyBle_PassApplCallback((uint32)CYBLE_EVT_PASSC_READ_CHAR_RESPONSE, &locCharValue);
            }
            else if(cyBle_passc.charInfo[locCharIndex].descrHandle[CYBLE_PASS_CCCD] == cyBle_passcReqHandle)
            {
                CYBLE_PASS_DESCR_VALUE_T locDescrValue;
                
                locDescrValue.connHandle = eventParam->connHandle;
                locDescrValue.charIndex = locCharIndex;
                locDescrValue.descrIndex = CYBLE_PASS_CCCD;
                locDescrValue.value = &eventParam->value;

                cyBle_passcReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                CyBle_PassApplCallback((uint32)CYBLE_EVT_PASSC_READ_DESCR_RESPONSE, &locDescrValue);
            }
            else
            {
            }
            
            if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_passcReqHandle)
            {
                break;
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_PasscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles the Write Response Event.

Parameters:
 *eventParam:  The pointer to a data structure specified by the event.

Return:
 None.

******************************************************************************/
void CyBle_PasscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    if((NULL != CyBle_PassApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_passcReqHandle))
    {
        CYBLE_PASS_CHAR_INDEX_T locCharIndex;
        
        for(locCharIndex = CYBLE_PASS_AS; locCharIndex < CYBLE_PASS_CP; locCharIndex++)
        {
            if(cyBle_passc.charInfo[locCharIndex].descrHandle[CYBLE_PASS_CCCD] == cyBle_passcReqHandle)
            {
                CYBLE_PASS_DESCR_VALUE_T locDescIndex;
                
                locDescIndex.connHandle = *eventParam;
                locDescIndex.charIndex = locCharIndex;
                locDescIndex.descrIndex = CYBLE_PASS_CCCD;
                locDescIndex.value = NULL;

                cyBle_passcReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
                CyBle_PassApplCallback((uint32)CYBLE_EVT_PASSC_WRITE_DESCR_RESPONSE, &locDescIndex);
                break;
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_PasscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles the Error Response Event.

Parameters:
 *eventParam:  Pointer to the data structure specified by the event.

Return:
 None.

******************************************************************************/
void CyBle_PasscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((NULL != eventParam) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_passcReqHandle))
    {
        cyBle_passcReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


#endif /* CYBLE_PASS_CLIENT */


/* [] END OF FILE */
