/*******************************************************************************
File Name: CYBLE_lns.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the source code for
 the Location and Navigation Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_LnsApplCallback;

#ifdef CYBLE_LNS_SERVER

`$LnsServer`
    
    uint8 cyBle_lnsFlag;
    
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_lnssReqHandle;

#endif /* CYBLE_LNS_SERVER */

#ifdef CYBLE_LNS_CLIENT
    
/* Server's Location and Navigation Service characteristics GATT DB handles structure */
CYBLE_LNSC_T cyBle_lnsc;

/* Internal storage for last request handle to check the response */
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_lnscReqHandle;
#endif /* CYBLE_LNS_CLIENT */


/******************************************************************************
##Function Name: CyBle_LnsInit
*******************************************************************************

Summary:
 Initializes the profile internals.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_LnsInit(void)
{
#ifdef CYBLE_LNS_SERVER
    cyBle_lnsFlag = 0u;
    cyBle_lnssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
#endif /* CYBLE_LNS_SERVER */
#ifdef CYBLE_LNS_CLIENT
    (void)memset(&cyBle_lnsc, 0, sizeof(cyBle_lnsc));
    cyBle_lnscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
#endif /* CYBLE_LNS_CLIENT */
}


/******************************************************************************
##Function Name: CyBle_LnsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc:  An application layer event callback function to receive 
                    events from the BLE Component. The definition of 
                    CYBLE_CALLBACK_T for LNS is,
            
                    typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                      void *eventParam)

                * eventCode indicates the event that triggered this 
                  callback.
                * eventParam contains the parameters corresponding to the 
                  current event.
Return:
 None

Side Effects:
 The *eventParams in the callback function should not be used by the 
 application once the callback function execution is finished. Otherwise this
 data may become corrupted.
 
******************************************************************************/
void CyBle_LnsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_LnsApplCallback = callbackFunc;
}


#ifdef CYBLE_LNS_SERVER


/******************************************************************************
##Function Name: CyBle_LnssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets the value of the characteristic, as identified by charIndex.

Parameters:
 charIndex: The index of the service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be
                  stored to the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
    
******************************************************************************/
CYBLE_API_RESULT_T CyBle_LnssSetCharacteristicValue(CYBLE_LNS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(charIndex >= CYBLE_LNS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_lnss.charInfo[charIndex].charHandle)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    } 
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Store characteristic value into GATT database */
        locHandleValuePair.attrHandle = cyBle_lnss.charInfo[charIndex].charHandle;
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
##Function Name: CyBle_LnssGetCharacteristicValue
*******************************************************************************

Summary:
 Gets the value of the characteristic, as identified by charIndex.

