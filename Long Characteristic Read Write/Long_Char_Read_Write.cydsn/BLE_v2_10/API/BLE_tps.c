/*******************************************************************************
File Name: CYBLE_tps.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the source code for the Tx Power Service of the BLE
 Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"


static CYBLE_CALLBACK_T CyBle_TpsApplCallback = NULL;

#ifdef CYBLE_TPS_SERVER

`$TpsServer`
    
#endif /* CYBLE_TPS_SERVER */


#ifdef CYBLE_TPS_CLIENT
    
CYBLE_TPSC_T cyBle_tpsc;

/* Internal storage for last request handle to check response */
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_tpscReqHandle;

#endif /* CYBLE_TPS_CLIENT */

/******************************************************************************
##Function Name: CyBle_TpsInit
*******************************************************************************

Summary:
 This function initializes the Tx Power Service.

Parameters: 
 None

Return:
 None

******************************************************************************/
void CyBle_TpsInit(void)
{

#ifdef CYBLE_TPS_CLIENT

    (void) memset((void *) &cyBle_tpsc, 0, sizeof(cyBle_tpsc));
    cyBle_tpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    
#endif /* CYBLE_TPS_CLIENT */
}


/******************************************************************************
##Function Name: CyBle_TpsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc:  An application layer event callback function to receive 
                    events from the BLE Component. The definition of 
                    CYBLE_CALLBACK_T for TPS is,
            
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
void CyBle_TpsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_TpsApplCallback = callbackFunc;
}


#ifdef CYBLE_TPS_SERVER

/******************************************************************************
##Function Name: CyBle_TpssWriteEventHandler
*******************************************************************************

Summary:
 Handles the Write Request Event for Tx Power service.

Parameters:
 CYBLE_GATTS_WRITE_REQ_PARAM_T * eventParam: The pointer to the data that came
                                              with a write request for TPS.

Return:
 Return value is of type CYBLE_GATT_ERR_CODE_T.
  * CYBLE_GATT_ERR_NONE - Write is successful.
  * CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED - Request is not supported.
  * CYBLE_GATT_ERR_INVALID_HANDLE - 'handleValuePair.attrHandle' is not valid.
  * CYBLE_GATT_ERR_WRITE_NOT_PERMITTED - Write operation is not permitted on
                                         this attribute.
  * CYBLE_GATT_ERR_INVALID_OFFSET - Offset value is invalid.
  * CYBLE_GATT_ERR_UNLIKELY_ERROR - Some other error occurred.

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_TpssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    uint32 event = CYBLE_EVT_TPSS_NOTIFICATION_DISABLED;
    
    /* Check event code if it defines write request */
    if(NULL != CyBle_TpsApplCallback)
    {
        /* Client Characteristic Configuration descriptor write request */
        if(eventParam->handleValPair.attrHandle == cyBle_tpss.txPowerLevelCccdHandle)
        {
            /* Verify that optional notification property is enabled for Tx Power Level characteristic */
            if(CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_tpss.txPowerLevelCharHandle))
            {
                if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                {
                    event = CYBLE_EVT_TPSS_NOTIFICATION_ENABLED;
                }
                
                gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                    &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                
                if(CYBLE_GATT_ERR_NONE == gattErr)
                {
                    CYBLE_TPS_CHAR_VALUE_T wrReqParam;
                    
                    wrReqParam.connHandle = eventParam->connHandle;
                    wrReqParam.charIndex = CYBLE_TPS_TX_POWER_LEVEL;
                    wrReqParam.value = NULL;
                    
                    CyBle_TpsApplCallback(event, (void *)&wrReqParam);
                }
            }
            else
            {
                gattErr = CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED;
            }
            /* Indicate that request was handled */
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
    
    return(gattErr);
}


/******************************************************************************
##Function Name: CyBle_TpssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets characteristic value of the Tx Power Service, which is identified by 
 charIndex.

Parameters:
 charIndex: The index of the service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be 
              stored in the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The characteristic value was read successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of input parameters failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_TpssSetCharacteristicValue(CYBLE_TPS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize,
                                                    int8 *attrValue)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(charIndex < CYBLE_TPS_CHAR_COUNT)
    {
        if((*attrValue <= CYBLE_TPS_TX_POWER_LEVEL_MAX) && (*attrValue >= CYBLE_TPS_TX_POWER_LEVEL_MIN))
        {
            locHandleValuePair.attrHandle = cyBle_tpss.txPowerLevelCharHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = (uint8 *) attrValue;
            
            /* Set Tx Power Level characteristic value to GATT database.
                Need to handle return type difference of CyBle_GattsWriteAttributeValue() and
                CyBle_TpssSetCharacteristicValue(). */
            if(CYBLE_GATT_ERR_NONE == 
                CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                /* Indicate success */
                apiResult = CYBLE_ERROR_OK;
            }
        }
    }
    
    /* Return status */
    return(apiResult);
}


