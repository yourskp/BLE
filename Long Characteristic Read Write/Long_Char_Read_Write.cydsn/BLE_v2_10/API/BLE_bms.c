/*******************************************************************************
File Name: CYBLE_bms.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the source code for the Bond Management Service
 of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_BmsApplCallback;

#ifdef CYBLE_BMS_SERVER

/* Service GATT DB handles structure */
`$BmsServer`

#endif /* CYBLE_BMS_SERVER */

#ifdef CYBLE_BMS_CLIENT
    
/* Server's BM Service GATT DB handles structure */
CYBLE_BMSC_T cyBle_bmsc;

/* Internal storage for last request handle to check response */
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_bmscReqHandle;
#endif /* CYBLE_BMS_CLIENT */


/****************************************************************************** 
##Function Name: CyBle_BmsInit
*******************************************************************************

Summary:
 Initializes the profile internals.

Parameters:
 None.

Return:
 None.

******************************************************************************/
void CyBle_BmsInit(void)
{
#ifdef CYBLE_BMS_CLIENT
    (void)memset(&cyBle_bmsc, 0, sizeof(cyBle_bmsc));
    cyBle_bmscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
#endif /* CYBLE_GATT_ROLE_CLIENT */
}


/******************************************************************************
##Function Name: CyBle_BmsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc:  An application layer event callback function to receive 
                   events from the BLE Component. The definition of 
                   CYBLE_CALLBACK_T for BM Service is,
            
                   typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                      void *eventParam)

                   * eventCode indicates the event that triggered this 
                     callback.
                   * eventParam contains the parameters corresponding to the 
                     current event.

Return:
 None.

Side Effects: The *eventParams in the callback function should not be used 
               by the application once the callback function execution is 
               finished. Otherwise this data may become corrupted.

******************************************************************************/
void CyBle_BmsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_BmsApplCallback = callbackFunc;
}


#ifdef CYBLE_BMS_SERVER


/******************************************************************************
##Function Name: CyBle_BmssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets a characteristic value of the service identified by charIndex.

Parameters:
 charIndex: The index of a service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be
              stored in the GATT database.

Return:
 The return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BmssSetCharacteristicValue(CYBLE_BMS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check parameters */
    if(charIndex >= CYBLE_BMS_CHAR_COUNT) 
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_bmss.charInfo[charIndex].charHandle)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Store characteristic value into GATT database */
        locHandleValuePair.attrHandle = cyBle_bmss.charInfo[charIndex].charHandle;
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
##Function Name: CyBle_BmssGetCharacteristicValue
*******************************************************************************

Summary:
 Gets a characteristic value of the service, which is identified by charIndex.

Parameters:
 charIndex: The index of a service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the location where Characteristic value data should 
              be stored.

