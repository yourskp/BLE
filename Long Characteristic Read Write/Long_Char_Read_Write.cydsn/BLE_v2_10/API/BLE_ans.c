/*******************************************************************************
File Name: CYBLE_ans.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the source code for Alert Notification Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_AnsApplCallback = NULL;

#ifdef CYBLE_ANS_SERVER

`$AnsServer`
    
#endif /* CYBLE_ANS_SERVER */

#ifdef CYBLE_ANS_CLIENT

CYBLE_ANSC_T cyBle_ansc;
    
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_anscReqHandle;

#endif /* (CYBLE_ANS_CLIENT) */


/****************************************************************************** 
##Function Name: CyBle_AnsInit
*******************************************************************************

Summary:
 This function initializes the Alert Notification Service.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_AnsInit(void)
{

#ifdef CYBLE_ANS_CLIENT

    (void)memset(&cyBle_ansc, 0, sizeof(cyBle_ansc));
    
    cyBle_anscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_ANS_CLIENT */

}


/******************************************************************************
##Function Name: CyBle_AnsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for Alert Notification Service specific
 attribute operations.

Parameters:
 callbackFunc: An application layer event callback function to receive 
                   service specific events from the BLE Component. The 
                   definition of CYBLE_CALLBACK_T for Alert Notification
                   Service is,

                   typedef void (* CYBLE_CALLBACK_T) 
                       (uint32 eventCode, void *eventParam)

                   * eventCode indicates the event that triggered this 
                      callback (e.g. CYBLE_EVT_ANSS_NOTIFICATION_ENABLED) 
                   * eventParam contains the parameters corresponding to 
                     the current event (e.g. Pointer to CYBLE_ANS_CHAR_VALUE_T 
                     structure that contains details of the  characteristic
                     for which notification enabled event was triggered).

Return:
 None

Side Effects: 
 The *eventParams in the callback function should not be used by the 
 application once the callback function execution is finished. Otherwise 
 this data may become corrupted.

******************************************************************************/
void CyBle_AnsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_AnsApplCallback = callbackFunc;
}


#ifdef CYBLE_ANS_SERVER

