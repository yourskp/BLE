/*******************************************************************************
File Name: CYBLE_cscs.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for Cycling Speed and Cadence
 Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_CSCS_H)
#define CY_BLE_CYBLE_CSCS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Data Struct Definition
***************************************/
/* Characteristic indexes */
typedef enum
{
    CYBLE_CSCS_CSC_MEASUREMENT,                      /* CSC Measurement Characteristic index */
    CYBLE_CSCS_CSC_FEATURE,                          /* CSC Feature Characteristic index */
    CYBLE_CSCS_SENSOR_LOCATION,                      /* CSC Sensor Location Characteristic index */
    CYBLE_CSCS_SC_CONTROL_POINT,                     /* CSC SC Control Point Characteristic index */
    CYBLE_CSCS_CHAR_COUNT                            /* Total count of CSCS Characteristics */
} CYBLE_CSCS_CHAR_INDEX_T;

/* Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_CSCS_CCCD,                                  /* Client Characteristic Configuration Descriptor index */
    CYBLE_CSCS_DESCR_COUNT                            /* Total count of Descriptors */
} CYBLE_CSCS_DESCR_INDEX_T;

/* Cycling Speed and Cadence Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                   /* Peer device handle */
    CYBLE_CSCS_CHAR_INDEX_T charIndex;                /* Index of Cycling Speed and Cadence Service Characteristic */
    CYBLE_GATT_VALUE_T *value;                        /* Characteristic value */
} CYBLE_CSCS_CHAR_VALUE_T;

/* Cycling Speed and Cadence Service Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T            connHandle;        /* Connection handle */
    CYBLE_CSCS_CHAR_INDEX_T        charIndex;         /* Characteristic index of the Service */
    CYBLE_CSCS_DESCR_INDEX_T       descrIndex;        /* Characteristic Descriptor index */
    CYBLE_GATT_VALUE_T             *value;            /* Pointer to value of the Service Characteristic Descriptor */
} CYBLE_CSCS_DESCR_VALUE_T;

#ifdef CYBLE_CSCS_SERVER
    
/* Characteristic with descriptors type */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                           /* Handle of the Characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_CSCS_DESCR_COUNT];  /* Handles of the Descriptors */
} CYBLE_CSCSS_CHAR_T;

/* Structure with Cycling Speed and Cadence Service attribute handles */
typedef struct
{
    /* Cycling Speed and Cadence Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /* Array of Cycling Speed and Cadence Service Characteristics and Descriptors handles */
    CYBLE_CSCSS_CHAR_T charInfo[CYBLE_CSCS_CHAR_COUNT];
} CYBLE_CSCSS_T;

#endif /* CYBLE_CSCS_SERVER */

#ifdef CYBLE_CSCS_CLIENT

/* Service full Characteristic information type */
typedef struct
{
    /* Characteristic handle and properties */
    CYBLE_SRVR_CHAR_INFO_T charInfo;
    
    /* Characteristic descriptors handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T descriptors[CYBLE_CSCS_DESCR_COUNT];
    
    /* End handle of Characteristic */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
} CYBLE_CSCSC_SRVR_FULL_CHAR_INFO_T;
    
/* Structure with discovered attributes information of Cycling Speed and Cadence Service */
typedef struct
{
    /* Characteristics handles array */
    CYBLE_CSCSC_SRVR_FULL_CHAR_INFO_T characteristics[CYBLE_CSCS_CHAR_COUNT];
} CYBLE_CSCSC_T;

#endif /* CYBLE_CSCS_CLIENT */


/***************************************
##API Constants
***************************************/

#define CYBLE_CSCS_INVALID_CHAR_INDEX             (0xFFu)

/* SC Control Point Characteristic Op Codes */
#define CYBLE_CSCS_SET_CUMMULATIVE_VALUE          (0x01u)
#define CYBLE_CSCS_START_SENSOR_CALIBRATION       (0x02u)
#define CYBLE_CSCS_UPDATE_SENSOR_LOCATION         (0x03u)
#define CYBLE_CSCS_REQ_SUPPORTED_SENSOR_LOCATION  (0x04u)
#define CYBLE_CSCS_RESPONSE_CODE                  (0x10u)

/* SC Control Point Characteristic Response Codes */
#define CYBLE_CSCS_ERR_SUCCESS                    (0x01u)
#define CYBLE_CSCS_ERR_OP_CODE_NOT_SUPPORTED      (0x02u)
#define CYBLE_CSCS_ERR_INVALID_PARAMETER          (0x03u)
#define CYBLE_CSCS_ERR_OPERATION_FAILED           (0x04u)


/***************************************
##Function Prototypes
***************************************/

void CyBle_CscsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_CSCS_SERVER

CYBLE_API_RESULT_T CyBle_CscssSetCharacteristicValue(CYBLE_CSCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CscssGetCharacteristicValue(CYBLE_CSCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CscssGetCharacteristicDescriptor(CYBLE_CSCS_CHAR_INDEX_T charIndex,
    CYBLE_CSCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CscssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CSCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CscssSendIndication(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CSCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_CSCS_SERVER */

#ifdef CYBLE_CSCS_CLIENT

CYBLE_API_RESULT_T CyBle_CscscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_CSCS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CscscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_CSCS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_CscscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CSCS_CHAR_INDEX_T charIndex, CYBLE_CSCS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CscscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CSCS_CHAR_INDEX_T charIndex, CYBLE_CSCS_DESCR_INDEX_T descrIndex);

#endif /* (CYBLE_CSCS_CLIENT) */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_CscsInit(void);

#ifdef CYBLE_CSCS_SERVER
    
CYBLE_GATT_ERR_CODE_T CyBle_CscssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_CscssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);

#endif /* CYBLE_CSCS_SERVER */

#ifdef CYBLE_CSCS_CLIENT

void CyBle_CscscDiscoverCharacteristicsEventHandler(const CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_CscscDiscoverCharDescriptorsEventHandler(CYBLE_CSCS_CHAR_INDEX_T charIndex,
                                                    CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_CscscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_CscscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_CscscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_CscscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_CscscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);
CYBLE_API_RESULT_T CyBle_CscscStoreProfileData(uint8 isForceWrite);

#endif /* (CYBLE_CSCS_CLIENT) */
/* DOM-IGNORE-END */


/***************************************
##External data references
***************************************/

#ifdef CYBLE_CSCS_SERVER
extern const CYBLE_CSCSS_T cyBle_cscss;
#endif /* CYBLE_CSCS_SERVER */

#ifdef CYBLE_CSCS_CLIENT
extern CYBLE_CSCSC_T cyBle_cscsc;
#endif /* (CYBLE_CSCS_CLIENT) */


#endif /* CY_BLE_CYBLE_CSCS_H */

/* [] END OF FILE */
