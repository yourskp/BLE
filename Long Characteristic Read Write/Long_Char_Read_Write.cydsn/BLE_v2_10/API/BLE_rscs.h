/*******************************************************************************
File Name: CYBLE_rscs.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for Running Speed and Cadence
 Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_RSCS_H)
#define CY_BLE_CYBLE_RSCS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Data Struct Definition
***************************************/

/* RSCS Characteristic indexes */
typedef enum
{
    CYBLE_RSCS_RSC_MEASUREMENT,                      /* RSC Measurement Characteristic index */
    CYBLE_RSCS_RSC_FEATURE,                          /* RSC Feature Characteristic index */
    CYBLE_RSCS_SENSOR_LOCATION,                      /* Sensor Location Characteristic index */
    CYBLE_RSCS_SC_CONTROL_POINT,                     /* SC Control Point Characteristic index */
    CYBLE_RSCS_CHAR_COUNT                            /* Total count of RSCS characteristics */
} CYBLE_RSCS_CHAR_INDEX_T;

/* RSCS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_RSCS_CCCD,                                  /* Client Characteristic Configuration Descriptor index */
    CYBLE_RSCS_DESCR_COUNT                            /* Total count of descriptors */
} CYBLE_RSCS_DESCR_INDEX_T;

/* Running Speed and Cadence Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                   /* Peer device handle */
    CYBLE_RSCS_CHAR_INDEX_T charIndex;                /* Index of Running Speed and Cadence Service Characteristic */
    CYBLE_GATT_VALUE_T *value;                        /* Characteristic value */
} CYBLE_RSCS_CHAR_VALUE_T;

/* Running Speed and Cadence Service Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T            connHandle;        /* Connection handle */
    CYBLE_RSCS_CHAR_INDEX_T        charIndex;         /* Characteristic index of the Service */
    CYBLE_RSCS_DESCR_INDEX_T       descrIndex;        /* Characteristic index Descriptor the Service */
    CYBLE_GATT_VALUE_T             *value;            /* Pointer to value of the Service Characteristic Descriptor */
} CYBLE_RSCS_DESCR_VALUE_T;

#if defined(CYBLE_RSCS_SERVER)
    
/* RSCS Characteristic with descriptors */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                           /* Handle of the characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_RSCS_DESCR_COUNT];  /* Handle of the descriptor */
} CYBLE_RSCSS_CHAR_T;

/* Structure with Running Speed and Cadence Service attribute handles */
typedef struct
{
    /* Running Speed and Cadence Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /* Array of Running Speed and Cadence Service Characteristics + Descriptors handles */
    CYBLE_RSCSS_CHAR_T charInfo[CYBLE_RSCS_CHAR_COUNT];
} CYBLE_RSCSS_T;

#endif /* CYBLE_RSCS_SERVER */

#if defined(CYBLE_RSCS_CLIENT)

/* RSCS Service Full characteristic information type */
typedef struct
{
    /* Characteristic handle + properties */
    CYBLE_SRVR_CHAR_INFO_T charInfo;
    
    /* Characteristic descriptors handles handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T descriptors[CYBLE_RSCS_DESCR_COUNT];
    
    /* End handle of characteristic */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
} CYBLE_RSCSC_SRVR_FULL_CHAR_INFO_T;
    
/* Structure with discovered attributes information of Running Speed and Cadence Service */
typedef struct
{
    /* Characteristics handles array */
    CYBLE_RSCSC_SRVR_FULL_CHAR_INFO_T characteristics[CYBLE_RSCS_CHAR_COUNT];
} CYBLE_RSCSC_T;

#endif /* CYBLE_RSCS_CLIENT */

/***************************************
##API Constants
***************************************/

#define CYBLE_RSCS_INVALID_CHAR_INDEX             (0xFFu)

/* SC Control Point Characteristic Op Codes */
#define CYBLE_RSCS_SET_CUMMULATIVE_VALUE          (0x01u)
#define CYBLE_RSCS_START_SENSOR_CALIBRATION       (0x02u)
#define CYBLE_RSCS_UPDATE_SENSOR_LOCATION         (0x03u)
#define CYBLE_RSCS_REQ_SUPPORTED_SENSOR_LOCATION  (0x04u)
#define CYBLE_RSCS_RESPONSE_CODE                  (0x10u)

/* SC Control Point Characteristic Response Codes */
#define CYBLE_RSCS_ERR_SUCCESS                    (0x01u)
#define CYBLE_RSCS_ERR_OP_CODE_NOT_SUPPORTED      (0x02u)
#define CYBLE_RSCS_ERR_INVALID_PARAMETER          (0x03u)
#define CYBLE_RSCS_ERR_OPERATION_FAILED           (0x04u)


/***************************************
##Function Prototypes
***************************************/

void CyBle_RscsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#if defined(CYBLE_RSCS_SERVER)

CYBLE_API_RESULT_T CyBle_RscssSetCharacteristicValue(CYBLE_RSCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_RscssGetCharacteristicValue(CYBLE_RSCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_RscssGetCharacteristicDescriptor(CYBLE_RSCS_CHAR_INDEX_T charIndex,
    CYBLE_RSCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_RscssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_RSCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_RscssSendIndication(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_RSCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_RSCS_SERVER */

#if defined(CYBLE_RSCS_CLIENT)

CYBLE_API_RESULT_T CyBle_RscscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_RSCS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_RscscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_RSCS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_RscscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_RSCS_CHAR_INDEX_T charIndex, CYBLE_RSCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_RscscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_RSCS_CHAR_INDEX_T charIndex, uint8 descrIndex);

#endif /* (CYBLE_RSCS_CLIENT) */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_RscsInit(void);

#if defined(CYBLE_RSCS_SERVER)
    
CYBLE_GATT_ERR_CODE_T CyBle_RscssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_RscssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);

#endif /* CYBLE_RSCS_SERVER */

#if defined(CYBLE_RSCS_CLIENT)

void CyBle_RscscDiscoverCharacteristicsEventHandler(const CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_RscscDiscoverCharDescriptorsEventHandler(CYBLE_RSCS_CHAR_INDEX_T charIndex,
                                                    CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_RscscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_RscscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_RscscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_RscscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_RscscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_RSCS_CLIENT) */
/* DOM-IGNORE-END */


/***************************************
##External data references
***************************************/

#if defined(CYBLE_RSCS_SERVER)
extern const CYBLE_RSCSS_T cyBle_rscss;
#endif /* CYBLE_RSCS_SERVER */

#if defined(CYBLE_RSCS_CLIENT)
extern CYBLE_RSCSC_T cyBle_rscsc;
#endif /* (CYBLE_RSCS_CLIENT) */


#endif /* CY_BLE_CYBLE_RSCS_H  */

/* [] END OF FILE */