/******************************************************************************
##Function Name: CyBle_AnssWriteEventHandler
*******************************************************************************

Summary:
 Handles Write Request Event for Alert Notification Service.

Parameters:
 *eventParam: The pointer to the data that came with a write request for
               Alert Notification Service.

Return:
 Return a value of type CYBLE_GATT_ERR_CODE_T:
  * CYBLE_GATT_ERR_NONE - The function terminated successfully.
  * CYBLE_GATT_ERR_INVALID_HANDLE - The handle of Client Configuration
    Characteristic Descriptor or Characteristic of Alert Notification Service
	is not valid.
  * CYBLE_GATT_ERR_UNLIKELY_ERROR - An Internal Stack error occurred.
  * CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED - The notification property of a 
    specific characteristic of Alert Notification Service is disabled.

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_AnssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_ANS_CHAR_VALUE_T wrReqParam;
    CYBLE_GATT_DB_ATTR_HANDLE_T tmpHandle;
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    uint8 flag = 0u;
    uint32 event = (uint32)CYBLE_EVT_ANSS_CHAR_WRITE;
    
    tmpHandle = eventParam->handleValPair.attrHandle;
    
    /* Client Characteristic Configuration Descriptor Write Request */
    if((tmpHandle == cyBle_anss.charInfo[CYBLE_ANS_NEW_ALERT].descrHandle[CYBLE_ANS_CCCD]) || 
        (tmpHandle == cyBle_anss.charInfo[CYBLE_ANS_UNREAD_ALERT_STATUS].descrHandle[CYBLE_ANS_CCCD]))
    {   
        /* Verify that optional notification property is enabled for Characteristic */
        if(CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_anss.charInfo[CYBLE_ANS_NEW_ALERT].charHandle) ||
            CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_anss.charInfo[CYBLE_ANS_UNREAD_ALERT_STATUS].charHandle))
        {
            if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
            {
                event = (uint32)CYBLE_EVT_ANSS_NOTIFICATION_ENABLED;
            }
            else
            {
                event = (uint32)CYBLE_EVT_ANSS_NOTIFICATION_DISABLED;
            }
            
            if(tmpHandle == cyBle_anss.charInfo[CYBLE_ANS_NEW_ALERT].descrHandle[CYBLE_ANS_CCCD])
            {
                wrReqParam.charIndex = CYBLE_ANS_NEW_ALERT;
            }
            else
            {
                wrReqParam.charIndex = CYBLE_ANS_UNREAD_ALERT_STATUS;
            }
            
            /* Value is NULL for descriptors */
            wrReqParam.value = NULL;
        #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
            /* Set flag to store bonding data to flash */
            cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
        #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
        }
        else
        {
            gattErr = CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED;
        }
    }
    else if(tmpHandle == cyBle_anss.charInfo[CYBLE_ANS_ALERT_NTF_CONTROL_POINT].charHandle)
    {
		/* Validate Command ID and Category ID ranges */
        if((eventParam->handleValPair.value.val[0u] <= CYBLE_ANS_IMM_UNREAD_ALERT_STATUS_NTF) &&
            ((eventParam->handleValPair.value.val[1u] <= CYBLE_ANS_CAT_ID_INSTANT_MESSAGE) ||
                (eventParam->handleValPair.value.val[1u] == CYBLE_ANS_CAT_ID_ALL)))
        {
            wrReqParam.charIndex = CYBLE_ANS_ALERT_NTF_CONTROL_POINT;
            wrReqParam.value = &eventParam->handleValPair.value;
        }
        else /* Command ID or Category ID received is invalid */
        {
			/* Erroneous request won't be notified to user but Error response will be sent. */
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            
            gattErr = CYBLE_GATT_ERR_ANS_COMMAND_NOT_SUPPORTED;
        }
    }
    else
    {
        /* Requested handle does not belong to Alert Notification Service
            Characteristic or Descriptor. */
        flag = 1u;
    }
    
    if((CYBLE_GATT_ERR_NONE == gattErr) && (0u == flag))
    {
        /* Write value to GATT database */
        gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
            &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
        
        if(CYBLE_GATT_ERR_NONE == gattErr)
        {
            wrReqParam.connHandle = eventParam->connHandle;

            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            
            if(NULL != CyBle_AnsApplCallback)
            {
                CyBle_AnsApplCallback(event, &wrReqParam);
            }
        }
    }
    
    if(CYBLE_GATT_ERR_NONE != gattErr)
    {
        /* Indicate that request was handled */
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }

    return(gattErr);
}


