/*******************************************************************************
File Name: CYBLE_scps.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the the source code for the Scan Parameter service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_ScpsApplCallback = NULL;

#ifdef CYBLE_SCPS_SERVER

`$ScpsServer`

#endif /* CYBLE_SCPS_SERVER */

#ifdef CYBLE_SCPS_CLIENT

CYBLE_SCPSC_T cyBle_scpsc;
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_scpscReqHandle;

#endif /* CYBLE_SCPS_SERVER */

/******************************************************************************
##Function Name: CyBle_ScpsInit
*******************************************************************************

Summary:
 This function initializes the SCPS Service.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_ScpsInit(void)
{

#ifdef CYBLE_SCPS_CLIENT
    (void)memset(&cyBle_scpsc, 0, sizeof(cyBle_scpsc));
    cyBle_scpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
#endif /* CYBLE_SCPS_CLIENT */
}


/******************************************************************************
##Function Name: CyBle_ScpsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc:  An application layer event callback function to receive 
                    events from the BLE Component. The definition of 
                    CYBLE_CALLBACK_T for ScPS is,
            
                    typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                      void *eventParam)

                    * eventCode indicates the event that triggered this 
                      callback.
                    * eventParam contains the parameters corresponding to the 
                      current event.

Return:
 None

******************************************************************************/
void CyBle_ScpsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_ScpsApplCallback = callbackFunc;
}

#ifdef CYBLE_SCPS_SERVER


/******************************************************************************
##Function Name: CyBle_ScpssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets a characteristic value of the Scan Parameters service, which is 
 identified by charIndex.

Parameters:
 charIndex: The index of the service characteristic.
             * CYBLE_SCPS_SCAN_INT_WIN - The Scan Interval Window characteristic index
             * CYBLE_SCPS_SCAN_REFRESH - The Scan Refresh characteristic index

 attrSize: The size of the characteristic value attribute.

 *attrValue: The pointer to the characteristic value data that should be 
              stored to the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is absent

******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpssSetCharacteristicValue(CYBLE_SCPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_DB_ATTR_HANDLE_T charValueHandle;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
    uint16 scanInterval;
    uint16 locScanWindow;

    if(charIndex >= CYBLE_SCPS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(charIndex == CYBLE_SCPS_SCAN_INT_WIN)
        {
            scanInterval = CyBle_Get16ByPtr(attrValue);
            locScanWindow = CyBle_Get16ByPtr(attrValue + sizeof(scanInterval));
            if( (scanInterval < CYBLE_SCAN_INTERVAL_WINDOW_MIN) ||
                (scanInterval > CYBLE_SCAN_INTERVAL_WINDOW_MAX) ||
                (locScanWindow < CYBLE_SCAN_INTERVAL_WINDOW_MIN) || (locScanWindow > scanInterval) ||
                (attrSize > CYBLE_INTERVAL_WINDOW_CHAR_LEN) )
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
            else
            {
                charValueHandle = cyBle_scpss.intervalWindowCharHandle;
            }
        }
        else    /* Scan Refresh characteristic */
        {
            if(attrSize > CYBLE_REFRESH_CHAR_LEN)
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
            else
            {
                charValueHandle = cyBle_scpss.refreshCharHandle;
            }
        }
    }
    if(apiResult == CYBLE_ERROR_OK)
    {
        if(charValueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            /* Store data in database */
            locHandleValuePair.attrHandle = charValueHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;
            if(CYBLE_GATT_ERR_NONE !=
                CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
        }
    }
    return (apiResult);
}


/*******************************************************************************
##Function Name: CyBle_ScpssGetCharacteristicValue
********************************************************************************

Summary:
 Gets a characteristic value of the Scan Parameters service, which is identified
 by charIndex.

Parameters:
 charIndex: The index of the service characteristic.
            * CYBLE_SCPS_SCAN_INT_WIN - The Scan Interval Window characteristic index
            * CYBLE_SCPS_SCAN_REFRESH - The Scan Refresh characteristic index

 attrSize: The size of the characteristic value attribute.

 *attrValue: The pointer to the location where characteristic value data 
              should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpssGetCharacteristicValue(CYBLE_SCPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_DB_ATTR_HANDLE_T charValueHandle;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex >= CYBLE_SCPS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(charIndex == CYBLE_SCPS_SCAN_INT_WIN)
        {
            if(attrSize > CYBLE_INTERVAL_WINDOW_CHAR_LEN)
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
            else
            {
                charValueHandle = cyBle_scpss.intervalWindowCharHandle;
            }
        }
        else    /* Scan Refresh characteristic */
        {
            if(attrSize > CYBLE_REFRESH_CHAR_LEN)
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
            else
            {
                charValueHandle = cyBle_scpss.refreshCharHandle;
            }
        }
    }
    if(apiResult == CYBLE_ERROR_OK)
    {
        if(charValueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            /* Read characteristic value from database */
            locHandleValuePair.attrHandle = charValueHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;
            if(CYBLE_GATT_ERR_NONE !=
                CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
        }
    }
    return (apiResult);
}