Return:
 The return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BmssGetCharacteristicValue(CYBLE_BMS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check parameters */
    if(charIndex >= CYBLE_BMS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_bmss.charInfo[charIndex].charHandle)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else
    {
        CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
        
        /* Get characteristic value from GATT database */
        locHandleValuePair.attrHandle = cyBle_bmss.charInfo[charIndex].charHandle;
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
##Function Name: CyBle_BmssSetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sets a characteristic descriptor of a specified characteristic of the service.

Parameters:
 charIndex:       The index of a service characteristic of type 
                  CYBLE_BMS_CHAR_INDEX_T.
 descrIndex:      The index of a service characteristic descriptor of type 
                  CYBLE_BMS_DESCR_INDEX_T.
 attrSize:        The size of the characteristic descriptor attribute.
 *attrValue:      The pointer to the descriptor value data that should 
                  be stored to the GATT database. 

Return:
 The return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request is handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BmssSetCharacteristicDescriptor(CYBLE_BMS_CHAR_INDEX_T charIndex,
    CYBLE_BMS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((charIndex >= CYBLE_BMS_CHAR_COUNT) || (descrIndex >= CYBLE_BMS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        locHandleValuePair.attrHandle = cyBle_bmss.charInfo[charIndex].descrHandle[descrIndex];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        if(CYBLE_GATT_ERR_NONE !=
                CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
    }
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_BmssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets a characteristic descriptor of a specified characteristic of the service.

Parameters:
 charIndex:       The index of a service characteristic of type 
                  CYBLE_BMS_CHAR_INDEX_T.
 descrIndex:      The index of a service characteristic descriptor of type 
                  CYBLE_BMS_DESCR_INDEX_T.
 attrSize:        The size of the characteristic descriptor attribute.
 *attrValue:      The pointer to the location where characteristic descriptor
                  value data should be stored.

Return:
 The return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BmssGetCharacteristicDescriptor(CYBLE_BMS_CHAR_INDEX_T charIndex,
    CYBLE_BMS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((charIndex >= CYBLE_BMS_CHAR_COUNT) || (descrIndex >= CYBLE_BMS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        locHandleValuePair.attrHandle = cyBle_bmss.charInfo[charIndex].descrHandle[descrIndex];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        if(CYBLE_GATT_ERR_NONE !=
                CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
    }
    return (apiResult);
}

/******************************************************************************
##Function Name: CyBle_BmssWriteEventHandler
*******************************************************************************

Summary:
 Handles a Write Request event. Calls the registered BMS application 
 callback function.

Note: Writing the attribute into GATT DB (if needed) is on the user's responsibility
 after the Authorization Code check in the registered BMS application 
 callback function.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 CYBLE_GATT_ERR_CODE_T - An API result state if the API succeeded 
 (CYBLE_GATT_ERR_NONE) or failed with error codes:
  * CYBLE_GATT_ERR_OP_CODE_NOT_SUPPORTED
  * CYBLE_GATT_ERR_INSUFFICIENT_AUTHORIZATION


******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_BmssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_BMS_CHAR_VALUE_T locCharValue;
    locCharValue.gattErrorCode = CYBLE_GATT_ERR_NONE;
    
    if((NULL != CyBle_BmsApplCallback) && 
        (eventParam->handleValPair.attrHandle == cyBle_bmss.charInfo[CYBLE_BMS_BMCP].charHandle))
    {
        if(locCharValue.gattErrorCode == CYBLE_GATT_ERR_NONE)
        {
            locCharValue.connHandle = eventParam->connHandle;
            locCharValue.charIndex = CYBLE_BMS_BMCP;
            locCharValue.value = &eventParam->handleValPair.value;
            
            CyBle_BmsApplCallback((uint32)CYBLE_EVT_BMSS_WRITE_CHAR, &locCharValue);
            
            if(locCharValue.gattErrorCode == CYBLE_GATT_ERR_NONE)
            {
                locCharValue.gattErrorCode = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 
            	                      0u, &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                
                if(locCharValue.gattErrorCode == CYBLE_GATT_ERR_NONE)
                {
                    CYBLE_GATT_DB_ATTR_SET_ATTR_GEN_LEN(cyBle_bmss.charInfo[CYBLE_BMS_BMCP].charHandle, locCharValue.value->len);
                }   
            }
        }
        
        /* Indicate that request was handled */
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }

    return (locCharValue.gattErrorCode);
}

/****************************************************************************** 
##Function Name: CyBle_BmssPrepareWriteEventHandler
*******************************************************************************

Summary:
 Handles a Prepare Write Request event.

Parameters:
 CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam: The pointer to the data that  
                                                 came with a prepare write
                                                 request.

Return:
 None.

******************************************************************************/
void CyBle_BmssPrepareWriteRequestEventHandler(CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam)
{
    if(eventParam->baseAddr[eventParam->currentPrepWriteReqCount - 1u].handleValuePair.attrHandle ==
        cyBle_bmss.charInfo[CYBLE_BMS_BMCP].charHandle)
    {
        uint16 locMtu = CYBLE_GATT_MTU;
        (void) CyBle_GattGetMtuSize(&locMtu);
        
        if(((cyBle_bmss.charInfo[CYBLE_BMS_BMCP].descrHandle[CYBLE_BMS_CEPD] != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            && CYBLE_IS_RELIABLE_WRITE_ENABLED(cyBle_bmss.charInfo[CYBLE_BMS_BMCP].descrHandle[CYBLE_BMS_CEPD])) ||
            (CYBLE_GATT_DB_ATTR_GET_ATTR_GEN_MAX_LEN(cyBle_bmss.charInfo[CYBLE_BMS_BMCP].charHandle) > (locMtu - 3u)))
        {   
            if(eventParam->currentPrepWriteReqCount == 1u) /* The first prepare write event */
            {
                CyBle_GattsPrepWriteReqSupport(CYBLE_GATTS_PREP_WRITE_SUPPORT);
            }
        } 
        else
        {
            eventParam->gattErrorCode = CYBLE_GATT_ERR_ATTRIBUTE_NOT_LONG;
        }
        
        /* Indicate that request was handled */
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
}

/****************************************************************************** 
##Function Name: CyBle_BmssExecuteWriteRequestEventHandler
*******************************************************************************

Summary:
 Handles an Execute Write Request event.

Parameters:
 CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam: The pointer to the data that came 
                                           with a write request.

Return:
 None.

******************************************************************************/
void CyBle_BmssExecuteWriteRequestEventHandler(CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam)
{
    uint8 count;
    CYBLE_GATT_VALUE_T locGattValue;
    locGattValue.len = 0u;
    locGattValue.val = NULL;
    
    for(count = 0u; count < eventParam->prepWriteReqCount; count++)
    {
        if(eventParam->baseAddr[count].handleValuePair.attrHandle ==
            cyBle_bmss.charInfo[CYBLE_BMS_BMCP].charHandle)
        {
            locGattValue.len = eventParam->baseAddr[count].offset + 
                               eventParam->baseAddr[count].handleValuePair.value.len;

            if(locGattValue.val == NULL)
            {
                locGattValue.val = eventParam->baseAddr[count].handleValuePair.value.val;
            }
            else if(eventParam->baseAddr[count].offset == 0u)
            {
                /* The case when client wants to rewrite the value from the begin */
                locGattValue.val = eventParam->baseAddr[count].handleValuePair.value.val;
            }
            else
            {
                /* do nothing */   
            }
        }
    }
    
    if((locGattValue.len != 0u) &&
        (locGattValue.len <= CYBLE_GATT_DB_ATTR_GET_ATTR_GEN_MAX_LEN(cyBle_bmss.charInfo[CYBLE_BMS_BMCP].charHandle)))
    {
        CYBLE_BMS_CHAR_VALUE_T locCharValue;
        locCharValue.gattErrorCode = CYBLE_GATT_ERR_NONE;
        locCharValue.connHandle = eventParam->connHandle;
        locCharValue.value = &locGattValue;
        
        CyBle_BmsApplCallback((uint32)CYBLE_EVT_BMSS_WRITE_CHAR, &locCharValue);
                   
        if(locCharValue.gattErrorCode == CYBLE_GATT_ERR_NONE)
        {
            CYBLE_GATT_DB_ATTR_SET_ATTR_GEN_LEN(cyBle_bmss.charInfo[CYBLE_BMS_BMCP].charHandle, locGattValue.len);    
        }
       
        eventParam->gattErrorCode = locCharValue.gattErrorCode;
        
        /* Indicate that request was handled */
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
}

#endif /* CYBLE_BMS_SERVER */

#ifdef CYBLE_BMS_CLIENT


/******************************************************************************
##Function Name: CyBle_BmscGetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to read the characteristic value from a server
 which is identified by charIndex.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.

Return:
 The return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The read request was sent successfully.  
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                    characteristic.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BmscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_BMS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    /* Check parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex != CYBLE_BMS_BMFT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_bmsc.charInfo[charIndex].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_READ & cyBle_bmsc.charInfo[charIndex].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_bmsc.charInfo[CYBLE_BMS_BMFT].valueHandle);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_bmscReqHandle = cyBle_bmsc.charInfo[charIndex].valueHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_BmscSetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to write the characteristic (which is identified by
 charIndex) value attribute to the server. The function supports a long write
 procedure - it depends on the attrSize parameter - if it is larger than the current
 MTU size - 1, then the long write will be executed.

 The Write response just confirms the operation success.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of a service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be 
              sent to the server device.

Return:
 The return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic.
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                    characteristic.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BmscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_BMS_CHAR_INDEX_T charIndex,
                                                            uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    uint16 locMtu = CYBLE_GATT_MTU;
    (void) CyBle_GattGetMtuSize(&locMtu);
    
    /* Check parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex != CYBLE_BMS_BMCP)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_WRITE & cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else if((locMtu - 3u) < attrSize)
    {
        CYBLE_GATTC_PREP_WRITE_REQ_T prepWriteReqParam;
        
        prepWriteReqParam.handleValuePair.attrHandle = cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].valueHandle;
        prepWriteReqParam.offset = 0u;
        prepWriteReqParam.handleValuePair.value.val = attrValue;
        prepWriteReqParam.handleValuePair.value.len = attrSize;   
        
        apiResult = CyBle_GattcWriteLongCharacteristicValues(connHandle, &prepWriteReqParam);
        
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_bmscReqHandle = cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].valueHandle;
        }
    }
    else
    {
        CYBLE_GATTC_WRITE_REQ_T writeReqParam;
        
        writeReqParam.attrHandle = cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].valueHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &writeReqParam);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_bmscReqHandle = cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].valueHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_BmscReliableWriteCharacteristicValue
*******************************************************************************

Summary:
 This function is used to perform a reliable write command for the 
 Bond Management Control Pointcharacteristic (identified by
 charIndex) value attribute to the server.

 The Write response just confirms the operation success.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of a service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be 
              sent to the server device.

Return:
 The return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic.
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                    characteristic.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BmscReliableWriteCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_BMS_CHAR_INDEX_T charIndex,
                                                            uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex != CYBLE_BMS_BMCP)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
	else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].valueHandle)
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else if(0u == (CYBLE_CHAR_PROP_EXTENDED_PROPERTIES & cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].properties))
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
        CYBLE_GATTC_PREP_WRITE_REQ_T prepWriteReqParam;
        
        prepWriteReqParam.handleValuePair.attrHandle = cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].valueHandle;
        prepWriteReqParam.handleValuePair.value.val = attrValue;
        prepWriteReqParam.handleValuePair.value.len = attrSize;
        prepWriteReqParam.offset = 0u;

        apiResult = CyBle_GattcReliableWrites(connHandle, &prepWriteReqParam, 1u);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_bmscReqHandle = cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].valueHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_BmscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets the characteristic descriptor of the specified characteristic.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of a service characteristic.
 descrIndex: The index of the service characteristic descriptor.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_INVALID_STATE - The state is not valid.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular descriptor.
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
                                    the specified attribute.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BmscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                       CYBLE_BMS_CHAR_INDEX_T charIndex, CYBLE_BMS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Check parameters */
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_BMS_CHAR_COUNT) || (descrIndex >= CYBLE_BMS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].descrHandle[CYBLE_BMS_CEPD])
    {
		apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
	}
    else
    {
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].descrHandle[CYBLE_BMS_CEPD]);
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_bmscReqHandle = cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].descrHandle[CYBLE_BMS_CEPD];
        }
    }
    
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_BmscDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 *discCharInfo: The pointer to a characteristic information structure.

Return:
 None.

******************************************************************************/
void CyBle_BmscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    /* BM Service characteristics UUIDs */
    static const CYBLE_UUID16 cyBle_bmscCharUuid[CYBLE_BMS_CHAR_COUNT] =
    {
        CYBLE_UUID_BOND_MANAGEMENT_CONTROL_POINT,
        CYBLE_UUID_BOND_MANAGEMENT_FEATURE
    };
    static CYBLE_GATT_DB_ATTR_HANDLE_T *bmsLastEndHandle = NULL;
    uint8 i;
    
    /* Update last characteristic endHandle to declaration handle of this characteristic */
    if(bmsLastEndHandle != NULL)
    {
        *bmsLastEndHandle = discCharInfo->charDeclHandle - 1u;
        bmsLastEndHandle = NULL;
    }
    
    for(i = 0u; i < (uint8) CYBLE_BMS_CHAR_COUNT; i++)
    {
        if(cyBle_bmscCharUuid[i] == discCharInfo->uuid.uuid16)
        {
            if(cyBle_bmsc.charInfo[i].valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                cyBle_bmsc.charInfo[i].valueHandle = discCharInfo->valueHandle;
                cyBle_bmsc.charInfo[i].properties = discCharInfo->properties;
                bmsLastEndHandle = &cyBle_bmsc.charInfo[i].endHandle;
            }
            else
            {
                CyBle_ApplCallback((uint32)CYBLE_EVT_GATTC_CHAR_DUPLICATION, &discCharInfo->uuid);
            }
        }
    }
    
    /* Init characteristic endHandle to Service endHandle.
       Characteristic endHandle will be updated to declaration
       Handler of following characteristic,
       in following characteristic discovery procedure. */
    if(bmsLastEndHandle != NULL)
    {
        *bmsLastEndHandle = cyBle_serverInfo[CYBLE_SRVI_BMS].range.endHandle;
    }
}

/******************************************************************************
##Function Name: CyBle_BmscDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 *discCharInfo: The pointer to a characteristic information structure.

Return:
 None.

******************************************************************************/
void CyBle_BmscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    if(CYBLE_UUID_CHAR_EXTENDED_PROPERTIES == discDescrInfo->uuid.uuid16)
    {
        if(cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].descrHandle[CYBLE_BMS_CEPD] == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].descrHandle[CYBLE_BMS_CEPD] = discDescrInfo->descrHandle;
        }
        else    /* Duplication of descriptor */
        {
            CyBle_ApplCallback((uint32)CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
        }
    }
}