/******************************************************************************
##Function Name: CyBle_AnssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets a characteristic value of Alert Notification Service, which is a value
 identified by charIndex, to the local database.

Parameters:
 charIndex: The index of the service characteristic of type 
             CYBLE_ANS_CHAR_INDEX_T. The valid values are,
             * CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT
             * CYBLE_ANS_SUPPORTED_UNREAD_ALERT_CAT
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to characteristic value data that should be stored
              in the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request is handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_AnssSetCharacteristicValue(CYBLE_ANS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex < CYBLE_ANS_CHAR_COUNT)
    {
        locHandleValuePair.attrHandle = cyBle_anss.charInfo[charIndex].charHandle;
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        
        /* Store data in database */
        if(CYBLE_GATT_ERR_NONE ==
            CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_OK;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_AnssGetCharacteristicValue
*******************************************************************************

Summary:
 Gets a characteristic value of Alert Notification Service. The value is 
 identified by charIndex.

Parameters:
 charIndex: The index of the service characteristic of type CYBLE_ANS_CHAR_INDEX_T. 
             The valid values are,
             * CYBLE_ANS_NEW_ALERT
             * CYBLE_ANS_UNREAD_ALERT_STATUS
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the location where characteristic value data 
              should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request is handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_AnssGetCharacteristicValue(CYBLE_ANS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex < CYBLE_ANS_CHAR_COUNT)
    {
        locHandleValuePair.attrHandle = cyBle_anss.charInfo[charIndex].charHandle;
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        
        /* Read characteristic value from database */
        if(CYBLE_GATT_ERR_NONE == 
            CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_OK;
        }
    }
    
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_AnssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets a characteristic descriptor of the specified characteristic of Alert
 Notification Service.

Parameters:
 charIndex:  The index of the service characteristic of type CYBLE_ANS_CHAR_INDEX_T.
             The valid values are,
             * CYBLE_ANS_NEW_ALERT 
             * CYBLE_ANS_UNREAD_ALERT_STATUS
 descrIndex: The index of the service characteristic descriptor of type 
             CYBLE_ANS_DESCR_INDEX_T. The valid value is,
             * CYBLE_ANS_CCCD
 attrSize:   The size of the characteristic descriptor attribute.
 *attrValue: The pointer to the location where characteristic descriptor value 
             data should be stored. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request is handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_AnssGetCharacteristicDescriptor(CYBLE_ANS_CHAR_INDEX_T charIndex,
    CYBLE_ANS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(((CYBLE_ANS_NEW_ALERT == charIndex) || (CYBLE_ANS_UNREAD_ALERT_STATUS == charIndex)) && 
        (descrIndex == CYBLE_ANS_CCCD))
    {
        if(cyBle_anss.charInfo[charIndex].descrHandle[descrIndex] != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            locHandleValuePair.attrHandle = cyBle_anss.charInfo[charIndex].descrHandle[descrIndex];
			locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;

            if(CYBLE_GATT_ERR_NONE ==
                CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                apiResult = CYBLE_ERROR_OK;
            }
        }
    }
    
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_AnssSendNotification
*******************************************************************************

Summary:
 Sends a notification with the characteristic value, as specified by its charIndex, 
 to the Client device.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic of type CYBLE_ANS_CHAR_INDEX_T.
             The valid values are, 
             * CYBLE_ANS_UNREAD_ALERT_STATUS
             * CYBLE_ANS_NEW_ALERT
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be sent
              to the Client device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The function completed successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of input parameter is failed.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this.
                                     characteristic.
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the client.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_AnssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ANS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    
    if((CYBLE_ANS_NEW_ALERT == charIndex) || (CYBLE_ANS_UNREAD_ALERT_STATUS == charIndex))
    {
        /* Send Notification if it is enabled and connected */
        if(CYBLE_STATE_CONNECTED != CyBle_GetState())
        {
            apiResult = CYBLE_ERROR_INVALID_STATE;
        }
        else if(!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_anss.charInfo[charIndex].descrHandle[CYBLE_ANS_CCCD]))
        {
            apiResult = CYBLE_ERROR_NTF_DISABLED;
        }
        else
        {
            CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;
            
            /* Fill all fields of write request structure ... */
            ntfReqParam.attrHandle = cyBle_anss.charInfo[charIndex].charHandle;
            ntfReqParam.value.val = attrValue;
            ntfReqParam.value.len = attrSize;
            
            /* Send notification to client using previously filled structure */
            apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
        }
    }
    return (apiResult);
}

#endif /* CYBLE_ANS_SERVER */


#ifdef CYBLE_ANS_CLIENT

