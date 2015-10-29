/*******************************************************************************
File Name: CYBLE_ancs.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the source code for
 the Apple Notification Center Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_AncsApplCallback;

#ifdef CYBLE_ANCS_SERVER

`$AncsServer`
    
    uint8 cyBle_ancsFlag;
    
//static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_ancssReqHandle;

#endif /* CYBLE_ANCS_SERVER */

#ifdef CYBLE_ANCS_CLIENT
    
/* Server's Apple Notification Center Service characteristics GATT DB handles structure */
CYBLE_ANCSC_T cyBle_ancsc;

/* Apple Notification Center Service UUID */
const CYBLE_UUID128_T cyBle_ancscServUuid =
{
    {0x79u, 0x05u, 0xF4u, 0x31u, 0xB5u, 0xCEu, 0x4Eu, 0x99u, 0xA4u, 0x0Fu, 0x4Bu, 0x1Eu, 0x12u, 0x2Du, 0x00u, 0xD0u}
};    

/* Apple Notification Center Service characteristics UUIDs */
const CYBLE_UUID128_T cyBle_ancscCharUuid[CYBLE_ANCS_CHAR_COUNT] =
{
    /* Notification Source characteristic UUID */
    {{0x9Fu, 0xBFu, 0x12u, 0x0Du, 0x63u, 0x01u, 0x42u, 0xD9u, 0x8Cu, 0x58u, 0x25u, 0xE6u, 0x99u, 0xA2u, 0x1Du, 0xBDu}},
    /* Control Point characteristic UUID */
    {{0x69u, 0xD1u, 0xD8u, 0xF3u, 0x45u, 0xE1u, 0x49u, 0xA8u, 0x98u, 0x21u, 0x9Bu, 0xBDu, 0xFDu, 0xAAu, 0xD9u, 0xD9u}},
    /* Data Source characteristic UUID */
    {{0x69u, 0xD1u, 0xD8u, 0xF3u, 0x45u, 0xE1u, 0x49u, 0xA8u, 0x98u, 0x21u, 0x9Bu, 0xBDu, 0xFDu, 0xAAu, 0xD9u, 0xD9u}}
};


/* Internal storage for last request handle to check the response */
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_ancscReqHandle;
#endif /* CYBLE_ANCS_CLIENT */


/******************************************************************************
##Function Name: CyBle_AncsInit
*******************************************************************************

Summary:
 Initializes the profile internals.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_AncsInit(void)
{
#ifdef CYBLE_ANCS_SERVER
    cyBle_ancsFlag = 0u;
//    cyBle_ancssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
#endif /* CYBLE_ANCS_SERVER */
#ifdef CYBLE_ANCS_CLIENT
    (void)memset(&cyBle_ancsc, 0, sizeof(cyBle_ancsc));
    cyBle_ancscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
#endif /* CYBLE_ANCS_CLIENT */
}


