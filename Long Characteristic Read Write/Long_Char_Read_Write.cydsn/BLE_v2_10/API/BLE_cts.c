/*******************************************************************************
File Name: CYBLE_cts.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the source code for the Current Time Service of the BLE
 Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_CtsApplCallback = NULL;

#ifdef CYBLE_CTS_SERVER

`$CtsServer`

#endif /* (CYBLE_CTS_SERVER) */

#ifdef CYBLE_CTS_CLIENT

CYBLE_CTSC_T cyBle_ctsc;
    
/* Internal storage for last request handle to check the response */
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_ctscReqHandle;

#endif /* (CYBLE_CTS_CLIENT) */


/****************************************************************************** 
##Function Name: CyBle_CtsInit
*******************************************************************************

Summary:
 This function initializes the Current Time Service.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_CtsInit(void)
{

#ifdef CYBLE_CTS_CLIENT
    
    (void) memset((void *) &cyBle_ctsc, 0, sizeof(cyBle_ctsc));
    cyBle_ctscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* (CYBLE_CTS_CLIENT) */
}


/****************************************************************************** 
##Function Name: CyBle_CtsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc: An application layer event callback function to receive 
                   events from the BLE Component. The definition of 
                   CYBLE_CALLBACK_T for Current Time Service is,
            
                   typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                      void *eventParam)

                   * eventCode indicates the event that triggered this 
                     callback (e.g. CYBLE_EVT_CTSS_NOTIFICATION_ENABLED)
                   * eventParam contains the parameters corresponding to the 
                     current event (e.g. Pointer to CYBLE_CTS_CHAR_VALUE_T 
                     structure that contains details of the characteristic 
                     for which notification enabled event was triggered).
Return:
 None

******************************************************************************/
void CyBle_CtsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_CtsApplCallback = callbackFunc;
}


#ifdef CYBLE_CTS_SERVER

/****************************************************************************** 
##Function Name: CyBle_CtssWriteEventHandler
*******************************************************************************

Summary:
 Handles the Write Request Event for the Current Time Service.

Parameters:
 eventParam: The pointer to the data that came with a write request for the 
             Current Time Service.

Return:
 Return a value of type CYBLE_GATT_ERR_CODE_T:
  * CYBLE_GATT_ERR_NONE - Function terminated successfully.
  * CYBLE_GATT_ERR_INVALID_HANDLE - The Handle of the Current Time Client 
                                     Configuration Characteristic Descriptor
                                     is not valid.
  * CYBLE_GATT_ERR_UNLIKELY_ERROR - An Internal Stack error occurred.
  * CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED - The notification property of the
                                            Current Time Client Configuration
                                            Characteristic Descriptor is 
                                            disabled.

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_CtssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_CTS_CHAR_VALUE_T wrReqParam;
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    uint32 event = (uint32)CYBLE_EVT_CTSS_NOTIFICATION_DISABLED;
    
    if(CyBle_CtsApplCallback != NULL)
    {
        wrReqParam.connHandle = eventParam->connHandle;
        wrReqParam.value = &eventParam->handleValPair.value;

        /* Client Characteristic Configuration descriptor write request */
        if(eventParam->handleValPair.attrHandle == cyBle_ctss.currTimeCccdHandle)
        {
            /* Verify that optional notification property is enabled for Current Time
            * Characteristic.
            */
            if(CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_ctss.currTimeCharHandle))
            {
                if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                {
                    event = (uint32)CYBLE_EVT_CTSS_NOTIFICATION_ENABLED;
                }

                gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                    &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                
                if(CYBLE_GATT_ERR_NONE == gattErr)
                {
                    wrReqParam.charIndex = CYBLE_CTS_CURRENT_TIME;
                    wrReqParam.value = NULL;
                    CyBle_CtsApplCallback(event, &wrReqParam);
                }
            #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
                (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
                /* Set flag to store bonding data to flash */
                cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
            #endif /* (CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
                   * (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)
                   */
            }
            else
            {
                gattErr = CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED;
            }

            /* Clear the callback flag indicating that request was handled */
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
        else if((eventParam->handleValPair.attrHandle == cyBle_ctss.currTimeCharHandle) ||
                (eventParam->handleValPair.attrHandle == cyBle_ctss.localTimeInfCharHandle))
        {
            if(eventParam->handleValPair.attrHandle == cyBle_ctss.currTimeCharHandle)
            {
                wrReqParam.charIndex = CYBLE_CTS_CURRENT_TIME;
            }
            else
            {
                wrReqParam.charIndex = CYBLE_CTS_LOCAL_TIME_INFO;
            }

            /* Send callback to user. User must validate the fields of the 
            * Current Time Characteristic and then perform database 
            * Characteristic Write procedure to write all or only selected 
            * fields to the database. In case if user want to ignore some 
            * fields of Current Time Characteristic user has to call
            * CyBle_SetGattError() with CYBLE_GATT_ERR_CTS_DATA_FIELD_IGNORED
            * parameter. In case if no fields in the Current Time Characteristic
            * is ignored there is no need to call CyBle_SetGattError().
            */
            CyBle_CtsApplCallback((uint32) CYBLE_EVT_CTSS_CHAR_WRITE, (void *)&wrReqParam);

            /* Get the value from cyBle_gattError in case if user wants to return error */
            gattErr = cyBle_gattError;

            /* Clear the callback flag indicating that request was handled */
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK; 
        }
        else
        {
        }
    }

    return(gattErr);
}


