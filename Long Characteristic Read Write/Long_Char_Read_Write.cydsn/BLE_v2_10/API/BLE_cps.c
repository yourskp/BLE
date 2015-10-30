/*******************************************************************************
File Name: CYBLE_cps.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the source code for Cycling Power Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_CpsApplCallback = NULL;

#ifdef CYBLE_CPS_SERVER

`$CpsServer`
CYBLE_CPS_CP_ADJUSTMENT_T cyBle_cpssAdjustment;
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_cpssReqHandle;
static uint8 cyBle_cpssFlag;
static CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T cyBle_cpssConnParam;
    
#endif /* CYBLE_CPS_SERVER */

#ifdef CYBLE_CPS_CLIENT

CYBLE_CPSC_T cyBle_cpsc;
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_cpscReqHandle;
static uint8 cyBle_cpscFlag;

#endif /* (CYBLE_CPS_CLIENT) */

#if CYBLE_GAP_ROLE_BROADCASTER

CYBLE_GAPP_DISC_PARAM_T cyBle_cpssBroadcastParam;
CYBLE_GAPP_DISC_DATA_T cyBle_cpssBroadcastData =
{
    {   /* Length, FLAGS, BR/EDR NOT Supported */
        CYBLE_GAP_ADV_FLAGS_PACKET_LENGTH, CYBLE_GAP_ADV_FLAGS, CYBLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED, 
        /* Advertising Interval data type contains the advInterval value */
        CYBLE_GAP_ADV_ADVERT_INTERVAL_PACKET_LENGTH, CYBLE_GAP_ADV_ADVERT_INTERVAL,
        LO8(CYBLE_GAP_ADV_ADVERT_INTERVAL_NONCON_MIN), HI8(CYBLE_GAP_ADV_ADVERT_INTERVAL_NONCON_MIN), 
        /* The Service Data data type consists of a CPS service UUID with the Cycling Power Measurement 
           characteristic value */
        CYBLE_CPSS_BROADCAST_DATA_LEN_MIN,                          /* Packet length */
        CYBLE_GAP_ADV_SRVC_DATA_16UUID,                             /* Service Data */
        LO8(CYBLE_UUID_CPS_SERVICE), HI8(CYBLE_UUID_CPS_SERVICE),   /* CPS service UUID */
        0x00u, 0x00u,                                               /* Flags */
        0x11u, 0x11u,                                               /* Instantaneous Power */
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u }, 
    /* Advertising data length */
    CYBLE_CPSS_BROADCAST_DATA_LEN_MIN + CYBLE_CPSS_BROADCAST_HEADER_LEN,
};

CYBLE_GAPP_DISC_MODE_INFO_T cyBle_cpssBroadcastModeInfo =
{
    CYBLE_GAPP_NONE_DISC_BROADCAST_MODE,        /* discMode */
    &cyBle_cpssBroadcastParam,
    &cyBle_cpssBroadcastData,
    NULL,                                       
    0u,                                         /* advTo */
};

#endif /* CYBLE_GAP_ROLE_BROADCASTER */

/* DOM-IGNORE-BEGIN */
#ifdef CYBLE_CPS_SERVER

    
/****************************************************************************** 
##Function Name: CyBle_CpssInit
*******************************************************************************

Summary:
 This function initializes CPS Service in Server role.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_CpssInit(void)
{
    cyBle_cpssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    cyBle_cpssFlag = 0u;
}

#endif /* CYBLE_CPS_SERVER */

/****************************************************************************** 
##Function Name: CyBle_CpsInit
*******************************************************************************

Summary:
 This function initializes CPS Service.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_CpsInit(void)
{

#ifdef CYBLE_CPS_SERVER
    CyBle_CpssInit();
    cyBle_cpssAdjustment.samplingRate = CYBLE_CPS_SAMLING_RATE_DEFAULT;
#endif /* CYBLE_CPS_SERVER */

#ifdef CYBLE_CPS_CLIENT

    (void)memset(&cyBle_cpsc, 0, sizeof(cyBle_cpsc));
    cyBle_cpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    cyBle_cpscFlag = 0u;

#endif /* CYBLE_CPS_CLIENT */
}
/* DOM-IGNORE-END */


/****************************************************************************** 
##Function Name: CyBle_CpsRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
callbackFunc:   An application layer event callback function to receive 
                events from the BLE Component. The definition of 
                CYBLE_CALLBACK_T for CPS is,
        
                typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                  void *eventParam)

                * eventCode indicates the event that triggered this 
                  callback.
                * eventParam contains the parameters corresponding to the 
                  current event.

Return:
 None.

******************************************************************************/
void CyBle_CpsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_CpsApplCallback = callbackFunc;
}

#ifdef CYBLE_CPS_SERVER

/****************************************************************************** 
##Function Name: CyBle_CpssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets a characteristic value of the service in the local database.

Parameters:
 charIndex:       The index of a service characteristic of type 
                  CYBLE_CPS_CHAR_INDEX_T.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the characteristic value data that should be
                  stored to the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request is handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CpssSetCharacteristicValue(CYBLE_CPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex >= CYBLE_CPS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Store data in database */
        locHandleValuePair.attrHandle = cyBle_cpss.charInfo[charIndex].charHandle;
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        if(CYBLE_GATT_ERR_NONE !=
            CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
    }
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpssGetCharacteristicValue
*******************************************************************************

Summary:
 Gets a characteristic value of the service, which is a value identified by charIndex.

