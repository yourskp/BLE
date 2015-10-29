/*******************************************************************************
File Name: CYBLE_uds.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in
 the User Data Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_UDS_H)
#define CY_BLE_CYBLE_UDS_H

#include "`$INSTANCE_NAME`_gatt.h"
    
#define CYBLE_UDS_FLAG_PROCESS      (0x01u)


/***************************************
##Data Types
***************************************/

/* UDS Service Characteristics indexes */
typedef enum
{
    CYBLE_UDS_FNM,          /* First Name characteristic index */
	CYBLE_UDS_LNM,          /* Last Name characteristic index */
	CYBLE_UDS_EML,          /* Email Address characteristic index */
	CYBLE_UDS_AGE,          /* Age characteristic index */
	CYBLE_UDS_DOB,          /* Date of Birth characteristic index */
	CYBLE_UDS_GND,          /* Gender characteristic index */
	CYBLE_UDS_WGT,          /* Weight characteristic index */
	CYBLE_UDS_HGT,          /* Height characteristic index */
	CYBLE_UDS_VO2,          /* VO2 Max characteristic index */
	CYBLE_UDS_HRM,          /* Heart Rate Max characteristic index */
	CYBLE_UDS_RHR,          /* Resting Heart Rate characteristic index */
	CYBLE_UDS_MRH,          /* Maximum Recommended Heart Rate characteristic index */
    CYBLE_UDS_AET,          /* Aerobic Threshold characteristic index */
    CYBLE_UDS_ANT,          /* Anaerobic Threshold characteristic index */
    CYBLE_UDS_STP,          /* Sport Type for Aerobic and Anaerobic Thresholds characteristic index */
    CYBLE_UDS_DTA,          /* Date of Threshold Assessment characteristic index */
    CYBLE_UDS_WCC,          /* Waist Circumference characteristic index */
    CYBLE_UDS_HCC,          /* Hip Circumference characteristic index */
    CYBLE_UDS_FBL,          /* Fat Burn Heart Rate Lower Limit characteristic index */
    CYBLE_UDS_FBU,          /* Fat Burn Heart Rate Upper Limit characteristic index */
    CYBLE_UDS_AEL,          /* Aerobic Heart Rate Lower Limit characteristic index */
    CYBLE_UDS_AEU,          /* Aerobic Heart Rate Upper Limit characteristic index */
    CYBLE_UDS_ANL,          /* Anaerobic Heart Rate Lower Limit characteristic index */
    CYBLE_UDS_ANU,          /* Anaerobic Heart Rate Upper Limit characteristic index */
    CYBLE_UDS_5ZL,          /* Five Zone Heart Rate Limits characteristic index */
    CYBLE_UDS_3ZL,          /* Three Zone Heart Rate Limits characteristic index */
    CYBLE_UDS_2ZL,          /* Two Zone Heart Rate Limit characteristic index */
	CYBLE_UDS_DCI,          /* Database Change Increment characteristic index */
    CYBLE_UDS_UIX,          /* User Index characteristic index */
    CYBLE_UDS_UCP,          /* User Control Point characteristic index */
    CYBLE_UDS_LNG,          /* Language characteristic index */
    CYBLE_UDS_CHAR_COUNT    /* Total count of UDS characteristics */
}CYBLE_UDS_CHAR_INDEX_T;

/* UDS Service Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_UDS_CCCD,           /* Client Characteristic Configuration descriptor index */
    CYBLE_UDS_DESCR_COUNT     /* Total count of UDS descriptors */
}CYBLE_UDS_DESCR_INDEX_T;


#ifdef CYBLE_UDS_SERVER
/* Location and Navigation Server Characteristic structure type */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                          /* Handle of characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_UDS_DESCR_COUNT];  /* Handle of descriptor */
}CYBLE_UDSS_CHAR_T;
    
/* Structure with Location and Navigation Service attribute handles */
typedef struct
{
    /* Location and Navigation Service handle*/
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /* Location and Navigation Service characteristics info array */
    CYBLE_UDSS_CHAR_T charInfo[CYBLE_UDS_CHAR_COUNT];
} CYBLE_UDSS_T;
#endif /* CYBLE_UDS_SERVER */