/******************************************************************************
##Function Name: CyBle_AnscDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 CYBLE_DISC_CHAR_INFO_T *discCharInfo: The pointer to a characteristic
                                       information structure.
 
Return:
 None

******************************************************************************/
void CyBle_AnscDiscoverCharacteristicsEventHandler(const CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    /* ANS characteristics UUIDs */
    static const CYBLE_UUID16 cyBle_anscCharUuid[CYBLE_ANS_CHAR_COUNT] =
    {
        CYBLE_UUID_CHAR_SPRTD_NEW_ALRT_CTGRY,
        CYBLE_UUID_CHAR_NEW_ALERT,
        CYBLE_UUID_CHAR_SPRT_UNRD_ALRT_CTGRY,
        CYBLE_UUID_CHAR_UNREAD_ALRT_STATUS,
        CYBLE_UUID_CHAR_ALERT_NTF_CONTROL_POINT
    };
    uint8 i;
    static CYBLE_GATT_DB_ATTR_HANDLE_T *ansLastEndHandle = NULL;

    /* Update last characteristic endHandle to declaration handle of this characteristic */ 
    if(ansLastEndHandle != NULL)
    {
        *ansLastEndHandle = discCharInfo->charDeclHandle - 1u;
        ansLastEndHandle = NULL;
    }

    for(i = (uint8) CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT; i < (uint8) CYBLE_ANS_CHAR_COUNT; i++)
    {
        if(cyBle_anscCharUuid[i] == discCharInfo->uuid.uuid16)
        {
            cyBle_ansc.characteristics[i].charInfo.valueHandle = discCharInfo->valueHandle;
            cyBle_ansc.characteristics[i].charInfo.properties  = discCharInfo->properties;
            ansLastEndHandle = &cyBle_ansc.characteristics[i].endHandle;
            break;
        }
    }

    /* Initially characteristic endHandle equals to the Service endHandle.
    * Characteristic endHandle will be updated to declaration handler of the
    * following characteristic,in the following characteristic discovery 
    * procedure.
    */
    if(ansLastEndHandle != NULL)
    {
        *ansLastEndHandle = cyBle_serverInfo[cyBle_disCount].range.endHandle;
    }
}