/******************************************************************************
##Function Name: CyBle_BmscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles the Read Response Event.

Parameters:
 *eventParam: The pointer to the data structure.

Return:
 None.

******************************************************************************/
void CyBle_BmscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    if((NULL != CyBle_BmsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_bmscReqHandle))
    {   
        if(cyBle_bmsc.charInfo[CYBLE_BMS_BMFT].valueHandle == cyBle_bmscReqHandle)
        {
            CYBLE_BMS_CHAR_VALUE_T locCharValue;
            
            locCharValue.connHandle = eventParam->connHandle;
            locCharValue.charIndex = CYBLE_BMS_BMFT;
            locCharValue.value = &eventParam->value;
            
            cyBle_bmscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            CyBle_BmsApplCallback((uint32)CYBLE_EVT_BMSC_READ_CHAR_RESPONSE, &locCharValue);
        }
        else if(cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].descrHandle[CYBLE_BMS_CEPD] == cyBle_bmscReqHandle)
        {
            CYBLE_BMS_DESCR_VALUE_T locDescrValue;
            
            locDescrValue.connHandle = eventParam->connHandle;
            locDescrValue.charIndex = CYBLE_BMS_BMCP;
            locDescrValue.descrIndex = CYBLE_BMS_CEPD;
            locDescrValue.value = &eventParam->value;

            cyBle_bmscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            CyBle_BmsApplCallback((uint32)CYBLE_EVT_BMSC_READ_DESCR_RESPONSE, &locDescrValue);
        }
        else
        {
            /* Nothing to do */
        }
    }
}