/****************************************************************************** 
##Function Name: CyBle_CtssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets a value for one of three characteristic values of the Current Time 
 Service. The characteristic is identified by charIndex.

Parameters:
 charIndex: The index of the Current Time Service characteristic.
 attrSize:  The size of the characteristic value attribute.
 attrValue: The pointer to the characteristic value data that should be 
             stored to the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The characteristic value was written successfully
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CtssSetCharacteristicValue(CYBLE_CTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    uint16 year;
    CYBLE_CTS_CURRENT_TIME_T currTime;
    CYBLE_CTS_LOCAL_TIME_INFO_T localTime;
    CYBLE_CTS_REFERENCE_TIME_INFO_T refTime;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T ctsHandleValuePair;

    if(NULL != attrValue)
    {
        switch(charIndex)
        {
        case CYBLE_CTS_CURRENT_TIME:
            if(CYBLE_CTS_CURRENT_TIME_SIZE == attrSize)
            {
                (void) memcpy((void *) &currTime, (void *) attrValue, (uint16) attrSize);

                /* Validate characteristic value */
                /* First, validate "Year" field */
                year = ((uint16) ((uint16) currTime.yearHigh << CYBLE_CTS_8_BIT_OFFSET)) | ((uint16) currTime.yearLow);
                
                if((year >= CYBLE_CTS_YEAR_MIN) && (year <= CYBLE_CTS_YEAR_MAX))
                {
                    /* Validation passed */
                    apiResult = CYBLE_ERROR_OK;
                }

                /* Validate "Month" field */
                if((CYBLE_ERROR_INVALID_PARAMETER != apiResult) && (CYBLE_CTS_MONTH_MAX < currTime.month))
                {
                    apiResult = CYBLE_ERROR_INVALID_PARAMETER;
                }

                /* Next is "Day" field */
                if((CYBLE_ERROR_INVALID_PARAMETER != apiResult) && (CYBLE_CTS_DAY_MAX < currTime.day))
                {
                    apiResult = CYBLE_ERROR_INVALID_PARAMETER;
                }

                /* Validate "Hours" field */
                if((CYBLE_ERROR_INVALID_PARAMETER != apiResult) && (CYBLE_CTS_HOURS_MAX < currTime.hours))
                {
                    apiResult = CYBLE_ERROR_INVALID_PARAMETER;
                }

                /* Validate "Minutes" field */
                if((CYBLE_ERROR_INVALID_PARAMETER != apiResult) && (CYBLE_CTS_MINUTES_MAX < currTime.minutes))
                {
                    apiResult = CYBLE_ERROR_INVALID_PARAMETER;
                }

                /* Validate "Seconds" field */
                if((CYBLE_ERROR_INVALID_PARAMETER != apiResult) && (CYBLE_CTS_SECONDS_MAX < currTime.seconds))
                {
                    apiResult = CYBLE_ERROR_INVALID_PARAMETER;
                }

                /* Validate "dayOfWeek" field */
                if((CYBLE_ERROR_INVALID_PARAMETER != apiResult) && (CYBLE_CTS_DAY_OF_WEEK_MAX <= currTime.dayOfWeek))
                {
                    apiResult = CYBLE_ERROR_INVALID_PARAMETER;
                }

                if(CYBLE_ERROR_INVALID_PARAMETER != apiResult)
                {
                    /* Verify "Adjust Reason". If it is set to incorrect value then it will
                      be changed to "Unknown" Adjust Reason. */
                    CYBLE_CTS_CHECK_ADJUST_REASON(currTime.adjustReason);

                    /* Set Current Time Characteristic value to GATT database.
                      Need to handle return type difference of CyBle_GattsWriteAttributeValue() and
                      CyBle_CtssSetCharacteristicValue(). */
                    ctsHandleValuePair.attrHandle = cyBle_ctss.currTimeCharHandle;
                    ctsHandleValuePair.value.len = attrSize;
                    ctsHandleValuePair.value.val = (uint8 *) &currTime;
                    
                    if(CYBLE_GATT_ERR_NONE == 
                        CyBle_GattsWriteAttributeValue(&ctsHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
                    {
                        /* Indicate success */
                        apiResult = CYBLE_ERROR_OK;
                    }
                }
            }
            break;
        case CYBLE_CTS_LOCAL_TIME_INFO:
            if(CYBLE_CTS_LOCAL_TIME_INFO_SIZE == attrSize)
            {
                (void) memcpy((void *) &localTime, (void *) attrValue, (uint16) attrSize);

                if(((localTime.timeZone <= CYBLE_CTS_UTC_OFFSET_MAX) && 
                    (localTime.timeZone >= CYBLE_CTS_UTC_OFFSET_MIN))
                        || (CYBLE_CTS_UTC_OFFSET_UNKNOWN == localTime.timeZone))
                {
                    /* Validation passed */
                    apiResult = CYBLE_ERROR_OK;
                }

                if(CYBLE_ERROR_INVALID_PARAMETER != apiResult)
                {
                    /* Validate DST offset */
                    switch (localTime.dst)
                    {
                    case CYBLE_CTS_STANDARD_TIME:
                    case CYBLE_CTS_DAYLIGHT_TIME_0_5:
                    case CYBLE_CTS_DAYLIGHT_TIME_1_0:
                    case CYBLE_CTS_DAYLIGHT_TIME_2_0:
                    case CYBLE_CTS_DST_UNKNOWN:
                        /* DST offset is correct */
                        apiResult = CYBLE_ERROR_OK;
                        break;

                    default:
                        /* Validation failed */
                        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
                        break;
                    }
                }

                if(CYBLE_ERROR_INVALID_PARAMETER != apiResult)
                {
                    if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ctss.localTimeInfCharHandle)
                    {
                        /* Set Local Time Characteristic value to GATT database.
                            Need to handle return type difference of CyBle_GattsWriteAttributeValue() and
                            CyBle_CtssSetCharacteristicValue(). */
                        ctsHandleValuePair.attrHandle = cyBle_ctss.localTimeInfCharHandle;
                        ctsHandleValuePair.value.len = attrSize;
                        ctsHandleValuePair.value.val = attrValue;
                        
                        if(CYBLE_GATT_ERR_NONE == 
                            CyBle_GattsWriteAttributeValue(&ctsHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
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
            }
            break;
            
        case CYBLE_CTS_REFERENCE_TIME_INFO:
            if(CYBLE_CTS_REFERENCE_TIME_INFO_SIZE == attrSize)
            {
                (void) memcpy((void *) &refTime, (void *) attrValue, (uint16) attrSize);

                /* Validate "Time Source" value */
                if(CYBLE_CTS_TIME_SRC_CELL_NET >= refTime.timeSource)
                {
                    /* Time source is correct */
                    apiResult = CYBLE_ERROR_OK;
                }

                if(CYBLE_ERROR_INVALID_PARAMETER != apiResult)
                {
                    /* Validate "Hours since update" field */
                    if(refTime.hoursSinseUpdate <= CYBLE_CTS_HOURS_MAX)
                    {
                        /* Value is correct */
                    }
                    else if(CYBLE_CTS_MAX_DAYS_SINCE_UPDATE == refTime.daysSinceUpdate)
                    {
                        /* Per CTS spec "Hours since update" is set to 255 as "Days since update"
                          is 255. */
                        refTime.hoursSinseUpdate = CYBLE_CTS_MAX_HOURS_SINCE_UPDATE;
                    }
                    else
                    {
                        /* Invalid value encountered */
                        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
                    }
                }

                if(CYBLE_ERROR_INVALID_PARAMETER != apiResult)
                {
                    if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ctss.localTimeInfCharHandle)
                    {
                        /* Set Reference Time Characteristic value to GATT database.
                            Need to handle return type difference of CyBle_GattsWriteAttributeValue() and
                            CyBle_CtssSetCharacteristicValue(). */
                        ctsHandleValuePair.attrHandle = cyBle_ctss.refTimeInfCharHandle;
                        ctsHandleValuePair.value.len = attrSize;
                        ctsHandleValuePair.value.val = (uint8 *) &refTime;
                        
                        if(CYBLE_GATT_ERR_NONE == 
                            CyBle_GattsWriteAttributeValue(&ctsHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
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
            }
            break;
        default:
            /* Characteristic wasn't found */
            break;
        }
    }

    /* Return status */
    return(apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CtssGetCharacteristicValue
*******************************************************************************

Summary:
 Gets a characteristic value of the Current Time Service, which is identified 
 by charIndex.

Parameters:
 charIndex: The index of a Current Time Service characteristic.
 attrSize: The size of the Current Time Service characteristic value attribute.
 attrValue: The pointer to the location where characteristic value data
            should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The characteristic value was read successfully
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CtssGetCharacteristicValue(CYBLE_CTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_GATT_DB_ATTR_HANDLE_T tmpCharHandle;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T ctsHandleValuePair;

    if(NULL != attrValue)
    {
        switch(charIndex)
        {
        case CYBLE_CTS_CURRENT_TIME:
            if(CYBLE_CTS_CURRENT_TIME_SIZE == attrSize)
            {
                tmpCharHandle = cyBle_ctss.currTimeCharHandle;
                apiResult = CYBLE_ERROR_OK;
            }
            break;
        case CYBLE_CTS_LOCAL_TIME_INFO:
            if(CYBLE_CTS_LOCAL_TIME_INFO_SIZE == attrSize)
            {
                tmpCharHandle = cyBle_ctss.localTimeInfCharHandle;
                apiResult = CYBLE_ERROR_OK;
            }
            break;
        case CYBLE_CTS_REFERENCE_TIME_INFO:
            if(CYBLE_CTS_REFERENCE_TIME_INFO_SIZE == attrSize)
            {
                tmpCharHandle = cyBle_ctss.refTimeInfCharHandle;
                apiResult = CYBLE_ERROR_OK;
            }
            break;
        default:
            /* Characteristic wasn't found */
            break;
        }

        if(CYBLE_ERROR_INVALID_PARAMETER != apiResult)
        {
            if(tmpCharHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                ctsHandleValuePair.attrHandle = tmpCharHandle;
                ctsHandleValuePair.value.len = attrSize;
                ctsHandleValuePair.value.val = attrValue;
            
                /* Get characteristic value from GATT database */
                if(CYBLE_GATT_ERR_NONE == 
                    CyBle_GattsReadAttributeValue(&ctsHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
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
    }

    /* Return status */
    return(apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CtssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets a characteristic descriptor of a specified characteristic of the Current 
 Time Service.

Parameters:
 charIndex: The index of the characteristic.
 descrIndex: The index of the descriptor.
 attrSize: The size of the descriptor value.
 attrValue: The pointer to the location where characteristic descriptor value
            data should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional descriptor is absent

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CtssGetCharacteristicDescriptor(CYBLE_CTS_CHAR_INDEX_T charIndex,
    CYBLE_CTS_CHAR_DESCRIPTORS_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T ctsHandleValuePair;
    
    if((charIndex == CYBLE_CTS_CURRENT_TIME) && (descrIndex == CYBLE_CTS_CURRENT_TIME_CCCD))
    {
        if(cyBle_ctss.currTimeCccdHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            ctsHandleValuePair.attrHandle = cyBle_ctss.currTimeCccdHandle;
            ctsHandleValuePair.value.len = attrSize;
            ctsHandleValuePair.value.val = attrValue;
        
            /* Get characteristic value from GATT database */
            if(CYBLE_GATT_ERR_NONE == 
                CyBle_GattsReadAttributeValue(&ctsHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
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
##Function Name: CyBle_CtssSendNotification
*******************************************************************************

Summary:
 Sends a notification to the client's device. A characteristic value also gets
 written to the GATT database.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of a service characteristic to be send as a notification
            to the Client device.
 attrSize: The size of the characteristic value attribute.
 attrValue: The pointer to the characteristic value data that should be 
            sent to the Client device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The characteristic notification was sent successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this.
                                     characteristic.
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the client.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CtssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
                                              CYBLE_CTS_CHAR_INDEX_T charIndex,
                                              uint8 attrSize,
                                              uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    
    if(CYBLE_CTS_CURRENT_TIME != charIndex)
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else 
    {
        apiResult = CyBle_CtssSetCharacteristicValue(charIndex, attrSize, attrValue);
        
        if(CYBLE_ERROR_OK == apiResult)
        {
            if(CYBLE_STATE_CONNECTED != CyBle_GetState())
            {
                apiResult = CYBLE_ERROR_INVALID_STATE;
            }
            else if((cyBle_ctss.currTimeCccdHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
                || (!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_ctss.currTimeCccdHandle)))
            {
                apiResult = CYBLE_ERROR_NTF_DISABLED;
            }
            else
            {
                CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;
                /* Fill all fields of write request structure ... */
                ntfReqParam.attrHandle = cyBle_ctss.currTimeCharHandle; 
                ntfReqParam.value.val = attrValue;
                ntfReqParam.value.len = attrSize;
                /* Send notification to client using previously filled structure */
                apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
            }
        }
    }

    /* Return status */
    return(apiResult);
}

#endif /* (CYBLE_CTS_SERVER) */


#ifdef CYBLE_CTS_CLIENT

/****************************************************************************** 
##Function Name: CyBle_CtscDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 discCharInfo: The pointer to a characteristic information structure.

Return:
 None

******************************************************************************/
void CyBle_CtscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    uint8 locCharIndex = (uint8) CYBLE_CTS_INVALID_CHAR_INDEX;

    /* Using characteristic UUID store handle of requested characteristic */
    switch(discCharInfo->uuid.uuid16)
    {
    case CYBLE_UUID_CHAR_CURRENT_TIME:
        locCharIndex = (uint8) CYBLE_CTS_CURRENT_TIME;
        break;

    case CYBLE_UUID_CHAR_LOCAL_TIME_INFO:
        locCharIndex = (uint8) CYBLE_CTS_LOCAL_TIME_INFO;
        break;

    case CYBLE_UUID_CHAR_REF_TIME_INFO:
        locCharIndex = (uint8) CYBLE_CTS_REFERENCE_TIME_INFO;
        break;

    default:
        break;
    }
    
    if(((uint8) CYBLE_CTS_CHAR_COUNT) > locCharIndex)
    {
        if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ctsc.currTimeCharacteristics[locCharIndex].valueHandle)
        {
            cyBle_ctsc.currTimeCharacteristics[locCharIndex].valueHandle = discCharInfo->valueHandle;
            cyBle_ctsc.currTimeCharacteristics[locCharIndex].properties  = discCharInfo->properties;
        }
        else
        {
            CyBle_ApplCallback(CYBLE_EVT_GATTC_CHAR_DUPLICATION, &discCharInfo->uuid);
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CtscDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event. 
 This event is generated when a server successfully sends the data for 
 CYBLE_EVT_GATTC_FIND_INFO_REQ" Based on the service UUID, an appropriate data 
 structure is populated to the service with a service callback.

Parameters:
 discCharInfo: The pointer to a characteristic information structure.

Return:
 None

******************************************************************************/
void CyBle_CtscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo)
{
    if(CYBLE_UUID_CHAR_CLIENT_CONFIG == discDescrInfo->uuid.uuid16)
    {
        if(cyBle_ctsc.currTimeCccdHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_ctsc.currTimeCccdHandle = discDescrInfo->descrHandle;
        }
        else    /* Duplication of descriptor */
        {
            CyBle_ApplCallback((uint32)CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CtscNotificationEventHandler
*******************************************************************************

Summary:
 Handles the Notification Event.

Parameters:
 eventParam: The pointer to the CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T data
             structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_CtscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    if(NULL != CyBle_CtsApplCallback)
    {
        if(cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_CURRENT_TIME].valueHandle == eventParam->handleValPair.attrHandle)
        {
            CYBLE_CTS_CHAR_VALUE_T ntfParam;
            
            ntfParam.connHandle = eventParam->connHandle;
            ntfParam.charIndex = CYBLE_CTS_CURRENT_TIME;
            ntfParam.value = &eventParam->handleValPair.value;
            
            CyBle_CtsApplCallback((uint32)CYBLE_EVT_CTSC_NOTIFICATION, (void *)&ntfParam);
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CtscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles the Read Response Event for the Current Time Service.

Parameters:
 eventParam: The pointer to the data that came with a read response for CTS.

Return:
 None

******************************************************************************/
void CyBle_CtscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    uint8 flag = 1u;
    uint8 attrVal = 0u;
    CYBLE_CTS_CHAR_INDEX_T idx;
    
    if((NULL != CyBle_CtsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ctscReqHandle))
    {
        if(cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_CURRENT_TIME].valueHandle == cyBle_ctscReqHandle)
        {
            idx = CYBLE_CTS_CURRENT_TIME;
        }
        else 
        if(cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_LOCAL_TIME_INFO].valueHandle == cyBle_ctscReqHandle)
        {
            idx = CYBLE_CTS_LOCAL_TIME_INFO;
        }
        else 
        if(cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_REFERENCE_TIME_INFO].valueHandle ==
            cyBle_ctscReqHandle)
        {
            idx = CYBLE_CTS_REFERENCE_TIME_INFO;
        }
        else if(cyBle_ctsc.currTimeCccdHandle == cyBle_ctscReqHandle)
        {
            /* Attribute is Characteristic Descriptor  */
            attrVal = 1u;
        }
        else
        {
            /* No CTS characteristic were requested for read */
            flag = 0u;
        }

        if(0u != flag)
        {
            /* Read response for characteristic */
            if(0u == attrVal)
            {
                CYBLE_CTS_CHAR_VALUE_T rdRspParam;

                /* Fill Current Time Service read response parameter structure with
                   characteristic info. */
                rdRspParam.connHandle = eventParam->connHandle;
                rdRspParam.charIndex = idx;
                rdRspParam.value = &eventParam->value;

                CyBle_CtsApplCallback((uint32)CYBLE_EVT_CTSC_READ_CHAR_RESPONSE, (void *)&rdRspParam);
            }
            else /* Read response for characteristic descriptor */
            {
                CYBLE_CTS_DESCR_VALUE_T rdRspParam;

                /* Fill Current Time Service read response parameter structure with
                    characteristic descriptor info. */
                rdRspParam.connHandle = eventParam->connHandle;
                rdRspParam.charIndex = CYBLE_CTS_CURRENT_TIME;
                rdRspParam.descrIndex = CYBLE_CTS_CURRENT_TIME_CCCD;
                rdRspParam.value = &eventParam->value;

                CyBle_CtsApplCallback((uint32)CYBLE_EVT_CTSC_READ_DESCR_RESPONSE, (void *)&rdRspParam);
            }

            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            cyBle_ctscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CtscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles the Write Response Event for the Current Time Service.

Parameters:
 eventParam: The pointer to the CYBLE_CONN_HANDLE_T data structure.

Return:
 None

******************************************************************************/
void CyBle_CtscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    if((NULL != CyBle_CtsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ctscReqHandle))
    {
        if(cyBle_ctsc.currTimeCccdHandle == cyBle_ctscReqHandle)
        {
            CYBLE_CTS_DESCR_VALUE_T wrRspParam;
            
            wrRspParam.connHandle = *eventParam;
            wrRspParam.charIndex = CYBLE_CTS_CURRENT_TIME;
            wrRspParam.descrIndex = CYBLE_CTS_CURRENT_TIME_CCCD;
            wrRspParam.value = NULL;
            
            cyBle_ctscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            CyBle_CtsApplCallback((uint32)CYBLE_EVT_CTSC_WRITE_DESCR_RESPONSE, (void *)&wrRspParam);
        }
        else if((cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_CURRENT_TIME].valueHandle == cyBle_ctscReqHandle) ||
            (cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_LOCAL_TIME_INFO].valueHandle == cyBle_ctscReqHandle))
        {
            CYBLE_CTS_CHAR_VALUE_T wrRspParam;
            
            if((cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_CURRENT_TIME].valueHandle == cyBle_ctscReqHandle))
            {
                wrRspParam.charIndex = CYBLE_CTS_CURRENT_TIME;
            }
            else
            {
                wrRspParam.charIndex = CYBLE_CTS_LOCAL_TIME_INFO;
            }
            wrRspParam.connHandle = *eventParam;
            wrRspParam.value = NULL;
            
            cyBle_ctscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            CyBle_CtsApplCallback((uint32)CYBLE_EVT_CTSC_WRITE_CHAR_RESPONSE, (void *)&wrRspParam);
        }
        else
        {
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CtscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles the Error Response Event for the Current Time Service.

Parameters:
 eventParam: The pointer to the CYBLE_GATTC_ERR_RSP_PARAM_T structure.

Return:
 None.

******************************************************************************/
void CyBle_CtscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ctscReqHandle)
    {
        cyBle_ctscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


/****************************************************************************** 
##Function Name: CyBle_CtscGetCharacteristicValue
*******************************************************************************

Summary:
 Gets a characteristic value of the Current Time Service, which is identified 
 by charIndex.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of a service characteristic.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                     characteristic.
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Peer device doesn't have a 
                                               particular characteristic.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CtscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_CTS_CHAR_INDEX_T charIndex)
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
        case CYBLE_CTS_CURRENT_TIME:
            tmpCharHandle = cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_CURRENT_TIME].valueHandle;
            break;
        case CYBLE_CTS_LOCAL_TIME_INFO:
            tmpCharHandle = cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_LOCAL_TIME_INFO].valueHandle;
            break;
        case CYBLE_CTS_REFERENCE_TIME_INFO:
            tmpCharHandle = cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_REFERENCE_TIME_INFO].valueHandle;
            break;
        default:
            /* Characteristic wasn't found */
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            break;
        }
        
        if(CYBLE_ERROR_OK == apiResult)
        {
            if(tmpCharHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                /* Send request to read characteristic value */
                apiResult = CyBle_GattcReadCharacteristicValue(connHandle, tmpCharHandle);
            
                if(CYBLE_ERROR_OK == apiResult)
                {
                    cyBle_ctscReqHandle = tmpCharHandle;
                }
            }
            else
            {
                apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
            }
        }
    }

    /* Return status */
    return(apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CtscSetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sets a characteristic descriptor of the Current Time Characteristic of the 
 Current Time Service.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the Current Time Service characteristic.
 descrIndex: The index of the Current Time Service characteristic descriptor.
 attrSize: The size of the characteristic descriptor attribute.
 attrValue: Pointer to the characteristic descriptor value data that should be 
            sent to the server device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The  request was sent successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
                                     specified attribute.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Peer device doesn't have a 
                                               particular descriptor.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CtscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                         CYBLE_CTS_CHAR_INDEX_T charIndex,
                                                         CYBLE_CTS_CHAR_DESCRIPTORS_T descrIndex,
                                                         uint8 attrSize,
                                                         uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((NULL != attrValue)
            && (CYBLE_CTS_CURRENT_TIME == charIndex) 
            && (CYBLE_CTS_CURRENT_TIME_CCCD == descrIndex)
            && (CYBLE_CCCD_LEN == attrSize))
    {
        CYBLE_GATTC_WRITE_REQ_T writeReqParam;
        
        if(cyBle_ctsc.currTimeCccdHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            /* Fill all fields of write request structure ... */
            writeReqParam.value.val = attrValue;
            writeReqParam.value.len = attrSize;
            writeReqParam.attrHandle = cyBle_ctsc.currTimeCccdHandle;
            
            /* ... and send request to server device. */        
            apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
            
            if(CYBLE_ERROR_OK == apiResult)
            {
                cyBle_ctscReqHandle = cyBle_ctsc.currTimeCccdHandle;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }
    else
    {
        /* Parameter validation failed */   
    }
    
    /* Return the status */
    return(apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CtscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets a characteristic descriptor of the Current Time Characteristic of the 
 Current Time Service.

Parameters:
 CYBLE_CONN_HANDLE_T connHandle: The connection handle.
 CYBLE_CTS_CHAR_INDEX_T charIndex: The index of the service characteristic.
 CYBLE_CTS_CHAR_DESCRIPTORS_T descrIndex: The index of a service 
                                           characteristic descriptor.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
  * CYBLE_ERROR_INVALID_STATE - State is not valid.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
                                     specified attribute.
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Peer device doesn't have a 
                                               particular descriptor.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CtscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                   CYBLE_CTS_CHAR_INDEX_T charIndex,
                                                   uint8 descrIndex)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((CYBLE_CTS_CURRENT_TIME == charIndex) && (((uint8) CYBLE_CTS_CURRENT_TIME_CCCD) == descrIndex))
    {
        if(cyBle_ctsc.currTimeCccdHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, cyBle_ctsc.currTimeCccdHandle);

            if(CYBLE_ERROR_OK == apiResult)
            {
                cyBle_ctscReqHandle = cyBle_ctsc.currTimeCccdHandle;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }
    else
    {
        /* Validation of input parameters failed */
    }

    /* Return status */
    return(apiResult);
}

#endif /* (CYBLE_CTS_CLIENT) */


/* [] END OF FILE */
