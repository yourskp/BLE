/*******************************************************************************
File Name: CYBLE_bcs.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the source code for Body Composition Service.

********************************************************************************
Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_eventHandler.h"

#ifdef CYBLE_BCS_SERVER

`$BcsServer`

static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_bcssReqHandle;

#endif /* CYBLE_BCS_SERVER */

#ifdef CYBLE_BCS_CLIENT

`$BcsClient`

static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_bcscReqHandle;

#endif /* (CYBLE_BCS_CLIENT) */

static CYBLE_CALLBACK_T CyBle_BcsApplCallback = NULL;


/******************************************************************************
##Function Name: CyBle_BcsInit
*******************************************************************************

Summary:
 This function initializes Body Composition Service.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_BcsInit(void)
{
#ifdef CYBLE_BCS_SERVER

    cyBle_bcssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_BCS_SERVER */

#ifdef CYBLE_BCS_CLIENT

    cyBle_bcscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_BCS_CLIENT */
}


/******************************************************************************
##Function Name: CyBle_BcsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc - An application layer event callback function to receive
                events from the BLE Component. The definition of
                CYBLE_CALLBACK_T is:

                typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode,
                                                   void *eventParam)

                * eventCode indicates the event that triggered this
                  callback (e.g. CYBLE_EVT_BCSS_INDICATION_ENABLED).
                * eventParam contains the parameters corresponding to the
                  current event. (e.g. pointer to CYBLE_BCS_CHAR_VALUE_T
                  structure that contains details of the characteristic
                  for which notification enabled event was triggered).

Return:
 None

******************************************************************************/
void CyBle_BcsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_BcsApplCallback = callbackFunc;
}


#ifdef CYBLE_BCS_SERVER

/******************************************************************************
##Function Name: CyBle_BcssWriteEventHandler
*******************************************************************************

Summary:
 Handles Write Request Event for Body Composition Service.

Parameters:
 void *eventParam: The pointer to the data structure specified by the event.

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
CYBLE_GATT_ERR_CODE_T CyBle_BcssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    uint32 event = (uint32) CYBLE_EVT_BCSS_INDICATION_DISABLED;
    
    if(NULL != CyBle_BcsApplCallback)
    {
        /* Client Characteristic Configuration descriptor write request */
        if(eventParam->handleValPair.attrHandle ==
            cyBle_bcss.charInfo[CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT].descrHandle[CYBLE_BCS_CCCD])
        {
            if(CYBLE_IS_INDICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
            {
                event = (uint32) CYBLE_EVT_BCSS_INDICATION_ENABLED;
            }

            gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);

            if(CYBLE_GATT_ERR_NONE == gattErr)
            {
                CYBLE_BCS_CHAR_VALUE_T wrReqParam;
                
                wrReqParam.connHandle = eventParam->connHandle;
                wrReqParam.charIndex = CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT;
                wrReqParam.value = NULL;
                
                CyBle_BcsApplCallback(event, &wrReqParam);
            }
        #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
            (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
            /* Set flag to store bonding data to flash */
            cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
        #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
            
            /* Clear the callback flag indicating that request was handled */
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }

    return(gattErr);
}


