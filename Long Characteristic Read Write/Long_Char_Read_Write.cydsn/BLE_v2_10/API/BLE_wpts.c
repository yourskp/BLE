/*******************************************************************************
File Name: CYBLE_wpts.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the source code for Wireless Power Transfer Service.

********************************************************************************
Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_eventHandler.h"

#ifdef CYBLE_WPTS_SERVER

`$WptsServer`

static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_wptssReqHandle;

#endif /* CYBLE_WPTS_SERVER */

#ifdef CYBLE_WPTS_CLIENT

CYBLE_WPTSC_T cyBle_wptsc;
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_wptscReqHandle;

/* WPTS characteristics 128-bit UUIDs */
const CYBLE_UUID128_T cyBle_wptscCharUuid128[CYBLE_WPTS_CHAR_COUNT] = {
    /* PRU Control Characteristic*/
    {{ 0x67u, 0x9Au, 0x0Cu, 0x20u, 0x00u, 0x08u, 0x96u, 0x9Eu, 0xE2u, 0x11u, 0x46u, 0xA1u, 0x70u, 0xE6u, 0x55u, 0x64u}},
    /* PTU Static Parameter Characteristic*/
    {{ 0x68u, 0x9Au, 0x0Cu, 0x20u, 0x00u, 0x08u, 0x96u, 0x9Eu, 0xE2u, 0x11u, 0x46u, 0xA1u, 0x70u, 0xE6u, 0x55u, 0x64u}},
    /* PRU Alert Characteristic*/
    {{ 0x69u, 0x9Au, 0x0Cu, 0x20u, 0x00u, 0x08u, 0x96u, 0x9Eu, 0xE2u, 0x11u, 0x46u, 0xA1u, 0x70u, 0xE6u, 0x55u, 0x64u}},
    /* PRU Static Parameter Characteristic*/
    {{ 0x6Au, 0x9Au, 0x0Cu, 0x20u, 0x00u, 0x08u, 0x96u, 0x9Eu, 0xE2u, 0x11u, 0x46u, 0xA1u, 0x70u, 0xE6u, 0x55u, 0x64u}},
    /* PRU Dynamic Parameter Characteristic*/
    {{ 0x6Bu, 0x9Au, 0x0Cu, 0x20u, 0x00u, 0x08u, 0x96u, 0x9Eu, 0xE2u, 0x11u, 0x46u, 0xA1u, 0x70u, 0xE6u, 0x55u, 0x64u}}
};

#endif /* (CYBLE_WPTS_CLIENT) */

static CYBLE_CALLBACK_T CyBle_WptsApplCallback = NULL;


/******************************************************************************
##Function Name: CyBle_WptsInit
*******************************************************************************

Summary:
 This function initializes Wireless Power Transfer Service.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_WptsInit(void)
{
#ifdef CYBLE_WPTS_SERVER

    cyBle_wptssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_WPTS_SERVER */

#ifdef CYBLE_WPTS_CLIENT

    cyBle_wptscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    (void)memset(&cyBle_wptsc, 0, sizeof(cyBle_wptsc));

#endif /* CYBLE_WPTS_CLIENT */
}


/******************************************************************************
##Function Name: CyBle_WptsRegisterAttrCallback
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
                  callback (e.g. CYBLE_EVT_WPTSS_INDICATION_ENABLED).
                * eventParam contains the parameters corresponding to the
                  current event. (e.g. pointer to CYBLE_WPTS_CHAR_VALUE_T
                  structure that contains details of the characteristic
                  for which notification enabled event was triggered).

Return:
 None

******************************************************************************/
void CyBle_WptsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_WptsApplCallback = callbackFunc;
}


#ifdef CYBLE_WPTS_SERVER


/******************************************************************************
##Function Name: CyBle_WptssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets a characteristic value of the Wireless Power Transfer Service in the
 local GATT database. The characteristic is identified by charIndex.

Parameters:
 charIndex:       The index of a service characteristic of type
                  CYBLE_WPTS_CHAR_INDEX_T.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the characteristic value data that should be
                  stored to the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The characteristic value was written successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_WptssSetCharacteristicValue(CYBLE_WPTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T wssHandleValuePair;

    if((attrValue != NULL) && (charIndex < CYBLE_WPTS_CHAR_COUNT))
    {
        /* Fill the structure */
        wssHandleValuePair.attrHandle = cyBle_wptss.charInfo[charIndex].charHandle;
        wssHandleValuePair.value.len = attrSize;
        wssHandleValuePair.value.val = attrValue;

        if(CyBle_GattsWriteAttributeValue(&wssHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED) ==
            CYBLE_GATT_ERR_NONE)
        {
            /* Indicate success */
            apiResult = CYBLE_ERROR_OK;
        }
    }

    /* Return status */
    return(apiResult);
}


