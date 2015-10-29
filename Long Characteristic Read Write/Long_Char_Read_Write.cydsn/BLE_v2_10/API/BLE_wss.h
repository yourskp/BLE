/*******************************************************************************
File Name: CYBLE_wss.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for Weight Scale Service.
 

********************************************************************************
Copyright 2015, Cypress Semiconductor Corporation. All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_WSS_H)
#define CY_BLE_CYBLE_WSS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Conditional Compilation Parameters
***************************************/


/***************************************
##Data Struct Definition
***************************************/

/* WSS Characteristic indexes */
typedef enum
{
    CYBLE_WSS_WEIGHT_SCALE_FEATURE,              /* Weight Scale Feature Characteristic index */
    CYBLE_WSS_WEIGHT_MEASUREMENT,                /* Weight Measurement Characteristic index */
    CYBLE_WSS_CHAR_COUNT                         /* Total count of WSS Characteristics */
}CYBLE_WSS_CHAR_INDEX_T;

/* WSS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_WSS_CCCD,                              /* Client Characteristic Configuration Descriptor index */
    CYBLE_WSS_DESCR_COUNT                        /* Total count of Descriptors */
}CYBLE_WSS_DESCR_INDEX_T;

/* WSS Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_WSS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_WSS_CHAR_VALUE_T;

/* WSS Characteristic descriptor value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_WSS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_WSS_DESCR_INDEX_T descrIndex;                     /* Index of descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_WSS_DESCR_VALUE_T;

#ifdef CYBLE_WSS_SERVER

/* Structure with Weight Scale Service attribute handles */
typedef struct
{
    /* Handle of Characteristic Value */
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;
    /* Array of Descriptor handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_WSS_DESCR_COUNT];
} CYBLE_WSSS_CHAR_T;

/* WSS Characteristic with descriptors handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;              /* Weight Scale Service handle */
    CYBLE_WSSS_CHAR_T charInfo[CYBLE_WSS_CHAR_COUNT];       /* Array of characteristics and descriptors handles */
} CYBLE_WSSS_T;

#endif /* CYBLE_WSS_SERVER */

#ifdef CYBLE_WSS_CLIENT

/* WSS Service Full characteristic information structure */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;                 /* Handle of characteristic value */
    uint8  properties;                                       /* Properties for value field */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;                   /* End handle of a characteristic */
} CYBLE_WSSC_CHAR_T;

/* Structure with discovered attributes information of Weight Scale Service */
typedef struct
{
    /* Weight Scale Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    /* Weight Scale Service characteristics info structure */
    CYBLE_WSSC_CHAR_T charInfo[CYBLE_WSS_CHAR_COUNT];
    /* Weight Measurement Client Characteristic Configuration handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T weightMeasurementCccdHandle;
} CYBLE_WSSC_CHAR_T;

#endif /* CYBLE_WSS_CLIENT */


/***************************************
##API Constants
***************************************/


/***************************************
##Function Prototypes
***************************************/

void CyBle_WssRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_WSS_SERVER

CYBLE_API_RESULT_T CyBle_WsssSetCharacteristicValue(CYBLE_WSS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WsssGetCharacteristicValue(CYBLE_WSS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WsssSetCharacteristicDescriptor(CYBLE_WSS_CHAR_INDEX_T charIndex,
    CYBLE_WSS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WsssGetCharacteristicDescriptor(CYBLE_WSS_CHAR_INDEX_T charIndex,
    CYBLE_WSS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WsssSendIndication(CYBLE_CONN_HANDLE_T connHandle, CYBLE_WSS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_WSS_SERVER */

#ifdef CYBLE_WSS_CLIENT

CYBLE_API_RESULT_T CyBle_WsscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_WSS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_WsscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_WSS_CHAR_INDEX_T charIndex, CYBLE_WSS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WsscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_WSS_CHAR_INDEX_T charIndex, CYBLE_WSS_DESCR_INDEX_T descrIndex);

#endif /* CYBLE_WSS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_WssInit(void);

#ifdef CYBLE_WSS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_WsssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_WsssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);

#endif /* CYBLE_WSS_SERVER */

#ifdef CYBLE_WSS_CLIENT

void CyBle_WsscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_WsscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_WsscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_WsscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_WsscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_WsscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_WSS_CLIENT */

/* DOM-IGNORE-END */


/***************************************
##External data references
***************************************/

#ifdef CYBLE_WSS_SERVER

#endif /* CYBLE_WSS_SERVER */

#ifdef CYBLE_WSS_CLIENT

#endif /* CYBLE_WSS_CLIENT */


#endif /* CY_BLE_CYBLE_WSS_H */

/* [] END OF FILE */