/******************************************************************************
##Function Name: CyBle_AnsDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event. 
 This event is generated when a server successfully sends the data for 
 CYBLE_EVT_GATTC_FIND_INFO_REQ. Based on the service UUID, an appropriate data 
 structure is populated to the service with a service callback.

Parameters:
 CYBLE_DISC_DESCR_INFO_T * discCharInfo: The pointer to a characteristic 
                                          information structure.

Return:
 None

******************************************************************************/
void CyBle_AnscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo)
{
    if(CYBLE_UUID_CHAR_CLIENT_CONFIG == discDescrInfo->uuid.uuid16)
    {
        if(cyBle_ansc.characteristics[CYBLE_ANS_NEW_ALERT].descriptors[CYBLE_ANS_CCCD] ==
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_ansc.characteristics[CYBLE_ANS_NEW_ALERT].descriptors[CYBLE_ANS_CCCD] =
                discDescrInfo->descrHandle;
        }
        else if(cyBle_ansc.characteristics[CYBLE_ANS_UNREAD_ALERT_STATUS].descriptors[CYBLE_ANS_CCCD] ==
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_ansc.characteristics[CYBLE_ANS_UNREAD_ALERT_STATUS].descriptors[CYBLE_ANS_CCCD] =
                discDescrInfo->descrHandle;
        }
        else    /* Duplication of descriptor */
        {
            CyBle_ApplCallback((uint32)CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_AnscNotificationEventHandler
*******************************************************************************

Summary:
 Handles Notification Event for Alert Notification Service.

Parameters:
 CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam: The pointer to a 
                                                    CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T
                                                    data structure specified by
                                                    the event.

Return:
 None

******************************************************************************/
void CyBle_AnscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    if(NULL != CyBle_AnsApplCallback)
    {
        if((cyBle_ansc.characteristics[CYBLE_ANS_NEW_ALERT].charInfo.valueHandle == 
            eventParam->handleValPair.attrHandle) ||
            (cyBle_ansc.characteristics[CYBLE_ANS_UNREAD_ALERT_STATUS].charInfo.valueHandle == 
                eventParam->handleValPair.attrHandle))
        {
            CYBLE_ANS_CHAR_VALUE_T ntfParam;
            
            ntfParam.charIndex = CYBLE_ANS_NEW_ALERT;
            ntfParam.connHandle = eventParam->connHandle;
            ntfParam.value = &eventParam->handleValPair.value;
            
            CyBle_AnsApplCallback((uint32)CYBLE_EVT_ANSC_NOTIFICATION, (void *)&ntfParam);
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_AnscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles Read Response Event for Alert Notification Service.

Parameters:
 CYBLE_GATTC_READ_RSP_PARAM_T *eventParam: The pointer to the data that came 
                                           with a read response for ANS.

Return:
 None

******************************************************************************/
void CyBle_AnscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    uint8 flag = 1u;
    uint8 attrVal = 0u;
    CYBLE_ANS_CHAR_INDEX_T idx;
    
    if((NULL != CyBle_AnsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_anscReqHandle))
    {
        if(cyBle_ansc.characteristics[CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT].charInfo.valueHandle == cyBle_anscReqHandle)
        {
            idx = CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT;
        }
        else if(cyBle_ansc.characteristics[CYBLE_ANS_SUPPORTED_UNREAD_ALERT_CAT].charInfo.valueHandle == cyBle_anscReqHandle)
        {
            idx = CYBLE_ANS_SUPPORTED_UNREAD_ALERT_CAT;
        }
        else if(cyBle_ansc.characteristics[CYBLE_ANS_NEW_ALERT].descriptors[CYBLE_ANS_CCCD] == 
            cyBle_anscReqHandle)
        {
            /* Attribute is characteristic descriptor */
            attrVal = 1u;
            idx = CYBLE_ANS_NEW_ALERT;
        }
        else if(cyBle_ansc.characteristics[CYBLE_ANS_UNREAD_ALERT_STATUS].descriptors[CYBLE_ANS_CCCD] == 
            cyBle_anscReqHandle)
        {
            /* Attribute is characteristic descriptor */
            attrVal = 1u;
            idx = CYBLE_ANS_UNREAD_ALERT_STATUS;
        }
        else
        {
            /* No ANS characteristic was requested for read */
            flag = 0u;
        }

        if(0u != flag)
        {
            /* Read response for characteristic */
            if(0u == attrVal)
            {
                CYBLE_ANS_CHAR_VALUE_T rdRspParam;

                /* Fill Alert Notification Service read response parameter structure with
                  characteristic info. */
                rdRspParam.connHandle = eventParam->connHandle;
                rdRspParam.charIndex = idx;
                rdRspParam.value = &eventParam->value;

                CyBle_AnsApplCallback((uint32)CYBLE_EVT_ANSC_READ_CHAR_RESPONSE, (void *)&rdRspParam);
            }
            else /* Read response for characteristic descriptor */
            {
                CYBLE_ANS_DESCR_VALUE_T rdRspParam;

                /* Fill Alert Notification Service read response parameter structure with
                  characteristic descriptor info. */
                rdRspParam.connHandle = eventParam->connHandle;
                rdRspParam.charIndex = idx;
                rdRspParam.descrIndex = CYBLE_ANS_CCCD;
                rdRspParam.value = &eventParam->value;

                CyBle_AnsApplCallback((uint32)CYBLE_EVT_ANSC_READ_DESCR_RESPONSE, (void *)&rdRspParam);
            }

            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            cyBle_anscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_AnscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles Write Response Event for Alert Notification Service.

Parameters:
 CYBLE_CONN_HANDLE_T *eventParam: The pointer to the CYBLE_CONN_HANDLE_T data 
                                  structure.

Return:
 None

******************************************************************************/
void CyBle_AnscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    uint8 flag = 1u;
    uint8 attrVal = 0u;
    CYBLE_ANS_CHAR_INDEX_T idx;

    if((NULL != CyBle_AnsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_anscReqHandle))
    {
        if(cyBle_ansc.characteristics[CYBLE_ANS_ALERT_NTF_CONTROL_POINT].charInfo.valueHandle == cyBle_anscReqHandle)
        {
            idx = CYBLE_ANS_ALERT_NTF_CONTROL_POINT;
        }
        else if(cyBle_ansc.characteristics[CYBLE_ANS_NEW_ALERT].descriptors[CYBLE_ANS_CCCD] == 
            cyBle_anscReqHandle)
        {
            /* Attribute is Characteristic Descriptor */
            attrVal = 1u;
            idx = CYBLE_ANS_NEW_ALERT;
        }
        else if(cyBle_ansc.characteristics[CYBLE_ANS_UNREAD_ALERT_STATUS].descriptors[CYBLE_ANS_CCCD] == 
            cyBle_anscReqHandle)
        {
            /* Attribute is Characteristic Descriptor */
            attrVal = 1u;
            idx = CYBLE_ANS_UNREAD_ALERT_STATUS;
        }
        else
        {
            /* No ANS characteristic was requested for write */
            flag = 0u;
        }
        
        if(0u != flag)
        {
            /* Write response for characteristic */
            if(0u == attrVal)
            {
                CYBLE_ANS_CHAR_VALUE_T wrRspParam;

                /* Fill Alert Notification Service write response parameter structure with
                  characteristic info. */
                wrRspParam.connHandle = *eventParam;
                wrRspParam.charIndex = idx;
                wrRspParam.value = NULL;

                CyBle_AnsApplCallback((uint32)CYBLE_EVT_ANSC_WRITE_CHAR_RESPONSE, (void *)&wrRspParam);
            }
            else /* Write response for characteristic descriptor */
            {
                CYBLE_ANS_DESCR_VALUE_T wrRspParam;

                /* Fill Alert Notification Service write response parameter structure with
                  characteristic descriptor info. */
                wrRspParam.connHandle = *eventParam;
                wrRspParam.charIndex = idx;
                wrRspParam.descrIndex = CYBLE_ANS_CCCD;
                wrRspParam.value = NULL;

                CyBle_AnsApplCallback((uint32)CYBLE_EVT_ANSC_WRITE_DESCR_RESPONSE, (void *)&wrRspParam);
            }

            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            cyBle_anscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_AnscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles Error Response Event for Alert Notification Service.

Parameters:
 CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam: The pointer to 
                                          the CYBLE_GATTC_ERR_RSP_PARAM_T 
                                          structure.

Return:
 None

******************************************************************************/
void CyBle_AnscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_anscReqHandle)
    {
        cyBle_anscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


/******************************************************************************
##Function Name: CyBle_AnscSetCharacteristicValue
*******************************************************************************

Summary:
 Sends a request to the peer device to set the characteristic value, as 
 identified by its charIndex.

Parameters:
 connHandle: The connection handle.
 charIndex:  The index of the service characteristic.
 attrSize:   Size of the Characteristic value attribute.
 *attrValue: Pointer to the characteristic value data that should be 
             sent to the server device. 
Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully.
  * CYBLE_ERROR_INVALID_STATE - The component in in invalid state for current
                                 operation.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                     characteristic.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_AnscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_ANS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_GATTC_WRITE_CMD_REQ_T writeReqParam;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((NULL != attrValue) && (CYBLE_ANS_ALERT_NTF_CONTROL_POINT == charIndex) &&
        (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=  
            cyBle_ansc.characteristics[CYBLE_ANS_ALERT_NTF_CONTROL_POINT].charInfo.valueHandle))
    {
        /* Fill all fields of write command request structure ... */
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;
        writeReqParam.attrHandle = cyBle_ansc.characteristics[CYBLE_ANS_ALERT_NTF_CONTROL_POINT].charInfo.valueHandle;

        /* Send request to write characteristic value */
        apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &writeReqParam);
        
        /* Save handle to support service specific write response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_anscReqHandle = writeReqParam.attrHandle;
        }
    }
    else
    {
        /* Validation of the input parameters failed */
    }

    return(apiResult);
}


/******************************************************************************
##Function Name: CyBle_AnscGetCharacteristicValue
*******************************************************************************

Summary:
 Sends a request to the peer device to get a characteristic value, as 
 identified by its charIndex.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully;
  * CYBLE_ERROR_INVALID_STATE - The component in in invalid state for current
                                 operation.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                     characteristic.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_AnscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_ANS_CHAR_INDEX_T charIndex)
{
    CYBLE_GATT_DB_ATTR_HANDLE_T tmpCharHandle;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else
    {
        /* Select characteristic */
        switch(charIndex)
        {
        case CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT:
            tmpCharHandle = cyBle_ansc.characteristics[CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT].charInfo.valueHandle;
            break;
        case CYBLE_ANS_SUPPORTED_UNREAD_ALERT_CAT:
            tmpCharHandle = cyBle_ansc.characteristics[CYBLE_ANS_SUPPORTED_UNREAD_ALERT_CAT].charInfo.valueHandle;
            break;
        default:
            /* Characteristic wasn't found */
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            break;
        }

        if(CYBLE_ERROR_OK == apiResult)
        {
            /* Send request to read characteristic value */
            apiResult = CyBle_GattcReadCharacteristicValue(connHandle, tmpCharHandle);
        
            if(CYBLE_ERROR_OK == apiResult)
            {
                cyBle_anscReqHandle = tmpCharHandle;
            }
        }
    }

    return(apiResult);
}


/******************************************************************************
##Function Name: CyBle_AnscSetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sends a request to the peer device to set the characteristic descriptor of the 
 specified characteristic of Alert Notification Service.

Parameters:
 connHandle: The BLE peer device connection handle.
 charIndex: The index of the ANS characteristic.
 descrIndex: The index of the ANS characteristic descriptor.
 attrSize: The size of the characteristic descriptor attribute.
 *attrValue: Pointer to the characteristic descriptor value data that should be 
             sent to the server device. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The  request was sent successfully.
  * CYBLE_ERROR_INVALID_STATE - The component in in invalid state for current
                                 operation.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                     characteristic.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_AnscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_ANS_CHAR_INDEX_T charIndex,
                                                        CYBLE_ANS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize,
                                                        uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((NULL != attrValue) && (CYBLE_ANS_CCCD == descrIndex) && (CYBLE_CCCD_LEN == attrSize))
    {
        if((CYBLE_ANS_NEW_ALERT == charIndex) || (CYBLE_ANS_UNREAD_ALERT_STATUS == charIndex))
        {
            CYBLE_GATTC_WRITE_REQ_T writeReqParam;
            
            /* Fill all fields of write request structure ... */
            writeReqParam.value.val = attrValue;
            writeReqParam.value.len = attrSize;
            
            if(CYBLE_ANS_NEW_ALERT == charIndex)
            {
                writeReqParam.attrHandle =
                    cyBle_ansc.characteristics[CYBLE_ANS_NEW_ALERT].descriptors[CYBLE_ANS_CCCD];
            }
            else
            {
                writeReqParam.attrHandle =
                    cyBle_ansc.characteristics[CYBLE_ANS_UNREAD_ALERT_STATUS].descriptors[CYBLE_ANS_CCCD];
            }

            /* ... and send request to server device. */        
            apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
            
            if(CYBLE_ERROR_OK == apiResult)
            {
                cyBle_anscReqHandle = writeReqParam.attrHandle;
            }
        }
    }
    else
    {
        /* Characteristic has not been discovered or has invalid fields */   
    }

    return(apiResult);
}


/******************************************************************************
##Function Name: CyBle_AnscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sends a request to the peer device to get the characteristic descriptor of the
 specified characteristic of Alert Notification Service.

Parameters:
 connHandle: BLE peer device connection handle.
 charIndex: The index of the Service Characteristic.
 descrIndex: The index of the Service Characteristic Descriptor.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - A request was sent successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_INVALID_STATE - The component is in invalid state for current 
                                operation.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_OPERATION - Cannot process a request to send PDU due
                                    to invalid operation performed by the 
                                    application.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_AnscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                         CYBLE_ANS_CHAR_INDEX_T charIndex,
                                                         uint8 descrIndex)
{
    CYBLE_GATT_DB_ATTR_HANDLE_T tmpHandle;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(((CYBLE_ANS_NEW_ALERT == charIndex) || (CYBLE_ANS_UNREAD_ALERT_STATUS == charIndex)) &&
        ((uint8) CYBLE_ANS_CCCD == descrIndex))
    {
        if(CYBLE_ANS_NEW_ALERT == charIndex)
        {
            tmpHandle = cyBle_ansc.characteristics[CYBLE_ANS_NEW_ALERT].descriptors[CYBLE_ANS_CCCD];
        }
        else
        {
            tmpHandle = cyBle_ansc.characteristics[CYBLE_ANS_UNREAD_ALERT_STATUS].descriptors[CYBLE_ANS_CCCD];
        }
        
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, tmpHandle);
        
        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_anscReqHandle = tmpHandle;
        }
    }
    else
    {
        /* Characteristic has not been discovered or had invalid fields */
    }

    return(apiResult);
}

#endif /* CYBLE_ANS_CLIENT */


/* [] END OF FILE */