/******************************************************************************
##Function Name: CyBle_WptssGetCharacteristicValue
*******************************************************************************

Summary:
 Reads a characteristic value of the Wireless Power Transfer Service, which
 is identified by charIndex from the GATT database.

Parameters:
 charIndex:       The index of a service characteristic of type
                  CYBLE_WPTS_CHAR_INDEX_T.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the location where characteristic value data
                  should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The characteristic value was read successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_WptssGetCharacteristicValue(CYBLE_WPTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T wssHandleValuePair;

    if((attrValue != NULL) && (charIndex < CYBLE_WPTS_CHAR_COUNT))
    {
        /* Fill the structure */
        wssHandleValuePair.attrHandle = cyBle_wptss.charInfo[charIndex].charHandle;
        wssHandleValuePair.value.len = attrSize;
        wssHandleValuePair.value.val = attrValue;

        /* Get characteristic value from GATT database */
        if(CyBle_GattsReadAttributeValue(&wssHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED) ==
            CYBLE_GATT_ERR_NONE)
        {
            /* Indicate success */
            apiResult = CYBLE_ERROR_OK;
        }
    }
    /* Return status */
    return(apiResult);
}


/******************************************************************************
##Function Name: CyBle_WptssSetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sets the characteristic descriptor of the specified characteristic.

Parameters:
 charIndex:       The index of a service characteristic of type
                  CYBLE_WPTS_CHAR_INDEX_T.
 descrIndex:      The index of a service characteristic descriptor of type
                  CYBLE_WPTS_DESCR_INDEX_T.
 attrSize:        The size of the characteristic descriptor attribute.
 *attrValue:      The pointer to the descriptor value data that should
                  be stored to the GATT database.

Return:
 A return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_WptssSetCharacteristicDescriptor(CYBLE_WPTS_CHAR_INDEX_T charIndex,
    CYBLE_WPTS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((charIndex < CYBLE_WPTS_CHAR_COUNT) && (descrIndex < CYBLE_WPTS_DESCR_COUNT) && (attrValue != NULL))
    {
        locHandleValuePair.attrHandle = cyBle_wptss.charInfo[charIndex].descrHandle[descrIndex];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;

        /* Read characteristic value from database */
        if(CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED) ==
            CYBLE_GATT_ERR_NONE)
        {
            apiResult = CYBLE_ERROR_OK;
        }
    }
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_WptssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Reads a a characteristic descriptor of a specified characteristic of the
 Wireless Power Transfer Service from the GATT database.