/*******************************************************************************
##Function Name: CyBle_ScpssGetCharacteristicDescriptor
********************************************************************************

Summary:
 Gets a characteristic descriptor of the specified characteristic of the 
 Scan Parameters service.

Parameters:
 charIndex: The index of the characteristic.
            * CYBLE_SCPS_SCAN_REFRESH - The Scan Refresh characteristic index

 descrIndex: The index of the descriptor.
            * CYBLE_SCPS_SCAN_REFRESH_CCCD - The Client Characteristic 
               Configuration descriptor index of the Scan Refresh characteristic

 attrSize: The size of the characteristic value attribute.

 *attrValue: The pointer to the location where the characteristic descriptor
              value data should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional descriptor is absent

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpssGetCharacteristicDescriptor(CYBLE_SCPS_CHAR_INDEX_T charIndex,
    CYBLE_SCPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((charIndex != CYBLE_SCPS_SCAN_REFRESH) || (descrIndex >= CYBLE_SCPS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(cyBle_scpss.refreshCccdHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            /* Get data from database */
            locHandleValuePair.attrHandle = cyBle_scpss.refreshCccdHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;
            if(CYBLE_GATT_ERR_NONE !=
                CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
        }
    }
    return (apiResult);
}


/*******************************************************************************
##Function Name: CyBle_ScpssWriteEventHandler
********************************************************************************

Summary:
 Handles the Write Request Event for the service.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 Return value is of type CYBLE_GATT_ERR_CODE_T.
  * CYBLE_GATT_ERR_NONE - Write request handled successfully.
  * CYBLE_GATT_ERR_UNLIKELY_ERROR - Internal error while writing attribute value

*******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_ScpssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    CYBLE_SCPS_CHAR_VALUE_T locChar;

    if(NULL != CyBle_ScpsApplCallback)
    {
        locChar.connHandle = eventParam->connHandle;
        
        /* Client Characteristic Configuration descriptor write request */
        if(eventParam->handleValPair.attrHandle == cyBle_scpss.refreshCccdHandle)
        {
            locChar.charIndex = CYBLE_SCPS_SCAN_REFRESH;
            locChar.value = NULL;
            if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
            {
                CyBle_ScpsApplCallback((uint32)CYBLE_EVT_SCPSS_NOTIFICATION_ENABLED, &locChar);
            }
            else
            {
                CyBle_ScpsApplCallback((uint32)CYBLE_EVT_SCPSS_NOTIFICATION_DISABLED, &locChar);
            }
        #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
            /* Set flag to store bonding data to flash */
            cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
        #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
            
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
        /* Scan Interval Window characteristic write without response request */
        else if(eventParam->handleValPair.attrHandle == cyBle_scpss.intervalWindowCharHandle)
        {
            locChar.charIndex = CYBLE_SCPS_SCAN_INT_WIN;
            locChar.value = &eventParam->handleValPair.value;
            CyBle_ScpsApplCallback((uint32)CYBLE_EVT_SCPSS_SCAN_INT_WIN_CHAR_WRITE, &locChar);
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
        else
        {
            /* Characteristic is not found */    
        }

        /* Store data to database if event is handled */
        if((cyBle_eventHandlerFlag & CYBLE_CALLBACK) == 0u)
        {
            gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
        }
    }
    return (gattErr);
}


/*******************************************************************************
##Function Name: CyBle_ScpssSendNotification
********************************************************************************

Summary:
 This function notifies the client that the server requires the Scan Interval
 Window Characteristic to be written with the latest values upon notification.
 
 The CYBLE_EVT_SCPSC_NOTIFICATION event is received by the peer device, on 
 invoking this function.

Parameters:
 connHandle: The connection handle

 charIndex: The index of the characteristic.
			 * CYBLE_SCPS_SCAN_REFRESH - The Scan Refresh characteristic index

 attrSize: The size of the characteristic value attribute.

 *attrValue: The pointer to the characteristic value data that should be 
              sent to the Client device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed. 
  * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the client.

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_SCPS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;

    if((charIndex != CYBLE_SCPS_SCAN_REFRESH) || (attrSize != CYBLE_REFRESH_CHAR_LEN))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Send Notification if it is enabled and connected */
        if( (cyBle_scpss.refreshCccdHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
             && CYBLE_IS_NOTIFICATION_ENABLED(cyBle_scpss.refreshCccdHandle) )
        {
            if(CYBLE_STATE_CONNECTED == CyBle_GetState())
            {
                /* Fill all fields of write request structure ... */
                ntfReqParam.attrHandle = cyBle_scpss.refreshCharHandle;
                ntfReqParam.value.val = attrValue;
                ntfReqParam.value.len = attrSize;

                /* Send notification to client using previously filled structure */
                apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
            }
            else
            {
                apiResult = CYBLE_ERROR_INVALID_STATE;
            }
        }
    }
    return (apiResult);
}

