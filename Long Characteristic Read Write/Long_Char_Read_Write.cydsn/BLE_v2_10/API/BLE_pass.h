/*******************************************************************************
File Name: CYBLE_pass.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in
 the Phone Alert Status Profile of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BLE_CYBLE_PASS_H)
#define CY_BLE_CYBLE_PASS_H

#include "`$INSTANCE_NAME`_gatt.h"

   
/***************************************
##Data Types
***************************************/

/* Service Characteristics indexes */
typedef enum
{
    CYBLE_PASS_AS,          /* Alert Status characteristic index */
    CYBLE_PASS_RS,          /* Ringer Setting characteristic index */
    CYBLE_PASS_CP,          /* Ringer Control Point characteristic index */
    CYBLE_PASS_CHAR_COUNT   /* Total count of PASS characteristics */
}CYBLE_PASS_CHAR_INDEX_T;

/* Service Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_PASS_CCCD,           /* Client Characteristic Configuration descriptor index */
    CYBLE_PASS_DESCR_COUNT     /* Total count of PASS descriptors */
}CYBLE_PASS_DESCR_INDEX_T;

/* Alert Status values */
#define CYBLE_PASS_AS_RINGER    (0x01)  /* Ringer State */
#define CYBLE_PASS_AS_VIBRATE   (0x02)  /* Vibrate State */
#define CYBLE_PASS_AS_DISPLAY   (0x04)  /* Display Alert Status */

/* Ringer Setting values */
typedef enum
{
    CYBLE_PASS_RS_SILENT,   /* Ringer Silent */
    CYBLE_PASS_RS_NORMAL    /* Ringer Normal */
}CYBLE_PASS_RS_T;

/* Ringer Control Point values */
typedef enum
{
    CYBLE_PASS_CP_SILENT = 1,   /* Silent Mode */
    CYBLE_PASS_CP_MUTE,         /* Mute Once */
    CYBLE_PASS_CP_CANCEL        /* Cancel Silent Mode */
}CYBLE_PASS_CP_T;

#ifdef CYBLE_PASS_SERVER
/* Structure with Phone Alert Status Service characteristics and descriptors attribute handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                           /* Handle of characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_PASS_DESCR_COUNT];  /* Handle of descriptor */
}CYBLE_PASSS_CHAR_T;
    
/* Structure with Phone Alert Status Service attribute handles */
typedef struct
{
    /* Phone Alert Status Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /* Phone Alert Status Service characteristics info array */
    CYBLE_PASSS_CHAR_T charInfo[CYBLE_PASS_CHAR_COUNT];
} CYBLE_PASSS_T;
#endif /* CYBLE_PASS_SERVER */

#ifdef CYBLE_PASS_CLIENT
/* Phone Alert Status Client Server's Characteristic structure type */
typedef struct
{
    /* Properties for value field */
    uint8  properties;

    /* Handle of Server database attribute value entry */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;
    
    /* Phone Alert Status Client characteristics descriptors handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_PASS_DESCR_COUNT];
    
    /* Characteristic End Handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
}CYBLE_PASSC_CHAR_T;

/* Structure with discovered attributes information of Phone Alert Status Service */
typedef struct
{
    CYBLE_PASSC_CHAR_T charInfo[CYBLE_PASS_CHAR_COUNT];     /* Characteristics handle and properties array */
}CYBLE_PASSC_T;

#endif /* CYBLE_PASS_CLIENT */

/* Phone Alert Status Service Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_PASS_CHAR_INDEX_T charIndex;                      /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_PASS_CHAR_VALUE_T;

/* Phone Alert Status Service Characteristic descriptor value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_PASS_CHAR_INDEX_T charIndex;                      /* Index of service characteristic */
    CYBLE_PASS_DESCR_INDEX_T descrIndex;                    /* Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Descriptor value */
} CYBLE_PASS_DESCR_VALUE_T;


/***************************************
##Function Prototypes
***************************************/

void CyBle_PassRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_PASS_SERVER

CYBLE_API_RESULT_T CyBle_PasssSetCharacteristicValue(CYBLE_PASS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_PasssGetCharacteristicValue(CYBLE_PASS_CHAR_INDEX_T charIndex, 
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_PasssGetCharacteristicDescriptor(CYBLE_PASS_CHAR_INDEX_T charIndex, 
                                                        CYBLE_PASS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_PasssSendNotification(CYBLE_CONN_HANDLE_T connHandle, 
                                                CYBLE_PASS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);
#endif /* CYBLE_PASS_SERVER */

#ifdef CYBLE_PASS_CLIENT

CYBLE_API_RESULT_T CyBle_PasscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_PASS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_PasscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
                                                        CYBLE_PASS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_PasscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_PASS_CHAR_INDEX_T charIndex,
                                                        CYBLE_PASS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_PasscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_PASS_CHAR_INDEX_T charIndex,
                                                        CYBLE_PASS_DESCR_INDEX_T descrIndex);
#endif /* CYBLE_PASS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_PassInit(void);

#ifdef CYBLE_PASS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_PasssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_PasssWriteCmdEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* CYBLE_PASS_SERVER */

#ifdef CYBLE_PASS_CLIENT

void CyBle_PasscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T * discCharInfo);
void CyBle_PasscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_PasscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_PasscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_PasscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_PasscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_PASS_CLIENT */

/* Internal macros */
#ifdef CYBLE_PASS_SERVER
#define CYBLE_PASS_IS_GMC_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_passs.charInfo[CYBLE_PASS_GLMC].charHandle)
#endif /* CYBLE_PASS_SERVER */
/* DOM-IGNORE-BEGIN */


/***************************************
## External data references
***************************************/

#ifdef CYBLE_PASS_SERVER
    /* Phone Alert Status Service GATT DB handles structure */
    extern const CYBLE_PASSS_T                                   cyBle_passs;
    extern uint8 cyBle_passFlag;
#endif /* CYBLE_PASSC_SERVER */

#ifdef CYBLE_PASS_CLIENT
    /* Phone Alert Status Service Server's GATT DB handles structure */
    extern CYBLE_PASSC_T                                         cyBle_passc;
#endif /* CYBLE_PASS_CLIENT */

#endif /* CY_BLE_CYBLE_PASS_H */

/* [] END OF FILE */
