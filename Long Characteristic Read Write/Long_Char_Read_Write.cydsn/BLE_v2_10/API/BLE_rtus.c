/*******************************************************************************
File Name: CYBLE_rtus.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the source code for Reference Time Update Service.

********************************************************************************

Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_RtusApplCallback = NULL;

#if defined(CYBLE_RTUS_SERVER)

`$RtusServer`
    
#endif /* CYBLE_RTUS_SERVER */

#if defined(CYBLE_RTUS_CLIENT)

CYBLE_RTUSC_T cyBle_rtusc;
    
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_rtuscReqHandle;

#endif /* (CYBLE_RTUS_CLIENT) */


/* DOM-IGNORE-BEGIN */
/*******************************************************************************
##Function Name: CyBle_RtusInit
********************************************************************************

Summary:
 This function initializes the Reference Time Update Service.

Parameters:
 None

Return:
 None

*******************************************************************************/
void CyBle_RtusInit(void)
{
#ifdef CYBLE_RTUS_CLIENT

    (void)memset(&cyBle_rtusc, 0, sizeof(cyBle_rtusc));
    
    cyBle_rtuscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_RTUS_CLIENT */
/* DOM-IGNORE-END */
}


/*******************************************************************************
##Function Name: CyBle_RtusRegisterAttrCallback
********************************************************************************

Summary:
 Registers a callback function for Reference Time Update Service specific
 attribute operations.

Parameters:
 callbackFunc:  An application layer event callback function to receive 
                    events from the BLE Component. The definition of 
                    CYBLE_CALLBACK_T for RTUS is,
            
                    typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                      void *eventParam)

                    * eventCode indicates the event that triggered this 
                      callback.
                    * eventParam contains the parameters corresponding to the 
                      current event.

Return:
 None.

*******************************************************************************/
void CyBle_RtusRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_RtusApplCallback = callbackFunc;
}


#if defined(CYBLE_RTUS_SERVER)

/*******************************************************************************
##Function Name: CyBle_RtussWriteEventHandler
********************************************************************************

Summary:
 Handles the Write Without Response Request Event for the Reference Time Update
 Service.

Parameters:
 CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T * eventParam: The pointer to a data structure
                                                 specified by the event.

Return:
 None.

*******************************************************************************/
void CyBle_RtussWriteCmdEventHandler(CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *eventParam)
{
    CYBLE_RTUS_CHAR_VALUE_T wrCmdParam;
    
    if(NULL != CyBle_RtusApplCallback)
    {
        /* Check if attribute handle is handle of Time Update Control Point
           Characteristic of Reference Time Update Service. */
        if((cyBle_rtuss.timeUpdateCpHandle == eventParam->handleValPair.attrHandle) &&
            (CYBLE_RTUS_TIME_UPDATE_CP_SIZE == eventParam->handleValPair.value.len) &&
                (CYBLE_RTUS_CANCEL_REF_UPDATE >= eventParam->handleValPair.value.val[0u]))
        {   
            /* Input parameters validation passed, so save Time Update Control Point value */
            if(CYBLE_GATT_ERR_NONE ==
                CyBle_GattsWriteAttributeValue(&eventParam->handleValPair,
                                               0u,
                                               &eventParam->connHandle,
                                               CYBLE_GATT_DB_PEER_INITIATED))
            {
                wrCmdParam.connHandle = eventParam->connHandle;
                wrCmdParam.charIndex = CYBLE_RTUS_TIME_UPDATE_CONTROL_POINT;
                wrCmdParam.value = &eventParam->handleValPair.value;
                
                /* Send callback to user if no error occurred while writing Time Update Control
                * Point.
                */
                CyBle_RtusApplCallback((uint32) CYBLE_EVT_RTUSS_WRITE_CHAR_CMD, (void *) &wrCmdParam);
                cyBle_eventHandlerFlag &= (uint8) (~CYBLE_CALLBACK);
            }
        }
        /* As this handler handles Write Without Response request the error response
          can't be sent for the Client. The erroneous value will be with
          CYBLE_EVT_GATTS_WRITE_CMD_REQ event. User will decide how to handle it. */
    }
}


