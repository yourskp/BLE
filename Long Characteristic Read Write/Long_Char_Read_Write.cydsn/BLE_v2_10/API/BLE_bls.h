/*******************************************************************************
File Name: CYBLE_bls.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in
 the Blood Pressure Profile of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_BLS_H)
#define CY_BLE_CYBLE_BLS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Data Types
***************************************/

/* Service Characteristics indexes */
typedef enum
{
    CYBLE_BLS_BPM,              /* Blood Pressure Measurement characteristic index */
    CYBLE_BLS_ICP,              /* Intermediate Cuff Pressure Context characteristic index */
    CYBLE_BLS_BPF,              /* Blood Pressure Feature characteristic index */
    CYBLE_BLS_CHAR_COUNT        /* Total count of BLS characteristics */
}CYBLE_BLS_CHAR_INDEX_T;

/* Service Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_BLS_CCCD,             /* Client Characteristic Configuration descriptor index */
    CYBLE_BLS_DESCR_COUNT       /* Total count of BLS descriptors */
}CYBLE_BLS_DESCR_INDEX_T;

#ifdef CYBLE_BLS_SERVER

/* Characteristic with descriptors */
typedef struct
{
    /* Blood Pressure Service characteristic's handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;

    /* Blood Pressure Service char. descriptor's handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T cccdHandle;
}CYBLE_BLSS_CHAR_T;
    
/* Structure with Blood Pressure Service attribute handles */
typedef struct
{
    /* Blood Pressure Service handle*/
    CYBLE_GATT_DB_ATTR_HANDLE_T    serviceHandle;
    /* Array of Blood Pressure Service Characteristics + Descriptors handles */
    CYBLE_BLSS_CHAR_T charInfo[CYBLE_BLS_CHAR_COUNT];
} CYBLE_BLSS_T;

#endif /* CYBLE_BLS_SERVER */

#ifdef CYBLE_BLS_CLIENT

/* Blood Pressure Client Server's Characteristic structure type */
typedef struct
{
    /* Properties for value field */
    uint8  properties;

    /* Handle of server database attribute value entry */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;
    
    /* Blood Pressure client char. config. descriptor's handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T cccdHandle;
    
    /* Characteristic end handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
}CYBLE_BLSC_CHAR_T;

/* Structure with discovered atributes information of Blood Pressure Service */
typedef struct
{
    /* Structure with Characteristic handles + properties of Blood Pressure Service */
    CYBLE_BLSC_CHAR_T charInfo[CYBLE_BLS_CHAR_COUNT];
}CYBLE_BLSC_T;

#endif /* CYBLE_BLS_CLIENT */

/* Blood Pressure Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_BLS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_BLS_CHAR_VALUE_T;

/* Blood Pressure Service Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_BLS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_BLS_DESCR_INDEX_T descrIndex;                     /* Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Descriptor value */
} CYBLE_BLS_DESCR_VALUE_T;


/***************************************
##Function Prototypes
***************************************/

void CyBle_BlsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_BLS_SERVER

CYBLE_API_RESULT_T CyBle_BlssSetCharacteristicValue(CYBLE_BLS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BlssGetCharacteristicValue(CYBLE_BLS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BlssGetCharacteristicDescriptor(CYBLE_BLS_CHAR_INDEX_T charIndex,
                                                        CYBLE_BLS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BlssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
                                                CYBLE_BLS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BlssSendIndication(CYBLE_CONN_HANDLE_T connHandle,
                                                CYBLE_BLS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);
#endif /* CYBLE_BLS_SERVER */

#ifdef CYBLE_BLS_CLIENT

CYBLE_API_RESULT_T CyBle_BlscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_BLS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_BlscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_BLS_CHAR_INDEX_T charIndex,
                                                        CYBLE_BLS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BlscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_BLS_CHAR_INDEX_T charIndex,
                                                        CYBLE_BLS_DESCR_INDEX_T descrIndex);
#endif /* CYBLE_BLS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_BlsInit(void);

#ifdef CYBLE_BLS_SERVER

void CyBle_BlssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
CYBLE_GATT_ERR_CODE_T CyBle_BlssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* CYBLE_BLS_SERVER */

#ifdef CYBLE_BLS_CLIENT

void CyBle_BlscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T * discCharInfo);
void CyBle_BlscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_BlscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_BlscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_BlscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_BlscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_BlscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_BLS_CLIENT */
/* DOM-IGNORE-END */

/***************************************
##Internal macros
***************************************/

#ifdef CYBLE_BLS_SERVER
#define CYBLE_BLS_IS_ICP_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_blss.charInfo[CYBLE_BLS_ICP].charHandle)
#endif /* CYBLE_BLS_SERVER */


/***************************************
##External data references
***************************************/

#ifdef CYBLE_BLS_SERVER
    /* Blood Pressure Service GATT DB handles structure */
    extern const CYBLE_BLSS_T                                   cyBle_blss;
#endif /* CYBLE_BLSC_SERVER */

#ifdef CYBLE_BLS_CLIENT
    /* Blood Pressure Service server's GATT DB handles structure */
    extern CYBLE_BLSC_T                                         cyBle_blsc;
#endif /* CYBLE_BLS_CLIENT */

#endif /* CY_BLE_CYBLE_BLS_H */


/* [] END OF FILE */