/******************************************************************************
##Function Name: CyBle_BmscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles a Write Response event.

Parameters:
 *eventParam: The pointer to a data structure specified by the event.

Return:
 None.

******************************************************************************/
void CyBle_BmscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    if((NULL != CyBle_BmsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_bmscReqHandle))
    {
        if(cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].valueHandle == cyBle_bmscReqHandle)
        {
            CYBLE_BMS_CHAR_VALUE_T locCharValue;
            
            locCharValue.connHandle = *eventParam;
            locCharValue.charIndex = CYBLE_BMS_BMCP;
            locCharValue.value = NULL;
            
            cyBle_bmscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            CyBle_BmsApplCallback((uint32)CYBLE_EVT_BMSC_WRITE_CHAR_RESPONSE, &locCharValue);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_BmscExecuteWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles a Write Response event.

Parameters:
 *eventParam: The pointer to a data structure specified by the event.

Return:
 None.

******************************************************************************/
void CyBle_BmscExecuteWriteResponseEventHandler(const CYBLE_GATTC_EXEC_WRITE_RSP_T *eventParam)
{
    if((NULL != CyBle_BmsApplCallback) &&
        (cyBle_bmsc.charInfo[CYBLE_BMS_BMCP].valueHandle == cyBle_bmscReqHandle))
    {   
        CYBLE_BMS_CHAR_VALUE_T locCharValue;
        
        locCharValue.connHandle = eventParam->connHandle;
        locCharValue.charIndex = CYBLE_BMS_BMCP;
        locCharValue.value = NULL;
        
        cyBle_bmscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        CyBle_BmsApplCallback((uint32)CYBLE_EVT_BMSC_WRITE_CHAR_RESPONSE, &locCharValue);
    }    
}


/******************************************************************************
##Function Name: CyBle_BmscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles the Error Response event.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 None.

******************************************************************************/
void CyBle_BmscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((NULL != eventParam) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_bmscReqHandle))
    {
        cyBle_bmscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


#endif /* CYBLE_BMS_CLIENT */


/* [] END OF FILE */
