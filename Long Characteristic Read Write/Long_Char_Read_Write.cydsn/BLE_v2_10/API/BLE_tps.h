/*******************************************************************************
File Name: CYBLE_tps.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in the Tx
 Power Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_TPS_H)
#define CY_BLE_CYBLE_TPS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Conditional Compilation Parameters
***************************************/

/* Maximum supported Tx Power Services */
#define CYBLE_TPS_SERVICE_COUNT             (1u)


/***************************************
##Data Struct Definition
***************************************/

/* TPS Characteristic indexes */
typedef enum
{
    CYBLE_TPS_TX_POWER_LEVEL,                           /* Tx Power Level characteristic index */
    CYBLE_TPS_CHAR_COUNT                                /* Total count of characteristics */
}CYBLE_TPS_CHAR_INDEX_T;

/* TPS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_TPS_CCCD,                    /* Tx Power Level Client Characteristic configuration descriptor index */
    CYBLE_TPS_DESCR_COUNT              /* Total count of Tx Power Service characteristic descriptors */
}CYBLE_TPS_CHAR_DESCRIPTORS_T;

/* Tx Power Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T         connHandle;             /* Connection handle */
    CYBLE_TPS_CHAR_INDEX_T      charIndex;              /* Characteristic index of Tx Power Service */
    CYBLE_GATT_VALUE_T          *value;                 /* Pointer to value of Tx Power Service characteristic */
} CYBLE_TPS_CHAR_VALUE_T;

/* Tx Power Service Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T             connHandle;         /* Connection handle */
    CYBLE_TPS_CHAR_INDEX_T          charIndex;          /* Characteristic index of Tx Power Service */
    CYBLE_TPS_CHAR_DESCRIPTORS_T    descrIndex;         /* Characteristic index Descriptor of Tx Power Service */
    CYBLE_GATT_VALUE_T              *value;             /* Pointer to value of Tx Power Service characteristic */
} CYBLE_TPS_DESCR_VALUE_T;

#ifdef CYBLE_TPS_SERVER

/* Structure with Tx Power Service attribute handles */
typedef struct
{
    /* Tx Power Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T    serviceHandle;
    
    /* Tx Power Level Characteristic handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T    txPowerLevelCharHandle;
    
    /* Tx Power Level Client Characteristic Configuration Descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T    txPowerLevelCccdHandle;
} CYBLE_TPSS_T;

#endif /* (CYBLE_TPS_SERVER) */

#ifdef CYBLE_TPS_CLIENT

/* Structure with discovered attributes information of Tx Power Service */
typedef struct
{
    /* Tx Power Level Characteristic handle */
    CYBLE_SRVR_CHAR_INFO_T       txPowerLevelChar;
    
    /* Tx Power Level Client Characteristic Configuration Descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T  txPowerLevelCccdHandle;
} CYBLE_TPSC_T;

#endif /* CYBLE_TPS_CLIENT */


/***************************************
##Function Prototypes
***************************************/

void CyBle_TpsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_TPS_SERVER

CYBLE_API_RESULT_T CyBle_TpssSetCharacteristicValue(CYBLE_TPS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize,
                                                    int8 *attrValue);
CYBLE_API_RESULT_T CyBle_TpssGetCharacteristicValue(CYBLE_TPS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize,
                                                    int8 *attrValue);
CYBLE_API_RESULT_T CyBle_TpssGetCharacteristicDescriptor(CYBLE_TPS_CHAR_INDEX_T charIndex,
                                                         CYBLE_TPS_CHAR_DESCRIPTORS_T descrIndex,
                                                         uint8 attrSize,
                                                         uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_TpssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
                                              CYBLE_TPS_CHAR_INDEX_T charIndex,
                                              uint8 attrSize,
                                              int8 *attrValue);

#endif /* (CYBLE_TPS_SERVER) */

#ifdef CYBLE_TPS_CLIENT

CYBLE_API_RESULT_T CyBle_TpscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                    CYBLE_TPS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_TpscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                         CYBLE_TPS_CHAR_INDEX_T charIndex,
                                                         CYBLE_TPS_CHAR_DESCRIPTORS_T descrIndex,
                                                         uint8 attrSize,
                                                         uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_TpscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                         CYBLE_TPS_CHAR_INDEX_T charIndex,
                                                         CYBLE_TPS_CHAR_DESCRIPTORS_T descrIndex);

#endif /* (CYBLE_TPS_CLIENT) */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_TpsInit(void);

#ifdef CYBLE_TPS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_TpssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* (CYBLE_TPS_SERVER) */

#ifdef CYBLE_TPS_CLIENT

void CyBle_TpscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_TpscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_TpscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_TpscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam);
void CyBle_TpscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_TpscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_TPS_CLIENT) */
/* DOM-IGNORE-END */


/***************************************
##API Constants
***************************************/

#define CYBLE_TPS_TX_POWER_LEVEL_SIZE               (1u)

#define CYBLE_TPS_TX_POWER_LEVEL_MIN                (-18)
#define CYBLE_TPS_TX_POWER_LEVEL_MAX                (3)


/***************************************
##Variables with external linkage
***************************************/

#ifdef CYBLE_TPS_SERVER

extern const CYBLE_TPSS_T cyBle_tpss;

#endif /* (CYBLE_TPS_SERVER) */

#ifdef CYBLE_TPS_CLIENT

extern CYBLE_TPSC_T cyBle_tpsc;

#endif /* (CYBLE_TPS_CLIENT) */

#endif /* CY_BLE_CYBLE_TPS_H */


/* [] END OF FILE */