#endif /* CYBLE_SCPS_SERVER */

#ifdef CYBLE_SCPS_CLIENT


/*******************************************************************************
##Function Name: CyBle_ScpscDiscoverCharacteristicsEventHandler
********************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP event.
 Based on the service UUID, an appropriate data structure is populated using the
 data received as part of the callback.

Parameters:
 *discCharInfo: The pointer to a characteristic information structure.

Return:
 None

*******************************************************************************/
void CyBle_ScpscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    switch(discCharInfo->uuid.uuid16)
    {
        case CYBLE_UUID_CHAR_SCAN_REFRESH:
            CyBle_CheckStoreCharHandle(cyBle_scpsc.refreshChar);
            break;
        case CYBLE_UUID_CHAR_SCAN_WINDOW:
            CyBle_CheckStoreCharHandle(cyBle_scpsc.intervalWindowChar);
            break;
        default:
            break;
    }
}



/*******************************************************************************
##Function Name: CyBle_ScpscDiscoverCharDescriptorsEventHandler
********************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event. 
 This event is generated when the server successfully sends the data for 
 CYBLE_EVT_GATTC_FIND_INFO_REQ. Based on the service UUID, an appropriate data 
 structure is populated to the service with a service callback.

Parameters:
 *discDescrInfo: The pointer to a descriptor information structure.

Return:
 None

*******************************************************************************/
void CyBle_ScpscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_CLIENT_CONFIG)
    {
        CyBle_CheckStoreCharDescrHandle(cyBle_scpsc.refreshCccdHandle);
    }
}


/*******************************************************************************
##Function Name: CyBle_ScpscSetCharacteristicValue
********************************************************************************

Summary:
 Sets a characteristic value of the Scan Parameters Service, which is 
 identified by charIndex. 
 
 This function call can result in generation of the following events based on 
 the response from the server device:
 * CYBLE_EVT_GATTC_WRITE_RSP
 * CYBLE_EVT_GATTC_ERROR_RSP

 The CYBLE_EVT_SCPSS_SCAN_INT_WIN_CHAR_WRITE event is received by the peer 
 device on invoking this function.

Parameters:
 connHandle: The connection handle.
 charIndex:  The index of the service characteristic.
 attrSize:   The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be 
             sent to the server device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic.
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                    characteristic.

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_SCPS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_CMD_REQ_T writeCmdParam;

    if(charIndex != CYBLE_SCPS_SCAN_INT_WIN)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(cyBle_scpsc.intervalWindowChar.valueHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            writeCmdParam.attrHandle = cyBle_scpsc.intervalWindowChar.valueHandle;
            writeCmdParam.value.val = attrValue;
            writeCmdParam.value.len = attrSize;

            apiResult = CyBle_GattcWriteWithoutResponse(connHandle, &writeCmdParam);
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }

    return (apiResult);
}


/*******************************************************************************
##Function Name: CyBle_ScpscSetCharacteristicDescriptor
********************************************************************************

Summary:
 Sets characteristic descriptor of specified characteristic of the Scan 
 Parameters Service.
 
 This function call can result in generation of the following events based on 
 the response from the server device:
 * CYBLE_EVT_SCPSC_WRITE_DESCR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

 Following events can be received by the peer device on invoking this function:
 * CYBLE_EVT_SCPSS_NOTIFICATION_ENABLED
 * CYBLE_EVT_SCPSS_NOTIFICATION_DISABLED

Parameters:
 connHandle: The connection handle.
 charIndex:  The index of the service characteristic.
 descrIndex:  The index of the service characteristic descriptor.
 attrSize:   The size of the descriptor value attribute.
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

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_SCPS_CHAR_INDEX_T charIndex, CYBLE_SCPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;


    if((charIndex != CYBLE_SCPS_SCAN_REFRESH) || (descrIndex >= CYBLE_SCPS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(cyBle_scpsc.refreshChar.valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            /* Fill all fields of write request structure ... */
            writeReqParam.attrHandle = cyBle_scpsc.refreshCccdHandle;
            writeReqParam.value.val = attrValue;
            writeReqParam.value.len = attrSize;

            /* ... and send request to server device. */
            apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
            
            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_scpscReqHandle = writeReqParam.attrHandle;
            }
        }
    }

    return (apiResult);
}


