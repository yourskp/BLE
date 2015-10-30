/*******************************************************************************
File Name: CYBLE_ias.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the source code for the Immediate Alert Service of the BLE
 Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"


#ifdef CYBLE_IAS_SERVER

static CYBLE_CALLBACK_T CyBle_IasApplCallback = NULL;

#endif /* CYBLE_IAS_SERVER */

#ifdef CYBLE_IAS_SERVER

`$IasServer`

#endif /* (CYBLE_IAS_SERVER) */

#ifdef CYBLE_IAS_CLIENT

CYBLE_IASC_T cyBle_iasc;

#endif /* (CYBLE_IAS_CLIENT) */


/****************************************************************************** 
##Function Name: CyBle_IasInit
*******************************************************************************

Summary:
 This function initializes the Immediate Alert Service.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_IasInit(void)
{
#ifdef CYBLE_IAS_CLIENT

    cyBle_iasc.alertLevelChar.valueHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* (CYBLE_IAS_CLIENT) */
}


#ifdef CYBLE_IAS_SERVER

/****************************************************************************** 
##Function Name: CyBle_IasRegisterAttrCallback
*******************************************************************************

Summary:
 Registers callback function for service specific attribute operations.

Parameters:
 callbackFunc: An application layer event callback function to receive 
                    events from the BLE Component. The definition of 
                    CYBLE_CALLBACK_T for IAS Service is,
            
                    typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                       void *eventParam)

                    * eventCode indicates the event that triggered this 
                      callback (e.g. CYBLE_EVT_IASS_NOTIFICATION_ENABLED).
                    * eventParam contains the parameters corresponding to the 
                      current event. (e.g. pointer to CYBLE_IAS_CHAR_VALUE_T
                      structure that contains details of the characteristic 
                      for which notification enabled event was triggered).
Return:
 None

Side Effects:
 The *eventParams in the callback function should not be used by the
 application once the callback function execution is finished. Otherwise 
 this data may become corrupted.
Note:
 IAS only has events for the GATT server. There are no events for the GATT 
 client since the client sends data without waiting for response. Therefore 
 there is no need to register a callback through CyBle_IasRegisterAttrCallback
 for an IAS GATT client.

******************************************************************************/
void CyBle_IasRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_IasApplCallback = callbackFunc;
}


/****************************************************************************** 
##Function Name: CyBle_IassDisconnectEventHandler
*******************************************************************************

Summary:
 Handles the Disconnection Indication Event for the Immediate Alert service.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_IassDisconnectEventHandler(void)
{
    uint8 tmpAlertLevel = CYBLE_NO_ALERT;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T handleVal;
    
    handleVal.attrHandle = cyBle_iass.alertLevelCharHandle;
    handleVal.value.val = &tmpAlertLevel;
    handleVal.value.len = CYBLE_IAS_ALERT_LEVEL_SIZE;
    
    /* Set alert level to "No Alert" per IAS spec */
    (void) CyBle_GattsWriteAttributeValue(&handleVal, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED);
}


/****************************************************************************** 
##Function Name: CyBle_IassWriteCmdEventHandler
*******************************************************************************

Summary:
 Handles the Write Without Response Request Event for the Immediate Alert 
 Service.

Parameters:
 CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T * eventParam: The pointer to a data structure
                                                   specified by the event.

Return:
 None

******************************************************************************/
void CyBle_IassWriteCmdEventHandler(CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *eventParam)
{
    CYBLE_IAS_CHAR_VALUE_T wrCmdParam;
    
    if(NULL != CyBle_IasApplCallback)
    {
        /* Check if attribute handle is handle of Alert Level characteristic of
           Immediate Alert service. */
        if((cyBle_iass.alertLevelCharHandle == eventParam->handleValPair.attrHandle) &&
            (CYBLE_IAS_ALERT_LEVEL_SIZE == eventParam->handleValPair.value.len) &&
                (CYBLE_HIGH_ALERT >= eventParam->handleValPair.value.val[0u]))
        {   
            /* Input parameters validation passed, so save Alert Level */
            if(CYBLE_GATT_ERR_NONE ==
                CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 
                                               0u, 
                                               &eventParam->connHandle,
                                               CYBLE_GATT_DB_PEER_INITIATED))
            {
                wrCmdParam.connHandle = eventParam->connHandle;
                wrCmdParam.charIndex = CYBLE_IAS_ALERT_LEVEL;
                wrCmdParam.value = &eventParam->handleValPair.value;
                
                /* Send callback to user if no error occurred while writing Alert Level */
                CyBle_IasApplCallback((uint32) CYBLE_EVT_IASS_WRITE_CHAR_CMD, (void *) &wrCmdParam);
                cyBle_eventHandlerFlag &= (uint8) (~CYBLE_CALLBACK);
            }
        }
        /* As this handler handles Write Without Response request the error response
          can't be sent for the Client. The erroneous value will be with
          CYBLE_EVT_GATTS_WRITE_CMD_REQ event. User will decide how to handle it. */
    }
}