/******************************************************************************
##Function Name: CyBle_BcssConfirmationEventHandler
*******************************************************************************

Summary:
 Handles a Value Confirmation request event from the BLE stack.

Parameters:
 *eventParam - The pointer to a structure of type CYBLE_CONN_HANDLE_T.

Return:
 None.

******************************************************************************/
void CyBle_BcssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    CYBLE_BCS_CHAR_VALUE_T locCharValue;

    if((NULL != CyBle_BcsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_bcssReqHandle))
    {
        /* Only Weight Measurement Characteristic has Indication property. Check if
        * the requested handle is the handle of Descriptor Value Change handle.
        */
        if(cyBle_bcssReqHandle ==
            cyBle_bcss.charInfo[CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT].descrHandle[CYBLE_BCS_CCCD])
        {
            /* Fill in event data and inform Application about successfully
            * confirmed indication.
            */
            locCharValue.connHandle = *eventParam;
            locCharValue.charIndex = CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT;
            locCharValue.value = NULL;

            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            cyBle_bcssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            CyBle_BcsApplCallback((uint32) CYBLE_EVT_BCSS_INDICATION_CONFIRMED, &locCharValue);
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_BcssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets a value for one of three characteristic values of the Weight Scale 
 Service. The characteristic is identified by charIndex.

Parameters:
 charIndex: The index of a Body Composition Service characteristic.
 attrSize: The size of the characteristic value attribute.
 attrValue: The pointer to the characteristic value data that should be 
            stored to the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The characteristic value was written successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BcssSetCharacteristicValue(CYBLE_BCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T wssHandleValuePair;

    if(NULL != attrValue)
    {
        if(charIndex == CYBLE_BCS_BODY_COMPOSITION_FEATURE)
        {
            /* Fill the structure */
            wssHandleValuePair.attrHandle = cyBle_bcss.charInfo[CYBLE_BCS_BODY_COMPOSITION_FEATURE].charHandle;
            wssHandleValuePair.value.len = attrSize;
            wssHandleValuePair.value.val = attrValue;
            
            if(CYBLE_GATT_ERR_NONE == 
                CyBle_GattsWriteAttributeValue(&wssHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
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
##Function Name: CyBle_BcssGetCharacteristicValue
*******************************************************************************

Summary:
 Reads a characteristic value of the Body Composition Service, which is identified 
 by charIndex from the GATT database.

Parameters:
 charIndex: The index of the Body Composition Service characteristic.
 attrSize: The size of the Body Composition Service characteristic value attribute.
 *attrValue: The pointer to the location where characteristic value data
             should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The characteristic value was read successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BcssGetCharacteristicValue(CYBLE_BCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_GATT_DB_ATTR_HANDLE_T tmpCharHandle;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T wssHandleValuePair;

    if((NULL != attrValue) && (charIndex < CYBLE_BCS_CHAR_COUNT))
    {
        if(charIndex == CYBLE_BCS_BODY_COMPOSITION_FEATURE)
        {
            tmpCharHandle = cyBle_bcss.charInfo[CYBLE_BCS_BODY_COMPOSITION_FEATURE].charHandle;
        }
        else
        {
            tmpCharHandle = cyBle_bcss.charInfo[CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT].charHandle;
        }
        
        wssHandleValuePair.attrHandle = tmpCharHandle;
        wssHandleValuePair.value.len = attrSize;
        wssHandleValuePair.value.val = attrValue;
    
        /* Get characteristic value from GATT database */
        if(CYBLE_GATT_ERR_NONE == 
            CyBle_GattsReadAttributeValue(&wssHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            /* Indicate success */
            apiResult = CYBLE_ERROR_OK;
        }
    }

    /* Return status */
    return(apiResult);
}


/******************************************************************************
##Function Name: CyBle_BcssSetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sets the characteristic descriptor of the specified characteristic.

Parameters:
 charIndex: The index of the service characteristic.
 charInstance: The instance of the characteristic specified by "charIndex".
 attrSize: The size of the characteristic descriptor attribute.
 *attrValue: The pointer to the descriptor value data to be stored in the GATT
             database.

Return:
 A return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BcssSetCharacteristicDescriptor(CYBLE_BCS_CHAR_INDEX_T charIndex,
    CYBLE_BCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((charIndex == CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT) && (descrIndex == CYBLE_BCS_CCCD) && (NULL != attrValue))
    {
        locHandleValuePair.attrHandle = 
            cyBle_bcss.charInfo[CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT].descrHandle[CYBLE_BCS_CCCD];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;

        /* Read characteristic value from database */
        if(CYBLE_GATT_ERR_NONE ==
            CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_OK;
        }
    }
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_BcssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Reads a a characteristic descriptor of a specified characteristic of the
 Body Composition Service from the GATT database.

Parameters:
 charIndex: The index of the characteristic.
 descrIndex: The index of the descriptor.
 attrSize: The size of the descriptor value.
 *attrValue: The pointer to the location where characteristic descriptor value
             data should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully.
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional descriptor is absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BcssGetCharacteristicDescriptor(CYBLE_BCS_CHAR_INDEX_T charIndex,
    CYBLE_BCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T wssHandleValuePair;
    
    if((charIndex == CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT) && (descrIndex == CYBLE_BCS_CCCD) && (NULL != attrValue))
    {
        if(cyBle_bcss.charInfo[CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT].descrHandle[CYBLE_BCS_CCCD] !=
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            wssHandleValuePair.attrHandle =
                cyBle_bcss.charInfo[CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT].descrHandle[CYBLE_BCS_CCCD];
            wssHandleValuePair.value.len = attrSize;
            wssHandleValuePair.value.val = attrValue;
        
            /* Get characteristic value from GATT database */
            if(CYBLE_GATT_ERR_NONE == 
                CyBle_GattsReadAttributeValue(&wssHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                /* Indicate success */
                apiResult = CYBLE_ERROR_OK;
            }
        }
    }
    
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_BcssSendIndication
*******************************************************************************

Summary:
 Sends an indication with a characteristic value of the Body Composition Service,
 which is a value specified by charIndex, to the client's device.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be
             sent to the client's device.

Return:
 A return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_IND_DISABLED - Indication is not enabled by the client.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BcssSendIndication(CYBLE_CONN_HANDLE_T connHandle, CYBLE_BCS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    /* Store new data in database */
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(charIndex == CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT)
    {
        /* Send indication if it is enabled and connected */
        if(CYBLE_STATE_CONNECTED != CyBle_GetState())
        {
            apiResult = CYBLE_ERROR_INVALID_STATE;
        }
        else if(!CYBLE_IS_INDICATION_ENABLED(
            cyBle_bcss.charInfo[CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT].descrHandle[CYBLE_BCS_CCCD]))
        {
            apiResult = CYBLE_ERROR_IND_DISABLED;
        }
        else
        {
            CYBLE_GATTS_HANDLE_VALUE_IND_T indReqParam;

            /* Fill all fields of write request structure ... */
            indReqParam.attrHandle =
                cyBle_bcss.charInfo[CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT].descrHandle[CYBLE_BCS_CCCD];
            indReqParam.value.val = attrValue;
            indReqParam.value.len = attrSize;

            /* Send indication to client using previously filled structure */
            apiResult = CyBle_GattsIndication(connHandle, &indReqParam);
            
            /* Save handle to support service specific value confirmation response from client */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_bcssReqHandle = indReqParam.attrHandle;
            }
        }
    }
    return (apiResult);
}

#endif /* CYBLE_BCS_SERVER */


#ifdef CYBLE_BCS_CLIENT

/****************************************************************************** 
##Function Name: CyBle_BcscDiscoverCharacteristicsEventHandler
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
void CyBle_BcscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    uint8 i;
    static CYBLE_GATT_DB_ATTR_HANDLE_T *wssLastEndHandle = NULL;
    /* WSS characteristics UUIDs */
    static const CYBLE_UUID16 cyBle_bcscCharUuid[CYBLE_BCS_CHAR_COUNT] =
    {
        CYBLE_UUID_CHAR_BODY_COMPOSITION_SCALE_FEATURE,
        CYBLE_UUID_CHAR_BODY_COMPOSITION_MEASUREMENT
    };

    /* Update last characteristic endHandle to declaration handle of this characteristic */
    if(wssLastEndHandle != NULL)
    {
        *wssLastEndHandle = discCharInfo->charDeclHandle - 1u;
        wssLastEndHandle = NULL;
    }

    /* Search through all available characteristics */
    for(i = (uint8) CYBLE_BCS_BODY_COMPOSITION_SCALE_FEATURE; (i < (uint8) CYBLE_BCS_CHAR_COUNT); i++)
    {
        if(cyBle_bcscCharUuid[i] == discCharInfo->uuid.uuid16)
        {
            if(cyBle_bcsc.charInfo[i].valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                cyBle_bcsc.charInfo[i].valueHandle = discCharInfo->valueHandle;
                cyBle_bcsc.charInfo[i].properties = discCharInfo->properties;
                wssLastEndHandle = &cyBle_bcsc.charInfo[i].endHandle;
                break;
            }
        }
    }

    /* Init characteristic endHandle to Service endHandle. Characteristic endHandle
    * will be updated to the declaration handler of the following characteristic,
    * in the following characteristic discovery procedure.
    */
    if(WssLastEndHandle != NULL)
    {
        *wssLastEndHandle = cyBle_serverInfo[cyBle_disCount].range.endHandle;
    }
}


/******************************************************************************
##Function Name: CyBle_BcscDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event.
 Based on the descriptor UUID, an appropriate data structure is populated using
 the data received as part of the callback.

Parameters:
 *discDescrInfo: The pointer to a descriptor information structure.

Return:
 None.

******************************************************************************/
void CyBle_BcscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    CYBLE_BCS_DESCR_INDEX_T locDescIndex;

    switch(discDescrInfo->uuid.uuid16)
    {
    case CYBLE_UUID_CHAR_CLIENT_CONFIG:
        locDescIndex = CYBLE_BCS_CCCD;
        break;
    default:
        /* Not supported descriptor */
        locDescIndex = CYBLE_BCS_DESCR_COUNT;
        break;
    }

    if(locDescIndex < CYBLE_BCS_DESCR_COUNT)
    {
        if(cyBle_bcsc.bodyCompositionMeasurementCccdHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_bcsc.bodyCompositionMeasurementCccdHandle = discDescrInfo->descrHandle;

        }
        else    /* Duplication of descriptor */
        {
            CyBle_ApplCallback(CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
        }
    }
}


/******************************************************************************
##Function Name: CyBle_BcscIndicationEventHandler
*******************************************************************************

Summary:
 Handles an indication event for Body Composition Service.

Parameters:
 *eventParam: The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_BcscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam)
{
    CYBLE_BCS_CHAR_VALUE_T indicationValue;

    if(NULL != CyBle_BcsApplCallback)
    {
        if(cyBle_bcsc.charInfo[CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT].valueHandle ==
            eventParam->handleValPair.attrHandle)
        {
            indicationValue.connHandle = eventParam->connHandle;
            indicationValue.charIndex = CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT;
            indicationValue.value = &eventParam->handleValPair.value;
            CyBle_BcsApplCallback((uint32) CYBLE_EVT_BCSC_INDICATION, &indicationValue);
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_BcscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles a Read Response event for the Body Composition Service.

Parameters:
 *eventParam: The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_BcscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    uint8 j;
    uint8 locReqHandle = 1u;

    if((NULL != CyBle_BcsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_bcscReqHandle))
    {
        if((cyBle_bcscReqHandle == cyBle_bcsc.charInfo[CYBLE_BCS_BODY_COMPOSITION_SCALE_FEATURE].valueHandle) ||
            (cyBle_bcscReqHandle == cyBle_bcsc.charInfo[CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT].valueHandle))
        {
            CYBLE_BCS_CHAR_VALUE_T locCharValue;

            if(cyBle_bcscReqHandle == cyBle_bcsc.charInfo[CYBLE_BCS_BODY_COMPOSITION_SCALE_FEATURE].valueHandle)
            {
                locCharValue.charIndex = CYBLE_BCS_BODY_COMPOSITION_SCALE_FEATURE;
            }
            else
            {
                locCharValue.charIndex = CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT;
            }
            
            locCharValue.connHandle = eventParam->connHandle;
            locCharValue.value = &eventParam->value;
            cyBle_bcscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            CyBle_BcsApplCallback((uint32) CYBLE_EVT_BCSC_READ_CHAR_RESPONSE, &locCharValue);
        }
        else if(cyBle_bcscReqHandle == cyBle_bcsc.bodyCompositionMeasurementCccdHandle)
        {
            CYBLE_BCS_DESCR_VALUE_T locDescrValue;

            locDescrValue.connHandle = eventParam->connHandle;
            locDescrValue.charIndex = CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT;
            locDescrValue.descrIndex = CYBLE_BCS_CCCD;
            locDescrValue.value = &eventParam->value;
            cyBle_esscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            CyBle_BcsApplCallback((uint32) CYBLE_EVT_BCSC_READ_DESCR_RESPONSE, &locDescrValue);
        }
        else
        {
            locReqHandle = 0u;
        }
        
        if(locReqHandle != 0u)
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_BcscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles a Write Response event for the Environmental Sensing Service.

Parameters:
 *eventParam: The pointer to a data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_BcscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    /* Check if service handler was registered and request handle is valid */
    if((NULL != CyBle_EssApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != WyBle_esscReqHandle))
    {
        if(cyBle_bcscReqHandle == cyBle_bcsc.bodyCompositionMeasurementCccdHandle)
        {
            CYBLE_BCS_DESCR_VALUE_T locDescrValue;

            locDescrValue.connHandle = eventParam->connHandle;
            locDescrValue.charIndex = CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT;
            locDescrValue.descrIndex = CYBLE_BCS_CCCD;
            locDescrValue.value = &eventParam->value;
            cyBle_esscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            CyBle_BcsApplCallback((uint32) CYBLE_EVT_BCSC_WRITE_DESCR_RESPONSE, &locDescrValue);
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_BcscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles an Error Response event for the Body Composition Service.

Parameters:
 *eventParam: The pointer to a data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_BcscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((NULL != eventParam) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_bcscReqHandle))
    {
        cyBle_bcscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


/******************************************************************************
##Function Name: CyBle_BcscGetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to read a characteristic value, which is a value
 identified by charIndex, from the server.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance of the characteristic specified by "charIndex".

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The read request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                             the particular characteristic.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                   characteristic.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BcscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_BCS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_BCS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_bcsc.charInfo[charIndex].valueHandle)
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_bcsc.charInfo[charIndex].valueHandle);

        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_bcscReqHandle = cyBle_bcsc.charInfo[charIndex].valueHandle;
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_BcscSetCharacteristicDescriptor
*******************************************************************************

Summary:
 This function is used to write the characteristic descriptor to the server,
 which is identified by charIndex and descrIndex.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic. Starts with zero.
 descrIndex: The index of the service characteristic descriptor.
 charInstance: The instance of the characteristic specified by "charIndex".
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic descriptor value data that
              should be sent to the server device.

Return:
 A return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_INVALID_STATE - The state is not valid.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on
                                   the specified attribute.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BcscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_BCS_CHAR_INDEX_T charIndex, CYBLE_BCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_BCS_CHAR_COUNT) || (descrIndex >= CYBLE_BCS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_bcsc.bodyCompositionMeasurementCccdHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        /* Fill all fields of write request structure ... */
        writeReqParam.attrHandle = cyBle_bcsc.bodyCompositionMeasurementCccdHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        /* ... and send request to server's device. */
        apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);

        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_bcscReqHandle = writeReqParam.attrHandle;
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_BcscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sends a request to get the characteristic descriptor of the specified 
 characteristic of the service.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance of the characteristic specified by "charIndex".
 descrIndex: The index of the service characteristic descriptor.

Return:
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_INVALID_STATE - The state is not valid.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on
                                    the specified attribute.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_BcscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_BCS_CHAR_INDEX_T charIndex, CYBLE_BCS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_BCS_CHAR_COUNT) || (descrIndex >= CYBLE_BCS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_bcsc.bodyCompositionMeasurementCccdHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, cyBle_bcsc.bodyCompositionMeasurementCccdHandle);

        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_bcscReqHandle = cyBle_bcsc.bodyCompositionMeasurementCccdHandle;
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }

    return (apiResult);
}

#endif /* CYBLE_BCS_CLIENT */


/* [] END OF FILE */
