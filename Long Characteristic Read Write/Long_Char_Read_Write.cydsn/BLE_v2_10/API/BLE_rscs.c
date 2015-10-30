/*******************************************************************************
File Name: CYBLE_rscs.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the source code for Running Speed and Cadence Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_RscsApplCallback = NULL;

#if defined(CYBLE_RSCS_SERVER)

`$RscsServer`
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_rscssReqHandle;
    
#endif /* CYBLE_RSCS_SERVER */

#if defined(CYBLE_RSCS_CLIENT)

CYBLE_RSCSC_T cyBle_rscsc;
    
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_rscscReqHandle;

#endif /* (CYBLE_RSCS_CLIENT) */


/******************************************************************************
##Function Name: CyBle_RscsInit
*******************************************************************************

Summary:
 This function initializes the Running Speed and Cadence Service.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_RscsInit(void)
{

#if defined(CYBLE_RSCS_SERVER)

    cyBle_rscssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    
#endif /* CYBLE_RSCS_SERVER */

#ifdef CYBLE_RSCS_CLIENT

    (void)memset(&cyBle_rscsc, 0, sizeof(cyBle_rscsc));
    
    cyBle_rscscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_RSCS_CLIENT */
}


/******************************************************************************
##Function Name: CyBle_RscsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for Running Speed and Cadence Service specific
 attribute operations.

Parameters:
 callbackFunc: An application layer event callback function to receive 
                    events from the BLE Component. The definition of 
                    CYBLE_CALLBACK_T for RSCS is,
            
                    typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                      void *eventParam)

                    * eventCode indicates the event that triggered this 
                      callback.
                    * eventParam contains the parameters corresponding to the 
                      current event.

Return:
 None

******************************************************************************/
void CyBle_RscsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_RscsApplCallback = callbackFunc;
}


#if defined(CYBLE_RSCS_SERVER)

