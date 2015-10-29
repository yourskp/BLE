/*******************************************************************************
File Name: CYBLE_ias.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in
 the Immediate Alert Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_IAS_H)
#define CY_BLE_CYBLE_IAS_H

#include "`$INSTANCE_NAME`_gatt.h"


/*************************************** 
## Data Struct Definition
***************************************/

/* Immediate Alert Service Characteristic indexes */
typedef enum
{
    CYBLE_IAS_ALERT_LEVEL,                          /* Alert Level Characteristic index */
    CYBLE_IAS_CHAR_COUNT                            /* Total count of characteristics */
}CYBLE_IAS_CHAR_INDEX_T;

#ifdef CYBLE_IAS_SERVER

/* Structure with Immediate Alert Service attribute handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T    serviceHandle;           /* Immediate Alert Service handle*/
    CYBLE_GATT_DB_ATTR_HANDLE_T    alertLevelCharHandle;    /* Handle of Alert Level Characteristic */
} CYBLE_IASS_T;

/* Immediate Alert Service Characteristic Value parameters structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T         connHandle;         /* Connection handle */
    CYBLE_IAS_CHAR_INDEX_T      charIndex;          /* Characteristic index of Immediate Alert Service */
    CYBLE_GATT_VALUE_T          *value;             /* Pointer to value of Immediate Alert Service characteristic */
} CYBLE_IAS_CHAR_VALUE_T;

#endif /* (CYBLE_IAS_SERVER) */

#ifdef CYBLE_IAS_CLIENT

/* Structure with discovered attributes information of Immediate Alert Service */
typedef struct
{
    CYBLE_SRVR_CHAR_INFO_T  alertLevelChar;     /* Handle of Alert Level Characteristic of Immediate Alert Service */
} CYBLE_IASC_T;

#endif /* (CYBLE_IAS_CLIENT) */


/*************************************** 
##Function Prototypes
***************************************/

#ifdef CYBLE_IAS_SERVER

void CyBle_IasRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);
CYBLE_API_RESULT_T CyBle_IassGetCharacteristicValue(CYBLE_IAS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

#endif /* (CYBLE_IAS_SERVER) */

#ifdef CYBLE_IAS_CLIENT

CYBLE_API_RESULT_T CyBle_IascSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                    CYBLE_IAS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize,
                                                    uint8 *attrValue);

#endif /* (CYBLE_IAS_CLIENT) */


/*************************************** 
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_IasInit(void);

#ifdef CYBLE_IAS_SERVER

void CyBle_IassDisconnectEventHandler(void);
void CyBle_IassWriteCmdEventHandler(CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *eventParam);

#endif /* (CYBLE_IAS_SERVER) */

#ifdef CYBLE_IAS_CLIENT

void CyBle_IascDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);

#endif /* (CYBLE_IAS_CLIENT) */
/* DOM-IGNORE-END */


/*************************************** 
##API Constants
***************************************/

#define CYBLE_IAS_ALERT_LEVEL_SIZE               (1u)


/***************************************
##Variables with external linkage
***************************************/

#ifdef CYBLE_IAS_SERVER

extern const CYBLE_IASS_T cyBle_iass;

#endif /* (CYBLE_IAS_SERVER) */

#ifdef CYBLE_IAS_CLIENT
    extern CYBLE_IASC_T cyBle_iasc;
#endif /* (CYBLE_IAS_CLIENT) */

#endif /* CY_BLE_CYBLE_IAS_H */


/* [] END OF FILE */
