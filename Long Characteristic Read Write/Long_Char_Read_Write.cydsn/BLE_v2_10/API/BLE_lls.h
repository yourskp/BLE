/*******************************************************************************
File Name: CYBLE_lls.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in the Link
 Loss Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_LLS_H)
#define CY_BLE_CYBLE_LLS_H

#include "`$INSTANCE_NAME`_gatt.h"


/*************************************** 
##Data Struct Definition
***************************************/

/* Link Loss Service Characteristic indexes */
typedef enum
{
    CYBLE_LLS_ALERT_LEVEL,                          /* Alert Level Characteristic index */
    CYBLE_LLS_CHAR_COUNT                            /* Total count of characteristics */
}CYBLE_LLS_CHAR_INDEX_T;

/* Link Loss Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T         connHandle;         /* Connection handle */
    CYBLE_LLS_CHAR_INDEX_T      charIndex;          /* Characteristic index of Link Loss Service */
    CYBLE_GATT_VALUE_T       *  value;              /* Pointer to value of Link Loss Service characteristic */
} CYBLE_LLS_CHAR_VALUE_T;

#ifdef CYBLE_LLS_SERVER

/* Structure with Link Loss Service attribute handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T    serviceHandle;        /* Link Loss Service handle*/
    CYBLE_GATT_DB_ATTR_HANDLE_T    alertLevelCharHandle; /* Handle of Alert Level Characteristic */
} CYBLE_LLSS_T;

#endif /* (CYBLE_LLS_SERVER) */


#ifdef CYBLE_LLS_CLIENT

/* Structure with discovered attributes information of Link Loss Service */
typedef struct
{
    CYBLE_SRVR_CHAR_INFO_T    alertLevelChar;            /* Handle of Alert Level Characteristic of Link Loss Service */
} CYBLE_LLSC_T;

#endif /* (CYBLE_LLS_CLIENT) */


/*************************************** 
##Function Prototypes
***************************************/

void CyBle_LlsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_LLS_SERVER

CYBLE_API_RESULT_T CyBle_LlssGetCharacteristicValue(CYBLE_LLS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

#endif /* (CYBLE_LLS_SERVER) */

#ifdef CYBLE_LLS_CLIENT

CYBLE_API_RESULT_T CyBle_LlscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                    CYBLE_LLS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize,
                                                    uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_LlscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_LLS_CHAR_INDEX_T charIndex);

#endif /* (CYBLE_LLS_CLIENT) */


/*************************************** 
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_LlsInit(void);

#ifdef CYBLE_LLS_SERVER

void CyBle_LlssConnectEventHandler(void);
CYBLE_GATT_ERR_CODE_T CyBle_LlssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* (CYBLE_LLS_SERVER) */

#ifdef CYBLE_LLS_CLIENT

void CyBle_LlscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_LlscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam);
void CyBle_LlscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_LlscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_LLS_CLIENT) */
/* DOM-IGNORE-END */


/***************************************
##API Constants
***************************************/

#define CYBLE_LLS_ALERT_LEVEL_SIZE               (1u)


/***************************************
##Variables with external linkage
***************************************/

#ifdef CYBLE_LLS_SERVER
extern const CYBLE_LLSS_T cyBle_llss;
#endif /* (CYBLE_LLS_SERVER) */

#ifdef CYBLE_LLS_CLIENT
extern CYBLE_LLSC_T cyBle_llsc;
#endif /* (CYBLE_LLS_CLIENT) */

#endif /* CY_BLE_CYBLE_LLS_H */


/* [] END OF FILE */