/******************************************************************************
##Function Name: CyBle_RscssWriteEventHandler
*******************************************************************************

Summary:
 Handles the Write Request Event for the Running Speed and Cadence Service.

Parameters:
 CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam: The pointer to the data that came 
                                            with a write request for the
                                            Running Speed and Cadence Service.

Return:
 Return a value of type CYBLE_GATT_ERR_CODE_T:
  * CYBLE_GATT_ERR_NONE - Function terminated successfully.
  * CYBLE_GATT_ERR_INVALID_HANDLE - The handle of a Client Configuration 
                                    Characteristic Descriptor is not valid.
  * CYBLE_GATT_ERR_UNLIKELY_ERROR - An Internal Stack error occurred.
  * CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED - The notification or indication 
                                           property of a specific 
                                           Characteristic of Running Speed and
                                           Cadence Service is disabled.

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_RscssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_RSCS_CHAR_VALUE_T wrReqParam;
    CYBLE_GATT_DB_ATTR_HANDLE_T tmpHandle;
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    uint32 event;
    uint8 handleWasFound = 1u;
    
    tmpHandle = eventParam->handleValPair.attrHandle;
    
    if(NULL != CyBle_RscsApplCallback)
    {
        /* Client Characteristic Configuration Descriptor Write Request */
        if(tmpHandle == cyBle_rscss.charInfo[CYBLE_RSCS_RSC_MEASUREMENT].descrHandle[CYBLE_RSCS_CCCD])
        {   
            /* Verify that optional notification property is enabled for the Characteristic */
            if(CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_rscss.charInfo[CYBLE_RSCS_RSC_MEASUREMENT].charHandle))
            {
                if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                {
                    event = (uint32)CYBLE_EVT_RSCSS_NOTIFICATION_ENABLED;
                }
                else
                {
                    event = (uint32)CYBLE_EVT_RSCSS_NOTIFICATION_DISABLED;
                }
                
                wrReqParam.charIndex = CYBLE_RSCS_RSC_MEASUREMENT;
                
                /* Value is NULL for descriptors */
                wrReqParam.value = NULL;
            
            #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
                (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
                /* Set flag to store bonding data to flash */
                cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
            #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
                
            }
            else
            {
                gattErr = CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED;
            }
        }
        else if(tmpHandle == cyBle_rscss.charInfo[CYBLE_RSCS_SC_CONTROL_POINT].descrHandle[CYBLE_RSCS_CCCD])
        {
            /* Verify that optional indication property is enabled for the Characteristic */
            if(CYBLE_IS_INDICATION_SUPPORTED(cyBle_rscss.charInfo[CYBLE_RSCS_SC_CONTROL_POINT].charHandle))
            {
                if(CYBLE_IS_INDICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                {
                    event = (uint32)CYBLE_EVT_RSCSS_INDICATION_ENABLED;
                }
                else
                {
                    event = (uint32)CYBLE_EVT_RSCSS_INDICATION_DISABLED;
                }
                
                wrReqParam.charIndex = CYBLE_RSCS_SC_CONTROL_POINT;
                
                /* Value is NULL for descriptors */
                wrReqParam.value = NULL;
            
            #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
                (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
                /* Set flag to store bonding data to flash */
                cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
            #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
            }
            else
            {
                gattErr = CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED;
            }
        }
        else if(tmpHandle == cyBle_rscss.charInfo[CYBLE_RSCS_SC_CONTROL_POINT].charHandle)
        {
            if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_rscssReqHandle)
            {
                /* Verify if indication are enabled for SC Control Point Characteristic */
                if(CYBLE_IS_INDICATION_SUPPORTED(cyBle_rscss.charInfo[CYBLE_RSCS_SC_CONTROL_POINT].charHandle))
                {
                    if(CYBLE_IS_INDICATION_ENABLED(cyBle_rscss.charInfo[CYBLE_RSCS_SC_CONTROL_POINT].descrHandle[CYBLE_RSCS_CCCD]))
                    {
                        event = (uint32)CYBLE_EVT_RSCSS_CHAR_WRITE;
                        wrReqParam.charIndex = CYBLE_RSCS_RSC_MEASUREMENT;
                        wrReqParam.value = &eventParam->handleValPair.value;
                    }
                    else
                    {
                        gattErr = CYBLE_GATTS_ERR_CCCD_IMPROPERLY_CONFIGURED;
                    }
                }
                else
                {
                    gattErr = CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED;
                }
            }
            else
            {
                gattErr = CYBLE_GATTS_ERR_PROCEDURE_ALREADY_IN_PROGRESS;
                cyBle_rscssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            }
        }
        else
        {
            /* No valid service handles were found */
            handleWasFound = 0u;
        }
        
        if((CYBLE_GATT_ERR_NONE == gattErr) && (0u != handleWasFound))
        {
            /* Write value to GATT database */
            gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
            
            if(CYBLE_GATT_ERR_NONE == gattErr)
            {
                wrReqParam.connHandle = eventParam->connHandle;

                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                
                CyBle_RscsApplCallback(event, &wrReqParam);
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


/*******************************************************************************
##Function Name: CyBle_RscssConfirmationEventHandler
********************************************************************************

Summary:
 Handles the Value Confirmation request event from the BLE stack.

Parameters:
 CYBLE_CONN_HANDLE_T *event_params: Pointer to a structure of type 
                                    CYBLE_CONN_HANDLE_T.

Return:
 None

*******************************************************************************/
void CyBle_RscssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    CYBLE_RSCS_CHAR_VALUE_T locCharValue;

    if((NULL != CyBle_RscsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_rscssReqHandle))
    {
        if(cyBle_rscssReqHandle == cyBle_rscss.charInfo[CYBLE_RSCS_SC_CONTROL_POINT].charHandle)
        {
            locCharValue.connHandle = *eventParam;
            locCharValue.charIndex = CYBLE_RSCS_SC_CONTROL_POINT;
            locCharValue.value = NULL;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            cyBle_rscssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            CyBle_RscsApplCallback((uint32)CYBLE_EVT_RSCSS_INDICATION_CONFIRMATION, &locCharValue);
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_RscssSetCharacteristicValue
********************************************************************************

Summary:
 Sets the characteristic value of the Running Speed and Cadence Service in the
 local GATT database. The characteristic is identified by charIndex.

Parameters:
 charIndex: The index of a service characteristic. Valid values are,
             * CYBLE_RSCS_RSC_FEATURE
             * CYBLE_RSCS_SENSOR_LOCATION.
 attrSize:  The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be 
              stored in the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RscssSetCharacteristicValue(CYBLE_RSCS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((CYBLE_RSCS_RSC_FEATURE == charIndex) || (CYBLE_RSCS_SENSOR_LOCATION == charIndex))
    {
        if(cyBle_rscss.charInfo[charIndex].charHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            /* Store data in database */
            locHandleValuePair.attrHandle = cyBle_rscss.charInfo[charIndex].charHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;

            if(CYBLE_GATT_ERR_NONE ==
                CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
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


/*******************************************************************************
##Function Name: CyBle_RscssGetCharacteristicValue
********************************************************************************

Summary:
 Gets the characteristic value of the Running Speed and Cadence Service from
 the GATT database. The characteristic is identified by charIndex.

Parameters:
 charIndex: The index of a service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the location where characteristic value data 
             should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Peer device doesn't have a 
                                               particular characteristic

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RscssGetCharacteristicValue(CYBLE_RSCS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex < CYBLE_RSCS_CHAR_COUNT)
    {
        if(cyBle_rscss.charInfo[charIndex].charHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            /* Read characteristic value from database */
            locHandleValuePair.attrHandle = cyBle_rscss.charInfo[charIndex].charHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;
            
            if(CYBLE_GATT_ERR_NONE == 
                CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
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


/*******************************************************************************
##Function Name: CyBle_RscssGetCharacteristicDescriptor
********************************************************************************

Summary:
 Gets the characteristic descriptor of a specified characteristic of the Running
 Speed and Cadence Service from the GATT database.

Parameters:
 charIndex: The index of a service characteristic. Valid values are,
             * CYBLE_RSCS_RSC_MEASUREMENT 
             * CYBLE_RSCS_SC_CONTROL_POINT
 descrIndex: The index of a service characteristic descriptor. Valid value is,
             * CYBLE_RSCS_CCCD
 attrSize: The size of the characteristic descriptor attribute.
 *attrValue: The pointer to the location where characteristic descriptor value
              data should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Peer device doesn't have a 
                                               particular descriptor

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RscssGetCharacteristicDescriptor(CYBLE_RSCS_CHAR_INDEX_T charIndex,
    CYBLE_RSCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(((CYBLE_RSCS_RSC_MEASUREMENT == charIndex) || (CYBLE_RSCS_SC_CONTROL_POINT == charIndex)) && 
        (descrIndex == CYBLE_RSCS_CCCD))
    {
        locHandleValuePair.attrHandle = cyBle_rscss.charInfo[charIndex].descrHandle[descrIndex];
     
        if(locHandleValuePair.attrHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;

            if(CYBLE_GATT_ERR_NONE ==
                CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
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


/*******************************************************************************
##Function Name: CyBle_RscssSendNotification
********************************************************************************

Summary:
 Sends a notification with the characteristic value to the Client device. This
 is specified by charIndex of the Running Speed and Cadence Service.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of a service characteristic. Valid value is, 
             * CYBLE_RSCS_RSC_MEASUREMENT
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be sent
              to the client device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of input parameter is failed
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this.
                                     characteristic.
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the client
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RscssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_RSCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    /* Store new data in database */
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    
    if(charIndex == CYBLE_RSCS_RSC_MEASUREMENT)  
    {
        /* Send Notification if it is enabled and connected */
        if(CYBLE_STATE_CONNECTED != CyBle_GetState())
        {
            apiResult = CYBLE_ERROR_INVALID_STATE;
        }
        else if(!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_rscss.charInfo[charIndex].descrHandle[CYBLE_RSCS_CCCD]))
        {
            apiResult = CYBLE_ERROR_NTF_DISABLED;
        }
        else
        {
            CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;
            
            /* Fill all fields of write request structure ... */
            ntfReqParam.attrHandle = cyBle_rscss.charInfo[charIndex].charHandle;
            ntfReqParam.value.val = attrValue;
            ntfReqParam.value.len = attrSize;
            
            /* Send notification to client using previously filled structure */
            apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
        }
    }
    return (apiResult);
}


/*******************************************************************************
##Function Name: CyBle_RscssSendIndication
********************************************************************************

Summary:
 Sends an indication with a characteristic value to the Client device. This is 
 specified by charIndex of the Running Speed and Cadence Service.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of a service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be sent
              to the client device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of input parameter is failed
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this.
                                     characteristic.
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_IND_DISABLED - Indication is not enabled by the client
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Peer device doesn't have a 
                                               particular characteristic

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RscssSendIndication(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_RSCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    /* Store new data in database */
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    
    if(charIndex == CYBLE_RSCS_SC_CONTROL_POINT)
    {
        /* Send Indication if it is enabled and connected */
        if(CYBLE_STATE_CONNECTED != CyBle_GetState())
        {
            apiResult = CYBLE_ERROR_INVALID_STATE;
        }
        else if(!CYBLE_IS_INDICATION_ENABLED(cyBle_rscss.charInfo[charIndex].descrHandle[CYBLE_RSCS_CCCD]))
        {
            apiResult = CYBLE_ERROR_IND_DISABLED;
        }
        else
        {
            if(cyBle_rscss.charInfo[charIndex].charHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                CYBLE_GATTS_HANDLE_VALUE_IND_T indReqParam;
                
                /* Fill all fields of write request structure ... */
                indReqParam.attrHandle = cyBle_rscss.charInfo[charIndex].charHandle;
                indReqParam.value.val = attrValue;
                indReqParam.value.len = attrSize;
                
                /* Send indication to client using previously filled structure */
                apiResult = CyBle_GattsIndication(connHandle, &indReqParam);
                /* Save handle to support service specific value confirmation response from client */
                if(apiResult == CYBLE_ERROR_OK)
                {
                    cyBle_rscssReqHandle = indReqParam.attrHandle;
                }
            }
            else
            {
                apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
            }
        }
    }
    return (apiResult);
}

#endif /* CYBLE_RSCS_SERVER */


#if defined(CYBLE_RSCS_CLIENT)

/*******************************************************************************
##Function Name: CyBle_RscscDiscoverCharacteristicsEventHandler
********************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 CYBLE_DISC_CHAR_INFO_T *discCharInfo: The pointer to a characteristic
                                       information structure.

Return:
 None

*******************************************************************************/
void CyBle_RscscDiscoverCharacteristicsEventHandler(const CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    /* RSCS characteristics UUIDs */
    static const CYBLE_UUID16 cyBle_rscscCharUuid[CYBLE_RSCS_CHAR_COUNT] =
    {
        CYBLE_UUID_CHAR_RSC_MSRMT,
        CYBLE_UUID_CHAR_RSC_FEATURE,
        CYBLE_UUID_CHAR_SENSOR_LOCATION,
        CYBLE_UUID_CHAR_SC_CONTROL_POINT
    };
    uint8 i;
    static CYBLE_GATT_DB_ATTR_HANDLE_T *rscsLastEndHandle = NULL;

    /* Update last characteristic endHandle to declaration handle of this characteristic */ 
    if(rscsLastEndHandle != NULL)
    {
        *rscsLastEndHandle = discCharInfo->charDeclHandle - 1u;
        rscsLastEndHandle = NULL;
    }

    for(i = (uint8) CYBLE_RSCS_RSC_MEASUREMENT; i < (uint8) CYBLE_RSCS_CHAR_COUNT; i++)
    {
        if(cyBle_rscscCharUuid[i] == discCharInfo->uuid.uuid16)
        {
            cyBle_rscsc.characteristics[i].charInfo.valueHandle = discCharInfo->valueHandle;
            cyBle_rscsc.characteristics[i].charInfo.properties  = discCharInfo->properties;
            rscsLastEndHandle = &cyBle_rscsc.characteristics[i].endHandle;
            break;
        }
    }

    /* Initially characteristic endHandle equals to the Service endHandle.
    * Characteristic endHandle will be updated to declaration handler of the
    * following characteristic,in the following characteristic discovery 
    * procedure.
    */
    if(rscsLastEndHandle != NULL)
    {
        *rscsLastEndHandle = cyBle_serverInfo[cyBle_disCount].range.endHandle;
    }
}


/*******************************************************************************
##Function Name: CyBle_RscsDiscoverCharDescriptorsEventHandler
********************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event. 
 This event is generated when a server successfully sends the data for 
 CYBLE_EVT_GATTC_FIND_INFO_REQ. Based on the service UUID, an appropriate data 
 structure is populated to the service with a service callback.

Parameters:
 CYBLE_DISC_DESCR_INFO_T * discCharInfo: The pointer to a characteristic information
                                         structure.

Return:
 None

*******************************************************************************/
void CyBle_RscscDiscoverCharDescriptorsEventHandler(CYBLE_RSCS_CHAR_INDEX_T charIndex,
                                                    CYBLE_DISC_DESCR_INFO_T * discDescrInfo)
{
    if(CYBLE_UUID_CHAR_CLIENT_CONFIG == discDescrInfo->uuid.uuid16)
    {
        if((cyBle_rscsc.characteristics[CYBLE_RSCS_RSC_MEASUREMENT].descriptors[CYBLE_RSCS_CCCD] ==
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE) && (CYBLE_RSCS_RSC_MEASUREMENT == charIndex))
        {
            cyBle_rscsc.characteristics[CYBLE_RSCS_RSC_MEASUREMENT].descriptors[CYBLE_RSCS_CCCD] =
                discDescrInfo->descrHandle;
        }
        else if((cyBle_rscsc.characteristics[CYBLE_RSCS_SC_CONTROL_POINT].descriptors[CYBLE_RSCS_CCCD] ==
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE) && (CYBLE_RSCS_SC_CONTROL_POINT == charIndex))
        {
            cyBle_rscsc.characteristics[CYBLE_RSCS_SC_CONTROL_POINT].descriptors[CYBLE_RSCS_CCCD] =
                discDescrInfo->descrHandle;
        }
        else    /* Duplication of descriptor */
        {
            CyBle_ApplCallback((uint32)CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_RscscNotificationEventHandler
********************************************************************************

Summary:
 Handles the Notification Event for the Running Speed and Cadence Service.

Parameters:
 CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam: The pointer to the 
                                                   CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T
                                                   data structure specified by
                                                   the event.

Return:
 None.

*******************************************************************************/
void CyBle_RscscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    if(NULL != CyBle_RscsApplCallback)
    {
        if(cyBle_rscsc.characteristics[CYBLE_RSCS_RSC_MEASUREMENT].charInfo.valueHandle == 
            eventParam->handleValPair.attrHandle)
        {
            CYBLE_RSCS_CHAR_VALUE_T ntfParam;
            
            ntfParam.charIndex = CYBLE_RSCS_RSC_MEASUREMENT;
            ntfParam.connHandle = eventParam->connHandle;
            ntfParam.value = &eventParam->handleValPair.value;
            
            CyBle_RscsApplCallback((uint32)CYBLE_EVT_RSCSC_NOTIFICATION, (void *)&ntfParam);
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_RscscIndicationEventHandler
********************************************************************************

Summary:
 Handles the Indication Event for the Running Speed and Cadence Service.

Parameters:
 CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam: The pointer to the 
                                                   CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T
                                                   data structure specified by the event.

Return:
 None.

*******************************************************************************/
void CyBle_RscscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam)
{
    if(NULL != CyBle_RscsApplCallback)
    {
        if(cyBle_rscsc.characteristics[CYBLE_RSCS_SC_CONTROL_POINT].charInfo.valueHandle == 
            eventParam->handleValPair.attrHandle)
        {
            CYBLE_RSCS_CHAR_VALUE_T ntfParam;

            ntfParam.charIndex = CYBLE_RSCS_SC_CONTROL_POINT;
            ntfParam.connHandle = eventParam->connHandle;
            ntfParam.value = &eventParam->handleValPair.value;
            
            CyBle_RscsApplCallback((uint32)CYBLE_EVT_RSCSC_INDICATION, (void *)&ntfParam);
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_RscscReadResponseEventHandler
********************************************************************************

Summary:
 Handles the Read Response Event for the Running Speed and Cadence Service.

Parameters:
 CYBLE_GATTC_READ_RSP_PARAM_T *eventParam: The pointer to the data that came 
                                           with a read response for RSCS.

Return:
 None.

*******************************************************************************/
void CyBle_RscscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    uint8 flag = 1u;
    uint8 attrVal = 0u;
    CYBLE_RSCS_CHAR_INDEX_T idx;
    
    if((NULL != CyBle_RscsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_rscscReqHandle))
    {
        if(cyBle_rscsc.characteristics[CYBLE_RSCS_RSC_FEATURE].charInfo.valueHandle == cyBle_rscscReqHandle)
        {
            idx = CYBLE_RSCS_RSC_FEATURE;
        }
        else if(cyBle_rscsc.characteristics[CYBLE_RSCS_SENSOR_LOCATION].charInfo.valueHandle == cyBle_rscscReqHandle)
        {
            idx = CYBLE_RSCS_SENSOR_LOCATION;
        }
        else if(cyBle_rscsc.characteristics[CYBLE_RSCS_RSC_MEASUREMENT].descriptors[CYBLE_RSCS_CCCD] == 
            cyBle_rscscReqHandle)
        {
            /* Attribute is Characteristic Descriptor */
            attrVal = 1u;
            idx = CYBLE_RSCS_RSC_MEASUREMENT;
        }
        else if(cyBle_rscsc.characteristics[CYBLE_RSCS_SC_CONTROL_POINT].descriptors[CYBLE_RSCS_CCCD] == 
            cyBle_rscscReqHandle)
        {
            /* Attribute is Characteristic Descriptor */
            attrVal = 1u;
            idx = CYBLE_RSCS_SC_CONTROL_POINT;
        }
        else
        {
            /* No RSCS Characteristic were requested for read */
            flag = 0u;
        }

        if(0u != flag)
        {
            /* Read response for characteristic */
            if(0u == attrVal)
            {
                CYBLE_RSCS_CHAR_VALUE_T rdRspParam;

                /* Fill Running Speed and Cadence Service read response parameter structure with
                  Characteristic info. */
                rdRspParam.connHandle = eventParam->connHandle;
                rdRspParam.charIndex = idx;
                rdRspParam.value = &eventParam->value;

                CyBle_RscsApplCallback((uint32)CYBLE_EVT_RSCSC_READ_CHAR_RESPONSE, (void *)&rdRspParam);
            }
            else /* Read response for characteristic descriptor */
            {
                CYBLE_RSCS_DESCR_VALUE_T rdRspParam;

                /* Fill Running Speed and Cadence Service read response parameter structure with
                  Characteristic Descriptor info. */
                rdRspParam.connHandle = eventParam->connHandle;
                rdRspParam.charIndex = idx;
                rdRspParam.descrIndex = CYBLE_RSCS_CCCD;
                rdRspParam.value = &eventParam->value;

                CyBle_RscsApplCallback((uint32)CYBLE_EVT_RSCSC_READ_DESCR_RESPONSE, (void *)&rdRspParam);
            }

            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            cyBle_rscscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_RscscWriteResponseEventHandler
********************************************************************************

Summary:
 Handles the Write Response Event for the Running Speed and Cadence Service.

Parameters:
 CYBLE_CONN_HANDLE_T *eventParam: The pointer to the CYBLE_CONN_HANDLE_T data 
                                  structure.

Return:
 None.

*******************************************************************************/
void CyBle_RscscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    uint8 flag = 1u;
    uint8 attrType = 0u;
    CYBLE_RSCS_CHAR_INDEX_T idx;

    if((NULL != CyBle_RscsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_rscscReqHandle))
    {
        if(cyBle_rscsc.characteristics[CYBLE_RSCS_RSC_FEATURE].charInfo.valueHandle == cyBle_rscscReqHandle)
        {
            idx = CYBLE_RSCS_RSC_FEATURE;
        }
        else if(cyBle_rscsc.characteristics[CYBLE_RSCS_SENSOR_LOCATION].charInfo.valueHandle == cyBle_rscscReqHandle)
        {
            idx = CYBLE_RSCS_SENSOR_LOCATION;
        }
        else if(cyBle_rscsc.characteristics[CYBLE_RSCS_SC_CONTROL_POINT].charInfo.valueHandle == cyBle_rscscReqHandle)
        {
            idx = CYBLE_RSCS_SC_CONTROL_POINT;
        }
        else if(cyBle_rscsc.characteristics[CYBLE_RSCS_RSC_MEASUREMENT].descriptors[CYBLE_RSCS_CCCD] == 
            cyBle_rscscReqHandle)
        {
            /* Attribute is Characteristic Descriptor */
            attrType = 1u;
            idx = CYBLE_RSCS_RSC_MEASUREMENT;
        }
        else if(cyBle_rscsc.characteristics[CYBLE_RSCS_SC_CONTROL_POINT].descriptors[CYBLE_RSCS_CCCD] == 
            cyBle_rscscReqHandle)
        {
            /* Attribute is Characteristic Descriptor */
            attrType = 1u;
            idx = CYBLE_RSCS_SC_CONTROL_POINT;
        }
        else
        {
            /* No RSCS Characteristic were requested for write */
            flag = 0u;
        }
        
        if(0u != flag)
        {
            /* This should be cleared before calling to */
            cyBle_rscscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            
            /* Write response for characteristic */
            if(0u == attrType)
            {
                CYBLE_RSCS_CHAR_VALUE_T rdRspParam;

                /* Fill Running Speed and Cadence Service write response parameter structure with
                  Characteristic info. */
                rdRspParam.connHandle = *eventParam;
                rdRspParam.charIndex = idx;
                rdRspParam.value = NULL;

                CyBle_RscsApplCallback((uint32)CYBLE_EVT_RSCSC_WRITE_CHAR_RESPONSE, (void *)&rdRspParam);
            }
            else /* Write response for characteristic descriptor */
            {
                CYBLE_RSCS_DESCR_VALUE_T rdRspParam;

                /* Fill Running Speed and Cadence Service write response parameter structure with
                  Characteristic Descriptor info. */
                rdRspParam.connHandle = *eventParam;
                rdRspParam.charIndex = idx;
                rdRspParam.descrIndex = CYBLE_RSCS_CCCD;
                rdRspParam.value = NULL;

                CyBle_RscsApplCallback((uint32)CYBLE_EVT_RSCSC_WRITE_DESCR_RESPONSE, (void *)&rdRspParam);
            }

            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_RscscErrorResponseEventHandler
********************************************************************************

Summary:
 Handles the Error Response Event for the Running Speed and Cadence Service.

Parameters:
 CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam: The pointer to the 
                                          CYBLE_GATTC_ERR_RSP_PARAM_T structure.

Return:
 None.

*******************************************************************************/
void CyBle_RscscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_rscscReqHandle)
    {
        cyBle_rscscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


/*******************************************************************************
##Function Name: CyBle_RscscSetCharacteristicValue
********************************************************************************

Summary:
 Sends a request to the peer device to get the characteristic descriptor of 
 the specified characteristic of the Running Speed and Cadence Service.

Parameters:
 connHandle: The connection handle.
 charIndex:  The index of a service characteristic.
 attrSize:   Size of the characteristic value attribute.
 *attrValue: Pointer to the characteristic value data that should be
             sent to the server device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                     characteristic.
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Peer device doesn't have a 
                                               particular characteristic

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RscscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_RSCS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_GATTC_WRITE_CMD_REQ_T wrReqParam;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((NULL != attrValue) && (CYBLE_RSCS_SC_CONTROL_POINT == charIndex))
    {
        if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=  
            cyBle_rscsc.characteristics[CYBLE_RSCS_SC_CONTROL_POINT].charInfo.valueHandle)
        {
            /* Fill all fields of write command request structure ... */
            wrReqParam.value.val = attrValue;
            wrReqParam.value.len = attrSize;
            wrReqParam.attrHandle = cyBle_rscsc.characteristics[CYBLE_RSCS_SC_CONTROL_POINT].charInfo.valueHandle;

            /* Send request to write characteristic value */
            apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &wrReqParam);
            
            if(CYBLE_ERROR_OK == apiResult)
            {
                cyBle_rscscReqHandle = wrReqParam.attrHandle;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }
    else
    {
        /* apiResult equals to CYBLE_ERROR_INVALID_PARAMETER */
    }

    return(apiResult);
}


/*******************************************************************************
##Function Name: CyBle_RscscGetCharacteristicValue
********************************************************************************

Summary:
 Sends a request to the peer device to set the characteristic value of the 
 Running Speed and Cadence Service.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                     characteristic.
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Peer device doesn't have a 
                                               particular characteristic

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RscscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_RSCS_CHAR_INDEX_T charIndex)
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
        case CYBLE_RSCS_RSC_FEATURE:
            tmpCharHandle = cyBle_rscsc.characteristics[CYBLE_RSCS_RSC_FEATURE].charInfo.valueHandle;
            break;
        case CYBLE_RSCS_SENSOR_LOCATION:
            tmpCharHandle = cyBle_rscsc.characteristics[CYBLE_RSCS_SENSOR_LOCATION].charInfo.valueHandle;
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
                /* Send request to read characteristicW value */
                apiResult = CyBle_GattcReadCharacteristicValue(connHandle, tmpCharHandle);
            
                if(CYBLE_ERROR_OK == apiResult)
                {
                    cyBle_rscscReqHandle = tmpCharHandle;
                }
            }
            else
            {
                apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
            }
        }
    }

    return(apiResult);
}


/*******************************************************************************
##Function Name: CyBle_RscscSetCharacteristicDescriptor
********************************************************************************

Summary:
 Sends a request to the peer device to get the characteristic descriptor of 
 the specified characteristic of the Running Speed and Cadence Service.

Parameters:
 connHandle: The connection handle.
 charIndex:  The index of a RSCS characteristic.
 descrIndex: The index of a RSCS characteristic descriptor.
 attrSize:   The size of the characteristic descriptor attribute.
 *attrValue: The pointer to the characteristic descriptor value data that 
             should be sent to the server device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - the  request was sent successfully
  * CYBLE_ERROR_INVALID_STATE - connection with the client is not established
  * CYBLE_ERROR_INVALID_PARAMETER - validation of the input parameters failed
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                     characteristic
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Peer device doesn't have a 
                                               particular descriptor

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RscscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                         CYBLE_RSCS_CHAR_INDEX_T charIndex,
                                                         CYBLE_RSCS_DESCR_INDEX_T descrIndex,
                                                         uint8 attrSize,
                                                         uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((NULL != attrValue) && (CYBLE_RSCS_CCCD == descrIndex) && (CYBLE_CCCD_LEN == attrSize))
    {
        if((CYBLE_RSCS_RSC_MEASUREMENT == charIndex) || (CYBLE_RSCS_SC_CONTROL_POINT == charIndex))
        {
            CYBLE_GATTC_WRITE_REQ_T writeReqParam;
            
            /* Fill all fields of write request structure ... */
            writeReqParam.value.val = attrValue;
            writeReqParam.value.len = attrSize;
          
            if(CYBLE_RSCS_RSC_MEASUREMENT == charIndex)
            {
                writeReqParam.attrHandle =
                    cyBle_rscsc.characteristics[CYBLE_RSCS_RSC_MEASUREMENT].descriptors[CYBLE_RSCS_CCCD];
            }
            else
            {
                writeReqParam.attrHandle =
                    cyBle_rscsc.characteristics[CYBLE_RSCS_SC_CONTROL_POINT].descriptors[CYBLE_RSCS_CCCD];
            }

            if(writeReqParam.attrHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                /* ... and send request to server device. */        
                apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
                
                if(CYBLE_ERROR_OK == apiResult)
                {
                    cyBle_rscscReqHandle =
                        cyBle_rscsc.characteristics[charIndex].descriptors[CYBLE_RSCS_CCCD];
                }
            }
            else
            {
                apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
            }
        }
    }
    else
    {
        /* alidation of input parameters is failed */   
    }
    
    /* Return the status */
    return(apiResult);
}


/*******************************************************************************
##Function Name: CyBle_RscscGetCharacteristicDescriptor
********************************************************************************

Summary:
 Sends a request to the peer device to get characteristic descriptor of the
 specified characteristic of the Running Speed and Cadence Service.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of a Service Characteristic.
 descrIndex: The index of a Service Characteristic Descriptor.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request was sent successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_INVALID_OPERATION - Cannot process a request to send PDU due to
                                    invalid operation performed by the 
                                    application
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Peer device doesn't have a 
                                               particular descriptor

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_RscscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                   CYBLE_RSCS_CHAR_INDEX_T charIndex,
                                                   uint8 descrIndex)
{
    CYBLE_GATT_DB_ATTR_HANDLE_T tmpHandle;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(((CYBLE_RSCS_RSC_MEASUREMENT == charIndex) || (CYBLE_RSCS_SC_CONTROL_POINT == charIndex)) &&
        ((uint8) CYBLE_RSCS_CCCD == descrIndex))
    {
        if(CYBLE_RSCS_RSC_MEASUREMENT == charIndex)
        {
            tmpHandle = cyBle_rscsc.characteristics[CYBLE_RSCS_RSC_MEASUREMENT].descriptors[CYBLE_RSCS_CCCD];
        }
        else
        {
            tmpHandle = cyBle_rscsc.characteristics[CYBLE_RSCS_SC_CONTROL_POINT].descriptors[CYBLE_RSCS_CCCD];
        }

        if(tmpHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, tmpHandle);
            
            if(CYBLE_ERROR_OK == apiResult)
            {
                cyBle_rscscReqHandle = tmpHandle;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }
    else
    {
        /* Characteristic has not been discovered or had invalid fields */
    }

    return(apiResult);
}

#endif /* (CYBLE_RSCS_CLIENT) */


/* [] END OF FILE */

