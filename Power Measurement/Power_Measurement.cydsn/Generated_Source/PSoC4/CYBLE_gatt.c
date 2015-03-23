/*******************************************************************************
File Name: CYBLE_gatt.c
Version 1.20

Description:
 This file contains the source code for the GATT API of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#include "CYBLE_eventHandler.h"


/***************************************
##Global variables
***************************************/

CYBLE_STATE_T cyBle_state;


#if(CYBLE_GATT_ROLE_SERVER)
    
    const CYBLE_GATTS_T cyBle_gatts =
{
    0x0008u,    /* Handle of the GATT service */
    0x000Au,    /* Handle of the Service Changed characteristic */
    0x000Bu,    /* Handle of the Client Characteristic Configuration descriptor */
};
    
    static uint8 cyBle_attValues[0x28u] = {
/* Device Name */
(uint8)'H', (uint8)'e', (uint8)'a', (uint8)'r', (uint8)'t', (uint8)' ', (uint8)'R', (uint8)'a', (uint8)'t', (uint8)'e',
(uint8)' ', (uint8)'S', (uint8)'e', (uint8)'n', (uint8)'s', (uint8)'o', (uint8)'r', 

/* Appearance */
0x40u, 0x03u, 

/* Peripheral Preferred Connection Parameters */
0x28u, 0x00u, 0x38u, 0x00u, 0x00u, 0x00u, 0x1Eu, 0x00u, 

/* Service Changed */
0x00u, 0x00u, 0x00u, 0x00u, 

/* Heart Rate Measurement */
0x04u, 0x00u, 

/* Manufacturer Name String */
(uint8)'C', (uint8)'y', (uint8)'p', (uint8)'r', (uint8)'e', (uint8)'s', (uint8)'s', 

};