/******************************************************************************
##Function Name: CyBle_TpssGetCharacteristicValue
*******************************************************************************

Summary:
 Gets characteristic value of the Tx Power Service, which is identified by 
 charIndex.

Parameters:
 charIndex: The index of the Tx Power characteristic.
 attrSize: The size of the Tx Power characteristic value attribute.
 *attrValue: The pointer to the location where Tx Power characteristic
              value data should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - Characteristic value was read successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_TpssGetCharacteristicValue(CYBLE_TPS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize,
                                                    int8 *attrValue)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if((NULL != attrValue) && (CYBLE_TPS_TX_POWER_LEVEL == charIndex) && (CYBLE_TPS_TX_POWER_LEVEL_SIZE == attrSize))
    {
        locHandleValuePair.attrHandle = cyBle_tpss.txPowerLevelCharHandle;
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = (uint8 *)attrValue;
        
        /* Get Tx Power Level characteristic value from GATT database */
        if(CYBLE_GATT_ERR_NONE == 
            CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            /* Indicate success */
            apiResult = CYBLE_ERROR_OK;
        }
    }
    
    /* Return status */
    return(apiResult);
}


/******************************************************************************
##Function Name: CyBle_TpssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets characteristic descriptor of specified characteristic of the Tx Power
 Service.

Parameters:
 charIndex: The index of the characteristic.
 descrIndex: The index of the descriptor.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the location where characteristic descriptor value
              data should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - Characteristic Descriptor value was read successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of input parameters failed
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional descriptor is absent

******************************************************************************/
CYBLE_API_RESULT_T CyBle_TpssGetCharacteristicDescriptor(CYBLE_TPS_CHAR_INDEX_T charIndex,
                                                         CYBLE_TPS_CHAR_DESCRIPTORS_T descrIndex,
                                                         uint8 attrSize,
                                                         uint8 *attrValue)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if((charIndex == CYBLE_TPS_TX_POWER_LEVEL) && (descrIndex == CYBLE_TPS_CCCD))
    {
        if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_tpss.txPowerLevelCccdHandle)
        {
            locHandleValuePair.attrHandle = cyBle_tpss.txPowerLevelCccdHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = (uint8 *) attrValue;
            
            /* Set Client Characteristic Configuration Descriptor of Tx Power Level 
                characteristic to GATT database.
                Need to handle return type difference of CyBle_GattsWriteAttributeValue() and
                CyBle_TpssGetCharacteristicDescriptor(). */
            if(CYBLE_GATT_ERR_NONE == 
                CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                /* Indicate success */
                apiResult = CYBLE_ERROR_OK;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_TpssSendNotification
*******************************************************************************

Summary:
 Sends a notification with the characteristic value, as specified by charIndex,
 to the Client device.

 The CYBLE_EVT_TPSC_NOTIFICATION event is received by the peer device on 
 invoking this function.

Parameters: 
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be 
              sent to the client's device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of input parameter failed.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this.
                                     characteristic.
  * CYBLE_ERROR_INVALID_STATE - Connection with client is not established.
  * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the client.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_TpssSendNotification(CYBLE_CONN_HANDLE_T connHandle, 
                                              CYBLE_TPS_CHAR_INDEX_T charIndex,
                                              uint8 attrSize,
                                              int8 *attrValue)
{
    CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
  
    if((NULL != attrValue) && (CYBLE_TPS_TX_POWER_LEVEL == charIndex) && (CYBLE_TPS_TX_POWER_LEVEL_SIZE == attrSize))
    {
        if(CYBLE_IS_NOTIFICATION_ENABLED(cyBle_tpss.txPowerLevelCccdHandle))
        {
            /* Fill all fields of write request structure ... */
            ntfReqParam.attrHandle = cyBle_tpss.txPowerLevelCharHandle; 
            ntfReqParam.value.val = (uint8 *) attrValue;
            ntfReqParam.value.len = attrSize;

            /* Set Tx Power Level characteristic value to GATT database.
                Need to handle return type difference of CyBle_GattsWriteAttributeValue() and
                CyBle_TpssSetCharacteristicValue(). */
            if(CYBLE_GATT_ERR_NONE == 
                CyBle_GattsWriteAttributeValue(&ntfReqParam, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                if(CYBLE_STATE_CONNECTED != CyBle_GetState())
                {
                    apiResult = CYBLE_ERROR_INVALID_STATE;
                }
                else
                {
                    /* Send notification to client using previously filled structure */
                    apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
                }
            }
        }
        else
        {
           apiResult = CYBLE_ERROR_NTF_DISABLED;
        }
    }

    /* Return status */
    return(apiResult);
}

#endif /* (CYBLE_GATT_ROLE_SERVER) */


#ifdef CYBLE_TPS_CLIENT

/******************************************************************************
##Function Name: CyBle_TpscDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP event.
 Based on the service UUID, an appropriate data structure is populated using the 
 data received as part of the callback.

Parameters: 
 CYBLE_DISC_CHAR_INFO_T * discCharInfo: The pointer to the characteristic 
                                         information structure.

Return: 
 None

******************************************************************************/
void CyBle_TpscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    if(discCharInfo->uuid.uuid16 == CYBLE_UUID_CHAR_TX_POWER_LEVEL)
    {
        /* Check for  invalid char handle */
        if(cyBle_tpsc.txPowerLevelChar.valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            /* Save Tx Power Level Characteristic handle */
            CyBle_CheckStoreCharHandle(cyBle_tpsc.txPowerLevelChar);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_TpscDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event. 
 This event is generated when a server successfully sends the data for 
 CYBLE_EVT_GATTC_FIND_INFO_REQ. Based on the service UUID, an appropriate data 
 structure is populated to the service with a service callback.

Parameters: 
 CYBLE_DISC_DESCR_INFO_T * discDescrInfo: The pointer to descriptor information
                                           structure.

Return: 
 None

******************************************************************************/
void CyBle_TpscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_CLIENT_CONFIG)
    {
        CyBle_CheckStoreCharDescrHandle(cyBle_tpsc.txPowerLevelCccdHandle);
    }
}


/******************************************************************************
##Function Name: CyBle_TpscNotificationEventHandler
*******************************************************************************

Summary:
 Handles the Notification Event for the Tx Power Service.

Parameters:
 CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam: The pointer to the data 
                                                    structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_TpscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    if(NULL != CyBle_TpsApplCallback)
    {
        if(cyBle_tpsc.txPowerLevelChar.valueHandle == eventParam->handleValPair.attrHandle)
        {
            CYBLE_TPS_CHAR_VALUE_T ntfParam;
            
            ntfParam.connHandle = eventParam->connHandle;
            ntfParam.charIndex = CYBLE_TPS_TX_POWER_LEVEL;
            ntfParam.value = &eventParam->handleValPair.value;
            
            CyBle_TpsApplCallback(CYBLE_EVT_TPSC_NOTIFICATION, (void *)&ntfParam);
            cyBle_eventHandlerFlag &= (uint8) (~CYBLE_CALLBACK);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_TpscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles the Read Response Event.

Parameters:
 CYBLE_GATTC_READ_RSP_PARAM_T *eventParam: The pointer to the data that came 
                                            with a read response for the Tx 
                                            Power Service.

Return:
 None

******************************************************************************/
void CyBle_TpscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    if((NULL != CyBle_TpsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_tpscReqHandle))
    {
        if(cyBle_tpsc.txPowerLevelChar.valueHandle == cyBle_tpscReqHandle)
        {
            CYBLE_TPS_CHAR_VALUE_T rdRspParam;
            
            /* Fill Tx Power Service read response parameter structure for Tx Power Level
              Characteristic. */
            rdRspParam.connHandle = eventParam->connHandle;
            rdRspParam.charIndex = CYBLE_TPS_TX_POWER_LEVEL;
            rdRspParam.value = &eventParam->value;
            
            cyBle_tpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            CyBle_TpsApplCallback(CYBLE_EVT_TPSC_READ_CHAR_RESPONSE, &rdRspParam);
        }
        else if(cyBle_tpsc.txPowerLevelCccdHandle == cyBle_tpscReqHandle)
        {
            CYBLE_TPS_DESCR_VALUE_T rdRspParam;
            
            /* Fill Tx Power Service read response parameter structure for Tx Power Level
                Client Characteristic Configuration Descriptor. */
            rdRspParam.connHandle = eventParam->connHandle;
            rdRspParam.charIndex = CYBLE_TPS_TX_POWER_LEVEL;
            rdRspParam.descrIndex = CYBLE_TPS_CCCD;
            rdRspParam.value = &eventParam->value;
            
            cyBle_tpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            CyBle_TpsApplCallback(CYBLE_EVT_TPSC_READ_DESCR_RESPONSE, &rdRspParam);
        }
        else
        {
            /* No TPS characteristic was requested for read */
        }
    }
}


/******************************************************************************
##Function Name: CyBle_TpscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles the Write Response Event.

Parameters:
 CYBLE_CONN_HANDLE_T *eventParam: The pointer to a CYBLE_CONN_HANDLE_T data structure.

Return:
 None

******************************************************************************/
void CyBle_TpscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    CYBLE_TPS_DESCR_VALUE_T wrRspParam;

    if((NULL != CyBle_TpsApplCallback) && (cyBle_tpsc.txPowerLevelCccdHandle == cyBle_tpscReqHandle) && 
        (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_tpscReqHandle))
    {
        wrRspParam.connHandle = *eventParam;
        wrRspParam.charIndex = CYBLE_TPS_TX_POWER_LEVEL;
        wrRspParam.descrIndex = CYBLE_TPS_CCCD;
        wrRspParam.value = NULL;
        
        cyBle_tpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        CyBle_TpsApplCallback(CYBLE_EVT_TPSC_WRITE_DESCR_RESPONSE, &wrRspParam);
    }
}


/******************************************************************************
##Function Name: CyBle_TpscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles the Error Response Event for the Tx Power Service.

Parameters:
 CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam: The pointer to the 
                                           CYBLE_GATTC_ERR_RSP_PARAM_T structure.

Return:
 None

******************************************************************************/
void CyBle_TpscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if(eventParam->attrHandle != cyBle_tpscReqHandle)
    {
        cyBle_tpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


/******************************************************************************
##Function Name: CyBle_TpscGetCharacteristicValue
*******************************************************************************

Summary:
 Gets the characteristic value of the Tx Power Service, which is 
 identified by charIndex.

 This function call can result in generation of the following events based on
 the response from the server device:
 * CYBLE_EVT_TPSC_READ_CHAR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the characteristic.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - Request was sent successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
  * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                    characteristic

******************************************************************************/
CYBLE_API_RESULT_T CyBle_TpscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                    CYBLE_TPS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    if((CYBLE_TPS_TX_POWER_LEVEL == charIndex))
    {
        /* Send request to write Tx Power Level characteristic value */
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_tpsc.txPowerLevelChar.valueHandle);
        
        if(CYBLE_ERROR_OK == apiResult)
        {
             cyBle_tpscReqHandle = cyBle_tpsc.txPowerLevelChar.valueHandle;
        }
    }
    else
    {
        /* apiResult equals CYBLE_ERROR_INVALID_PARAMETER */
    }
    
    /* Return status */
    return(apiResult);
}


