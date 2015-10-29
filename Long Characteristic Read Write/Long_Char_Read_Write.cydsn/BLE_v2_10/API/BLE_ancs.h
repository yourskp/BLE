/*******************************************************************************
File Name: CYBLE_ancs.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in
 the Apple Notification Center Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_ANCS_H)
#define CY_BLE_CYBLE_ANCS_H

#include "`$INSTANCE_NAME`_gatt.h"
    
#define CYBLE_ANCS_FLAG_PROCESS      (0x01u)


/***************************************
##Data Types
***************************************/

/* ANCS Service Characteristics indexes */
typedef enum
{
    CYBLE_ANCS_NS,          /* Notification Source characteristic index */
	CYBLE_ANCS_CP,          /* Control Point characteristic index */
	CYBLE_ANCS_DS,          /* Data Source characteristic index */
    CYBLE_ANCS_CHAR_COUNT    /* Total count of ANCS characteristics */
}CYBLE_ANCS_CHAR_INDEX_T;

/* ANCS Service Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_ANCS_CCCD,           /* Client Characteristic Configuration descriptor index */
    CYBLE_ANCS_DESCR_COUNT     /* Total count of ANCS descriptors */
}CYBLE_ANCS_DESCR_INDEX_T;


#ifdef CYBLE_ANCS_SERVER
/* Location and Navigation Server Characteristic structure type */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                          /* Handle of characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_ANCS_DESCR_COUNT];  /* Handle of descriptor */
}CYBLE_ANCSS_CHAR_T;
    
/* Structure with Location and Navigation Service attribute handles */
typedef struct
{
    /* Location and Navigation Service handle*/
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /* Location and Navigation Service characteristics info array */
    CYBLE_ANCSS_CHAR_T charInfo[CYBLE_ANCS_CHAR_COUNT];
} CYBLE_ANCSS_T;
#endif /* CYBLE_ANCS_SERVER */

#ifdef CYBLE_ANCS_CLIENT
/* Location and Navigation Client Characteristic structure type */
typedef struct
{
    /* Properties for value field */
    uint8  properties;

    /* Handle of server database attribute value entry */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;
    
    /* Location and Navigation client char. descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_ANCS_DESCR_COUNT];
    
    /* Characteristic End Handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
}CYBLE_ANCSC_CHAR_T;

/* Structure with discovered attributes information of Location and Navigation Service */
typedef struct
{
    CYBLE_ANCSC_CHAR_T charInfo[CYBLE_ANCS_CHAR_COUNT];   /* Characteristics handle + properties array */
}CYBLE_ANCSC_T;

#endif /* CYBLE_ANCS_CLIENT */

/* ANCS Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_ANCS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_ANCS_CHAR_VALUE_T;

/* ANCS Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_ANCS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_ANCS_DESCR_INDEX_T descrIndex;                     /* Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Descriptor value */
} CYBLE_ANCS_DESCR_VALUE_T;


/***************************************
##Function Prototypes
***************************************/

void CyBle_AncsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_ANCS_SERVER

CYBLE_API_RESULT_T CyBle_AncssSetCharacteristicValue(CYBLE_ANCS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_AncssGetCharacteristicValue(CYBLE_ANCS_CHAR_INDEX_T charIndex, 
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_AncssGetCharacteristicDescriptor(CYBLE_ANCS_CHAR_INDEX_T charIndex, 
                                                        CYBLE_ANCS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_AncssSendNotification(CYBLE_CONN_HANDLE_T connHandle, 
                                                CYBLE_ANCS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);
#endif /* CYBLE_ANCS_SERVER */

#ifdef CYBLE_ANCS_CLIENT

CYBLE_API_RESULT_T CyBle_AncscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_ANCS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_AncscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
                                                        CYBLE_ANCS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_AncscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_ANCS_CHAR_INDEX_T charIndex,
                                                        CYBLE_ANCS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_AncscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_ANCS_CHAR_INDEX_T charIndex,
                                                        CYBLE_ANCS_DESCR_INDEX_T descrIndex);
#endif /* CYBLE_ANCS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_AncsInit(void);

#ifdef CYBLE_ANCS_SERVER

void CyBle_AncssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
CYBLE_GATT_ERR_CODE_T CyBle_AncssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
//void CyBle_AncssPrepareWriteRequestEventHandler(CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam);
//void CyBle_AncssExecuteWriteRequestEventHandler(CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam);

#endif /* CYBLE_ANCS_SERVER */

#ifdef CYBLE_ANCS_CLIENT

void CyBle_AncscDiscoverServiceEventHandler(const CYBLE_DISC_SRVC128_INFO_T *discServInfo);
void CyBle_AncscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T * discCharInfo);
void CyBle_AncscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_AncscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
//void CyBle_AncscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
//void CyBle_AncscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_AncscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_AncscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_ANCS_CLIENT */

/* Internal macros */
#ifdef CYBLE_ANCS_SERVER
/*#define CYBLE_ANCS_IS_PQ_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ancss.charInfo[CYBLE_ANCS_PQ].charHandle)
    
#define CYBLE_ANCS_IS_CP_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ancss.charInfo[CYBLE_ANCS_UCP].charHandle)

#define CYBLE_ANCS_IS_NV_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_ancss.charInfo[CYBLE_ANCS_NV].charHandle)*/
    
#define CYBLE_ANCS_IS_PROCEDURE_IN_PROGRESS\
    (0u != (CYBLE_ANCS_FLAG_PROCESS & cyBle_ancsFlag))
#endif /* CYBLE_ANCS_SERVER */
/* DOM-IGNORE-END */


/***************************************
## External data references
***************************************/

#ifdef CYBLE_ANCS_SERVER
    /* Location and Navigation Service GATT DB handles structure */
    extern const CYBLE_ANCSS_T   cyBle_ancss;
    extern uint8                 cyBle_ancsFlag;
#endif /* CYBLE_ANCSC_SERVER */

#ifdef CYBLE_ANCS_CLIENT
    /* Location and Navigation Service server's GATT DB handles structure */
    extern CYBLE_ANCSC_T         cyBle_ancsc;
    extern const CYBLE_UUID128_T cyBle_ancscServUuid;
    extern const CYBLE_UUID128_T cyBle_ancscCharUuid[CYBLE_ANCS_CHAR_COUNT];
#endif /* CYBLE_ANCS_CLIENT */

#endif /* CY_BLE_CYBLE_ANCS_H */

/* [] END OF FILE */
