/*******************************************************************************
File Name: CYBLE_gls.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in the 
 Glucose Profile of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BLE_CYBLE_GLS_H)
#define CY_BLE_CYBLE_GLS_H

#include "`$INSTANCE_NAME`_gatt.h"
    
#define CYBLE_GLS_FLAG_PROCESS      (0x01u)
#define CYBLE_GLS_RACP_OPCODE_ABORT (0x03u)


/***************************************    
##Data Types
***************************************/

/* Service Characteristics indexes */
typedef enum
{
    CYBLE_GLS_GLMT,         /* Glucose Measurement characteristic index */
    CYBLE_GLS_GLMC,         /* Glucose Measurement Context characteristic index */
    CYBLE_GLS_GLFT,         /* Glucose Feature characteristic index */
    CYBLE_GLS_RACP,         /* Record Access Control Point characteristic index */
    CYBLE_GLS_CHAR_COUNT    /* Total count of GLS characteristics */
}CYBLE_GLS_CHAR_INDEX_T;

/* Service Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_GLS_CCCD,           /* Client Characteristic Configuration descriptor index */
    CYBLE_GLS_DESCR_COUNT     /* Total count of GLS descriptors */
}CYBLE_GLS_DESCR_INDEX_T;


#ifdef CYBLE_GLS_SERVER
/* Glucose Server Characteristic structure type */
typedef struct
{
    /* Glucose Service char handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;

    /* Glucose Service CCCD handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T cccdHandle;
}CYBLE_GLSS_CHAR_T;
    
/* Structure with Glucose Service attribute handles */
typedef struct
{
    /* Glucose Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /* Glucose Service characteristics info array */
    CYBLE_GLSS_CHAR_T charInfo[CYBLE_GLS_CHAR_COUNT];
} CYBLE_GLSS_T;
#endif /* CYBLE_GLS_SERVER */

#ifdef CYBLE_GLS_CLIENT
/* Glucose Client Characteristic structure type */
typedef struct
{
    /* Properties for value field */
    uint8  properties;

    /* Handle of server database attribute value entry */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;
    
    /* Glucose client char. descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T cccdHandle;
    
    /* Characteristic End Handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
}CYBLE_GLSC_CHAR_T;

/* Glucose Service structure type */
typedef struct
{
    /* Characteristics handle + properties array */
    CYBLE_GLSC_CHAR_T charInfo[CYBLE_GLS_CHAR_COUNT];
}CYBLE_GLSC_T;

#endif /* CYBLE_GLS_CLIENT */

/* Glucose Service Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_GLS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_GLS_CHAR_VALUE_T;

/* Glucose Service Characteristic descriptor value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_GLS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_GLS_DESCR_INDEX_T descrIndex;                     /* Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Descriptor value */
} CYBLE_GLS_DESCR_VALUE_T;


/***************************************
##Function Prototypes
***************************************/

void CyBle_GlsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_GLS_SERVER

CYBLE_API_RESULT_T CyBle_GlssSetCharacteristicValue(CYBLE_GLS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_GlssGetCharacteristicValue(CYBLE_GLS_CHAR_INDEX_T charIndex, 
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_GlssGetCharacteristicDescriptor(CYBLE_GLS_CHAR_INDEX_T charIndex, 
                                                        CYBLE_GLS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_GlssSendNotification(CYBLE_CONN_HANDLE_T connHandle, 
                                                CYBLE_GLS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_GlssSendIndication(CYBLE_CONN_HANDLE_T connHandle, 
                                                CYBLE_GLS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);
#endif /* CYBLE_GLS_SERVER */

#ifdef CYBLE_GLS_CLIENT

CYBLE_API_RESULT_T CyBle_GlscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_GLS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_GlscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
                                                        CYBLE_GLS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_GlscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_GLS_CHAR_INDEX_T charIndex,
                                                        CYBLE_GLS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_GlscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_GLS_CHAR_INDEX_T charIndex,
                                                        CYBLE_GLS_DESCR_INDEX_T descrIndex);
#endif /* CYBLE_GLS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_GlsInit(void);

#ifdef CYBLE_GLS_SERVER

void CyBle_GlssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
CYBLE_GATT_ERR_CODE_T CyBle_GlssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* CYBLE_GLS_SERVER */

#ifdef CYBLE_GLS_CLIENT

void CyBle_GlscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T * discCharInfo);
void CyBle_GlscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_GlscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_GlscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_GlscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_GlscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_GlscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_GLS_CLIENT */

/* Internal macros */
#ifdef CYBLE_GLS_SERVER
#define CYBLE_GLS_IS_GMC_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_glss.charInfo[CYBLE_GLS_GLMC].charHandle)
    
#define CYBLE_GLS_IS_PROCEDURE_IN_PROGRESS\
    (0u != (CYBLE_GLS_FLAG_PROCESS & cyBle_glsFlag))
#endif /* CYBLE_GLS_SERVER */
/* DOM-IGNORE-END */


/***************************************
## External data references
***************************************/

#ifdef CYBLE_GLS_SERVER
    /* Glucose Service GATT DB handles structure */
    extern const CYBLE_GLSS_T                                   cyBle_glss;
    extern uint8 cyBle_glsFlag;
#endif /* CYBLE_GLSC_SERVER */

#ifdef CYBLE_GLS_CLIENT
    /* Glucose Service server's GATT DB handles structure */
    extern CYBLE_GLSC_T                                         cyBle_glsc;
#endif /* CYBLE_GLS_CLIENT */

#endif /* CY_BLE_CYBLE_GLS_H */

/* [] END OF FILE */
