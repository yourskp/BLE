/*******************************************************************************
File Name: CYBLE_ndcs.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the source code for Next DST Change Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_NdcsApplCallback = NULL;

#if defined(CYBLE_NDCS_SERVER)

`$NdcsServer`
    
#endif /* CYBLE_NDCS_SERVER */

#if defined(CYBLE_NDCS_CLIENT)

CYBLE_NDCSC_T cyBle_ndcsc;
    
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_ndcscReqHandle;

#endif /* (CYBLE_NDCS_CLIENT) */


/* DOM-IGNORE-BEGIN */
/*******************************************************************************
##Function Name: CyBle_NdcsInit
********************************************************************************

Summary:
 This function initializes the Next DST Change Service.

Parameters:
 None

Return:
 None

*******************************************************************************/
void CyBle_NdcsInit(void)
{
#ifdef CYBLE_NDCS_CLIENT

    (void)memset(&cyBle_ndcsc, 0, sizeof(cyBle_ndcsc));
    
    cyBle_ndcscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_NDCS_CLIENT */
/* DOM-IGNORE-END */
}


/*******************************************************************************
##Function Name: CyBle_NdcsRegisterAttrCallback
********************************************************************************

Summary:
 Registers a callback function for Next DST Change Service specific attribute
 operations.

Parameters:
 callbackFunc:  An application layer event callback function to receive 
                events from the BLE Component. The definition of 
                CYBLE_CALLBACK_T for NDCS is,
        
                typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
                                                  void *eventParam)

                * eventCode indicates the event that triggered this 
                  callback.
                * eventParam contains the parameters corresponding to the 
                  current event.

Return:
 None.

*******************************************************************************/
void CyBle_NdcsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_NdcsApplCallback = callbackFunc;
}


#if defined(CYBLE_NDCS_SERVER)

/*******************************************************************************
##Function Name: CyBle_NdcssSetCharacteristicValue
********************************************************************************

Summary:
 Sets characteristic value of the Next DST Change Service, which is identified
 by charIndex in the local database.

Parameters:
 CYBLE_NDCS_CHAR_INDEX_T charIndex: The index of a service characteristic of
                                    type CYBLE_NDCS_CHAR_INDEX_T.
 uint8 attrSize:                    The size of the characteristic value 
                                    attribute.
 uint8 *attrValue:                  The pointer to the characteristic value
                                    data that should be stored to the GATT
                                    database.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - the request is handled successfully;
  * CYBLE_ERROR_INVALID_PARAMETER - validation of the input parameters failed.

*/
CYBLE_API_RESULT_T CyBle_NdcssSetCharacteristicValue(CYBLE_NDCS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(CYBLE_NDCS_TIME_WITH_DST == charIndex)
    {
        /* Store data in database */
        locHandleValuePair.attrHandle = cyBle_ndcss.timeWithDst;
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
##Function Name: CyBle_NdcssGetCharacteristicValue
********************************************************************************

Summary:
 Gets a characteristic value of the Next DST Change Service, which is
 identified by charIndex.

Parameters:
 CYBLE_NDCS_CHAR_INDEX_T charIndex: The index of a service characteristic of
                                    type CYBLE_NDCS_CHAR_INDEX_T.
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
CYBLE_API_RESULT_T CyBle_NdcssGetCharacteristicValue(CYBLE_NDCS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(CYBLE_NDCS_TIME_WITH_DST == charIndex)
    {
        /* Read characteristic value from database */
        locHandleValuePair.attrHandle = cyBle_ndcss.timeWithDst;
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

#endif /* CYBLE_NDCS_SERVER */


#if defined(CYBLE_NDCS_CLIENT)

/*******************************************************************************
##Function Name: CyBle_NdcscDiscoverCharacteristicsEventHandler
********************************************************************************

Summary:
 This function is called on receiving the CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 CYBLE_DISC_CHAR_INFO_T *discCharInfo: The pointer to a characteristic
                                       information structure.
 

Return:
 None

*******************************************************************************/
void CyBle_NdcscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    if(CYBLE_UUID_CHAR_TIME_WITH_DST == discCharInfo->uuid.uuid16)
    {
        /* Using characteristic UUID store handle of requested characteristic */
        if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE == cyBle_ndcsc.charInfo[CYBLE_NDCS_TIME_WITH_DST].valueHandle)
        {
            cyBle_ndcsc.charInfo[CYBLE_NDCS_TIME_WITH_DST].valueHandle = discCharInfo->valueHandle;
            cyBle_ndcsc.charInfo[CYBLE_NDCS_TIME_WITH_DST].properties  = discCharInfo->properties;
        }
        else
        {
            CyBle_ApplCallback(CYBLE_EVT_GATTC_CHAR_DUPLICATION, &discCharInfo->uuid);
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_NdcscReadResponseEventHandler
********************************************************************************

Summary:
 Handles Read Response Event for Next DST Change Service.

Parameters:
 CYBLE_GATTC_READ_RSP_PARAM_T *eventParam: The pointer to the data that came 
                                           with a read response for NDCS.

Return:
 None.

*******************************************************************************/
void CyBle_NdcscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam)
{
    CYBLE_NDCS_CHAR_VALUE_T rdRspParam;
    
    if((NULL != CyBle_NdcsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ndcscReqHandle))
    {
        if(cyBle_ndcsc.charInfo[CYBLE_NDCS_TIME_WITH_DST].valueHandle == cyBle_ndcscReqHandle)
        {
            /* Fill Reference Time Update Service read response parameter structure with
            * characteristic info.
            */
            rdRspParam.connHandle = eventParam->connHandle;
            rdRspParam.charIndex = CYBLE_NDCS_TIME_WITH_DST;
            rdRspParam.value = &eventParam->value;

            CyBle_NdcsApplCallback((uint32)CYBLE_EVT_NDCSC_READ_CHAR_RESPONSE, (void *)&rdRspParam);
                
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
            cyBle_ndcscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        }
    }
}


/*******************************************************************************
##Function Name: CyBle_NdcscGetCharacteristicValue
********************************************************************************

Summary:
 Sends a request to peer device to set characteristic value of the Next 
 DST Change Service, which is identified by charIndex.

Parameters:
 CYBLE_CONN_HANDLE_T connHandle:    The connection handle.
 CYBLE_NDCS_CHAR_INDEX_T charIndex: The index of a service characteristic.

Return:
 Return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - the request was sent successfully.
  * CYBLE_ERROR_INVALID_STATE - connection with the client is not established.
  * CYBLE_ERROR_INVALID_PARAMETER - validation of the input parameters failed.
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
                                     characteristic.

*******************************************************************************/
CYBLE_API_RESULT_T CyBle_NdcscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_NDCS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(CYBLE_NDCS_TIME_WITH_DST == charIndex)
    {
        /* Send request to read characteristic value */
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle,
            cyBle_ndcsc.charInfo[CYBLE_NDCS_TIME_WITH_DST].valueHandle);

        if(CYBLE_ERROR_OK == apiResult)
        {
            cyBle_ndcscReqHandle = cyBle_ndcsc.charInfo[CYBLE_NDCS_TIME_WITH_DST].valueHandle;
        }
    }
    else
    {
        /* apiResult equals to CYBLE_ERROR_INVALID_PARAMETER */
    }

    return(apiResult);
}

#endif /* CYBLE_NDCS_CLIENT */
    

/* [] END OF FILE */