/*******************************************************************************
##Function Name: CyBle_RtussSetCharacteristicValue
********************************************************************************

Summary:
 Sets characteristic value of the Reference Time Update Service, which is
 identified by charIndex in the local database.

Parameters:
 CYBLE_RTUS_CHAR_INDEX_T charIndex: The index of a service characteristic of
                                    type CYBLE_RTUS_CHAR_INDEX_T.
 uint8 attrSize:                    The size of the characteristic value 
                                    attribute.
 uint8 *attrValue:                  The pointer to the characteristic value
                                    data that should be stored to the GATT
                                    database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - the request is handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - validation of the input parameters failed

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RtussSetCharacteristicValue(CYBLE_RTUS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((CYBLE_RTUS_TIME_UPDATE_STATE == charIndex) && (*attrValue <= CYBLE_RTUS_RESULT_NO_UPDATE_ATTEMPT))
    {
        /* Store data in database */
        locHandleValuePair.attrHandle = cyBle_rtuss.timeUpdateStateHandle;
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        
        if(CYBLE_GATT_ERR_NONE ==
            CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_OK;
        }
    }

    return (apiResult);
}


/*******************************************************************************
##Function Name: CyBle_RtussGetCharacteristicValue
********************************************************************************

Summary:
 Gets a characteristic value of the Reference Time Update Service, which
 is identified by charIndex.

Parameters:
 CYBLE_RTUS_CHAR_INDEX_T charIndex: The index of a service characteristic of
                                    type CYBLE_RTUS_CHAR_INDEX_T. Valid value
                                    is CYBLE_RTUS_SC_CONTROL_POINT.

 uint8 attrSize:                    The size of the characteristic value 
                                    attribute.

 uint8 *attrValue:                  The pointer to the location where 
                                    characteristic value data should be
                                    stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - the request is handled successfully;
  * CYBLE_ERROR_INVALID_PARAMETER - validation of the input parameter failed.

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RtussGetCharacteristicValue(CYBLE_RTUS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(CYBLE_RTUS_TIME_UPDATE_CONTROL_POINT == charIndex)
    {
        /* Read characteristic value from database */
        locHandleValuePair.attrHandle = cyBle_rtuss.timeUpdateCpHandle;
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;

        if(CYBLE_GATT_ERR_NONE !=
            CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_OK;
        }
    }
    
    return (apiResult);
}

#endif /* (CYBLE_RTUS_SERVER) */


#if defined(CYBLE_RTUS_CLIENT)

