/*******************************************************************************
File Name: CYBLE_bcs.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for Body Composition Service.
 
********************************************************************************
Copyright 2015, Cypress Semiconductor Corporation. All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_BCS_H)
#define CY_BLE_CYBLE_BCS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Conditional Compilation Parameters
***************************************/


/***************************************
##Data Struct Definition
***************************************/

/* BCS Characteristic indexes */
typedef enum
{
    CYBLE_BCS_BODY_COMPOSITION_FEATURE,         /* Body Composition Feature Characteristic index */
    CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT,     /* Body Composition Characteristic index */
    CYBLE_BCS_CHAR_COUNT                        /* Total count of BCS Characteristics */
}CYBLE_BCS_CHAR_INDEX_T;

/* BCS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_BCS_CCCD,                             /* Client Characteristic Configuration Descriptor index */
    CYBLE_BCS_DESCR_COUNT                       /* Total count of Descriptors */
}CYBLE_BCS_DESCR_INDEX_T;

/* BCS Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;             /* Peer device handle */
    CYBLE_BCS_CHAR_INDEX_T charIndex;           /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                  /* Characteristic value */
} CYBLE_BCS_CHAR_VALUE_T;

/* BCS Characteristic descriptor value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;              /* Peer device handle */
    CYBLE_BCS_CHAR_INDEX_T charIndex;            /* Index of service characteristic */
    CYBLE_BCS_DESCR_INDEX_T descrIndex;          /* Index of descriptor */
    CYBLE_GATT_VALUE_T *value;                   /* Characteristic value */
} CYBLE_BCS_DESCR_VALUE_T;

#ifdef CYBLE_BCS_SERVER

/* Structure with Body Composition Service attribute handles */
typedef struct
{
    /* Handle of Characteristic Value */
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;
    /* Array of Descriptor handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_BCS_DESCR_COUNT];
} CYBLE_BCSS_CHAR_T;

/* BCS Characteristic with descriptors handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;              /* Body Composition Service handle */
    CYBLE_BCSS_CHAR_T charInfo[CYBLE_BCS_CHAR_COUNT];       /* Array of characteristics and descriptors handles */
} CYBLE_BCSS_T;

#endif /* CYBLE_BCS_SERVER */

#ifdef CYBLE_BCS_CLIENT

typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;                 /* Handle of characteristic value */
    uint8  properties;                                       /* Properties for value field */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;                   /* End handle of a characteristic */
} CYBLE_BCSC_CHAR_T;

/* BCS Characteristic with descriptors */
typedef struct
{
    /* Body Composition Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    /* Body Composition Service characteristics info structure */
    CYBLE_BCSC_CHAR_T charInfo[CYBLE_BCS_CHAR_COUNT];
    /* Body Composition Measurement Client Characteristic Configuration handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T bodyCompositionMeasurementCccdHandle;
} CYBLE_BCSC_CHAR_T;

#endif /* CYBLE_BCS_CLIENT */


/***************************************
##API Constants
***************************************/


/***************************************
##Function Prototypes
***************************************/

void CyBle_BcsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_BCS_SERVER

CYBLE_API_RESULT_T CyBle_BcssSetCharacteristicValue(CYBLE_BCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BcssGetCharacteristicValue(CYBLE_BCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BcssSetCharacteristicDescriptor(CYBLE_BCS_CHAR_INDEX_T charIndex,
    CYBLE_BCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BcssGetCharacteristicDescriptor(CYBLE_BCS_CHAR_INDEX_T charIndex,
    CYBLE_BCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BcssSendIndication(CYBLE_CONN_HANDLE_T connHandle, CYBLE_BCS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_BCS_SERVER */

#ifdef CYBLE_BCS_CLIENT

CYBLE_API_RESULT_T CyBle_BcscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_BCS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_BcscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_BCS_CHAR_INDEX_T charIndex, CYBLE_BCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BcscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_BCS_CHAR_INDEX_T charIndex, CYBLE_BCS_DESCR_INDEX_T descrIndex);

#endif /* CYBLE_BCS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */

#ifdef CYBLE_BCS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_BcssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_BcssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);

#endif /* CYBLE_BCS_SERVER */

#ifdef CYBLE_BCS_CLIENT

void CyBle_BcscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_BcscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_BcscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_BcscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_BcscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_BcscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_BCS_CLIENT */

/* DOM-IGNORE-END */


/***************************************
##External data references
***************************************/

#ifdef CYBLE_BCS_SERVER

#endif /* CYBLE_BCS_SERVER */

#ifdef CYBLE_BCS_CLIENT

#endif /* CYBLE_BCS_CLIENT */


#endif /* CY_BLE_CYBLE_BCS_H */

/* [] END OF FILE */
