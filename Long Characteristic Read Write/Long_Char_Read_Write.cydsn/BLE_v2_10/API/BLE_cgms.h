/*******************************************************************************
File Name: CYBLE_cgms.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in the 
 Continuous Glucose Monitoring Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BLE_CYBLE_CGMS_H)
#define CY_BLE_CYBLE_CGMS_H

#include "`$INSTANCE_NAME`_gatt.h"
    

/***************************************    
##Data Types
***************************************/

/* Service Characteristics indexes */
typedef enum
{
    CYBLE_CGMS_CGMT,        /* CGM Measurement characteristic index */
    CYBLE_CGMS_CGFT,        /* CGM Feature characteristic index */
    CYBLE_CGMS_CGST,        /* CGM Status characteristic index */
    CYBLE_CGMS_SSTM,        /* CGM Session Start Time characteristic index */
    CYBLE_CGMS_SRTM,        /* CGM Session Run Time characteristic index */
    CYBLE_CGMS_RACP,        /* Record Access Control Point characteristic index */
    CYBLE_CGMS_SOCP,        /* CGM Specific Ops Control Point characteristic index */
    CYBLE_CGMS_CHAR_COUNT   /* Total count of CGMS characteristics */
}CYBLE_CGMS_CHAR_INDEX_T;

/* Service Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_CGMS_CCCD,           /* Client Characteristic Configuration descriptor index */
    CYBLE_CGMS_DESCR_COUNT     /* Total count of CGMS descriptors */
}CYBLE_CGMS_DESCR_INDEX_T;


#ifdef CYBLE_CGMS_SERVER
/* Characteristic with descriptors type */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                             /* Handle of Characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_CGMS_DESCR_COUNT];    /* Handles of Descriptors */
} CYBLE_CGMSS_CHAR_T;
    
/* Structure with CGM Service attribute handles */
typedef struct
{
    /* CGM Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;                      
    
    /* CGM Service characteristics info array */
    CYBLE_CGMSS_CHAR_T charInfo[CYBLE_CGMS_CHAR_COUNT];
} CYBLE_CGMSS_T;
#endif /* CYBLE_CGMS_SERVER */

#ifdef CYBLE_CGMS_CLIENT
/* CGM Client Characteristic structure type */
typedef struct
{
    /* Properties for value field */
    uint8  properties;

    /* Handle of Server database attribute value entry */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;
    
    /* Characteristics descriptors handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_CGMS_DESCR_COUNT];
    
    /* Characteristic End Handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
}CYBLE_CGMSC_CHAR_T;

/* CGM Service structure type */
typedef struct
{
    /* Characteristics handle + properties array */
    CYBLE_CGMSC_CHAR_T charInfo[CYBLE_CGMS_CHAR_COUNT];
}CYBLE_CGMSC_T;

#endif /* CYBLE_CGMS_CLIENT */

/* CGM Service Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_CGMS_CHAR_INDEX_T charIndex;                      /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
    CYBLE_GATT_ERR_CODE_T gattErrorCode;                    /* GATT error code for access control */
} CYBLE_CGMS_CHAR_VALUE_T;

/* CGM Service Characteristic descriptor value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_CGMS_CHAR_INDEX_T charIndex;                      /* Index of service characteristic */
    CYBLE_CGMS_DESCR_INDEX_T descrIndex;                    /* Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Descriptor value */
} CYBLE_CGMS_DESCR_VALUE_T;


/***************************************
##API Constants
***************************************/

#define CYBLE_CGMS_FLAG_PROCESS      (0x01u)
#define CYBLE_CGMS_RACP_OPCODE_ABORT (0x03u)


/***************************************
##Function Prototypes
***************************************/

void CyBle_CgmsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_CGMS_SERVER

CYBLE_API_RESULT_T CyBle_CgmssSetCharacteristicValue(CYBLE_CGMS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CgmssGetCharacteristicValue(CYBLE_CGMS_CHAR_INDEX_T charIndex, 
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CgmssSetCharacteristicDescriptor(CYBLE_CGMS_CHAR_INDEX_T charIndex, 
                                                        CYBLE_CGMS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CgmssGetCharacteristicDescriptor(CYBLE_CGMS_CHAR_INDEX_T charIndex, 
                                                        CYBLE_CGMS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CgmssSendNotification(CYBLE_CONN_HANDLE_T connHandle, 
                                                        CYBLE_CGMS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CgmssSendIndication(CYBLE_CONN_HANDLE_T connHandle, 
                                                        CYBLE_CGMS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 *attrValue);
#endif /* CYBLE_CGMS_SERVER */

#ifdef CYBLE_CGMS_CLIENT

CYBLE_API_RESULT_T CyBle_CgmscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_CGMS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CgmscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
                                                        CYBLE_CGMS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_CgmscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_CGMS_CHAR_INDEX_T charIndex,
                                                        CYBLE_CGMS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CgmscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_CGMS_CHAR_INDEX_T charIndex,
                                                        CYBLE_CGMS_DESCR_INDEX_T descrIndex);
#endif /* CYBLE_CGMS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_CgmsInit(void);

#ifdef CYBLE_CGMS_SERVER

void CyBle_CgmssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
CYBLE_GATT_ERR_CODE_T CyBle_CgmssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* CYBLE_CGMS_SERVER */

#ifdef CYBLE_CGMS_CLIENT

void CyBle_CgmscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_CgmscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_CgmscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_CgmscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_CgmscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_CgmscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_CgmscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_CGMS_CLIENT */

/* Internal macros */
#ifdef CYBLE_CGMS_SERVER    
#define CYBLE_CGMS_IS_PROCEDURE_IN_PROGRESS\
    (0u != (CYBLE_CGMS_FLAG_PROCESS & cyBle_cgmsFlag))
#endif /* CYBLE_CGMS_SERVER */
/* DOM-IGNORE-END */


/***************************************
## External data references
***************************************/

#ifdef CYBLE_CGMS_SERVER
    /* CGM Service GATT DB handles structure */
    extern const CYBLE_CGMSS_T  cyBle_cgmss;
    extern uint8                cyBle_cgmsFlag;
#endif /* CYBLE_CGMS_SERVER */

#ifdef CYBLE_CGMS_CLIENT
    /* CGM Service server's GATT DB handles structure */
    extern CYBLE_CGMSC_T        cyBle_cgmsc;
#endif /* CYBLE_CGMS_CLIENT */

#endif /* CY_BLE_CYBLE_CGMS_H */

/* [] END OF FILE */
