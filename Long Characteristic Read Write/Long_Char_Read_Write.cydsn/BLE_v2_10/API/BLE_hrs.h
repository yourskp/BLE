/*******************************************************************************
File Name: CYBLE_hrs.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in
 the Heart Rate Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_HRS_H)
#define CY_BLE_CYBLE_HRS_H

/* Definitions for Body Sensor Location characteristic */
#define CYBLE_HRS_BSL_CHAR_LEN                       (1u)

/* Definitions for ControlPoint characteristic */
#define CYBLE_HRS_CPT_CHAR_LEN                       (1u)
#define CYBLE_HRS_RESET_ENERGY_EXPENDED              (0x01u)


/*************************************** 
##Data Types
***************************************/

/* HRS Characteristics indexes */
typedef enum
{
    CYBLE_HRS_HRM,          /* Heart Rate Measurement characteristic index */
    CYBLE_HRS_BSL,          /* Body Sensor Location characteristic index */
    CYBLE_HRS_CPT,          /* Control Point characteristic index */
    CYBLE_HRS_CHAR_COUNT    /* Total count of HRS characteristics */
}CYBLE_HRS_CHAR_INDEX_T;

/* HRS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_HRS_HRM_CCCD,    /* Heart Rate Measurement client char. config. descriptor index */
    CYBLE_HRS_DESCR_COUNT  /* Total count of HRS HRM descriptors */
}CYBLE_HRS_DESCR_INDEX_T;


#ifdef CYBLE_HRS_SERVER

/* Structure with Heart Rate Service attribute handles */
typedef struct
{
    /* Heart Rate Service handle*/
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /* Heart Rate Service characteristics handles and properties array */
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle[CYBLE_HRS_CHAR_COUNT];

    /* Heart Rate Measurement client char. config. descriptor Handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T hrmCccdHandle;
} CYBLE_HRSS_T;
#endif /* CYBLE_HRS_SERVER */

#ifdef CYBLE_HRS_CLIENT

/* Structure with discovered attributes information of Heart Rate Service */
typedef struct
{
    /* Heart Rate Service characteristics handles and properties array */
    CYBLE_SRVR_CHAR_INFO_T charInfo[CYBLE_HRS_CHAR_COUNT];

    /* Heart Rate Measurement client char. config. descriptor Handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T hrmCccdHandle;
}CYBLE_HRSC_T;
#endif /* CYBLE_HRS_CLIENT */

/* HRS Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_HRS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_HRS_CHAR_VALUE_T;

/* HRS Characteristic descriptor value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_HRS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_HRS_DESCR_INDEX_T descrIndex;                     /* Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Descriptor value */
} CYBLE_HRS_DESCR_VALUE_T;


/*************************************** 
##Function Prototypes
***************************************/

void CyBle_HrsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_HRS_SERVER

CYBLE_API_RESULT_T CyBle_HrssSetCharacteristicValue(CYBLE_HRS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HrssGetCharacteristicValue(CYBLE_HRS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HrssGetCharacteristicDescriptor(CYBLE_HRS_CHAR_INDEX_T charIndex, 
                                                        CYBLE_HRS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HrssSendNotification(CYBLE_CONN_HANDLE_T connHandle, CYBLE_HRS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_HRS_SERVER */

#ifdef CYBLE_HRS_CLIENT

CYBLE_API_RESULT_T CyBle_HrscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_HRS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HrscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_HRS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_HrscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_HRS_CHAR_INDEX_T charIndex, CYBLE_HRS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HrscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, CYBLE_HRS_CHAR_INDEX_T charIndex,
                                                        CYBLE_HRS_DESCR_INDEX_T descrIndex);

#endif /* CYBLE_HRS_CLIENT */


/*************************************** 
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_HrsInit(void);

#ifdef CYBLE_HRS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_HrssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* CYBLE_HRS_SERVER */

#ifdef CYBLE_HRS_CLIENT

void CyBle_HrscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T * discCharInfo);
void CyBle_HrscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_HrscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_HrscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_HrscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_HrscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_HRS_CLIENT */

/* Internal macros */
#ifdef CYBLE_HRS_SERVER
#define CYBLE_HRS_IS_BSL_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_hrss.charHandle[CYBLE_HRS_BSL])
#endif /* CYBLE_HRS_SERVER */
/* DOM-IGNORE-END */


/*************************************** 
##External data references
***************************************/

#ifdef CYBLE_HRS_SERVER
    /* Heart Rate Service GATT DB handles structure */
    extern const CYBLE_HRSS_T                                   cyBle_hrss;
#endif /* CYBLE_HRSC_SERVER */

#ifdef CYBLE_HRS_CLIENT
    /* Heart Rate Service server's GATT DB handles structure */
    extern CYBLE_HRSC_T                                         cyBle_hrsc;
#endif /* CYBLE_HRS_CLIENT */

#endif /* CY_BLE_CYBLE_HRS_H */


/* [] END OF FILE */