/*******************************************************************************
##Function Name: CyBle_RtuscDiscoverCharacteristicsEventHandler
********************************************************************************

Summary:
 This function is called on receiving a "CYBLE_EVT_GATTC_READ_BY_TYPE_RSP"
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 CYBLE_DISC_CHAR_INFO_T *discCharInfo: The pointer to a characteristic
                                       information structure.
 

Return:
 None

*******************************************************************************/
void CyBle_RtuscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    uint8 locCharIndex = (uint8) CYBLE_RTUS_INVALID_CHAR_INDEX;

    /* Using characteristic UUID store handle of requested characteristic */
    switch(discCharInfo->uuid.uuid16)
    {
    case CYBLE_UUID_CHAR_TIME_UPDATE_CONTROL_POINT:
        locCharIndex = (uint8) CYBLE_RTUS_TIME_UPDATE_CONTROL_POINT;
        break;

    case CYBLE_UUID_CHAR_TIME_UPDATE_STATE:
        locCharIndex = (uint8) CYBLE_RTUS_TIME_UPDATE_STATE;
        break;

    default:
        break;
    }

    if(((uint8) CYBLE_RTUS_CHAR_COUNT) > locCharIndex)
    {
        if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_rtusc.charInfo[locCharIndex].valueHandle)
        {
            cyBle_rtusc.charInfo[locCharIndex].valueHandle = discCharInfo->valueHandle;
            cyBle_rtusc.charInfo[locCharIndex].properties  = discCharInfo->properties;
        }
        else
        {
            CyBle_ApplCallback(CYBLE_EVT_GATTC_CHAR_DUPLICATION, &discCharInfo->uuid);
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_RtuscReadResponseEventHandler
********************************************************************************

Summary:
 Handles Read Response Event for Reference Time Update Service.

Parameters:
 CYBLE_GATTC_READ_RSP_PARAM_T *eventParam: The pointer to the data that came 
                                           with a read response for RTUS.

Return:
 None.

*/
void CyBle_RtuscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    CYBLE_RTUS_CHAR_VALUE_T rdRspParam;
    
    if((NULL != CyBle_RtusApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_rtuscReqHandle))
    {
        if(cyBle_rtusc.charInfo[CYBLE_RTUS_TIME_UPDATE_STATE].valueHandle == cyBle_rtuscReqHandle)
        {
            /* Fill Reference Time Update Service read response parameter structure with
            * characteristic info.
            */
            rdRspParam.connHandle = eventParam->connHandle;
            rdRspParam.charIndex = CYBLE_RTUS_TIME_UPDATE_STATE;
            rdRspParam.value = &eventParam->value;

            CyBle_RtusApplCallback((uint32)CYBLE_EVT_RTUSC_READ_CHAR_RESPONSE, (void *)&rdRspParam);
                
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            cyBle_rtuscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_RtuscSetCharacteristicValue
********************************************************************************

Summary:
 Sends a request to peer device to get characteristic descriptor of specified 
 characteristic of the Reference Time Update Service.

Parameters:
 CYBLE_CONN_HANDLE_T connHandle:    The connection handle.
 CYBLE_RTUS_CHAR_INDEX_T charIndex: The index of a service characteristic.
 uint8 attrSize:                    Size of the characteristic value attribute.
 uint8 *attrValue:                  Pointer to the characteristic value data 
                                    that should be sent to the server device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully.
  * CYBLE_ERROR_INVALID_STATE - Connection with the Client is not established.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                     characteristic.

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RtuscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_RTUS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_GATTC_WRITE_CMD_REQ_T wrReqParam;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((NULL != attrValue) && (CYBLE_RTUS_TIME_UPDATE_CONTROL_POINT == charIndex) &&
        (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=  
            cyBle_rtusc.charInfo[CYBLE_RTUS_TIME_UPDATE_CONTROL_POINT].valueHandle))
    {
        /* Fill all fields of write command request structure ... */
        wrReqParam.value.val = attrValue;
        wrReqParam.value.len = attrSize;
        wrReqParam.attrHandle = cyBle_rtusc.charInfo[CYBLE_RTUS_TIME_UPDATE_CONTROL_POINT].valueHandle;

        /* Send request to write characteristic value */
        apiResult = CyBle_GattcWriteWithoutResponse(connHandle, &wrReqParam);
    }
    else
    {
        /* apiResult equals to CYBLE_ERROR_INVALID_PARAMETER */
    }

    return(apiResult);
}


/*******************************************************************************
##Function Name: CyBle_RtuscGetCharacteristicValue
********************************************************************************

Summary:
 Sends a request to a peer device to set characteristic value of the Reference 
 Time Update Service, which is identified by charIndex.

Parameters:
 CYBLE_CONN_HANDLE_T connHandle:    the connection handle.
 CYBLE_RTUS_CHAR_INDEX_T charIndex: the index of a service characteristic.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - the request was sent successfully;
  * CYBLE_ERROR_INVALID_STATE - connection with the Client is not established.
  * CYBLE_ERROR_INVALID_PARAMETER - validation of the input parameters failed.

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RtuscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_RTUS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else
    {
        if(CYBLE_RTUS_TIME_UPDATE_STATE == charIndex)
        {
            /* Send request to read characteristic value */
            apiResult = CyBle_GattcReadCharacteristicValue(connHandle,
                cyBle_rtusc.charInfo[CYBLE_RTUS_TIME_UPDATE_STATE].valueHandle);
        
            if(CYBLE_ERROR_OK == apiResult)
            {
                cyBle_rtuscReqHandle = cyBle_rtusc.charInfo[CYBLE_RTUS_TIME_UPDATE_STATE].valueHandle;
            }
        }
    }

    return(apiResult);
}


#endif /* CYBLE_RTUS_CLIENT */


/* [] END OF FILE */