/******************************************************************************
##Function Name: CyBle_TpscSetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sets a characteristic descriptor value of the Tx Power Service.

 This function call can result in generation of the following events based 
 on the response from the server device:
 * CYBLE_EVT_TPSC_WRITE_DESCR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

 Following events can be received by the peer device, on invoking this function:
 * CYBLE_EVT_TPSS_NOTIFICATION_ENABLED
 * CYBLE_EVT_TPSS_NOTIFICATION_DISABLED

Parameters:
 connHandle: The connection handle.
 charIndex:  The index of the Characteristic
 descrIndex: The index of the TX Power Service characteristic descriptor.
 attrSize:   The size of the characteristic descriptor attribute.
 *attrValue: The pointer to the characteristic descriptor value data that 
             should be sent to the server device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on the
                                     specified attribute.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_TpscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                         CYBLE_TPS_CHAR_INDEX_T charIndex,
                                                         CYBLE_TPS_CHAR_DESCRIPTORS_T descrIndex,
                                                         uint8 attrSize,
                                                         uint8 *attrValue)
{
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    if((NULL != attrValue) && (CYBLE_TPS_TX_POWER_LEVEL == charIndex) && (CYBLE_TPS_CCCD == descrIndex) && 
        (CYBLE_CCCD_LEN == attrSize))
    {
        /* Fill all fields of write request structure ... */
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;
        writeReqParam.attrHandle = cyBle_tpsc.txPowerLevelCccdHandle;
        
        /* ... and send request to server device. */        
        apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
        
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_tpscReqHandle = cyBle_tpsc.txPowerLevelCccdHandle;
        }
    }
    else
    {
        /* apiResult equals CYBLE_ERROR_INVALID_PARAMETER */
    }

    /* Return status */
    return(apiResult);
}


/******************************************************************************
##Function Name: TpscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets a characteristic descriptor of the Tx Power Service. 
 
 This function call can result in generation of the following events based on 
 the response from the server device:
 * CYBLE_EVT_TPSC_READ_DESCR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the characteristic.
 descrIndex: The index of the characteristic descriptor.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - Request was sent successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_INVALID_STATE - The component in in invalid state for current 
                                operation.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_OPERATION - Cannot process request to send PDU due to
                                    invalid operation performed by the
                                    application.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_TpscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                         CYBLE_TPS_CHAR_INDEX_T charIndex,
                                                         CYBLE_TPS_CHAR_DESCRIPTORS_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex == CYBLE_TPS_TX_POWER_LEVEL) && (descrIndex == CYBLE_TPS_CCCD))
    {
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, cyBle_tpsc.txPowerLevelCccdHandle);
        
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_tpscReqHandle = cyBle_tpsc.txPowerLevelCccdHandle;
        }
    }
    else
    {
        /* apiResult equals CYBLE_ERROR_INVALID_PARAMETER */
    }
    
    /* Return status */
    return(apiResult);
}

#endif /* CYBLE_TPS_CLIENT */


/* [] END OF FILE */