Parameters:
 charIndex:  The index of the service characteristic.
 attrSize:   The size of the characteristic value attribute.
 *attrValue: The pointer to the location where characteristic 
             value data should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - Characteristic value was read successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the CyBle_GattsWriteAttributeValue
                                     input parameter failed.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this characteristic.
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Characteristic is absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_LnssGetCharacteristicValue(CYBLE_LNS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(charIndex >= CYBLE_LNS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_lnss.charInfo[charIndex].charHandle)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Get characteristic value from GATT database */
        locHandleValuePair.attrHandle = cyBle_lnss.charInfo[charIndex].charHandle;
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
##Function Name: CyBle_LnssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets a characteristic descriptor of the specified characteristic.

Parameters:
 charIndex:  The index of the characteristic.
 descrIndex: The index of the descriptor.
 attrSize:   The size of the descriptor value attribute.
 *attrValue: The pointer to the location where characteristic descriptor value 
             data should be stored. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - Characteristic Descriptor value was read successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Characteristic is absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_LnssGetCharacteristicDescriptor(CYBLE_LNS_CHAR_INDEX_T charIndex, 
                                                         CYBLE_LNS_DESCR_INDEX_T descrIndex,
                                                         uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
	if((charIndex >= CYBLE_LNS_CHAR_COUNT) || (descrIndex >= CYBLE_LNS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(((charIndex == CYBLE_LNS_CP) && (!CYBLE_LNS_IS_CP_SUPPORTED)) ||
            ((charIndex == CYBLE_LNS_NV) && (!CYBLE_LNS_IS_NV_SUPPORTED)))
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Get data from database */
        locHandleValuePair.attrHandle = cyBle_lnss.charInfo[charIndex].descrHandle[descrIndex];
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
##Function Name: CyBle_LnssSendNotification
*******************************************************************************

Summary:
 Sends a notification of the specified characteristic value, as identified by
 the charIndex.

Parameters:
 connHandle: The connection handle which consist of the device ID and ATT 
             connection ID.
 charIndex:  The index of the service characteristic.
 attrSize:   The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be sent 
             to the client device. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                     characteristic
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
  * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the client

******************************************************************************/
CYBLE_API_RESULT_T CyBle_LnssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
                    CYBLE_LNS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Send Notification if it is enabled and connected */
    if(CYBLE_STATE_CONNECTED != CyBle_GetState())
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    if(charIndex >= CYBLE_LNS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_lnss.charInfo[charIndex].descrHandle[CYBLE_LNS_CCCD])
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_lnss.charInfo[charIndex].descrHandle[CYBLE_LNS_CCCD]))
    {
        apiResult = CYBLE_ERROR_NTF_DISABLED;
    }
    else
    {
        CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfParam;
        
        /* Fill all fields of write request structure ... */
        ntfParam.attrHandle = cyBle_lnss.charInfo[charIndex].charHandle; 
        ntfParam.value.val = attrValue;
        ntfParam.value.len = attrSize;
        
        /* Send notification to client using previously filled structure */
        apiResult = CyBle_GattsNotification(connHandle, &ntfParam);
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_LnssSendIndication
*******************************************************************************

Summary:
 Sends an indication of the specified characteristic value, as identified by
 the charIndex.

Parameters:
 connHandle: The connection handle which consist of the device ID and ATT 
             connection ID.
 charIndex:  The index of the service characteristic.
 attrSize:   The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be sent 
             to the client device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                     characteristic
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
  * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the client
  * CYBLE_ERROR_IND_DISABLED - Indication is disabled for this characteristic

******************************************************************************/
CYBLE_API_RESULT_T CyBle_LnssSendIndication(CYBLE_CONN_HANDLE_T connHandle,
                    CYBLE_LNS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Send Indication if it is enabled and connected */
    if(CYBLE_STATE_CONNECTED != CyBle_GetState())
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    if(charIndex >= CYBLE_LNS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_lnss.charInfo[charIndex].descrHandle[CYBLE_LNS_CCCD])
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(!CYBLE_IS_INDICATION_ENABLED(cyBle_lnss.charInfo[charIndex].descrHandle[CYBLE_LNS_CCCD]))
    {
        apiResult = CYBLE_ERROR_IND_DISABLED;
    }
    else
    {
        CYBLE_GATTS_HANDLE_VALUE_IND_T indParam;
        
        /* Fill all fields of write request structure ... */
        indParam.attrHandle = cyBle_lnss.charInfo[charIndex].charHandle; 
        indParam.value.val = attrValue;
        indParam.value.len = attrSize;
        
        /* Send notification to client using previously filled structure */
        apiResult = CyBle_GattsIndication(connHandle, &indParam);
        
        if(CYBLE_ERROR_OK == apiResult)
        {
            /* Save handle to support service specific value confirmation response from client */
            cyBle_lnssReqHandle = indParam.attrHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_LnssConfirmationEventHandler
*******************************************************************************

Summary:
 Handles the Value Confirmation request event from the BLE stack.

Parameters:
 *eventParam: Pointer to a structure of type CYBLE_CONN_HANDLE_T

Return:
 None

******************************************************************************/
void CyBle_LnssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    if((NULL != CyBle_LnsApplCallback) 
        && (cyBle_lnss.charInfo[CYBLE_LNS_CP].charHandle == cyBle_lnssReqHandle))
    {   
        CYBLE_LNS_CHAR_VALUE_T locCharIndex;
        
        cyBle_lnssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        
        cyBle_lnsFlag &= (uint8) ~CYBLE_LNS_FLAG_PROCESS;
        
        locCharIndex.connHandle = *eventParam;
        locCharIndex.charIndex = CYBLE_LNS_CP;
        locCharIndex.value = NULL;
        
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        CyBle_LnsApplCallback((uint32)CYBLE_EVT_LNSS_INDICATION_CONFIRMED, &locCharIndex);
    }
}


/******************************************************************************
##Function Name: CyBle_LnssWriteEventHandler
*******************************************************************************

Summary:
 Handles the Write Request Event.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 CYBLE_GATT_ERR_CODE_T - an API result state if the API succeeded 
 (CYBLE_GATT_ERR_NONE) or failed with error codes:
  * CYBLE_GATTS_ERR_PROCEDURE_ALREADY_IN_PROGRESS
  * CYBLE_GATTS_ERR_CCCD_IMPROPERLY_CONFIGURED

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_LnssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    CYBLE_LNS_CHAR_VALUE_T locCharValue;
    
    if(NULL != CyBle_LnsApplCallback)
    {
	    locCharValue.connHandle = eventParam->connHandle;
	
	    if(eventParam->handleValPair.attrHandle == cyBle_lnss.charInfo[CYBLE_LNS_CP].charHandle)
	    {    
            if(CYBLE_LNS_IS_PROCEDURE_IN_PROGRESS)
            {
                gattErr = CYBLE_GATT_ERR_PROCEDURE_ALREADY_IN_PROGRESS;
            }
            else if(!CYBLE_IS_INDICATION_ENABLED(cyBle_lnss.charInfo[CYBLE_LNS_CP].descrHandle[CYBLE_LNS_CCCD]))
            {
                gattErr = CYBLE_GATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
            }
            else
            {
                locCharValue.charIndex = CYBLE_LNS_CP;
                locCharValue.value = &eventParam->handleValPair.value;
                gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 
    	                      0u, &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
    	        if(CYBLE_GATT_ERR_NONE == gattErr)
    	        {
    	            CyBle_LnsApplCallback((uint32)CYBLE_EVT_LNSS_WRITE_CHAR, &locCharValue);
                    cyBle_lnsFlag |= CYBLE_LNS_FLAG_PROCESS;
    	        }
            }
			
			cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
	    }
	    else
	    {    
	        for(locCharValue.charIndex = CYBLE_LNS_LS;
                    locCharValue.charIndex < CYBLE_LNS_CHAR_COUNT; locCharValue.charIndex++)
	        {
	            if(CYBLE_LNS_PQ == locCharValue.charIndex)
                {
                    locCharValue.charIndex++;
                }
                
                if(eventParam->handleValPair.attrHandle == cyBle_lnss.charInfo[locCharValue.charIndex].descrHandle[CYBLE_LNS_CCCD])
	            {
	                locCharValue.value = NULL;
                    gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair,
	                             0u, &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
	                
                    if(CYBLE_GATT_ERR_NONE == gattErr)
	                {
                        uint32 eventCode;
                        
                        if(locCharValue.charIndex == CYBLE_LNS_CP)
                        {
                            if(CYBLE_IS_INDICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                            {
                                eventCode = (uint32) CYBLE_EVT_LNSS_INDICATION_ENABLED;
                            }
                            else
                            {
                                eventCode = (uint32) CYBLE_EVT_LNSS_INDICATION_DISABLED;
                            }
                        }
                        else
                        {
                            if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                            {
                                eventCode = (uint32) CYBLE_EVT_LNSS_NOTIFICATION_ENABLED;
                            }
                            else
                            {
                                eventCode = (uint32) CYBLE_EVT_LNSS_NOTIFICATION_DISABLED;
                            }
                        }
                        
                        CyBle_LnsApplCallback(eventCode, &locCharValue);
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
    }

    return (gattErr);
}


#endif /* CYBLE_LNS_SERVER */

#ifdef CYBLE_LNS_CLIENT


/******************************************************************************
##Function Name: CyBle_LnscSetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to write the characteristic (which is identified by
 charIndex) value attribute in the server.

 The Write Response just confirms the operation success.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.
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
CYBLE_API_RESULT_T CyBle_LnscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_LNS_CHAR_INDEX_T charIndex,
                                                            uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_LNS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_lnsc.charInfo[charIndex].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_WRITE & cyBle_lnsc.charInfo[charIndex].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
        CYBLE_GATTC_WRITE_REQ_T writeReqParam;
        
        writeReqParam.attrHandle = cyBle_lnsc.charInfo[charIndex].valueHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &writeReqParam);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_lnscReqHandle = cyBle_lnsc.charInfo[charIndex].valueHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_LnscGetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to read the characteristic Value from a server,
 as identified by its charIndex

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
CYBLE_API_RESULT_T CyBle_LnscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_LNS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_LNS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_lnsc.charInfo[charIndex].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_READ & cyBle_lnsc.charInfo[charIndex].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_lnsc.charInfo[charIndex].valueHandle);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_lnscReqHandle = cyBle_lnsc.charInfo[charIndex].valueHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_LnscSetCharacteristicDescriptor
*******************************************************************************

Summary:
 This function is used to write the characteristic Value to the server,
 as identified by its charIndex.

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
CYBLE_API_RESULT_T CyBle_LnscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_LNS_CHAR_INDEX_T charIndex, CYBLE_LNS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_LNS_CHAR_COUNT)
         || (descrIndex >= CYBLE_LNS_DESCR_COUNT)
         || (attrSize != CYBLE_CCCD_LEN))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_lnsc.charInfo[charIndex].descrHandle[descrIndex])
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else
    {
        CYBLE_GATTC_WRITE_REQ_T writeReqParam;
        
        /* Fill all fields of write request structure ... */
        writeReqParam.attrHandle = cyBle_lnsc.charInfo[charIndex].descrHandle[descrIndex];
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = CYBLE_CCCD_LEN;

        /* ... and send request to server device. */
        apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
        if(CYBLE_ERROR_OK == apiResult)
        {
            /* Save handle to support service specific read response from device */
            cyBle_lnscReqHandle = writeReqParam.attrHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_LnscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets the characteristic descriptor of the specified characteristic.

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
CYBLE_API_RESULT_T CyBle_LnscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                       CYBLE_LNS_CHAR_INDEX_T charIndex, CYBLE_LNS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_LNS_CHAR_COUNT) || (descrIndex >= CYBLE_LNS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_lnsc.charInfo[charIndex].descrHandle[descrIndex])
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else
    {
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, cyBle_lnsc.charInfo[charIndex].descrHandle[descrIndex]);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_lnscReqHandle = cyBle_lnsc.charInfo[charIndex].descrHandle[descrIndex];
        }
    }
    
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_LnscDiscoverCharacteristicsEventHandler
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
void CyBle_LnscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    /* Location and Navigation Service characteristics UUIDs */
    static const CYBLE_UUID16 cyBle_lnscCharUuid[CYBLE_LNS_CHAR_COUNT] =
    {
        CYBLE_UUID_CHAR_LN_FEATURE,
        CYBLE_UUID_CHAR_LOCATION_AND_SPEED,
        CYBLE_UUID_CHAR_POSITION_QUALITY,
        CYBLE_UUID_CHAR_LN_CONTROL_POINT,
        CYBLE_UUID_CHAR_NAVIGATION
    };
    static CYBLE_GATT_DB_ATTR_HANDLE_T *lnsLastEndHandle = NULL;
    uint8 i;
    
    /* Update last characteristic endHandle to declaration handle of this characteristic */
    if(lnsLastEndHandle != NULL)
    {
        *lnsLastEndHandle = discCharInfo->charDeclHandle - 1u;
        lnsLastEndHandle = NULL;
    }
    
    for(i = 0u; i < (uint8) CYBLE_LNS_CHAR_COUNT; i++)
    {
        if(cyBle_lnscCharUuid[i] == discCharInfo->uuid.uuid16)
        {
            if(cyBle_lnsc.charInfo[i].valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                cyBle_lnsc.charInfo[i].valueHandle = discCharInfo->valueHandle;
                cyBle_lnsc.charInfo[i].properties = discCharInfo->properties;
                lnsLastEndHandle = &cyBle_lnsc.charInfo[i].endHandle;
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
    if(lnsLastEndHandle != NULL)
    {
        *lnsLastEndHandle = cyBle_serverInfo[CYBLE_SRVI_LNS].range.endHandle;
    }
}


/******************************************************************************
##Function Name: CyBle_LnscDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 *discCharInfo: The pointer to the characteristic information structure.

Return:
 None

******************************************************************************/
void CyBle_LnscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo)
{
    if(CYBLE_UUID_CHAR_CLIENT_CONFIG == discDescrInfo->uuid.uuid16)
    {
        uint8 descrIdx = (cyBle_disCount - CYBLE_SCDI_LNS_LS) + 1u;
        
        if(cyBle_disCount > (uint8) CYBLE_SCDI_LNS_LS)
        {
            descrIdx++;
        }
        
        if(cyBle_lnsc.charInfo[descrIdx].descrHandle[CYBLE_LNS_CCCD] == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_lnsc.charInfo[descrIdx].descrHandle[CYBLE_LNS_CCCD] = discDescrInfo->descrHandle;
        }
        else    /* Duplication of descriptor */
        {
            CyBle_ApplCallback(CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_LnscNotificationEventHandler
*******************************************************************************

Summary:
 Handles the Notification Event.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_LnscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    if(NULL != CyBle_LnsApplCallback)
    {
        CYBLE_LNS_CHAR_VALUE_T locCharValue;
        
        if(cyBle_lnsc.charInfo[CYBLE_LNS_LS].valueHandle == eventParam->handleValPair.attrHandle)
        {
            locCharValue.charIndex = CYBLE_LNS_LS;
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
        else if(cyBle_lnsc.charInfo[CYBLE_LNS_NV].valueHandle == eventParam->handleValPair.attrHandle)
        {
            locCharValue.charIndex = CYBLE_LNS_NV;
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
        else
        {
            /* Location and Navigation Service doesn't support any other notifications */
        }
        
        if(0u == (cyBle_eventHandlerFlag & CYBLE_CALLBACK))
        {
            locCharValue.connHandle = eventParam->connHandle;
            
            locCharValue.value = &eventParam->handleValPair.value;

            CyBle_LnsApplCallback(CYBLE_EVT_LNSC_NOTIFICATION, &locCharValue);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_LnscIndicationEventHandler
*******************************************************************************

Summary:
 Handles the Indication Event.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_LnscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam)
{
    if(NULL != CyBle_LnsApplCallback)
    {
        if(cyBle_lnsc.charInfo[CYBLE_LNS_CP].valueHandle == eventParam->handleValPair.attrHandle)
        {
            CYBLE_LNS_CHAR_VALUE_T locCharValue;
            locCharValue.charIndex = CYBLE_LNS_CP;
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            locCharValue.connHandle = eventParam->connHandle;       
            locCharValue.value = &eventParam->handleValPair.value;

            CyBle_LnsApplCallback(CYBLE_EVT_LNSC_INDICATION, &locCharValue);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_LnscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles the Read Response Event.

Parameters:
 *eventParam:  the pointer to the data structure.

Return:
 None

******************************************************************************/
void CyBle_LnscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    if((NULL != CyBle_LnsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_lnscReqHandle))
    {
        CYBLE_LNS_CHAR_INDEX_T i;
            
        for(i = CYBLE_LNS_FT; i < CYBLE_LNS_CHAR_COUNT; i++)
        {
            if(cyBle_lnsc.charInfo[i].valueHandle == cyBle_lnscReqHandle)
            {
                CYBLE_LNS_CHAR_VALUE_T locCharValue;
                
                locCharValue.connHandle = eventParam->connHandle;
                locCharValue.charIndex = i;
                locCharValue.value = &eventParam->value;
                
                cyBle_lnscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                CyBle_LnsApplCallback((uint32)CYBLE_EVT_LNSC_READ_CHAR_RESPONSE, &locCharValue);
            }
        }
        
        if(0u != (cyBle_eventHandlerFlag | CYBLE_CALLBACK))
        {
            for(i = CYBLE_LNS_FT; i < CYBLE_LNS_CHAR_COUNT; i++)
            {
                if(cyBle_lnsc.charInfo[i].descrHandle[CYBLE_LNS_CCCD] == cyBle_lnscReqHandle)
                {
                    CYBLE_LNS_DESCR_VALUE_T locDescrValue;
                    
                    locDescrValue.connHandle = eventParam->connHandle;
                    locDescrValue.charIndex = i;
                    locDescrValue.descrIndex = CYBLE_LNS_CCCD;
                    locDescrValue.value = &eventParam->value;

                    cyBle_lnscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                    cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                    CyBle_LnsApplCallback((uint32)CYBLE_EVT_LNSC_READ_DESCR_RESPONSE, &locDescrValue);
                    break;
                }
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_LnscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles the Write Response Event.

Parameters:
 *eventParam: The pointer to a data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_LnscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    if((NULL != CyBle_LnsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_lnscReqHandle))
    {
        if(cyBle_lnsc.charInfo[CYBLE_LNS_CP].valueHandle == cyBle_lnscReqHandle)
        {
            CYBLE_LNS_CHAR_VALUE_T locCharIndex;
            
            locCharIndex.connHandle = *eventParam;
            locCharIndex.charIndex = CYBLE_LNS_CP;
            locCharIndex.value = NULL;
            
            cyBle_lnscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            CyBle_LnsApplCallback((uint32)CYBLE_EVT_LNSC_WRITE_CHAR_RESPONSE, &locCharIndex);
        }
        else
        {
            CYBLE_LNS_CHAR_INDEX_T i;
            
            for(i = CYBLE_LNS_FT; i < CYBLE_LNS_CHAR_COUNT; i++)
            {
                if(cyBle_lnsc.charInfo[i].descrHandle[CYBLE_LNS_CCCD] == cyBle_lnscReqHandle)
                {
                    CYBLE_LNS_DESCR_VALUE_T locDescIndex;
                    
                    locDescIndex.connHandle = *eventParam;
                    locDescIndex.charIndex = i;
                    locDescIndex.descrIndex = CYBLE_LNS_CCCD;
                    locDescIndex.value = NULL;

                    cyBle_lnscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                    cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                    CyBle_LnsApplCallback((uint32)CYBLE_EVT_LNSC_WRITE_DESCR_RESPONSE, &locDescIndex);
                    break;
                }
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_LnscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles the Error Response Event.

Parameters:
 *eventParam: Pointer to the data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_LnscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((NULL != eventParam) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_lnscReqHandle))
    {
        cyBle_lnscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


#endif /* CYBLE_LNS_CLIENT */


/* [] END OF FILE */
