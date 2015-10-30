/*******************************************************************************
File Name: CYBLE_lls.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the source code for the Link Loss Service of the BLE
 Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_eventHandler.h"


static CYBLE_CALLBACK_T CyBle_LlsApplCallback = NULL;

#ifdef CYBLE_LLS_SERVER
    
`$LlsServer`

#endif /* (CYBLE_LLS_SERVER) */

#ifdef CYBLE_LLS_CLIENT
    
CYBLE_LLSC_T cyBle_llsc;

/* Internal storage for last request handle to check response */
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_llscReqHandle;

#endif /* (CYBLE_LLS_CLIENT) */

/****************************************************************************** 
##Function Name: CyBle_LlsInit
*******************************************************************************

Summary:
 This function initializes the Link Loss Service.

Parameters: 
 None

Return:
 None

******************************************************************************/
void CyBle_LlsInit(void)
{

#ifdef CYBLE_LLS_CLIENT

    cyBle_llsc.alertLevelChar.valueHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    cyBle_llscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    
#endif /* (CYBLE_LLS_CLIENT) */
}


/****************************************************************************** 
##Function Name: CyBle_LlsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc: An application layer event callback function to receive 
                   events from the BLE Component. The definition of 
                   CYBLE_CALLBACK_T for Link Loss Service is,
            
                   typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                      void *eventParam)

                   * eventCode indicates the event that triggered this 
                     callback (e.g. CYBLE_EVT_LLSS_NOTIFICATION_ENABLED).
                   * eventParam contains the parameters corresponding to the 
                     current event. (e.g. pointer to CYBLE_LLS_CHAR_VALUE_T
                     structure that contains details of the characteristic 
                     for which notification enabled event was triggered).
Return:
 None

Side Effects:
 The *eventParams in the callback function should not be used by the 
 application once the callback function execution is finished. Otherwise this
 data may become corrupted.

******************************************************************************/
void CyBle_LlsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_LlsApplCallback = callbackFunc;
}


#ifdef CYBLE_LLS_SERVER

/****************************************************************************** 
##Function Name: CyBle_LlssConnectEventHandler
*******************************************************************************

Summary:
 Handles the Connection Indication event for Link Loss Service. 

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_LlssConnectEventHandler(void)
{
    uint8 tmpAlertLevel = CYBLE_NO_ALERT;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T llsHandleValuePair;
    
    llsHandleValuePair.attrHandle = cyBle_llss.alertLevelCharHandle;
    llsHandleValuePair.value.len = CYBLE_LLS_ALERT_LEVEL_SIZE;
    llsHandleValuePair.value.val = &tmpAlertLevel;
    
    /* Input parameters validation passed, so save Alert Level */
    (void) CyBle_GattsWriteAttributeValue(&llsHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED);
}