/****************************************************************************** 
##Function Name: CyBle_IassGetCharacteristicValue
*******************************************************************************

Summary:
 Gets the Alert Level characteristic value of the service, which is identified
 by charIndex.

Parameters:
 charIndex: The index of the Alert Level characteristic.
 attrSize: The size of the Alert Level characteristic value attribute.
 *attrValue: The pointer to the location where the Alert Level characteristic
              value data should b stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The characteristic value was read successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_IassGetCharacteristicValue(CYBLE_IAS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T iasHandleValuePair;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if((NULL != attrValue) && (CYBLE_IAS_ALERT_LEVEL == charIndex) && (CYBLE_IAS_ALERT_LEVEL_SIZE == attrSize))
    {
        iasHandleValuePair.attrHandle = cyBle_iass.alertLevelCharHandle;
        iasHandleValuePair.value.len = attrSize;
        iasHandleValuePair.value.val = attrValue;
        
        /* Get Alert Level characteristic value from GATT database */
        if(CYBLE_GATT_ERR_NONE == 
            CyBle_GattsReadAttributeValue(&iasHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            /* Indicate success */
            apiResult = CYBLE_ERROR_OK;
        }
    }

    /* Return status */
    return(apiResult);
}

#endif /* (CYBLE_IAS_SERVER) */


#ifdef CYBLE_IAS_CLIENT

/****************************************************************************** 
##Function Name: CyBle_IascDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP event.
 Based on the service UUID, an appropriate data structure is populated using 
 the data received as part of the callback.

Parameters:
 CYBLE_DISC_CHAR_INFO_T * discCharInfo: The pointer to a characteristic 
                                         information structure.

Return:
 None

******************************************************************************/
void CyBle_IascDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    if(discCharInfo->uuid.uuid16 == CYBLE_UUID_CHAR_ALERT_LEVEL)
    {
        /* Check for invalid char handle */
        if(cyBle_iasc.alertLevelChar.valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            /* Save Alert Level Characteristic handle */
            CyBle_CheckStoreCharHandle(cyBle_iasc.alertLevelChar);
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_IascSetCharacteristicValue
*******************************************************************************

Summary:
 Sets a Alert Level characteristic value of the service, which is identified
 by charIndex.

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
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
  * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                    characteristic

******************************************************************************/
CYBLE_API_RESULT_T CyBle_IascSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                    CYBLE_IAS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize,
                                                    uint8 *attrValue)
{
    CYBLE_GATTC_WRITE_CMD_REQ_T wrCmdReq;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((NULL != attrValue) && (CYBLE_IAS_ALERT_LEVEL == charIndex) && (CYBLE_IAS_ALERT_LEVEL_SIZE == attrSize) &&
            (*attrValue <= CYBLE_HIGH_ALERT) &&
            (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_iasc.alertLevelChar.valueHandle))
    {
        /* Fill all fields of write command request structure ... */
        wrCmdReq.value.val = attrValue;
        wrCmdReq.value.len = attrSize;
        wrCmdReq.attrHandle = cyBle_iasc.alertLevelChar.valueHandle;

        /* Send request to write Alert Level characteristic value */
        apiResult = CyBle_GattcWriteWithoutResponse(connHandle, &wrCmdReq);
    }
    else
    {
        /* apiResult equals CYBLE_ERROR_INVALID_PARAMETER */
    }

    /* Return status */
    return(apiResult);
}

#endif /* (CYBLE_IAS_CLIENT) */


/* [] END OF FILE */