#if defined(__GNUC__) || defined(__ARMCC_VERSION)
const uint8 cyBle_attValuesCCCDFlashMemory[CYBLE_GAP_MAX_BONDED_DEVICE + 1u][CYBLE_GATT_DB_CCCD_COUNT] CYBLE_FLASH_ROW_ALIGNED = {
#elif defined(__ICCARM__)
/* The cyBle_attValuesCCCDFlashMemory[] should be aligned to the size of a Flash row */
#pragma data_alignment=CY_FLASH_SIZEOF_ROW
const uint8 cyBle_attValuesCCCDFlashMemory[CYBLE_GAP_MAX_BONDED_DEVICE + 1u][CYBLE_GATT_DB_CCCD_COUNT] = {
#endif /* defined(__GNUC__) || defined(__ARMCC_VERSION) */
{
/* Client Characteristic Configuration */
0x00u, 0x00u, 

/* Client Characteristic Configuration */
0x00u, 0x00u, 

},
{
/* Client Characteristic Configuration */
0x00u, 0x00u, 

/* Client Characteristic Configuration */
0x00u, 0x00u, 

},
{
/* Client Characteristic Configuration */
0x00u, 0x00u, 

/* Client Characteristic Configuration */
0x00u, 0x00u, 

},
{
/* Client Characteristic Configuration */
0x00u, 0x00u, 

/* Client Characteristic Configuration */
0x00u, 0x00u, 

},
{
/* Client Characteristic Configuration */
0x00u, 0x00u, 

/* Client Characteristic Configuration */
0x00u, 0x00u, 

},
};
uint8 cyBle_attValuesCCCD[CYBLE_GATT_DB_CCCD_COUNT];

const CYBLE_GATTS_DB_T cyBle_gattDB[0x12u] = {
{ 0x0001u, 0x2800u /* Primary service                     */, 0x00000001u /*        */, 0x0007u, {{0x1800u, NULL}}                           },
{ 0x0002u, 0x2803u /* Characteristic                      */, 0x00000201u /* rd     */, 0x0003u, {{0x2A00u, NULL}}                           },
{ 0x0003u, 0x2A00u /* Device Name                         */, 0x00000201u /* rd     */, 0x0003u, {{0x0011u, (void *)&cyBle_attValues[0]}}    },
{ 0x0004u, 0x2803u /* Characteristic                      */, 0x00000201u /* rd     */, 0x0005u, {{0x2A01u, NULL}}                           },
{ 0x0005u, 0x2A01u /* Appearance                          */, 0x00000201u /* rd     */, 0x0005u, {{0x0002u, (void *)&cyBle_attValues[17]}}   },
{ 0x0006u, 0x2803u /* Characteristic                      */, 0x00000201u /* rd     */, 0x0007u, {{0x2A04u, NULL}}                           },
{ 0x0007u, 0x2A04u /* Peripheral Preferred Connection Par */, 0x00000201u /* rd     */, 0x0007u, {{0x0008u, (void *)&cyBle_attValues[19]}}   },
{ 0x0008u, 0x2800u /* Primary service                     */, 0x00000001u /*        */, 0x000Bu, {{0x1801u, NULL}}                           },
{ 0x0009u, 0x2803u /* Characteristic                      */, 0x00002201u /* rd,ind */, 0x000Bu, {{0x2A05u, NULL}}                           },
{ 0x000Au, 0x2A05u /* Service Changed                     */, 0x00002201u /* rd,ind */, 0x000Bu, {{0x0004u, (void *)&cyBle_attValues[27]}}   },
{ 0x000Bu, 0x2902u /* Client Characteristic Configuration */, 0x00000A04u /* rd,wr  */, 0x000Bu, {{0x0002u, (void *)&cyBle_attValuesCCCD[0]}} },
{ 0x000Cu, 0x2800u /* Primary service                     */, 0x00000001u /*        */, 0x000Fu, {{0x180Du, NULL}}                           },
{ 0x000Du, 0x2803u /* Characteristic                      */, 0x00001001u /* ntf    */, 0x000Fu, {{0x2A37u, NULL}}                           },
{ 0x000Eu, 0x2A37u /* Heart Rate Measurement              */, 0x00001000u /* ntf    */, 0x000Fu, {{0x0002u, (void *)&cyBle_attValues[31]}}   },
{ 0x000Fu, 0x2902u /* Client Characteristic Configuration */, 0x00000A04u /* rd,wr  */, 0x000Fu, {{0x0002u, (void *)&cyBle_attValuesCCCD[2]}} },
{ 0x0010u, 0x2800u /* Primary service                     */, 0x00000001u /*        */, 0x0012u, {{0x180Au, NULL}}                           },
{ 0x0011u, 0x2803u /* Characteristic                      */, 0x00000201u /* rd     */, 0x0012u, {{0x2A29u, NULL}}                           },
{ 0x0012u, 0x2A29u /* Manufacturer Name String            */, 0x00000201u /* rd     */, 0x0012u, {{0x0007u, (void *)&cyBle_attValues[33]}}   },
};


#endif /* (CYBLE_GATT_ROLE_SERVER) */

#if(CYBLE_GATT_ROLE_CLIENT)
    
CYBLE_CLIENT_STATE_T cyBle_clientState;
CYBLE_GATTC_T cyBle_gattc;
    
#endif /* (CYBLE_GATT_ROLE_CLIENT) */


#if(CYBLE_GATT_ROLE_SERVER)

/****************************************************************************** 
##Function Name: CyBle_GattsReInitGattDb
*******************************************************************************

Summary:
 Reinitializes the GATT database.

Parameters:
 None

Return:
 CYBLE_API_RESULT_T: An API result states if the API succeeded or failed with
 error codes:
  * CYBLE_ERROR_OK: GATT database was reinitialized successfully
  * CYBLE_ERROR_INVALID_STATE: If the function is called in any state except
                                CYBLE_STATE_DISCONNECTED.
  * Any of the CyBle_GattsDbRegister() stack API function return values.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_GattsReInitGattDb(void)
{
    CYBLE_API_RESULT_T apiResult;
    
    if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
    {
        apiResult = CyBle_GattsDbRegister(cyBle_gattDB, CYBLE_GATT_DB_INDEX_COUNT, CYBLE_GATT_DB_MAX_VALUE_LEN);
    }
    else
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    
    return(apiResult);
}


/****************************************************************************** 
##Function Name: CyBle_GattsWriteEventHandler
*******************************************************************************

Summary:
 Handles the Write Request Event for GATT service.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 * CYBLE_GATT_ERR_CODE_T: An API result returns one of the following status 
    values
 * CYBLE_GATT_ERR_NONE - Write is successful.

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_GattsWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    
    /* Client Characteristic Configuration descriptor write request */
    if(eventParam->handleValPair.attrHandle == cyBle_gatts.cccdHandle)
    {
        /* Store value to database */
        gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                        &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
        
        if(CYBLE_GATT_ERR_NONE == gattErr)
        {
            if(CYBLE_IS_INDICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
            {
                CyBle_ApplCallback((uint32)CYBLE_EVT_GATTS_INDICATION_ENABLED, eventParam);
            }
            else
            {
                CyBle_ApplCallback((uint32)CYBLE_EVT_GATTS_INDICATION_DISABLED, eventParam);
            }
        }
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
    return (gattErr);
}


#endif /* (CYBLE_GATT_ROLE_SERVER) */

#if(CYBLE_GATT_ROLE_CLIENT)


/****************************************************************************** 
##Function Name: CyBle_GattcStartDiscovery
*******************************************************************************

Summary:
 Starts the automatic server discovery process. Two events may be generated 
 after calling this function - CYBLE_EVT_GATTC_DISCOVERY_COMPLETE or 
 CYBLE_EVT_GATTC_ERROR_RSP. The CYBLE_EVT_GATTC_DISCOVERY_COMPLETE event is 
 generated when the remote device was successfully discovered. The
 CYBLE_EVT_GATTC_ERROR_RSP is generated if the device discovery is failed.

Parameters:
 connHandle: The handle which consists of the device ID and ATT connection ID.

Return:
  CYBLE_API_RESULT_T : Return value indicates if the function succeeded or
                       failed. Following are the possible error codes.
  CYBLE_ERROR_OK - On successful operation
  CYBLE_ERROR_INVALID_PARAMETER - 'connHandle' value does not represent any
                                   existing entry.
                                                 in the Stack
  CYBLE_ERROR_INVALID_OPERATION - The operation is not permitted.
  CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_GattcStartDiscovery(CYBLE_CONN_HANDLE_T connHandle)
{
    uint8 j;
    CYBLE_API_RESULT_T apiResult;
    
    if(CYBLE_STATE_CONNECTED != CyBle_GetState())
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else
    {
        CyBle_ServiceInit();
        
        cyBle_connHandle = connHandle;
        cyBle_disCount = 0u;

        /* Clean old discovery information */
        for(j = 0u; j < (uint8) CYBLE_SRVI_COUNT; j++)
        {
            (void)memset(&cyBle_serverInfo[j].range, 0, sizeof(cyBle_serverInfo[0].range));
        }

        apiResult = CyBle_GattcDiscoverAllPrimaryServices(connHandle);

        if(CYBLE_ERROR_OK == apiResult)
        {
            CyBle_SetClientState(CYBLE_CLIENT_STATE_SRVC_DISCOVERING);
            cyBle_eventHandlerFlag |= CYBLE_AUTO_DISCOVERY;
        }
    }
    
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_GattcDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a "CYBLE_EVT_GATTC_READ_BY_TYPE_RSP"
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 *discCharInfo: The pointer to a characteristic information structure.

Return:
 None

******************************************************************************/
void CyBle_GattcDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    if(discCharInfo->uuid.uuid16 == CYBLE_UUID_CHAR_SERVICE_CHANGED)
    {
        CyBle_CheckStoreCharHandle(cyBle_gattc.serviceChanged);
    }
}


/******************************************************************************
##Function Name: CyBle_GattcDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a "CYBLE_EVT_GATTC_FIND_INFO_RSP" event.
 Based on the descriptor UUID, an appropriate data structure is populated 
 using the data received as part of the callback.

Parameters:
 *discDescrInfo: The pointer to a descriptor information structure.
 discoveryService: The index of the service instance

Return:
 None

******************************************************************************/
void CyBle_GattcDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_CLIENT_CONFIG)
    {
        CyBle_CheckStoreCharDescrHandle(cyBle_gattc.cccdHandle);
    }
}


/******************************************************************************
##Function Name: CyBle_GattcIndicationEventHandler
*******************************************************************************

Summary:
 Handles the Indication Event.

Parameters:
 *eventParam: The pointer to the data structure specified by the event.

Return:
 None.

******************************************************************************/
void CyBle_GattcIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam)
{
    if(cyBle_gattc.serviceChanged.valueHandle == eventParam->handleValPair.attrHandle)
    {
        CyBle_ApplCallback((uint32)CYBLE_EVT_GATTC_INDICATION, eventParam);
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
}


#endif /* (CYBLE_GATT_ROLE_CLIENT) */


/* [] END OF FILE */