/****************************************************************************** 
##Function Name: CyBle_LlssWriteEventHandler
*******************************************************************************

Summary:
 Handles the Write Request Event for the Link Loss service.

Parameters:
 CYBLE_GATTS_WRITE_REQ_PARAM_T * eventParam:  the pointer to the data structure
                                              specified by the event.

Return:
 CYBLE_GATT_ERR_CODE_T: API result returns one of the following status values:
  * CYBLE_GATT_ERR_NONE - The Alert Level Characteristic was written successfully
  * CYBLE_GATT_ERR_UNLIKELY_ERROR - An Internal Stack error occurred
  * CYBLE_GATT_ERR_INVALID_ATTRIBUTE_LEN - The Data length for the Alert Level
                                            Characteristic handle is invalid
  * CYBLE_GATT_ERR_INVALID_HANDLE - Alert Level Characteristic handle is invalid
  * CYBLE_GATT_ERR_OUT_OF_RANGE - Value is out of allowed range

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_LlssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_LLS_CHAR_VALUE_T wrReqParam;
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;

    if(NULL != CyBle_LlsApplCallback)
    {
        if(cyBle_llss.alertLevelCharHandle == eventParam->handleValPair.attrHandle)
        {
            /* Check if attribute handle is handle of Alert Level characteristic of 
               Link Loss service. */
            if ((CYBLE_HIGH_ALERT >= eventParam->handleValPair.value.val[0u]))
            {
                if(CYBLE_LLS_ALERT_LEVEL_SIZE == eventParam->handleValPair.value.len)
                {
                    /* Input parameters validation passed, so save Alert Level */
                    gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                                &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                    
                    /* Send callback to user if no error occurred while writing Alert Level */
                    if(CYBLE_GATT_ERR_NONE == gattErr)
                    {
                        wrReqParam.connHandle = eventParam->connHandle;
                        wrReqParam.charIndex = CYBLE_LLS_ALERT_LEVEL;
                        wrReqParam.value = &eventParam->handleValPair.value;
                        
                        CyBle_LlsApplCallback((uint32) CYBLE_EVT_LLSS_WRITE_CHAR_REQ, (void *) &wrReqParam);
                    }
                }
                else
                {
                    gattErr = CYBLE_GATT_ERR_INVALID_ATTRIBUTE_LEN;
                }
            }
            else
            {
                gattErr = CYBLE_GATT_ERR_OUT_OF_RANGE;
            }
            
            /* Indicate that request was handled */
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
        
    return(gattErr);
}


/****************************************************************************** 
##Function Name: CyBle_LlssGetCharacteristicValue
*******************************************************************************

Summary:
 Gets an Alert Level characteristic value of the service, which is identified
 by charIndex.

Parameters:
 charIndex: The index of an Alert Level characteristic.
 attrSize: The size of the Alert Level characteristic value attribute.
 *attrValue: The pointer to the location where an Alert Level characteristic
              value data should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The characteristic value was read successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_LlssGetCharacteristicValue(CYBLE_LLS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T llsHandleValuePair;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if((NULL != attrValue) && (CYBLE_LLS_ALERT_LEVEL == charIndex) && (CYBLE_LLS_ALERT_LEVEL_SIZE == attrSize))
    {
        llsHandleValuePair.attrHandle = cyBle_llss.alertLevelCharHandle;
        llsHandleValuePair.value.len = attrSize;
        llsHandleValuePair.value.val = attrValue;
        
        /* Get Alert Level characteristic value from GATT database */
        if(CYBLE_GATT_ERR_NONE == 
            CyBle_GattsReadAttributeValue(&llsHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            /* Indicate success */
            apiResult = CYBLE_ERROR_OK;
        }
    }
    
    /* Return status */
    return(apiResult);
}

#endif /* (CYBLE_LLS_SERVER) */


#ifdef CYBLE_LLS_CLIENT

/****************************************************************************** 
##Function Name: CyBle_LlscDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters: 
 CYBLE_DISC_CHAR_INFO_T * discCharInfo: The pointer to a characteristic 
                                         information structure.

Return: 
 None

******************************************************************************/
void CyBle_LlscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    if(discCharInfo->uuid.uuid16 == CYBLE_UUID_CHAR_ALERT_LEVEL)
    {
        /* Check for invalid char handle */
        if(cyBle_llsc.alertLevelChar.valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            /* Save Alert Level Characteristic handle */
            CyBle_CheckStoreCharHandle(cyBle_llsc.alertLevelChar);
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_LlscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles the Read Response Event.

Parameters:
 CYBLE_GATTC_READ_RSP_PARAM_T *eventParam: The pointer to the data structure.

Return:
 None

******************************************************************************/
void CyBle_LlscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    CYBLE_LLS_CHAR_VALUE_T rdRspParam;
    
    if((NULL != CyBle_LlsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_llscReqHandle))
    {
        /* Fill Link Loss Service read response parameter structure */
        rdRspParam.connHandle = eventParam->connHandle;
        rdRspParam.charIndex = CYBLE_LLS_ALERT_LEVEL;
        rdRspParam.value = &eventParam->value;
        
        cyBle_llscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        CyBle_LlsApplCallback((uint32) CYBLE_EVT_LLSC_READ_CHAR_RESPONSE, &rdRspParam);
    }
}


/****************************************************************************** 
##Function Name: CyBle_LlscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles the Write Response Event for the Link Loss Service.

Parameters:
 CYBLE_CONN_HANDLE_T *eventParam: The pointer to the connection handle.

Return:
 None

******************************************************************************/
void CyBle_LlscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    CYBLE_LLS_CHAR_VALUE_T wrRspParam;
    
    if((NULL != CyBle_LlsApplCallback) && (cyBle_llsc.alertLevelChar.valueHandle == cyBle_llscReqHandle) &&
        (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_llscReqHandle))
    {
        wrRspParam.connHandle = *eventParam;
        wrRspParam.charIndex = CYBLE_LLS_ALERT_LEVEL;
        wrRspParam.value = NULL;
        
        cyBle_llscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        CyBle_LlsApplCallback((uint32) CYBLE_EVT_LLSC_WRITE_CHAR_RESPONSE, (void *)&wrRspParam);
    }
}