Parameters:
 charIndex:       The index of a service characteristic of type 
                  CYBLE_CPS_CHAR_INDEX_T.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the location where characteristic value data
                  should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request is handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CpssGetCharacteristicValue(CYBLE_CPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex >= CYBLE_CPS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Read characteristic value from database */
        locHandleValuePair.attrHandle = cyBle_cpss.charInfo[charIndex].charHandle;
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        if(CYBLE_GATT_ERR_NONE !=
            CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
    }
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpssSetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sets a characteristic descriptor of a specified characteristic of the service.

Parameters:
 charIndex:       The index of a service characteristic of type 
                  CYBLE_CPS_CHAR_INDEX_T.
 descrIndex:      The index of a service characteristic descriptor of type 
                  CYBLE_CPS_DESCR_INDEX_T.
 attrSize:        The size of the characteristic descriptor attribute.
 *attrValue:      The pointer to the descriptor value data that should 
                  be stored to the GATT database. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request is handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CpssSetCharacteristicDescriptor(CYBLE_CPS_CHAR_INDEX_T charIndex,
    CYBLE_CPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((charIndex >= CYBLE_CPS_CHAR_COUNT) || (descrIndex >= CYBLE_CPS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        locHandleValuePair.attrHandle = cyBle_cpss.charInfo[charIndex].descrHandle[descrIndex];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        if(CYBLE_GATT_ERR_NONE !=
                CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
    }
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets a characteristic descriptor of a specified characteristic of the service.

Parameters:
 charIndex:       The index of a service characteristic of type 
                  CYBLE_CPS_CHAR_INDEX_T.
 descrIndex:      The index of a service characteristic descriptor of type 
                  CYBLE_CPS_DESCR_INDEX_T.
 attrSize:        The size of the characteristic descriptor attribute.
 *attrValue:      The pointer to the location where characteristic descriptor
                  value data should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CpssGetCharacteristicDescriptor(CYBLE_CPS_CHAR_INDEX_T charIndex,
    CYBLE_CPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((charIndex >= CYBLE_CPS_CHAR_COUNT) || (descrIndex >= CYBLE_CPS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        locHandleValuePair.attrHandle = cyBle_cpss.charInfo[charIndex].descrHandle[descrIndex];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        if(CYBLE_GATT_ERR_NONE !=
                CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
    }
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpssWriteEventHandler
*******************************************************************************

Summary:
 Handles Write Request Event for CPS service.

Parameters:
 void *eventParam: The pointer to the data structure specified by the event.

Return:
 Return value is of type CYBLE_GATT_ERR_CODE_T.
  * CYBLE_GATT_ERR_NONE - write is successful
  * CYBLE_GATT_ERR_PROCEDURE_ALREADY_IN_PROGRESS
  * CYBLE_GATT_ERR_CCCD_IMPROPERLY_CONFIGURED

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_CpssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_CPS_CHAR_INDEX_T locCharIndex;
    CYBLE_CPS_CHAR_VALUE_T locCharValue;
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    uint8 locReqHandle = 0u;

    if(NULL != CyBle_CpsApplCallback)
    {
        /* Error conditions for CP Characteristic value write request */
        if((eventParam->handleValPair.attrHandle == cyBle_cpss.charInfo[CYBLE_CPS_POWER_CP].charHandle) && 
           (!CYBLE_IS_INDICATION_ENABLED(cyBle_cpss.charInfo[CYBLE_CPS_POWER_CP].descrHandle[CYBLE_CPS_CCCD])))
        {
            gattErr = CYBLE_GATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
        else if((eventParam->handleValPair.attrHandle == cyBle_cpss.charInfo[CYBLE_CPS_POWER_CP].charHandle) && 
                ((cyBle_cpssFlag & CYBLE_CPSS_FLAG_CP_PROCEDURE_IN_PROGRESS) != 0u))
        {
            gattErr = CYBLE_GATT_ERR_PROCEDURE_ALREADY_IN_PROGRESS;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
        else
        {
            for(locCharIndex = CYBLE_CPS_POWER_MEASURE; (locCharIndex < CYBLE_CPS_CHAR_COUNT) && (locReqHandle == 0u); 
                locCharIndex++)
            {
                
                if((eventParam->handleValPair.attrHandle == 
                                                    cyBle_cpss.charInfo[locCharIndex].descrHandle[CYBLE_CPS_CCCD]) ||
                   (eventParam->handleValPair.attrHandle == 
                                                    cyBle_cpss.charInfo[locCharIndex].descrHandle[CYBLE_CPS_SCCD]) ||
                   (eventParam->handleValPair.attrHandle == cyBle_cpss.charInfo[locCharIndex].charHandle))
                {
                    /* Clear event handled flag to send Write response */
                    cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                    locReqHandle = 1u;
                    
                    locCharValue.connHandle = eventParam->connHandle;
                    locCharValue.charIndex = locCharIndex;
                    locCharValue.value = &eventParam->handleValPair.value;
                    /* Store value to database */
                    gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                                    &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                    
                    if(CYBLE_GATT_ERR_NONE == gattErr)
                    {
                        /* Characteristic value write request */
                        if(eventParam->handleValPair.attrHandle == cyBle_cpss.charInfo[locCharIndex].charHandle)
                        {
                            CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSS_CHAR_WRITE, &locCharValue);
                            /* In the context of the Cycling Power Control Point characteristic, a procedure is started
                               when a write to the Cycling Power Control Point characteristic is successfully completed
                            */
                            if(locCharIndex == CYBLE_CPS_POWER_CP)
                            {
                                cyBle_cpssFlag |= CYBLE_CPSS_FLAG_CP_PROCEDURE_IN_PROGRESS;
                            }
                        }
                        /* Client Characteristic Configuration descriptor write request */
                        else if(eventParam->handleValPair.attrHandle == 
                                cyBle_cpss.charInfo[locCharIndex].descrHandle[CYBLE_CPS_CCCD])
                        {
                            /* Check characteristic properties for Notification */
                            if(CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_cpss.charInfo[locCharIndex].charHandle))
                            {
                                if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                                {
                                    uint16 requiredConnIntv;
                                    requiredConnIntv = CYBLE_CPS_SAMLING_RATE_TO_CONN_INTV / 
                                                       cyBle_cpssAdjustment.samplingRate;
                                    /* Cycling Power Vector characteristic has special behavior 
                                       described in CPS specification, section 3.5.1 */
                                    if((locCharIndex == CYBLE_CPS_POWER_VECTOR) &&
                                       (cyBle_cpssConnParam.connIntv > requiredConnIntv))
                                    {
                                        /* If the current connection parameters do not allow sending of 
                                           notification, request new connection parameters by using the GAP Connection
                                           Parameter Update procedure.
                                        */
                                        CYBLE_GAP_CONN_UPDATE_PARAM_T connUpdateParam;
                                        
                                        /* Send Connection Parameter Update Request to Client */
                                        connUpdateParam.connIntvMin = requiredConnIntv;
                                        connUpdateParam.connIntvMax = requiredConnIntv;
                                        connUpdateParam.connLatency = CYBLE_GAPP_CONNECTION_SLAVE_LATENCY;
                                        connUpdateParam.supervisionTO = CYBLE_GAPP_CONNECTION_TIME_OUT;
                                        
                                        if(CYBLE_ERROR_OK == CyBle_L2capLeConnectionParamUpdateRequest(
                                                                          cyBle_connHandle.bdHandle, &connUpdateParam))
                                        {
                                            cyBle_cpssFlag |= CYBLE_CPSS_FLAG_PV_PROCEDURE_IN_PROGRESS;
                                            
                                            /* Set event handled flag to not send Write response. Response will be sent
                                               when Central accept the request for connection parameter update in 
                                               CyBle_CpssConnParamUpdateRspEventHandler function.
                                            */
                                            cyBle_eventHandlerFlag |= CYBLE_CALLBACK;
                                        }
                                    }
                                    else
                                    {
                                        CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSS_NOTIFICATION_ENABLED, &locCharValue);
                                    }
                                }
                                else
                                {
                                    CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSS_NOTIFICATION_DISABLED, &locCharValue);
                                }
                            }
                            /* Check characteristic properties for Indication */
                            if(CYBLE_IS_INDICATION_SUPPORTED(cyBle_cpss.charInfo[locCharIndex].charHandle))
                            {
                                if(CYBLE_IS_INDICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                                {
                                    CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSS_INDICATION_ENABLED, &locCharValue);
                                }
                                else
                                {
                                    CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSS_INDICATION_DISABLED, &locCharValue);
                                }
                            }
                        #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
                            (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
                            /* Set flag to store bonding data to flash */
                            cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
                        #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
                            
                        }
                        /* Server Characteristic Configuration descriptor write request */
                        else
                        {
                            /* Check characteristic properties for Broadcast */
                            if(CYBLE_IS_BROADCAST_SUPPORTED(cyBle_cpss.charInfo[locCharIndex].charHandle))
                            {
                                if(CYBLE_IS_BROADCAST_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                                {
                                    CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSS_BROADCAST_ENABLED, &locCharValue);
                                }
                                else
                                {
                                    CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSS_BROADCAST_DISABLED, &locCharValue);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return (gattErr);
}


/****************************************************************************** 
##Function Name: CyBle_CpssSendNotification
*******************************************************************************

Summary:
 Sends notification with a characteristic value of the CPS, which is a value
 specified by charIndex, to the Client device.

Parameters:
 connHandle:      The connection handle
 charIndex:       The index of a service characteristic of type 
                  CYBLE_CPS_CHAR_INDEX_T.
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
CYBLE_API_RESULT_T CyBle_CpssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CPS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{

    CYBLE_API_RESULT_T apiResult;
    
    /* Send Notification if it is enabled and connected */
    if(CYBLE_STATE_CONNECTED != CyBle_GetState())
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_CPS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_cpss.charInfo[charIndex].charHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else if((!CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_cpss.charInfo[charIndex].charHandle)) ||
            (!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_cpss.charInfo[charIndex].descrHandle[CYBLE_CPS_CCCD])))
    {
        apiResult = CYBLE_ERROR_NTF_DISABLED;
    }
    else
    {
        CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;
        
        /* Fill all fields of write request structure ... */
        ntfReqParam.attrHandle = cyBle_cpss.charInfo[charIndex].charHandle;
        ntfReqParam.value.val = attrValue;
        ntfReqParam.value.len = attrSize;
        
        /* Send notification to the Client using previously filled structure */
        apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
    }

    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpssSendIndication
*******************************************************************************

Summary:
 Sends indication with a characteristic value of the CPS, which is a value 
 specified by charIndex, to the Client device.

Parameters:
 connHandle:      The connection handle
 charIndex:       The index of a service characteristic of type 
                  CYBLE_CPS_CHAR_INDEX_T.
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
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
  * CYBLE_ERROR_IND_DISABLED - Indication is not enabled by the Client

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CpssSendIndication(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CPS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    /* Send Notification if it is enabled and connected */
    if(CYBLE_STATE_CONNECTED != CyBle_GetState())
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_CPS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_cpss.charInfo[charIndex].charHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    else if((!CYBLE_IS_INDICATION_SUPPORTED(cyBle_cpss.charInfo[charIndex].charHandle)) ||
            (!CYBLE_IS_INDICATION_ENABLED(cyBle_cpss.charInfo[charIndex].descrHandle[CYBLE_CPS_CCCD])))
    {
        apiResult = CYBLE_ERROR_IND_DISABLED;
    }
    else
    {
        CYBLE_GATTS_HANDLE_VALUE_IND_T indReqParam;
        
        /* Fill all fields of write request structure ... */
        indReqParam.attrHandle = cyBle_cpss.charInfo[charIndex].charHandle;
        indReqParam.value.val = attrValue;
        indReqParam.value.len = attrSize;
        
        /* Send indication to Client using previously filled structure */
        apiResult = CyBle_GattsIndication(connHandle, &indReqParam);
        /* Save handle to support service specific value confirmation response from Client */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_cpssReqHandle = indReqParam.attrHandle;
        }
    }
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpssConfirmationEventHandler
*******************************************************************************

Summary:
 Handles a Value Confirmation request event from the BLE stack.

Parameters:
 *eventParam - Pointer to a structure of type 'CYBLE_CONN_HANDLE_T'.

Return:
 None.

******************************************************************************/
void CyBle_CpssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    CYBLE_CPS_CHAR_INDEX_T locCharIndex;
    CYBLE_CPS_CHAR_VALUE_T locCharValue;
    uint8 locReqHandle = 0u;

    if((NULL != CyBle_CpsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_cpssReqHandle))
    {
        for(locCharIndex = CYBLE_CPS_POWER_MEASURE; (locCharIndex < CYBLE_CPS_CHAR_COUNT) && (locReqHandle == 0u); 
            locCharIndex++)
        {
            if(cyBle_cpssReqHandle == cyBle_cpss.charInfo[locCharIndex].charHandle)
            {
                locCharValue.connHandle = *eventParam;
                locCharValue.charIndex = locCharIndex;
                locCharValue.value = NULL;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                cyBle_cpssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                /* The Cycling Power Control Point procedure is complete when the Server indicated with the Op Code 
                   set to Response Code.
                */
                if(locCharIndex == CYBLE_CPS_POWER_CP)
                {
                    cyBle_cpssFlag &= (uint8)~CYBLE_CPSS_FLAG_CP_PROCEDURE_IN_PROGRESS;
                }
                CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSS_INDICATION_CONFIRMED, &locCharValue);
                locReqHandle = 1u;
            }
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CpssConnParamUpdateRspEventHandler
*******************************************************************************

Summary:
 Handles the L2CAP connection parameter response event from
 the BLE stack.

Parameters:
 uint16 response:
     * Accepted = 0x0000
     * Rejected = 0x0001

Return:
 None.

******************************************************************************/
void CyBle_CpssConnParamUpdateRspEventHandler(uint16 response)
{
    if((cyBle_cpssFlag & CYBLE_CPSS_FLAG_PV_PROCEDURE_IN_PROGRESS) != 0u)
    {
        cyBle_cpssFlag &= (uint8)~CYBLE_CPSS_FLAG_PV_PROCEDURE_IN_PROGRESS;
        if(response != 0u)
        {
            /* If the Client does not change the connection parameters Server shall return an ATT Error Response
               to the Write Request with an Error Code set to the Application Error Code 0x80 
               (Inappropriate Connection Parameters). */
            CYBLE_GATTS_ERR_PARAM_T err_param;
            
            err_param.opcode = (uint8) CYBLE_GATT_WRITE_REQ;
            err_param.attrHandle = cyBle_cpss.charInfo[CYBLE_CPS_POWER_VECTOR].descrHandle[CYBLE_CPS_CCCD];
            err_param.errorCode = CYBLE_GATT_ERR_CPS_INAPPROPRIATE_CONNECTION_PARAMETERS;

            /* Send Error Response */
            (void)CyBle_GattsErrorRsp(cyBle_connHandle, &err_param);
        }
        else
        {
            CYBLE_CPS_CHAR_VALUE_T locCharValue;
            CYBLE_GATT_VALUE_T locValue;
            uint16 locVal = CYBLE_CCCD_NOTIFICATION;
            
            /* Otherwise, the Server shall respond with a Write Response and start sending notifications 
               of the Cycling Power Vector characteristic. */
            (void)CyBle_GattsWriteRsp(cyBle_connHandle);
            
            locCharValue.connHandle = cyBle_connHandle;
            locCharValue.charIndex = CYBLE_CPS_POWER_VECTOR;
            locValue.len = CYBLE_CCCD_LEN;
            locValue.val = (uint8 *)&locVal;
            locCharValue.value = &locValue;
            
            CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSS_NOTIFICATION_ENABLED, &locCharValue);
            
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CpssConnUpdateCompleteEventHandler
*******************************************************************************

Summary:
 Handles the CYBLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE event from
 the BLE stack.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 None.

******************************************************************************/
void CyBle_CpssConnUpdateCompleteEventHandler(const CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *eventParam)
{
    /* Store connection parameters */
    cyBle_cpssConnParam = *eventParam;
}

#if CYBLE_GAP_ROLE_BROADCASTER

/******************************************************************************
##Function Name: CyBle_CpssStartBroadcast
*******************************************************************************

Summary:
 This function is used to start broadcasting of the Cycling Power 
 Measurement characteristic or update broadcasting data when it was started 
 before. 
 It is available only in Broadcaster role.  

Parameters:
 advInterval:     Advertising interval in 625 us units. The valid range is
                  from CYBLE_GAP_ADV_ADVERT_INTERVAL_NONCON_MIN
                  to CYBLE_GAP_ADV_ADVERT_INTERVAL_MAX.
 attrSize:        The size of the characteristic value attribute.
                  This size is limited by maximum advertising packet length and
                  advertising header size.
 *attrValue:      The pointer to the Cycling Power Measurement characteristic
                  that include the mandatory fields (e.g. the Flags field and the
                  Instantaneous Power field) and depending on the Flags field, 
                  some optional fields in a non connectable undirected advertising
                  event.

Return:
 CYBLE_API_RESULT_T : Return value indicates if the function succeeded or
 failed. Following are the possible error codes.
 <table>
  Errors codes                        Description
  ------------                        -----------
  CYBLE_ERROR_OK                      On successful operation.
  CYBLE_ERROR_INVALID_PARAMETER       On passing an invalid parameter.
 </table>

******************************************************************************/

CYBLE_API_RESULT_T CyBle_CpssStartBroadcast(uint16 advInterval, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    
    if((advInterval < CYBLE_GAP_ADV_ADVERT_INTERVAL_NONCON_MIN) || (advInterval > CYBLE_GAP_ADV_ADVERT_INTERVAL_MAX) ||
       (attrSize > (CYBLE_GAP_MAX_ADV_DATA_LEN - CYBLE_CPSS_BROADCAST_HEADER_LEN)))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Copy Cycling Power Measurement characteristic value to advertising data packet */
        (void) memcpy(&cyBle_cpssBroadcastData.advData[CYBLE_CPSS_BROADCAST_DATA_OFFSET], attrValue, attrSize);
        cyBle_cpssBroadcastData.advData[CYBLE_CPSS_BROADCAST_DATA_LEN_OFFSET] = attrSize;
        cyBle_cpssBroadcastData.advDataLen = attrSize + CYBLE_CPSS_BROADCAST_HEADER_LEN;
            
        if((cyBle_cpssFlag & CYBLE_CPSS_FLAG_BROADCAST_IN_PROGRESS) == 0u)
        {
            /* Put advertising interval to advertising data packet */
            cyBle_cpssBroadcastData.advData[CYBLE_CPSS_BROADCAST_ADVERT_INTERVAL_OFFSET] = LO8(advInterval);
            cyBle_cpssBroadcastData.advData[CYBLE_CPSS_BROADCAST_ADVERT_INTERVAL_OFFSET + 1u] = HI8(advInterval);
            /* Configure advertising timeout, interval and type */
            cyBle_cpssBroadcastParam.advIntvMin = advInterval; 
            cyBle_cpssBroadcastParam.advIntvMax = advInterval;
            cyBle_cpssBroadcastModeInfo.advTo = 0u;
            cyBle_cpssBroadcastParam.advType = CYBLE_GAPP_NON_CONNECTABLE_UNDIRECTED_ADV;
            cyBle_cpssBroadcastParam.advChannelMap = cyBle_discoveryParam.advChannelMap;
            /* Start broadcasting */
            apiResult = CyBle_GappEnterDiscoveryMode(&cyBle_cpssBroadcastModeInfo);
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_cpssFlag |= CYBLE_CPSS_FLAG_START_BROADCAST;
            }
        }
        else
        {
            /* Update broadcasting data */
            apiResult = CyBle_GapUpdateAdvData(&cyBle_cpssBroadcastData, NULL);
        }
    }
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpssStopBroadcast
*******************************************************************************

Summary:
 This function is used to stop broadcasting of the Cycling Power 
 Measurement characteristic.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_CpssStopBroadcast(void)
{
    if((cyBle_cpssFlag & CYBLE_CPSS_FLAG_BROADCAST_IN_PROGRESS) != 0u)
    {
        CyBle_GappExitDiscoveryMode();
        cyBle_cpssFlag |= CYBLE_CPSS_FLAG_STOP_BROADCAST;
    }
}


/****************************************************************************** 
##Function Name: CyBle_CpssAdvertisementStartStopEventHandler
*******************************************************************************

Summary:
 This function handles CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP event for CPS.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_CpssAdvertisementStartStopEventHandler(void)
{
    if((cyBle_cpssFlag & CYBLE_CPSS_FLAG_START_BROADCAST) != 0u)
    {
        cyBle_cpssFlag |= CYBLE_CPSS_FLAG_BROADCAST_IN_PROGRESS;
        cyBle_cpssFlag &= (uint8)~CYBLE_CPSS_FLAG_START_BROADCAST;
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
    if((cyBle_cpssFlag & CYBLE_CPSS_FLAG_STOP_BROADCAST) != 0u)
    {
        cyBle_cpssFlag &= (uint8)~(CYBLE_CPSS_FLAG_BROADCAST_IN_PROGRESS | CYBLE_CPSS_FLAG_STOP_BROADCAST);
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
}

#endif /* CYBLE_GAP_ROLE_BROADCASTER */

#endif /* CYBLE_CPS_SERVER */

#ifdef CYBLE_CPS_CLIENT


/****************************************************************************** 
##Function Name: CyBle_CpscDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 *discCharInfo:       The pointer to a characteristic information structure.

Return:
 None

******************************************************************************/
void CyBle_CpscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    /* CPS characteristics UUIDs */
    static const CYBLE_UUID16 cyBle_cpscCharUuid[CYBLE_CPS_CHAR_COUNT] =
    {
        CYBLE_UUID_CHAR_CPS_MSRMT,                  /* Cycling Power Measurement characteristic UUID */ 
        CYBLE_UUID_CHAR_CPS_FEATURE,                /* Cycling Power Feature characteristic UUID */ 
        CYBLE_UUID_CHAR_SENSOR_LOCATION,            /* Cycling Power Sensor Location characteristic UUID */
        CYBLE_UUID_CHAR_CPS_VECTOR,                 /* Cycling Power Vector characteristic UUID */ 
        CYBLE_UUID_CHAR_CPS_CP                      /* Cycling Power Control Point characteristic UUID */ 
    };
    uint8 i;
    static CYBLE_GATT_DB_ATTR_HANDLE_T *cpsLastEndHandle = NULL;

    /* Update last characteristic endHandle to declaration handle of this characteristic */ 
    if(cpsLastEndHandle != NULL)
    {
        *cpsLastEndHandle = discCharInfo->charDeclHandle - 1u;
        cpsLastEndHandle = NULL;
    }

    for(i = (uint8) CYBLE_CPS_POWER_MEASURE; i < (uint8) CYBLE_CPS_CHAR_COUNT; i++)
    {
        if(cyBle_cpscCharUuid[i] == discCharInfo->uuid.uuid16)
        {
            CyBle_CheckStoreCharHandle(cyBle_cpsc.charInfo[i]);
            cpsLastEndHandle = &cyBle_cpsc.charInfo[i].endHandle;
            break;
        }
    }
    
    /* Init characteristic endHandle to the Service endHandle.
       Characteristic endHandle will be updated to declaration
       handler of the following characteristic,
       in the following characteristic discovery procedure. */
    if(cpsLastEndHandle != NULL)
    {
        *cpsLastEndHandle = cyBle_serverInfo[cyBle_disCount].range.endHandle;
    }    
    
}


/****************************************************************************** 
##Function Name: CyBle_CpscDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a "CYBLE_EVT_GATTC_FIND_INFO_RSP" event.
 Based on the descriptor UUID, an appropriate data structure is populated using
 the data received as part of the callback.

Parameters:
 discoveryCharIndex:  The characteristic index which is discovered.
 *discDescrInfo:      The pointer to a descriptor information structure.

Return:
 None

******************************************************************************/
void CyBle_CpscDiscoverCharDescriptorsEventHandler(CYBLE_CPS_CHAR_INDEX_T discoveryCharIndex, 
                                                   CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    CYBLE_CPS_DESCR_INDEX_T locDescIndex;

    if(CYBLE_UUID_CHAR_CLIENT_CONFIG == discDescrInfo->uuid.uuid16)
    {
        locDescIndex = CYBLE_CPS_CCCD;
    }
    else if(CYBLE_UUID_CHAR_SERVER_CONFIG == discDescrInfo->uuid.uuid16)
    {
        locDescIndex = CYBLE_CPS_SCCD;
    }
    else    /* Not supported descriptor */
    {
        locDescIndex = CYBLE_CPS_DESCR_COUNT;
    }

    if(locDescIndex < CYBLE_CPS_DESCR_COUNT)
    {
        if(cyBle_cpsc.charInfo[discoveryCharIndex].descrHandle[locDescIndex] == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            cyBle_cpsc.charInfo[discoveryCharIndex].descrHandle[locDescIndex] = discDescrInfo->descrHandle;
        }
        else    /* Duplication of descriptor */
        {
            CyBle_ApplCallback(CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CpscSetCharacteristicValue
*******************************************************************************

Summary:
 Sends a request to set a characteristic value of the service, which is a value
 identified by charIndex, to the server device.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic of type 
                  CYBLE_CPS_CHAR_INDEX_T.
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
CYBLE_API_RESULT_T CyBle_CpscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_CPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T writeReqParam;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_CPS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_cpsc.charInfo[charIndex].valueHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        writeReqParam.attrHandle = cyBle_cpsc.charInfo[charIndex].valueHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &writeReqParam);
        /* Save handle to support service specific write response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_cpscReqHandle = writeReqParam.attrHandle;
            /* Start CP procedure. It will ends when the Collector sends a confirmation to acknowledge the CP 
               indication sent by the CP Sensor. A procedure is considered to have timed out if a CP indication
               is not received within the ATT transaction timeout, defined as 30 seconds. 
               CYBLE_EVT_TIMEOUT event with CYBLE_GENERIC_TO parameter will indicate about CP procedure timeout.
            */
            if(charIndex == CYBLE_CPS_POWER_CP)
            {
                (void)CyBle_StartTimer(CYBLE_CPS_CP_PROCEDURE_TIMEOUT);
                cyBle_cpscFlag |= CYBLE_CPSC_FLAG_CP_PROCEDURE_IN_PROGRESS;
            }
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpscGetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to read a characteristic value, which is a value identified by 
 charIndex, from the server.

 The Read Response returns the characteristic Value in the Attribute Value
 parameter.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic of type 
                  CYBLE_CPS_CHAR_INDEX_T.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The read request was sent successfully  
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                    characteristic

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CpscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_CPS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_CPS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_cpsc.charInfo[charIndex].valueHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, 
                                                       cyBle_cpsc.charInfo[charIndex].valueHandle);
        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_cpscReqHandle = cyBle_cpsc.charInfo[charIndex].valueHandle;
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpscSetCharacteristicDescriptor
*******************************************************************************

Summary:
 This function is used to write the characteristic descriptor to the server
 which is identified by charIndex

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic of type 
                  CYBLE_CPS_CHAR_INDEX_T.
 descrIndex:      The index of a service characteristic descriptor of type 
                  CYBLE_CPS_DESCR_INDEX_T.
 attrSize:        The size of the characteristic value attribute.
 *attrValue:      The pointer to the characteristic descriptor value data that 
                  should be sent to the server device. 

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_INVALID_STATE - The state is not valid
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
                                    the specified attribute

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CpscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CPS_CHAR_INDEX_T charIndex, CYBLE_CPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_CPS_CHAR_COUNT) || (descrIndex >= CYBLE_CPS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
     /* Fill all fields of write request structure ... */
        writeReqParam.attrHandle = cyBle_cpsc.charInfo[charIndex].descrHandle[descrIndex];
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        /* ... and send request to server device. */
        apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
        
        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_cpscReqHandle = writeReqParam.attrHandle;
        }
    }
    
    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sends a request to get the characteristic descriptor of the specified 
 characteristic of the service.

Parameters:
 connHandle:      The connection handle.
 charIndex:       The index of a service characteristic of type 
                  CYBLE_CPS_CHAR_INDEX_T.
 descrIndex:      The index of a service characteristic descriptor of type 
                  CYBLE_CPS_DESCR_INDEX_T.

Return:
 * CYBLE_ERROR_OK - The request was sent successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_INVALID_STATE - The state is not valid
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
                                    the specified attribute

******************************************************************************/
CYBLE_API_RESULT_T CyBle_CpscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CPS_CHAR_INDEX_T charIndex, CYBLE_CPS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex >= CYBLE_CPS_CHAR_COUNT) || (descrIndex >= CYBLE_CPS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, 
                                                    cyBle_cpsc.charInfo[charIndex].descrHandle[descrIndex]);
        
        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_cpscReqHandle = cyBle_cpsc.charInfo[charIndex].descrHandle[descrIndex];
        }
    }

    return (apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_CpscNotificationEventHandler
*******************************************************************************

Summary:
 Handles Notification Event.

Parameters:
 *eventParam:  The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_CpscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    CYBLE_CPS_CHAR_INDEX_T locCharIndex;
    CYBLE_CPS_CHAR_VALUE_T notifValue;

    if(NULL != CyBle_CpsApplCallback)
    {
        for(locCharIndex = CYBLE_CPS_POWER_MEASURE; locCharIndex < CYBLE_CPS_CHAR_COUNT; locCharIndex++)
        {
            if(cyBle_cpsc.charInfo[locCharIndex].valueHandle == eventParam->handleValPair.attrHandle)
            {
                notifValue.connHandle = eventParam->connHandle;
                notifValue.charIndex = locCharIndex;
                notifValue.value = &eventParam->handleValPair.value;
                CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSC_NOTIFICATION, &notifValue);
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                break;
            }
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CpscIndicationEventHandler
*******************************************************************************

Summary:
 Handles Indication Event.

Parameters:
 *eventParam:  The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_CpscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam)
{
    CYBLE_CPS_CHAR_INDEX_T locCharIndex;
    CYBLE_CPS_CHAR_VALUE_T indicationValue;

    if(NULL != CyBle_CpsApplCallback)
    {
        for(locCharIndex = CYBLE_CPS_POWER_MEASURE; locCharIndex < CYBLE_CPS_CHAR_COUNT; locCharIndex++)
        {
            if(cyBle_cpsc.charInfo[locCharIndex].valueHandle == eventParam->handleValPair.attrHandle)
            {
                /* Stop the timer. CP procedure is finished when received indication sent by the CP Sensor. */
                if((cyBle_cpscReqHandle == cyBle_cpsc.charInfo[CYBLE_CPS_POWER_CP].valueHandle) && 
                   ((cyBle_cpscFlag & CYBLE_CPSC_FLAG_CP_PROCEDURE_IN_PROGRESS) != 0u))
                {
                    (void)CyBle_StopTimer();
                    cyBle_cpscFlag &= (uint8)~CYBLE_CPSC_FLAG_CP_PROCEDURE_IN_PROGRESS;
                }

                indicationValue.connHandle = eventParam->connHandle;
                indicationValue.charIndex = locCharIndex;
                indicationValue.value = &eventParam->handleValPair.value;
                CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSC_INDICATION, &indicationValue);
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                break;
            }
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CpscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles Read Response Event.

Parameters:
 *eventParam:  The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_CpscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    uint8 locReqHandle = 0u;
    CYBLE_CPS_CHAR_INDEX_T locCharIndex;

    if((NULL != CyBle_CpsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_cpscReqHandle))
    {
        for(locCharIndex = CYBLE_CPS_POWER_MEASURE; (locCharIndex < CYBLE_CPS_CHAR_COUNT) && (locReqHandle == 0u); 
            locCharIndex++)
        {
            if(cyBle_cpscReqHandle == cyBle_cpsc.charInfo[locCharIndex].valueHandle)
            {
                CYBLE_CPS_CHAR_VALUE_T locCharValue;
                
                locCharValue.connHandle = eventParam->connHandle;
                locCharValue.charIndex = locCharIndex;
                locCharValue.value = &eventParam->value;
                cyBle_cpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSC_READ_CHAR_RESPONSE, &locCharValue);
                locReqHandle = 1u;
            }
            else
            {
                CYBLE_CPS_DESCR_INDEX_T locDescIndex;

                for(locDescIndex = CYBLE_CPS_CCCD; (locDescIndex < CYBLE_CPS_DESCR_COUNT) &&
                   (locReqHandle == 0u); locDescIndex++)
                {
                    if(cyBle_cpscReqHandle == cyBle_cpsc.charInfo[locCharIndex].descrHandle[locDescIndex])
                    {
                        CYBLE_CPS_DESCR_VALUE_T locDescrValue;
                        
                        locDescrValue.connHandle = eventParam->connHandle;
                        locDescrValue.charIndex = locCharIndex;
                        locDescrValue.descrIndex = locDescIndex;
                        locDescrValue.value = &eventParam->value;
                        cyBle_cpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                        CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSC_READ_DESCR_RESPONSE, &locDescrValue);
                        locReqHandle = 1u;
                    }
                }
            }
        }
        if(locReqHandle != 0u)
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CpscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles Write Response Event.

Parameters:
 *eventParam:  The pointer to a data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_CpscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    uint8 locReqHandle = 0u;
    CYBLE_CPS_CHAR_INDEX_T locCharIndex;
    
    if((NULL != CyBle_CpsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_cpscReqHandle))
    {
        for(locCharIndex = CYBLE_CPS_POWER_MEASURE; (locCharIndex < CYBLE_CPS_CHAR_COUNT) && (locReqHandle == 0u); 
            locCharIndex++)
        {
            if(cyBle_cpscReqHandle == cyBle_cpsc.charInfo[locCharIndex].valueHandle)
            {
                CYBLE_CPS_CHAR_VALUE_T locCharValue;
                
                locCharValue.connHandle = *eventParam;
                locCharValue.charIndex = locCharIndex;
                locCharValue.value = NULL;
                cyBle_cpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSC_WRITE_CHAR_RESPONSE, &locCharValue);
                locReqHandle = 1u;
            }
            else 
            {
            
                CYBLE_CPS_DESCR_INDEX_T locDescIndex;

                for(locDescIndex = CYBLE_CPS_CCCD; (locDescIndex < CYBLE_CPS_DESCR_COUNT) &&
                   (locReqHandle == 0u); locDescIndex++)
                {
                    if(cyBle_cpscReqHandle == cyBle_cpsc.charInfo[locCharIndex].descrHandle[locDescIndex])
                    {
                        CYBLE_CPS_DESCR_VALUE_T locDescrValue;
                        
                        locDescrValue.connHandle = *eventParam;
                        locDescrValue.charIndex = locCharIndex;
                        locDescrValue.descrIndex = locDescIndex;
                        locDescrValue.value = NULL;
                        cyBle_cpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                        CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSC_WRITE_DESCR_RESPONSE, &locDescrValue);
                        locReqHandle = 1u;
                    }
                }
            }
        }
        if(locReqHandle != 0u)
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_CpscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles Error Response Event.

Parameters:
 *eventParam:  The pointer to a data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_CpscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((NULL != eventParam) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_cpscReqHandle))
    {
        cyBle_cpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}

/******************************************************************************
##Function Name: CyBle_CpscStartObserve
*******************************************************************************

 Summary:
  This function is used for observing GAP peripheral devices. 
  A device performing the observer role receives only advertisement data from
  devices irrespective of their discoverable mode settings. Advertisement 
  data received is provided by the event, CYBLE_EVT_CPSC_SCAN_PROGRESS_RESULT.
  This procedure sets the scanType sub parameter to passive scanning. 
  
  If 'scanTo' sub-parameter is set to zero value, then passive scanning 
  procedure will continue until you call CyBle_GapcStopObserve API.
  Possible generated events are:
  * CYBLE_EVT_CPSC_SCAN_PROGRESS_RESULT 

 Parameters:
  None.

Return:
 CYBLE_API_RESULT_T : Return value indicates if the function succeeded or
 failed. Following are the possible error codes.
 <table>
  Errors codes                        Description
  ------------                        -----------
  CYBLE_ERROR_OK                      On successful operation.
  CYBLE_ERROR_STACK_INTERNAL          An error occurred in the BLE stack.
 </table>
 
******************************************************************************/
CYBLE_API_RESULT_T CyBle_CpscStartObserve(void)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GAPC_DISC_INFO_T locDiscoveryInfo;

    locDiscoveryInfo.discProcedure = CYBLE_GAPC_OBSER_PROCEDURE;
    locDiscoveryInfo.scanType = CYBLE_GAPC_PASSIVE_SCANNING;
    locDiscoveryInfo.scanIntv = CYBLE_FAST_SCAN_INTERVAL;
    locDiscoveryInfo.scanWindow = CYBLE_FAST_SCAN_WINDOW;
    locDiscoveryInfo.ownAddrType = CYBLE_GAP_ADDR_TYPE_PUBLIC;
    locDiscoveryInfo.scanFilterPolicy = CYBLE_GAPC_ADV_ACCEPT_ALL_PKT;
    locDiscoveryInfo.scanTo = 0u;           /* Disable Timeout */
    locDiscoveryInfo.filterDuplicates = CYBLE_GAPC_FILTER_DUP_DISABLE;

    apiResult = CyBle_GapcStartDiscovery(&locDiscoveryInfo);

    if(CYBLE_ERROR_OK == apiResult)
    {
        cyBle_cpscFlag |= CYBLE_CPSC_FLAG_START_OBSERVE;
    }
   
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_CpscStopObserve
*******************************************************************************

 Summary:
  This function used to stop the discovery of devices. On stopping discovery
  operation, CYBLE_EVT_GAPC_SCAN_START_STOP event is generated. Application
  layer needs to keep track of the function call made before receiving this
  event to associate this event with either the start or stop discovery
  function.

  Possible events generated are:
   * CYBLE_EVT_GAPC_SCAN_START_STOP

 Parameters:
  None

 Return:
  None

******************************************************************************/
void CyBle_CpscStopObserve(void)
{
    if((cyBle_cpscFlag & CYBLE_CPSC_FLAG_OBSERVE_IN_PROGRESS) != 0u)
    {
        CyBle_GapcStopDiscovery();
        cyBle_cpscFlag |= CYBLE_CPSC_FLAG_STOP_OBSERVE;
    }
}


/****************************************************************************** 
##Function Name: CyBle_CpscScanStartStopEventHandler
*******************************************************************************

Summary:
 This function handles CYBLE_EVT_GAPC_SCAN_START_STOP event for CPS.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_CpscScanStartStopEventHandler(void)
{
    if((cyBle_cpscFlag & CYBLE_CPSC_FLAG_START_OBSERVE) != 0u)
    {
        cyBle_cpscFlag |= CYBLE_CPSC_FLAG_OBSERVE_IN_PROGRESS;
        cyBle_cpscFlag &= (uint8)~CYBLE_CPSC_FLAG_START_OBSERVE;
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
    if((cyBle_cpscFlag & CYBLE_CPSC_FLAG_STOP_OBSERVE) != 0u)
    {
        cyBle_cpscFlag &= (uint8)~(CYBLE_CPSC_FLAG_OBSERVE_IN_PROGRESS | CYBLE_CPSC_FLAG_STOP_OBSERVE);
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
}


/****************************************************************************** 
##Function Name: CyBle_CpscScanProcessEventHandler
*******************************************************************************

Summary:
 This function handles CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT event for CPS.

Parameters:
 *eventParam:  the pointer to a data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_CpscScanProcessEventHandler(CYBLE_GAPC_ADV_REPORT_T *eventParam)
{
    if((cyBle_cpscFlag & CYBLE_CPSC_FLAG_OBSERVE_IN_PROGRESS) != 0u) 
    {
        if(eventParam->eventType == CYBLE_GAPC_NON_CONN_UNDIRECTED_ADV)
        {
            if(NULL != CyBle_CpsApplCallback)
            {
                CYBLE_CPS_CHAR_VALUE_T broadcastValue;
                CYBLE_GATT_VALUE_T locCharValue;
                uint8 advIndex = 0u;
                do
                {
                    /* Show Cycling Power Measurement characteristic value from Service Data packet. */
                    if((eventParam->data[advIndex] > CYBLE_CPSC_BROADCAST_DATA_OFFSET) &&
                       (eventParam->data[advIndex + 1u] == (uint8)CYBLE_GAP_ADV_SRVC_DATA_16UUID))
                    {
                        broadcastValue.connHandle = cyBle_connHandle;
                        broadcastValue.charIndex = CYBLE_CPS_POWER_MEASURE;
                        broadcastValue.value = &locCharValue;
                        locCharValue.val = &eventParam->data[advIndex + CYBLE_CPSC_BROADCAST_DATA_OFFSET];
                        locCharValue.len = eventParam->data[advIndex] - (CYBLE_CPSC_BROADCAST_DATA_OFFSET - 1u);
                        CyBle_CpsApplCallback((uint32)CYBLE_EVT_CPSC_SCAN_PROGRESS_RESULT, &broadcastValue);
                        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                    }
                    advIndex += eventParam->data[advIndex] + 1u;
                }while(advIndex < eventParam->dataLen);    
            }
        }
        else    /* Filter for all connectable advertising packets */
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
    
}

#endif /* (CYBLE_CPS_CLIENT) */


/* [] END OF FILE */

