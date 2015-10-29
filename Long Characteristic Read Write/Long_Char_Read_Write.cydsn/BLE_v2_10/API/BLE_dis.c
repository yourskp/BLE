/*******************************************************************************
File Name: CYBLE_dis.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the source code for the Device Information Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "`$INSTANCE_NAME`_eventHandler.h"

#ifdef CYBLE_DIS_SERVER

`$DisServer`

#endif /* CYBLE_DIS_SERVER */

#ifdef CYBLE_DIS_CLIENT

CYBLE_DISC_T cyBle_disc;
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_discReqHandle;
static CYBLE_CALLBACK_T CyBle_DisApplCallback = NULL;

#endif /* (CYBLE_DIS_CLIENT) */


/****************************************************************************** 
##Function Name: CyBle_DisInit
*******************************************************************************

Summary:
 This function initializes the DIS service.

Parameters:
 None

Return:
 None

******************************************************************************/
void CyBle_DisInit(void)
{

#ifdef CYBLE_DIS_CLIENT

    (void)memset(&cyBle_disc, 0, sizeof(cyBle_disc));
    cyBle_discReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_DIS_CLIENT */

}


/****************************************************************************** 
##Function Name: CyBle_DisRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.
 Callback doesn't have events in server role.

Parameters:
 callbackFunc: An application layer event callback function to receive 
                   events from the BLE Component. The definition of 
                   CYBLE_CALLBACK_T for Device Information Service is,
            
                   typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                      void *eventParam)

                   * eventCode indicates the event that triggered this 
                     callback.
                   * eventParam contains the parameters corresponding to the 
                     current event.

Return:
 None

******************************************************************************/
void CyBle_DisRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    
#ifdef CYBLE_DIS_CLIENT
    CyBle_DisApplCallback = callbackFunc;
#else
    if(callbackFunc != NULL) /* Callback doesn't have events in server role */
    {
    }
#endif /* CYBLE_DIS_CLIENT */
    
}

#ifdef CYBLE_DIS_SERVER


/******************************************************************************
##Function Name: CyBle_DissSetCharacteristicValue
*******************************************************************************

Summary:
 Sets a characteristic value of the service, which is identified by charIndex,
 to the local database.


Parameters:
 charIndex: The index of a service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the characteristic value data that should be
                  stored to the GATT database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_DissSetCharacteristicValue(CYBLE_DIS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex >= CYBLE_DIS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Store data in database */
        locHandleValuePair.attrHandle = cyBle_diss.charHandle[charIndex];
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
##Function Name: CyBle_DissGetCharacteristicValue
*******************************************************************************

Summary:
 Gets a characteristic value of the service, which is identified by charIndex,
 from the GATT database.

Parameters:
 charIndex: The index of a service characteristic.
 attrSize: The size of the characteristic value attribute.
 *attrValue: The pointer to the location where characteristic value data
              should be stored.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed

******************************************************************************/
CYBLE_API_RESULT_T CyBle_DissGetCharacteristicValue(CYBLE_DIS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex >= CYBLE_DIS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Read characteristic value from database */
        locHandleValuePair.attrHandle = cyBle_diss.charHandle[charIndex];
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

#endif /* CYBLE_DIS_SERVER */

#ifdef CYBLE_DIS_CLIENT


/******************************************************************************
##Function Name: CyBle_DiscHandleDiscoverCharacteristics
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP event.
 Based on the service UUID, an appropriate data structure is populated using the
 data received as part of the callback.

Parameters:
 *discCharInfo:  the pointer to a characteristic information structure.

Return:
 None

******************************************************************************/
void CyBle_DiscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    switch(discCharInfo->uuid.uuid16)
    {
        case CYBLE_UUID_CHAR_MANUFACTURER_NAME:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_MANUFACTURER_NAME]);
            break;
        case CYBLE_UUID_CHAR_MODEL_NUMBER:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_MODEL_NUMBER]);
            break;
        case CYBLE_UUID_CHAR_SERIAL_NUMBER:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_SERIAL_NUMBER]);
            break;
        case CYBLE_UUID_CHAR_HARDWARE_REV:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_HARDWARE_REV]);
            break;
        case CYBLE_UUID_CHAR_FIRMWARE_REV:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_FIRMWARE_REV]);
            break;
        case CYBLE_UUID_CHAR_SOFTWARE_REV:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_SOFTWARE_REV]);
            break;
        case CYBLE_UUID_CHAR_SYSTEM_ID:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_SYSTEM_ID]);
            break;
        case CYBLE_UUID_CHAR_REG_CERT_DATA:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_REG_CERT_DATA]);
            break;
        case CYBLE_UUID_CHAR_PNP_ID:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_PNP_ID]);
            break;
        default:
            break;
    }
}


/******************************************************************************
##Function Name: CyBle_DiscGetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to read the characteristic Value from a server
 which is identified by charIndex.

 The Read Response returns the characteristic value in the Attribute Value
 parameter. The Read Response only contains the characteristic value that is 
 less than or equal to (MTU - 1) octets in length. If the characteristic value 
 is greater than (MTU - 1) octets in length, a Read Long Characteristic Value
 procedure may be used if the rest of the characteristic value is required.

 This function call can result in generation of the following events based on
 the response from the server device.
 
 * CYBLE_EVT_DISC_READ_CHAR_RESPONSE
 * CYBLE_EVT_GATTC_ERROR_RSP

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The read request was sent successfully  
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                    characteristic

******************************************************************************/
CYBLE_API_RESULT_T CyBle_DiscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_DIS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(charIndex >= CYBLE_DIS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_disc.charInfo[charIndex].valueHandle);
        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_discReqHandle = cyBle_disc.charInfo[charIndex].valueHandle;
        }
    }
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_DiscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles the Read Response Event.

Parameters:
 *eventParam: the pointer to the data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_DiscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    CYBLE_DIS_CHAR_INDEX_T locCharIndex;

    if((NULL != CyBle_DisApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_discReqHandle))
    {
        for(locCharIndex = CYBLE_DIS_MANUFACTURER_NAME; locCharIndex < CYBLE_DIS_CHAR_COUNT; locCharIndex++)
        {
            if(cyBle_discReqHandle == cyBle_disc.charInfo[locCharIndex].valueHandle)
            {
                CYBLE_DIS_CHAR_VALUE_T locCharValue;
                
                locCharValue.connHandle = eventParam->connHandle;
                locCharValue.charIndex = locCharIndex;
                locCharValue.value = &eventParam->value;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                cyBle_discReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_DisApplCallback((uint32)CYBLE_EVT_DISC_READ_CHAR_RESPONSE, &locCharValue);
                break;
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_DiscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles the Error Response Event.

Parameters:
 *eventParam: the pointer to the data structure specified by the event.

Return:
 None

******************************************************************************/
void CyBle_DiscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((NULL != eventParam) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_discReqHandle))
    {
        cyBle_discReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


#endif /* (CYBLE_DIS_CLIENT) */


/* [] END OF FILE */

