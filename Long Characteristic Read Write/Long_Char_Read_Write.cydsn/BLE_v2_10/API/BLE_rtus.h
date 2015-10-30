/*******************************************************************************
File Name: CYBLE_rtus.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for Reference Time Update
 Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_RTUS_H)
#define CY_BLE_CYBLE_RTUS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Data Struct Definition
***************************************/

/* Characteristic indexes */
typedef enum
{
    CYBLE_RTUS_TIME_UPDATE_CONTROL_POINT,            /* Time Update Control Point Characteristic index */
    CYBLE_RTUS_TIME_UPDATE_STATE,                    /* Time Update State Characteristic index */
    CYBLE_RTUS_CHAR_COUNT                            /* Total count of RTUS characteristics */
} CYBLE_RTUS_CHAR_INDEX_T;


/* Reference Time Update Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                   /* Peer device handle */
    CYBLE_RTUS_CHAR_INDEX_T charIndex;                /* Index of Reference Time Update Service Characteristic */
    CYBLE_GATT_VALUE_T *value;                        /* Characteristic value */
} CYBLE_RTUS_CHAR_VALUE_T;

#if defined(CYBLE_RTUS_SERVER)

/* Structure with Reference Time Update Service atribute handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;           /* Handle of the Reference Time Update Service */
    CYBLE_GATT_DB_ATTR_HANDLE_T timeUpdateCpHandle;      /* Handle of the Time Update Control Point Characteristic */
    CYBLE_GATT_DB_ATTR_HANDLE_T timeUpdateStateHandle;   /* Handle of the Time Update State Characteristic */
} CYBLE_RTUSS_T;

#endif /* CYBLE_RTUS_SERVER */

#if defined(CYBLE_RTUS_CLIENT)

/* Structure with discovered attributes information of  Reference Time Update Service */
typedef struct
{
    /* Characteristic handle and properties */
    CYBLE_SRVR_CHAR_INFO_T charInfo[CYBLE_RTUS_CHAR_COUNT];
} CYBLE_RTUSC_T;

#endif /* CYBLE_RTUS_CLIENT */


/***************************************
##API Constants
***************************************/

#define CYBLE_RTUS_TIME_UPDATE_CP_SIZE                  (0x01u)

#define CYBLE_RTUS_INVALID_CHAR_INDEX                   (0xFFu)

/* Time Update Control Point Characteristic Values */
#define CYBLE_RTUS_GET_REF_UPDATE                       (0x01u)
#define CYBLE_RTUS_CANCEL_REF_UPDATE                    (0x02u)

/* Time Update State Characteristic Values */
/* Current State field */
#define CYBLE_RTUS_CURR_STATE_IDLE                      (0x00u)
#define CYBLE_RTUS_CURR_STATE_UPDATE_PENDING            (0x01u)

/* Result field */
#define CYBLE_RTUS_RESULT_SUCCESSFUL                    (0x00u)
#define CYBLE_RTUS_RESULT_CANCELED                      (0x01u)
#define CYBLE_RTUS_RESULT_NO_CONN                       (0x02u)
#define CYBLE_RTUS_RESULT_REF_ERROR                     (0x03u)
#define CYBLE_RTUS_RESULT_TIMEOUT                       (0x04u)
#define CYBLE_RTUS_RESULT_NO_UPDATE_ATTEMPT             (0x05u)


/***************************************
##Function Prototypes
***************************************/

void CyBle_RtusRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#if defined(CYBLE_RTUS_SERVER)

CYBLE_API_RESULT_T CyBle_RtussSetCharacteristicValue(CYBLE_RTUS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_RtussGetCharacteristicValue(CYBLE_RTUS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_RTUS_SERVER */

#if defined(CYBLE_RTUS_CLIENT)

CYBLE_API_RESULT_T CyBle_RtuscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_RTUS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_RtuscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_RTUS_CHAR_INDEX_T charIndex);

#endif /* (CYBLE_RTUS_CLIENT) */


/***************************************
##Private Function Prototypes
***************************************/
/* DOM-IGNORE-BEGIN */
void CyBle_RtusInit(void);

#if defined(CYBLE_RTUS_SERVER)
    
void CyBle_RtussWriteCmdEventHandler(CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T *eventParam);

#endif /* CYBLE_RTUS_SERVER */

#if defined(CYBLE_RTUS_CLIENT)

void CyBle_RtuscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_RtuscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_RTUS_CLIENT) */
/* DOM-IGNORE-END */


/***************************************
##External data references
***************************************/

#if defined(CYBLE_RTUS_SERVER)
extern const CYBLE_RTUSS_T cyBle_rtuss;
#endif /* CYBLE_RTUS_SERVER */

#if defined(CYBLE_RTUS_CLIENT)
extern CYBLE_RTUSC_T cyBle_rtusc;
#endif /* (CYBLE_RTUS_CLIENT) */


#endif /* CY_BLE_CYBLE_RTUS_H */
    
/* [] END OF FILE */