/*******************************************************************************
##Function Name: CyBle_ScpscGetCharacteristicDescriptor
********************************************************************************

Summary:
 Gets characteristic descriptor of specified characteristic of the Scan 
 Parameters Service.
 
 This function call can result in generation of the following events based on
 the response from the server device:
 * CYBLE_EVT_SCPSC_READ_DESCR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

Parameters:
 connHandle: The connection handle.
 charIndex:  The index of a Service Characteristic.
 descrIndex: The index of a Service Characteristic Descriptor.

Return:
 * CYBLE_ERROR_OK - The request was sent successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_INVALID_STATE - The state is not valid
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular descriptor
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
                                    the specified attribute

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_SCPS_CHAR_INDEX_T charIndex, CYBLE_SCPS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if((charIndex != CYBLE_SCPS_SCAN_REFRESH) || (descrIndex >= CYBLE_SCPS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(cyBle_scpsc.refreshChar.valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, cyBle_scpsc.refreshCccdHandle);

            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_scpscReqHandle = cyBle_scpsc.refreshCccdHandle;
            }
        }
    }

    return (apiResult);
}


/*******************************************************************************
##Function Name: CyBle_ScpscNotificationEventHandler
********************************************************************************

Summary:
 Handles the Notification Event.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 None

*******************************************************************************/
void CyBle_ScpscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    CYBLE_SCPS_CHAR_VALUE_T locCharValue;

    if(NULL != CyBle_ScpsApplCallback)
    {
        if(cyBle_scpsc.refreshChar.valueHandle == eventParam->handleValPair.attrHandle)
        {
            locCharValue.connHandle = eventParam->connHandle;
            locCharValue.charIndex = CYBLE_SCPS_SCAN_REFRESH;
            locCharValue.value = &eventParam->handleValPair.value;
            CyBle_ScpsApplCallback((uint32)CYBLE_EVT_SCPSC_NOTIFICATION, &locCharValue);
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_ScpscReadResponseEventHandler
********************************************************************************

Summary:
 Handles the Read Response Event.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 None

*******************************************************************************/
void CyBle_ScpscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    if((NULL != CyBle_ScpsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_scpscReqHandle))
    {
        if(cyBle_scpsc.refreshCccdHandle == cyBle_scpscReqHandle)
        {
            CYBLE_SCPS_DESCR_VALUE_T locDescrValue;
                
            locDescrValue.connHandle = eventParam->connHandle;
            locDescrValue.charIndex = CYBLE_SCPS_SCAN_REFRESH;
            locDescrValue.descrIndex = CYBLE_SCPS_SCAN_REFRESH_CCCD;
            locDescrValue.value = &eventParam->value;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            cyBle_scpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            CyBle_ScpsApplCallback((uint32)CYBLE_EVT_SCPSC_READ_DESCR_RESPONSE, &locDescrValue);
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_ScpscWriteResponseEventHandler
********************************************************************************

Summary:
 Handles the Write Response Event.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 None

*******************************************************************************/
void CyBle_ScpscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    if((NULL != CyBle_ScpsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_scpscReqHandle))
    {
        if(cyBle_scpsc.refreshCccdHandle == cyBle_scpscReqHandle)
        {
            CYBLE_SCPS_DESCR_VALUE_T locDescrValue;
                
            locDescrValue.connHandle = *eventParam;
            locDescrValue.charIndex = CYBLE_SCPS_SCAN_REFRESH;
            locDescrValue.descrIndex = CYBLE_SCPS_SCAN_REFRESH_CCCD;
            locDescrValue.value = NULL;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            cyBle_scpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            CyBle_ScpsApplCallback((uint32)CYBLE_EVT_SCPSC_WRITE_DESCR_RESPONSE, &locDescrValue);
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_ScpscErrorResponseEventHandler
********************************************************************************

Summary:
 Handles the Error Response Event.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 None

*******************************************************************************/
void CyBle_ScpscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((NULL != eventParam) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_scpscReqHandle))
    {
        cyBle_scpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}

#endif /* (CYBLE_SCPS_CLIENT) */

/* [] END OF FILE */