#ifdef CYBLE_UDS_CLIENT
/* Location and Navigation Client Characteristic structure type */
typedef struct
{
    /* Properties for value field */
    uint8  properties;

    /* Handle of server database attribute value entry */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;
    
    /* Location and Navigation client char. descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_UDS_DESCR_COUNT];
    
    /* Characteristic End Handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
}CYBLE_UDSC_CHAR_T;

/* Structure with discovered attributes information of Location and Navigation Service */
typedef struct
{
    CYBLE_UDSC_CHAR_T charInfo[CYBLE_UDS_CHAR_COUNT];   /* Characteristics handle + properties array */
}CYBLE_UDSC_T;

#endif /* CYBLE_UDS_CLIENT */

/* UDS Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_UDS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
    CYBLE_GATT_ERR_CODE_T gattErrorCode;                    /* GATT error code for access control */
} CYBLE_UDS_CHAR_VALUE_T;

/* UDS Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_UDS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_UDS_DESCR_INDEX_T descrIndex;                     /* Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Descriptor value */
} CYBLE_UDS_DESCR_VALUE_T;


/***************************************
##Function Prototypes
***************************************/

void CyBle_UdsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_UDS_SERVER

CYBLE_API_RESULT_T CyBle_UdssSetCharacteristicValue(CYBLE_UDS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_UdssGetCharacteristicValue(CYBLE_UDS_CHAR_INDEX_T charIndex, 
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_UdssGetCharacteristicDescriptor(CYBLE_UDS_CHAR_INDEX_T charIndex, 
                                                        CYBLE_UDS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_UdssSendNotification(CYBLE_CONN_HANDLE_T connHandle, 
                                                CYBLE_UDS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_UdssSendIndication(CYBLE_CONN_HANDLE_T connHandle, 
                                                CYBLE_UDS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);
#endif /* CYBLE_UDS_SERVER */

#ifdef CYBLE_UDS_CLIENT

CYBLE_API_RESULT_T CyBle_UdscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_UDS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_UdscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
                                                        CYBLE_UDS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_UdscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_UDS_CHAR_INDEX_T charIndex,
                                                        CYBLE_UDS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_UdscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_UDS_CHAR_INDEX_T charIndex,
                                                        CYBLE_UDS_DESCR_INDEX_T descrIndex);
#endif /* CYBLE_UDS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_UdsInit(void);

#ifdef CYBLE_UDS_SERVER

void CyBle_UdssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_UdssReadRequestEventHandler(CYBLE_GATTS_CHAR_VAL_READ_REQ_T *eventParam);
CYBLE_GATT_ERR_CODE_T CyBle_UdssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_UdssPrepareWriteRequestEventHandler(CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam);
void CyBle_UdssExecuteWriteRequestEventHandler(CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam);

#endif /* CYBLE_UDS_SERVER */

#ifdef CYBLE_UDS_CLIENT

void CyBle_UdscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T * discCharInfo);
void CyBle_UdscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_UdscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_UdscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_UdscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_UdscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_UdscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_UDS_CLIENT */

/* Internal macros */
#ifdef CYBLE_UDS_SERVER
/*#define CYBLE_UDS_IS_PQ_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_udss.charInfo[CYBLE_UDS_PQ].charHandle)
    
#define CYBLE_UDS_IS_CP_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_udss.charInfo[CYBLE_UDS_UCP].charHandle)

#define CYBLE_UDS_IS_NV_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_udss.charInfo[CYBLE_UDS_NV].charHandle)*/
    
#define CYBLE_UDS_IS_PROCEDURE_IN_PROGRESS\
    (0u != (CYBLE_UDS_FLAG_PROCESS & cyBle_udsFlag))
#endif /* CYBLE_UDS_SERVER */
/* DOM-IGNORE-END */


/***************************************
## External data references
***************************************/

#ifdef CYBLE_UDS_SERVER
    /* Location and Navigation Service GATT DB handles structure */
    extern const CYBLE_UDSS_T   cyBle_udss;
    extern uint8                cyBle_udsFlag;
#endif /* CYBLE_UDSC_SERVER */

#ifdef CYBLE_UDS_CLIENT
    /* Location and Navigation Service server's GATT DB handles structure */
    extern CYBLE_UDSC_T         cyBle_udsc;
#endif /* CYBLE_UDS_CLIENT */

#endif /* CY_BLE_CYBLE_UDS_H */

/* [] END OF FILE */