/******************************************************************************
##Function Name: CyBle_AncsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc:  An application layer event callback function to receive 
                    events from the BLE Component. The definition of 
                    CYBLE_CALLBACK_T for ANCS is,
            
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
void CyBle_AncsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_AncsApplCallback = callbackFunc;
}


#ifdef CYBLE_ANCS_SERVER


/******************************************************************************
##Function Name: CyBle_AncssSetCharacteristicValue
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
CYBLE_API_RESULT_T CyBle_AncssSetCharacteristicValue(CYBLE_ANCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(charIndex >= CYBLE_ANCS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ancss.charInfo[charIndex].charHandle)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    } 
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Store characteristic value into GATT database */
        locHandleValuePair.attrHandle = cyBle_ancss.charInfo[charIndex].charHandle;
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
##Function Name: CyBle_AncssGetCharacteristicValue
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
CYBLE_API_RESULT_T CyBle_AncssGetCharacteristicValue(CYBLE_ANCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(charIndex >= CYBLE_ANCS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ancss.charInfo[charIndex].charHandle)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Get characteristic value from GATT database */
        locHandleValuePair.attrHandle = cyBle_ancss.charInfo[charIndex].charHandle;
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
##Function Name: CyBle_AncssGetCharacteristicDescriptor
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
CYBLE_API_RESULT_T CyBle_AncssGetCharacteristicDescriptor(CYBLE_ANCS_CHAR_INDEX_T charIndex, 
                                                         CYBLE_ANCS_DESCR_INDEX_T descrIndex,
                                                         uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
	if((charIndex >= CYBLE_ANCS_CHAR_COUNT) || (descrIndex >= CYBLE_ANCS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ancss.charInfo[charIndex].descrHandle[CYBLE_ANCS_CCCD])
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Get data from database */
        locHandleValuePair.attrHandle = cyBle_ancss.charInfo[charIndex].descrHandle[descrIndex];
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
##Function Name: CyBle_AncssSendNotification
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
CYBLE_API_RESULT_T CyBle_AncssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
                    CYBLE_ANCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Send Notification if it is enabled and connected */
    if(CYBLE_STATE_CONNECTED != CyBle_GetState())
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    if(charIndex >= CYBLE_ANCS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ancss.charInfo[charIndex].descrHandle[CYBLE_ANCS_CCCD])
    {
		apiResult = CYBLE_ERROR_INVALID_OPERATION;
	}
    else if(!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_ancss.charInfo[charIndex].descrHandle[CYBLE_ANCS_CCCD]))
    {
        apiResult = CYBLE_ERROR_NTF_DISABLED;
    }
    else
    {
        CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfParam;
        
        /* Fill all fields of write request structure ... */
        ntfParam.attrHandle = cyBle_ancss.charInfo[charIndex].charHandle; 
        ntfParam.value.val = attrValue;
        ntfParam.value.len = attrSize;
        
        /* Send notification to client using previously filled structure */
        apiResult = CyBle_GattsNotification(connHandle, &ntfParam);
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_AncssWriteEventHandler
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
CYBLE_GATT_ERR_CODE_T CyBle_AncssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    
    if(NULL != CyBle_AncsApplCallback)
    {
	    CYBLE_ANCS_CHAR_VALUE_T locCharValue;
        locCharValue.charIndex = CYBLE_ANCS_CHAR_COUNT;
	
	    if(eventParam->handleValPair.attrHandle == cyBle_ancss.charInfo[CYBLE_ANCS_NS].descrHandle[CYBLE_ANCS_CCCD])
        {
            locCharValue.charIndex = CYBLE_ANCS_NS;
        }
        else if(eventParam->handleValPair.attrHandle == cyBle_ancss.charInfo[CYBLE_ANCS_DS].descrHandle[CYBLE_ANCS_CCCD])
        {
            locCharValue.charIndex = CYBLE_ANCS_DS;
        }
        else
        {
            /* Leave locCharValue.charIndex = CYBLE_ANCS_CHAR_COUNT */
        }
	    
        if(locCharValue.charIndex != CYBLE_ANCS_CHAR_COUNT)
        {
            locCharValue.connHandle = eventParam->connHandle;
            locCharValue.value = NULL;
            gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair,
                         0u, &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
            
            if(CYBLE_GATT_ERR_NONE == gattErr)
            {
                uint32 eventCode;
   
                if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                {
                    eventCode = (uint32) CYBLE_EVT_ANCSS_NOTIFICATION_ENABLED;
                }
                else
                {
                    eventCode = (uint32) CYBLE_EVT_ANCSS_NOTIFICATION_DISABLED;
                }

                CyBle_AncsApplCallback(eventCode, &locCharValue);
            }

        #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
            (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
            /* Set flag to store bonding data to flash */
            cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
        #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
			    
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
	    else
        {
            if(eventParam->handleValPair.attrHandle == cyBle_ancss.charInfo[CYBLE_ANCS_CP].charHandle)
            {
                locCharValue.charIndex = CYBLE_ANCS_CP;
                locCharValue.value = &eventParam->handleValPair.value;
                gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 
    	                      0u, &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
            
                if(gattErr == CYBLE_GATT_ERR_NONE)
    	        {
    	            CYBLE_GATT_DB_ATTR_SET_ATTR_GEN_LEN
                        (cyBle_ancss.charInfo[locCharValue.charIndex].charHandle,
                            locCharValue.value->len);
                    
                    CyBle_AncsApplCallback((uint32)CYBLE_EVT_ANCSS_WRITE_CHAR, &locCharValue);
    	        }
                
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
			}
        }
    }

    return (gattErr);
}

/****************************************************************************** 
##Function Name: CyBle_AncssPrepareWriteEventHandler
*******************************************************************************

Summary:
 Handles the Prepare Write Request Event.

Parameters:
 CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam: The pointer to the data that  
                                                 came with a prepare write
                                                 request.

Return:
 None.

******************************************************************************/
//void CyBle_AncssPrepareWriteRequestEventHandler(CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam)
//{
//    CYBLE_ANCS_CHAR_INDEX_T locCharIndex;
//    
//    for(locCharIndex = CYBLE_ANCS_FNM; locCharIndex <= CYBLE_ANCS_EML; locCharIndex++)
//    {
//        if(eventParam->attrHandle == cyBle_ancss.charInfo[locCharIndex].charHandle)
//        {
//            /* Send Prepare Write Response which identifies acknowledgment for
//            *  long/reliable characteristic value write.
//            */
//            CyBle_GattsPrepWriteReqSupport(CYBLE_GATTS_PREP_WRITE_SUPPORT);
//            
//            /* Indicate that request was handled */
//            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
//            break;
//        }
//    }
//}

/****************************************************************************** 
##Function Name: CyBle_AncssExecuteWriteRequestEventHandler
*******************************************************************************

Summary:
 Handles the Execute Write Request Event.

Parameters:
 CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam: The pointer to the data that came 
                                           with a write request.

Return:
 None.

******************************************************************************/
//void CyBle_AncssExecuteWriteRequestEventHandler(CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam)
//{
//    CYBLE_ANCS_CHAR_VALUE_T locCharValue;
//    CYBLE_GATT_VALUE_T     locGattValue;
//    
//    for(locCharValue.charIndex = CYBLE_ANCS_FNM; locCharValue.charIndex <= CYBLE_ANCS_EML; locCharValue.charIndex++)
//    {
//        if(eventParam->attrHandle == cyBle_ancss.charInfo[locCharValue.charIndex].charHandle)
//        {
//            locCharValue.connHandle = eventParam->connHandle;
//            locGattValue.len = eventParam->length;
//            locGattValue.val = NULL;
//            locCharValue.value = &locGattValue;
//            CyBle_AncsApplCallback((uint32)CYBLE_EVT_ANCSS_WRITE_CHAR, &locCharValue);
//            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK; /* Indicate that request was handled */
//            break;
//        }
//    }
//}


#endif /* CYBLE_ANCS_SERVER */

#ifdef CYBLE_ANCS_CLIENT


/******************************************************************************
##Function Name: CyBle_AncscSetCharacteristicValue
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
CYBLE_API_RESULT_T CyBle_AncscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_ANCS_CHAR_INDEX_T charIndex,
                                                            uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_ANCS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ancsc.charInfo[charIndex].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_WRITE & cyBle_ancsc.charInfo[charIndex].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
        CYBLE_GATTC_WRITE_REQ_T writeReqParam;
        
        writeReqParam.attrHandle = cyBle_ancsc.charInfo[charIndex].valueHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &writeReqParam);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_ancscReqHandle = cyBle_ancsc.charInfo[charIndex].valueHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_AncscGetCharacteristicValue
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
CYBLE_API_RESULT_T CyBle_AncscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_ANCS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_ANCS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ancsc.charInfo[charIndex].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_READ & cyBle_ancsc.charInfo[charIndex].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_ancsc.charInfo[charIndex].valueHandle);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_ancscReqHandle = cyBle_ancsc.charInfo[charIndex].valueHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_AncscSetCharacteristicDescriptor
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
CYBLE_API_RESULT_T CyBle_AncscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ANCS_CHAR_INDEX_T charIndex, CYBLE_ANCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_ANCS_CHAR_COUNT)
         || (descrIndex >= CYBLE_ANCS_DESCR_COUNT)
         || (attrSize != CYBLE_CCCD_LEN))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ancsc.charInfo[charIndex].descrHandle[descrIndex])
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else
    {
        CYBLE_GATTC_WRITE_REQ_T writeReqParam;
        
        /* Fill all fields of write request structure ... */
        writeReqParam.attrHandle = cyBle_ancsc.charInfo[charIndex].descrHandle[descrIndex];
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = CYBLE_CCCD_LEN;

        /* ... and send request to server device. */
        apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
        if(CYBLE_ERROR_OK == apiResult)
        {
            /* Save handle to support service specific read response from device */
            cyBle_ancscReqHandle = writeReqParam.attrHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_AncscGetCharacteristicDescriptor
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
CYBLE_API_RESULT_T CyBle_AncscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                       CYBLE_ANCS_CHAR_INDEX_T charIndex, CYBLE_ANCS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check the parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_ANCS_CHAR_COUNT) || (descrIndex >= CYBLE_ANCS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ancsc.charInfo[charIndex].descrHandle[descrIndex])
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else
    {
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, cyBle_ancsc.charInfo[charIndex].descrHandle[descrIndex]);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_ancscReqHandle = cyBle_ancsc.charInfo[charIndex].descrHandle[descrIndex];
        }
    }
    
    return (apiResult);
}

/******************************************************************************
##Function Name: CyBle_AncscDiscoverServiceEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a Read By Group Response event or 
 Read response with 128-bit service uuid. 

Parameters:
 *discServInfo: The pointer to a service information structure.

Return:
 None

******************************************************************************/
void CyBle_AncscDiscoverServiceEventHandler(const CYBLE_DISC_SRVC128_INFO_T *discServInfo)
{
//    uint16 j;
//    uint8 flag = 0u;
    
    /* Services with 128 bit UUID have discServInfo->uuid equal to 0 and address to 
       128 uuid is stored in cyBle_customCServ.uuid128
    */
//	for(j = 0u; (j < CYBLE_CUSTOMC_SERVICE_COUNT) && (flag == 0u); j++)
//    {
//        if(cyBle_customCServ[j].uuidFormat == CYBLE_GATT_128_BIT_UUID_FORMAT)
//        {
            if(memcmp(&cyBle_ancscServUuid, &discServInfo->uuid.uuid128, CYBLE_GATT_128_BIT_UUID_SIZE) == 0u)
            {
                if(cyBle_serverInfo[CYBLE_SRVI_ANCS].range.startHandle == 
                   CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
                {
                    cyBle_serverInfo[CYBLE_SRVI_ANCS].range = discServInfo->range;
                    cyBle_disCount++;
                //    flag = 1u;
                }
            }
//        }
//    }
}

/******************************************************************************
##Function Name: CyBle_AncscDiscoverCharacteristicsEventHandler
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
void CyBle_AncscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    static CYBLE_GATT_DB_ATTR_HANDLE_T *lnsLastEndHandle = NULL;
    uint8 i;
    
    /* Update last characteristic endHandle to declaration handle of this characteristic */
    if(lnsLastEndHandle != NULL)
    {
        *lnsLastEndHandle = discCharInfo->charDeclHandle - 1u;
        lnsLastEndHandle = NULL;
    }
    
    for(i = 0u; i < (uint8) CYBLE_ANCS_CHAR_COUNT; i++)
    {
        if(memcmp(&cyBle_ancscCharUuid[i], &discCharInfo->uuid.uuid128, CYBLE_GATT_128_BIT_UUID_SIZE) == 0u)
        {
            if(cyBle_ancsc.charInfo[i].valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                cyBle_ancsc.charInfo[i].valueHandle = discCharInfo->valueHandle;
                cyBle_ancsc.charInfo[i].properties = discCharInfo->properties;
                lnsLastEndHandle = &cyBle_ancsc.charInfo[i].endHandle;
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
        *lnsLastEndHandle = cyBle_serverInfo[CYBLE_SRVI_ANCS].range.endHandle;
    }
}


/******************************************************************************
##Function Name: CyBle_AncscDiscoverCharDescriptorsEventHandler
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
void CyBle_AncscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo)
{
    if(CYBLE_UUID_CHAR_CLIENT_CONFIG == discDescrInfo->uuid.uuid16)
    {
        uint8 descrIdx = (cyBle_disCount - CYBLE_SCDI_ANCS_NS);
        
        if(cyBle_disCount > (uint8) CYBLE_SCDI_ANCS_NS)
        {
            descrIdx++;
        }
        
        if(cyBle_ancsc.charInfo[descrIdx].descrHandle[CYBLE_ANCS_CCCD] == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_ancsc.charInfo[descrIdx].descrHandle[CYBLE_ANCS_CCCD] = discDescrInfo->descrHandle;
        }
        else    /* Duplication of descriptor */
        {
            CyBle_ApplCallback(CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_AncscNotificationEventHandler
*******************************************************************************

Summary:
 Handles the Notification Event.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_AncscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    if(NULL != CyBle_AncsApplCallback)
    {
        CYBLE_ANCS_CHAR_VALUE_T locCharValue;
        
        if(cyBle_ancsc.charInfo[CYBLE_ANCS_NS].valueHandle == eventParam->handleValPair.attrHandle)
        {
            locCharValue.charIndex = CYBLE_ANCS_NS;
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
        else if(cyBle_ancsc.charInfo[CYBLE_ANCS_DS].valueHandle == eventParam->handleValPair.attrHandle)
        {
            locCharValue.charIndex = CYBLE_ANCS_DS;
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
        else
        {
            /* Apple Notification Center Service doesn't support any other notifications */
        }
        
        if(0u == (cyBle_eventHandlerFlag & CYBLE_CALLBACK))
        {
            locCharValue.connHandle = eventParam->connHandle;
            
            locCharValue.value = &eventParam->handleValPair.value;

            CyBle_AncsApplCallback(CYBLE_EVT_ANCSC_NOTIFICATION, &locCharValue);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_AncscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles the Read Response Event.

Parameters:
 *eventParam:  the pointer to the data structure.

Return:
 None

******************************************************************************/
//void CyBle_AncscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
//{
//    if((NULL != CyBle_AncsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ancscReqHandle))
//    {
//        CYBLE_ANCS_CHAR_INDEX_T i;
//            
//        for(i = CYBLE_ANCS_FT; i < CYBLE_ANCS_CHAR_COUNT; i++)
//        {
//            if(cyBle_ancsc.charInfo[i].valueHandle == cyBle_ancscReqHandle)
//            {
//                CYBLE_ANCS_CHAR_VALUE_T locCharValue;
//                
//                locCharValue.connHandle = eventParam->connHandle;
//                locCharValue.charIndex = i;
//                locCharValue.value = &eventParam->value;
//                
//                cyBle_ancscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
//                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
//                CyBle_AncsApplCallback((uint32)CYBLE_EVT_ANCSC_READ_CHAR_RESPONSE, &locCharValue);
//            }
//        }
//        
//        if(0u != (cyBle_eventHandlerFlag | CYBLE_CALLBACK))
//        {
//            for(i = CYBLE_ANCS_FT; i < CYBLE_ANCS_CHAR_COUNT; i++)
//            {
//                if(cyBle_ancsc.charInfo[i].descrHandle[CYBLE_ANCS_CCCD] == cyBle_ancscReqHandle)
//                {
//                    CYBLE_ANCS_DESCR_VALUE_T locDescrValue;
//                    
//                    locDescrValue.connHandle = eventParam->connHandle;
//                    locDescrValue.charIndex = i;
//                    locDescrValue.descrIndex = CYBLE_ANCS_CCCD;
//                    locDescrValue.value = &eventParam->value;
//
//                    cyBle_ancscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
//                    cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
//                    CyBle_AncsApplCallback((uint32)CYBLE_EVT_ANCSC_READ_DESCR_RESPONSE, &locDescrValue);
//                    break;
//                }
//            }
//        }
//    }
//}


/******************************************************************************
##Function Name: CyBle_AncscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles the Write Response Event.

Parameters:
 *eventParam: The pointer to a data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_AncscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    if((NULL != CyBle_AncsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ancscReqHandle))
    {
        if(cyBle_ancsc.charInfo[CYBLE_ANCS_CP].valueHandle == cyBle_ancscReqHandle)
        {
            CYBLE_ANCS_CHAR_VALUE_T locCharIndex;
            
            locCharIndex.connHandle = *eventParam;
            locCharIndex.charIndex = CYBLE_ANCS_CP;
            locCharIndex.value = NULL;
            
            cyBle_ancscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            CyBle_AncsApplCallback((uint32)CYBLE_EVT_ANCSC_WRITE_CHAR_RESPONSE, &locCharIndex);
        }
        else
        {
            CYBLE_ANCS_DESCR_VALUE_T locDescIndex;
            
            if(cyBle_ancsc.charInfo[CYBLE_ANCS_NS].descrHandle[CYBLE_ANCS_CCCD] == cyBle_ancscReqHandle)
            {
                locDescIndex.charIndex = CYBLE_ANCS_NS;
                cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            }
            else if(cyBle_ancsc.charInfo[CYBLE_ANCS_DS].descrHandle[CYBLE_ANCS_CCCD] == cyBle_ancscReqHandle)
            {
                locDescIndex.charIndex = CYBLE_ANCS_DS;
                cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            }
            else
            {
                /* Apple Notification Center Service doesn't support any other notifications */
            }
            
            if(0u == (cyBle_eventHandlerFlag & CYBLE_CALLBACK))
            {
                locDescIndex.connHandle = *eventParam;
                locDescIndex.descrIndex = CYBLE_ANCS_CCCD;
                locDescIndex.value = NULL;

                cyBle_ancscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                CyBle_AncsApplCallback((uint32)CYBLE_EVT_ANCSC_WRITE_DESCR_RESPONSE, &locDescIndex);
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_AncscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles the Error Response Event.

Parameters:
 *eventParam: Pointer to the data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_AncscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((NULL != eventParam) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ancscReqHandle))
    {
        cyBle_ancscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


#endif /* CYBLE_ANCS_CLIENT */


/* [] END OF FILE */