/****************************************************************************** 
##Function Name: CyBle_LlscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles the Error Response Event for the Link Loss Service.

Parameters:
 CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam: Pointer to the 
                                           CYBLE_GATTC_ERR_RSP_PARAM_T structure.

Return:
 None

******************************************************************************/
void CyBle_LlscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_llscReqHandle)
    {
        cyBle_llscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


/****************************************************************************** 
##Function Name: CyBle_LlscSetCharacteristicValue
*******************************************************************************

Summary:
 Sets the Alert Level characteristic value of the Link Loss Service, which is
 identified by charIndex. 
 
 This function call can result in generation of the following events based on
 the response from the server device.
 * CYBLE_EVT_LLSC_WRITE_CHAR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the Alert Level service characteristic.
 attrSize: The size of the Alert Level characteristic value attribute.
 *attrValue: The pointer to the Alert Level characteristic value data that
              should be sent to the server device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_LlscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                    CYBLE_LLS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize,
                                                    uint8 *attrValue)
{
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if((NULL != attrValue) && (CYBLE_LLS_ALERT_LEVEL == charIndex) && (CYBLE_LLS_ALERT_LEVEL_SIZE == attrSize) && 
       ((*attrValue <= CYBLE_HIGH_ALERT)) &&  
       (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_llsc.alertLevelChar.valueHandle))
    {
        cyBle_llscReqHandle = cyBle_llsc.alertLevelChar.valueHandle;
        
        /* Fill all fields of write command request structure ... */
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;
        writeReqParam.attrHandle = cyBle_llsc.alertLevelChar.valueHandle;
        
        /* Send request to write Alert Level characteristic value */
        apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &writeReqParam);
    }
    
    /* Return status */
    return(apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_LlscGetCharacteristicValue
*******************************************************************************

Summary:
 Sends a request to get characteristic value of the Link Loss Service, which 
 is identified by charIndex. 
 
 This function call can result in generation of the following events based on 
 the response from the server device:
 * CYBLE_EVT_LLSC_READ_CHAR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the Link Loss Service characteristic.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                     characteristic

******************************************************************************/
CYBLE_API_RESULT_T CyBle_LlscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_LLS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((CYBLE_LLS_ALERT_LEVEL == charIndex))
    {
        cyBle_llscReqHandle = cyBle_llsc.alertLevelChar.valueHandle;
        
        /* Send request to write Alert Level characteristic value */
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_llsc.alertLevelChar.valueHandle);
    }
    else
    {
        /* apiResult equals CYBLE_ERROR_INVALID_PARAMETER */
    }
    
    /* Return status */
    return(apiResult);
}

#endif /* (CYBLE_LLS_CLIENT) */


/* [] END OF FILE */