Parameters:
 charIndex:       The index of a service characteristic of type
                  CYBLE_WPTS_CHAR_INDEX_T.
 descrIndex:      The index of a service characteristic descriptor of type
                  CYBLE_WPTS_DESCR_INDEX_T.
 attrSize:        The size of the characteristic descriptor attribute.
 *attrValue:      The pointer to the location where characteristic descriptor
                  value data should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully.
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_WptssGetCharacteristicDescriptor(CYBLE_WPTS_CHAR_INDEX_T charIndex,
    CYBLE_WPTS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T wssHandleValuePair;

    if((charIndex < CYBLE_WPTS_CHAR_COUNT) && (descrIndex < CYBLE_WPTS_DESCR_COUNT) && (attrValue != NULL))
    {
        wssHandleValuePair.attrHandle = cyBle_wptss.charInfo[charIndex].descrHandle[descrIndex];
        wssHandleValuePair.value.len = attrSize;
        wssHandleValuePair.value.val = attrValue;

        /* Get characteristic value from GATT database */
        if(CyBle_GattsReadAttributeValue(&wssHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED) ==
            CYBLE_GATT_ERR_NONE)
        {
            /* Indicate success */
            apiResult = CYBLE_ERROR_OK;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_WptssSendNotification
*******************************************************************************

Summary:
 Sends notification with a characteristic value of the WPTS, which is a value
 specified by charIndex, to the Client device.

Parameters:
 connHandle:      The connection handle
 charIndex:       The index of a service characteristic of type
                  CYBLE_WPTS_CHAR_INDEX_T.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the characteristic value data that should be
                  sent to the Client device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
  * CYBLE_ERROR_INVALID_STATE - Connection with the Client is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the Client.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_WptssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_WPTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{

    CYBLE_API_RESULT_T apiResult;

    /* Send Notification if it is enabled and connected */
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_WPTS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_wptss.charInfo[charIndex].charHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else if((!CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_wptss.charInfo[charIndex].charHandle)) ||
            (!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_wptss.charInfo[charIndex].descrHandle[CYBLE_WPTS_CCCD])))
    {
        apiResult = CYBLE_ERROR_NTF_DISABLED;
    }
    else
    {
        CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;

        /* Fill all fields of write request structure ... */
        ntfReqParam.attrHandle = cyBle_wptss.charInfo[charIndex].charHandle;
        ntfReqParam.value.val = attrValue;
        ntfReqParam.value.len = attrSize;

        /* Send notification to the Client using previously filled structure */
        apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_WptssSendIndication
*******************************************************************************

Summary:
 Sends an indication with a characteristic value of the Wireless Power Transfer
 Service, which is a value specified by charIndex, to the Client device.

Parameters:
 connHandle:      The connection handle
 charIndex:       The index of a service characteristic of type
                  CYBLE_WPTS_CHAR_INDEX_T.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the characteristic value data that should be
                  sent to the Client device.

Return:
 A return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_IND_DISABLED - Indication is not enabled by the client.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_WptssSendIndication(CYBLE_CONN_HANDLE_T connHandle, CYBLE_WPTS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;

    /* Send Notification if it is enabled and connected */
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_WPTS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_wptss.charInfo[charIndex].charHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else if((!CYBLE_IS_INDICATION_SUPPORTED(cyBle_wptss.charInfo[charIndex].charHandle)) ||
            (!CYBLE_IS_INDICATION_ENABLED(cyBle_wptss.charInfo[charIndex].descrHandle[CYBLE_WPTS_CCCD])))
    {
        apiResult = CYBLE_ERROR_IND_DISABLED;
    }
    else
    {
        CYBLE_GATTS_HANDLE_VALUE_IND_T indReqParam;

        /* Fill all fields of write request structure ... */
        indReqParam.attrHandle = cyBle_wptss.charInfo[charIndex].charHandle;
        indReqParam.value.val = attrValue;
        indReqParam.value.len = attrSize;

        /* Send indication to Client using previously filled structure */
        apiResult = CyBle_GattsIndication(connHandle, &indReqParam);
        /* Save handle to support service specific value confirmation response from Client */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_wptssReqHandle = indReqParam.attrHandle;
        }
    }
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_WptssWriteEventHandler
*******************************************************************************

Summary:
 Handles Write Request Event for Wireless Power Transfer Service.

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
CYBLE_GATT_ERR_CODE_T CyBle_WptssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_WPTS_CHAR_INDEX_T locCharIndex;
    CYBLE_WPTS_CHAR_VALUE_T locCharValue;
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    uint8 locReqHandle = 0u;

    if(CyBle_WptsApplCallback != NULL)
    {
        for(locCharIndex = CYBLE_WPTS_PRU_CONTROL; (locCharIndex < CYBLE_WPTS_CHAR_COUNT) && (locReqHandle == 0u);
            locCharIndex++)
        {
            if((eventParam->handleValPair.attrHandle == cyBle_wptss.charInfo[locCharIndex].descrHandle[CYBLE_WPTS_CCCD])
                || (eventParam->handleValPair.attrHandle == cyBle_wptss.charInfo[locCharIndex].charHandle))
            {
                locCharValue.connHandle = eventParam->connHandle;
                locCharValue.charIndex = locCharIndex;
                locCharValue.value = &eventParam->handleValPair.value;
                /* Store value to database */
                gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u,
                            &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                if(gattErr == CYBLE_GATT_ERR_NONE)
                {
                    /* Characteristic value write request */
                    if(eventParam->handleValPair.attrHandle == cyBle_wptss.charInfo[locCharIndex].charHandle)
                    {
                        CyBle_WptsApplCallback((uint32)CYBLE_EVT_WPTSS_WRITE_CHAR, &locCharValue);
                    }
                    else /* Client Characteristic Configuration descriptor write request */
                    {
                        /* Check characteristic properties for Notification */
                        if(CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_wptss.charInfo[locCharIndex].charHandle))
                        {
                            uint32 eventCode;
                            if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                            {
                                eventCode = (uint32)CYBLE_EVT_WPTSS_NOTIFICATION_ENABLED;
                            }
                            else
                            {
                                eventCode = (uint32)CYBLE_EVT_WPTSS_NOTIFICATION_DISABLED;
                            }
                            CyBle_WptsApplCallback(eventCode, &locCharValue);
                        }
                        /* Check characteristic properties for Indication */
                        if(CYBLE_IS_INDICATION_SUPPORTED(cyBle_wptss.charInfo[locCharIndex].charHandle))
                        {
                            uint32 eventCode;
                            if(CYBLE_IS_INDICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                            {
                                eventCode = (uint32)CYBLE_EVT_WPTSS_INDICATION_ENABLED;
                            }
                            else
                            {
                                eventCode = (uint32)CYBLE_EVT_WPTSS_INDICATION_DISABLED;
                            }
                            CyBle_WptsApplCallback(eventCode, &locCharValue);
                        }
                    #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
                        (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
                        /* Set flag to store bonding data to flash */
                        cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
                    #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */

                    }
                }
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                locReqHandle = 1u;
            }
        }
    }
    return (gattErr);
}


/******************************************************************************
##Function Name: CyBle_WptssConfirmationEventHandler
*******************************************************************************

Summary:
 Handles a Value Confirmation request event from the BLE stack.

Parameters:
 *eventParam - The pointer to a structure of type CYBLE_CONN_HANDLE_T.

Return:
 None.

******************************************************************************/
void CyBle_WptssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    CYBLE_WPTS_CHAR_INDEX_T locCharIndex;
    CYBLE_WPTS_CHAR_VALUE_T locCharValue;
    uint8 locReqHandle = 0u;

    if((CyBle_WptsApplCallback != NULL) && (cyBle_wptssReqHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE))
    {
        for(locCharIndex = CYBLE_WPTS_PRU_CONTROL; (locCharIndex < CYBLE_WPTS_CHAR_COUNT) && (locReqHandle == 0u);
            locCharIndex++)
        {
            if(cyBle_wptssReqHandle == cyBle_wptss.charInfo[locCharIndex].charHandle)
            {
                /* Fill in event data and inform Application about successfully confirmed indication. */
                locCharValue.connHandle = *eventParam;
                locCharValue.charIndex = locCharIndex;
                locCharValue.value = NULL;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                cyBle_wptssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_WptsApplCallback((uint32)CYBLE_EVT_WPTSS_INDICATION_CONFIRMED, &locCharValue);
                locReqHandle = 1u;
            }
        }
    }
}

#endif /* CYBLE_WPTS_SERVER */


#ifdef CYBLE_WPTS_CLIENT


/******************************************************************************
##Function Name: CyBle_WptscDiscovery
*******************************************************************************

Summary:
 This function discovers the PRU's WPT service and characteristics using the
 GATT Primary Service Handle, received through the WPT Service Data
 within the PRU advertisement payload, together with the handle offsets defined
 A4WP specification.

 The PTU may perform service discovery using the CyBle_GattcStartDiscovery()
 API. This function may be used in response to Service Changed indication or to
 discover services other than the WPT service supported by the PRU.

Parameters:
 servHandle: GATT Primary Service Handle of the WPT service.

Return:
 None

******************************************************************************/
void CyBle_WptscDiscovery(CYBLE_GATT_DB_ATTR_HANDLE_T servHandle)
{
    /* Clear old data */
    CyBle_WptsInit();

    cyBle_wptsc.serviceHandle = servHandle;
    cyBle_wptsc.charInfo[CYBLE_WPTS_PRU_CONTROL].valueHandle = servHandle + CYBLE_WPTS_PRU_CONTROL_CHAR_VALUE_OFFSET;
    cyBle_wptsc.charInfo[CYBLE_WPTS_PTU_STATIC_PAR].valueHandle = servHandle + CYBLE_WPTS_PTU_STATIC_PAR_CHAR_VALUE_OFFSET;
    cyBle_wptsc.charInfo[CYBLE_WPTS_PRU_ALERT].valueHandle = servHandle + CYBLE_WPTS_PRU_ALERT_PAR_CHAR_VALUE_OFFSET;
    cyBle_wptsc.charInfo[CYBLE_WPTS_PRU_ALERT].descrHandle[CYBLE_WPTS_CCCD] = servHandle + CYBLE_WPTS_PRU_ALERT_PAR_CHAR_CCCD_OFFSET;
    cyBle_wptsc.charInfo[CYBLE_WPTS_PRU_STATIC_PAR].valueHandle = servHandle + CYBLE_WPTS_PRU_STATIC_PAR_CHAR_VALUE_OFFSET;
    cyBle_wptsc.charInfo[CYBLE_WPTS_PRU_DYNAMIC_PAR].valueHandle = servHandle + CYBLE_WPTS_PRU_DYNAMIC_PAR_CHAR_VALUE_OFFSET;

    CyBle_SetClientState(CYBLE_CLIENT_STATE_DISCOVERED);
}

/******************************************************************************
##Function Name: CyBle_WptscDiscoverCharacteristicsEventHandler
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
void CyBle_WptscDiscoverCharacteristicsEventHandler(const CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    static CYBLE_GATT_DB_ATTR_HANDLE_T *wptscLastEndHandle = NULL;

    /* Update last characteristic endHandle to declaration handle of this characteristic */
    if(wptscLastEndHandle != NULL)
    {
        *wptscLastEndHandle = discCharInfo->charDeclHandle - 1u;
        wptscLastEndHandle = NULL;
    }

    /* WPTS service has only characteristics with 128-bit UUIDs */
    if(discCharInfo->uuidFormat == CYBLE_GATT_128_BIT_UUID_FORMAT)
    {
        /* Compare common part of characteristics 128-bit UUID (except first byte) */
        if(memcmp(&cyBle_wptscCharUuid128[CYBLE_WPTS_PRU_CONTROL].value[1u],
                  &discCharInfo->uuid.uuid128.value[1u], CYBLE_GATT_128_BIT_UUID_SIZE - 1u) == 0u)
        {
            /* Check the range of the first byte to be between the first and last characteristic */
            if((discCharInfo->uuid.uuid128.value[0u] >= cyBle_wptscCharUuid128[CYBLE_WPTS_PRU_CONTROL].value[0u]) &&
               (discCharInfo->uuid.uuid128.value[0u] <= cyBle_wptscCharUuid128[CYBLE_WPTS_CHAR_COUNT - 1u].value[0u]))
            {
                uint8 locCharIndex;
                locCharIndex = discCharInfo->uuid.uuid128.value[0u] -
                               cyBle_wptscCharUuid128[CYBLE_WPTS_PRU_CONTROL].value[0u];
                /* Verify that characteristic handler is not assigned already */
                if(cyBle_wptsc.charInfo[locCharIndex].valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
                {
                    cyBle_wptsc.charInfo[locCharIndex].valueHandle = discCharInfo->valueHandle;
                    cyBle_wptsc.charInfo[locCharIndex].properties = discCharInfo->properties;
                    wptscLastEndHandle = &cyBle_wptsc.charInfo[locCharIndex].endHandle;
                }
            }
        }
    }

    /* Initialize characteristic endHandle to Service endHandle. Characteristic endHandle
    * will be updated to the declaration handler of the following characteristic,
    * in the following characteristic discovery procedure.
    */
    if(wptscLastEndHandle != NULL)
    {
        *wptscLastEndHandle = cyBle_serverInfo[cyBle_disCount].range.endHandle;
    }
}


/******************************************************************************
##Function Name: CyBle_WptscDiscoverCharDescriptorsEventHandler
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
void CyBle_WptscDiscoverCharDescriptorsEventHandler(const CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_CLIENT_CONFIG)
    {
        if(cyBle_wptsc.charInfo[CYBLE_WPTS_PRU_ALERT].descrHandle[CYBLE_WPTS_CCCD] ==
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_wptsc.charInfo[CYBLE_WPTS_PRU_ALERT].descrHandle[CYBLE_WPTS_CCCD] = discDescrInfo->descrHandle;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_WptscNotificationEventHandler
*******************************************************************************

Summary:
 Handles notification event for Wireless Power Transfer Service.

Parameters:
 *eventParam:  The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_WptscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    CYBLE_WPTS_CHAR_INDEX_T locCharIndex;
    CYBLE_WPTS_CHAR_VALUE_T notifValue;

    if(CyBle_WptsApplCallback != NULL)
    {
        for(locCharIndex = CYBLE_WPTS_PRU_CONTROL; locCharIndex < CYBLE_WPTS_CHAR_COUNT; locCharIndex++)
        {
            if(cyBle_wptsc.charInfo[locCharIndex].valueHandle == eventParam->handleValPair.attrHandle)
            {
                notifValue.connHandle = eventParam->connHandle;
                notifValue.charIndex = locCharIndex;
                notifValue.value = &eventParam->handleValPair.value;
                CyBle_WptsApplCallback((uint32)CYBLE_EVT_WPTSC_NOTIFICATION, &notifValue);
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                break;
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_WptscIndicationEventHandler
*******************************************************************************

Summary:
 Handles an indication event for Wireless Power Transfer Service.

Parameters:
 *eventParam: The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_WptscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam)
{
    CYBLE_WPTS_CHAR_INDEX_T locCharIndex;
    CYBLE_WPTS_CHAR_VALUE_T indicationValue;

    if(CyBle_WptsApplCallback != NULL)
    {
        for(locCharIndex = CYBLE_WPTS_PRU_CONTROL; locCharIndex < CYBLE_WPTS_CHAR_COUNT; locCharIndex++)
        {
            if(cyBle_wptsc.charInfo[locCharIndex].valueHandle == eventParam->handleValPair.attrHandle)
            {
                indicationValue.connHandle = eventParam->connHandle;
                indicationValue.charIndex = locCharIndex;
                indicationValue.value = &eventParam->handleValPair.value;
                CyBle_WptsApplCallback((uint32) CYBLE_EVT_WPTSC_INDICATION, &indicationValue);
                cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
                break;
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_WptscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles a Read Response event for the Wireless Power Transfer Service.

Parameters:
 *eventParam: The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_WptscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    uint8 locReqHandle = 0u;
    CYBLE_WPTS_CHAR_INDEX_T locCharIndex;

    if((CyBle_WptsApplCallback != NULL) && (cyBle_wptscReqHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE))
    {
        for(locCharIndex = CYBLE_WPTS_PRU_CONTROL; (locCharIndex < CYBLE_WPTS_CHAR_COUNT) && (locReqHandle == 0u);
            locCharIndex++)
        {
            if(cyBle_wptscReqHandle == cyBle_wptsc.charInfo[locCharIndex].valueHandle)
            {
                CYBLE_WPTS_CHAR_VALUE_T locCharValue;

                locCharValue.connHandle = eventParam->connHandle;
                locCharValue.charIndex = locCharIndex;
                locCharValue.value = &eventParam->value;
                cyBle_wptscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_WptsApplCallback((uint32) CYBLE_EVT_WPTSC_READ_CHAR_RESPONSE, &locCharValue);
                locReqHandle = 1u;
            }
            else if(cyBle_wptscReqHandle == cyBle_wptsc.charInfo[locCharIndex].descrHandle[CYBLE_WPTS_CCCD])
            {
                CYBLE_WPTS_DESCR_VALUE_T locDescrValue;

                locDescrValue.connHandle = eventParam->connHandle;
                locDescrValue.charIndex = locCharIndex;
                locDescrValue.descrIndex = CYBLE_WPTS_CCCD;
                locDescrValue.value = &eventParam->value;
                cyBle_wptscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_WptsApplCallback((uint32) CYBLE_EVT_WPTSC_READ_DESCR_RESPONSE, &locDescrValue);
                locReqHandle = 1u;
            }
            else /* No more descriptors available */
            {
            }
        }
        if(locReqHandle != 0u)
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_WptscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles a Write Response event for the Wireless Power Transfer Service.

Parameters:
 *eventParam: The pointer to a data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_WptscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    uint8 locReqHandle = 0u;
    CYBLE_WPTS_CHAR_INDEX_T locCharIndex;

    /* Check if service handler was registered and request handle is valid */
    if((CyBle_WptsApplCallback != NULL) && (cyBle_wptscReqHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE))
    {
        for(locCharIndex = CYBLE_WPTS_PRU_CONTROL; (locCharIndex < CYBLE_WPTS_CHAR_COUNT) && (locReqHandle == 0u);
            locCharIndex++)
        {
            if(cyBle_wptscReqHandle == cyBle_wptsc.charInfo[locCharIndex].valueHandle)
            {
                CYBLE_WPTS_CHAR_VALUE_T locCharValue;

                locCharValue.connHandle = *eventParam;
                locCharValue.charIndex = locCharIndex;
                locCharValue.value = NULL;
                cyBle_wptscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_WptsApplCallback((uint32) CYBLE_EVT_WPTSC_WRITE_CHAR_RESPONSE, &locCharValue);
                locReqHandle = 1u;
            }
            else if(cyBle_wptscReqHandle == cyBle_wptsc.charInfo[locCharIndex].descrHandle[CYBLE_WPTS_CCCD])
            {
                CYBLE_WPTS_DESCR_VALUE_T locDescrValue;

                locDescrValue.connHandle = *eventParam;
                locDescrValue.charIndex = locCharIndex;
                locDescrValue.descrIndex = CYBLE_WPTS_CCCD;
                locDescrValue.value = NULL;
                cyBle_wptscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_WptsApplCallback((uint32) CYBLE_EVT_WPTSC_WRITE_DESCR_RESPONSE, &locDescrValue);
                locReqHandle = 1u;
            }
            else /* No more descriptors available */
            {
            }
        }
        if(locReqHandle != 0u)
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_WptscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles an Error Response event for the Wireless Power Transfer Service.

Parameters:
 *eventParam: The pointer to a data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_WptscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((eventParam != NULL) && (cyBle_wptscReqHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE))
    {
        cyBle_wptscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


/******************************************************************************
##Function Name: CyBle_WptscSetCharacteristicValue
*******************************************************************************

Summary:
 Sends a request to set a characteristic value of the service, which is a value
 identified by charIndex, to the server device.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic of type
                  CYBLE_WPTS_CHAR_INDEX_T.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the characteristic value data that
                  should be send to the server device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                    characteristic

******************************************************************************/
CYBLE_API_RESULT_T CyBle_WptscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_WPTS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T writeReqParam;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_WPTS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_wptsc.charInfo[charIndex].valueHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        writeReqParam.attrHandle = cyBle_wptsc.charInfo[charIndex].valueHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &writeReqParam);
        /* Save handle to support service specific write response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_wptscReqHandle = writeReqParam.attrHandle;
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_WptscGetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to read a characteristic value, which is a value
 identified by charIndex, from the server.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic of type
                  CYBLE_WPTS_CHAR_INDEX_T.

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
CYBLE_API_RESULT_T CyBle_WptscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_WPTS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_WPTS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_wptsc.charInfo[charIndex].valueHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_wptsc.charInfo[charIndex].valueHandle);

        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_wptscReqHandle = cyBle_wptsc.charInfo[charIndex].valueHandle;
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_WptscSetCharacteristicDescriptor
*******************************************************************************

Summary:
 This function is used to write the characteristic descriptor to the server,
 which is identified by charIndex and descrIndex.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic of type
                  CYBLE_WPTS_CHAR_INDEX_T.
 descrIndex:      The index of a service characteristic descriptor of type
                  CYBLE_WPTS_DESCR_INDEX_T.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the characteristic descriptor value data that
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
CYBLE_API_RESULT_T CyBle_WptscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_WPTS_CHAR_INDEX_T charIndex, CYBLE_WPTS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_WPTS_CHAR_COUNT) || (descrIndex >= CYBLE_WPTS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Fill all fields of write request structure ... */
        writeReqParam.attrHandle = cyBle_wptsc.charInfo[charIndex].descrHandle[descrIndex];
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        /* ... and send request to server's device. */
        apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);

        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_wptscReqHandle = writeReqParam.attrHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_WptscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sends a request to get the characteristic descriptor of the specified
 characteristic of the service.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic of type
                  CYBLE_WPTS_CHAR_INDEX_T.
 descrIndex:      The index of a service characteristic descriptor of type
                  CYBLE_WPTS_DESCR_INDEX_T.

Return:
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_INVALID_STATE - The state is not valid.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on
                                    the specified attribute.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_WptscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_WPTS_CHAR_INDEX_T charIndex, CYBLE_WPTS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_WPTS_CHAR_COUNT) || (descrIndex >= CYBLE_WPTS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle,
                                                    cyBle_wptsc.charInfo[charIndex].descrHandle[descrIndex]);

        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_wptscReqHandle = cyBle_wptsc.charInfo[charIndex].descrHandle[descrIndex];
        }
    }

    return (apiResult);
}

#endif /* CYBLE_WPTS_CLIENT */


/* [] END OF FILE */
