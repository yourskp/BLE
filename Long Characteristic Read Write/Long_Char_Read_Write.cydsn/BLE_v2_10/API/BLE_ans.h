/*******************************************************************************
File Name: CYBLE_ans.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for Alert Notification
 Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_ANS_H)
#define CY_BLE_CYBLE_ANS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Data Struct Definition
***************************************/

/* ANS Characteristic indexes */
typedef enum
{
    CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT,              /* Supported New Alert Category Characteristic index */
    CYBLE_ANS_NEW_ALERT,                            /* New Alert Characteristic index */
    CYBLE_ANS_SUPPORTED_UNREAD_ALERT_CAT,           /* Supported Unread Alert Category Characteristic index */
    CYBLE_ANS_UNREAD_ALERT_STATUS,                  /* Unread Alert Status Characteristic index */
    CYBLE_ANS_ALERT_NTF_CONTROL_POINT,              /* Alert Notification Control Point Characteristic index */
    CYBLE_ANS_CHAR_COUNT                            /* Total count of ANS characteristics */
}CYBLE_ANS_CHAR_INDEX_T;

/* ANS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_ANS_CCCD,                                  /* Client Characteristic Configuration Descriptor index */
    CYBLE_ANS_DESCR_COUNT                            /* Total count of descriptors */
}CYBLE_ANS_DESCR_INDEX_T;

/* Alert Notification Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                  /* Peer device handle */
    CYBLE_ANS_CHAR_INDEX_T charIndex;                /* Index of Alert Notification Service Characteristic */
    CYBLE_GATT_VALUE_T *value;                       /* Pointer to Characteristic value */
} CYBLE_ANS_CHAR_VALUE_T;

/* Alert Notification Service Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T           connHandle;        /* Connection handle */
    CYBLE_ANS_CHAR_INDEX_T        charIndex;         /* Characteristic index of Service */
    CYBLE_ANS_DESCR_INDEX_T       descrIndex;        /* Service Characteristic Descriptor index */
    CYBLE_GATT_VALUE_T            *value;            /* Pointer to value of Service Characteristic Descriptor value */
} CYBLE_ANS_DESCR_VALUE_T;

#ifdef CYBLE_ANS_SERVER
    
/* ANS Characteristic with descriptors */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                          /* Handle of Characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_ANS_DESCR_COUNT];  /* Handle of Descriptor */
} CYBLE_ANSS_CHAR_T;

/* Structure with Alert Notification Service attribute handles */
typedef struct
{
    /* Alert Notification Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T   serviceHandle;
    /* Array of Alert Notification Service Characteristics + Descriptors handles */
    CYBLE_ANSS_CHAR_T             charInfo[CYBLE_ANS_CHAR_COUNT];
} CYBLE_ANSS_T;

#endif /* CYBLE_ANS_SERVER */

#ifdef CYBLE_ANS_CLIENT

/* Service Full characteristic information type */
typedef struct
{
    /* Characteristic handle + properties */
    CYBLE_SRVR_CHAR_INFO_T charInfo;
    
    /* End handle of characteristic */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
    
    /* Characteristic descriptors handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T descriptors[CYBLE_ANS_DESCR_COUNT];
} CYBLE_SRVR_FULL_CHAR_INFO_T;
    
/* Structure with discovered attributes information of Alert Notification Service */
typedef struct
{
    /* Structure with Characteristic handles + properties of Alert Notification Service */
    CYBLE_SRVR_FULL_CHAR_INFO_T characteristics[CYBLE_ANS_CHAR_COUNT];
} CYBLE_ANSC_T;

#endif /* CYBLE_ANS_CLIENT */


/***************************************
##API Constants
***************************************/

#define CYBLE_ANS_INVALID_CHAR_INDEX             (0xFFu)

/* Alert Notification Characteristic Command ID */
#define CYBLE_ANS_EN_NEW_ALERT_NTF               (0x00u)  /* Enable New Incoming Alert Notification */
#define CYBLE_ANS_EN_UNREAD_ALERT_STATUS_NTF     (0x01u)  /* Enable Unread Category Status Notification */
#define CYBLE_ANS_DIS_NEW_ALERT_NTF              (0x02u)  /* Disable New Incoming Alert Notification */
#define CYBLE_ANS_DIS_UNREAD_ALERT_STATUS_NTF    (0x03u)  /* Disable Unread Category Status Notification */
#define CYBLE_ANS_IMM_NEW_ALERT_NTF              (0x04u)  /* Notify New Incoming Alert immediately */
#define CYBLE_ANS_IMM_UNREAD_ALERT_STATUS_NTF    (0x05u)  /* Notify Unread Category Status immediately */

/* Alert Notification Characteristic Category ID */
#define CYBLE_ANS_CAT_ID_SIMPLE_ALERT            (0x00u)  /* Simple Alert: General text alert or non-text alert */
#define CYBLE_ANS_CAT_ID_EMAIL                   (0x01u)  /* Email: Alert when Email messages arrives */
#define CYBLE_ANS_CAT_ID_NEWS                    (0x02u)  /* News: News feeds such as RSS, Atom */
#define CYBLE_ANS_CAT_ID_CALL                    (0x03u)  /* Call: Incoming call */
#define CYBLE_ANS_CAT_ID_MISSED_CALL             (0x04u)  /* Missed call: Missed Call */
#define CYBLE_ANS_CAT_ID_SMS_MMS                 (0x05u)  /* SMS/MMS: SMS/MMS message arrives */
#define CYBLE_ANS_CAT_ID_VOICE_MAIL              (0x06u)  /* Voice mail: Voice mail */
#define CYBLE_ANS_CAT_ID_SCHEDULE                (0x07u)  /* Schedule: Alert occurred on calendar, planner */
#define CYBLE_ANS_CAT_ID_HI_PRFIORITIZED_ALERT   (0x08u)  /* High Prioritized Alert: Alert that should be handled as 
                                                            high priority
                                                           */
#define CYBLE_ANS_CAT_ID_INSTANT_MESSAGE         (0x09u)  /* Instant Message: Alert for incoming instant messages */

#define CYBLE_ANS_CAT_ID_ALL                     (0xFFu)  /* Category ID - All categories */

#define CYBLE_ANS_CHAR_VALUE_OFFSET              (0x02u)


/***************************************
##Function Prototypes
***************************************/

void CyBle_AnsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_ANS_SERVER

CYBLE_API_RESULT_T CyBle_AnssSetCharacteristicValue(CYBLE_ANS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_AnssGetCharacteristicValue(CYBLE_ANS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_AnssGetCharacteristicDescriptor(CYBLE_ANS_CHAR_INDEX_T charIndex,
    CYBLE_ANS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_AnssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ANS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_ANS_SERVER */

#ifdef CYBLE_ANS_CLIENT

CYBLE_API_RESULT_T CyBle_AnscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_ANS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_AnscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_ANS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_AnscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ANS_CHAR_INDEX_T charIndex, CYBLE_ANS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_AnscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ANS_CHAR_INDEX_T charIndex, uint8 descrIndex);

#endif /* (CYBLE_ANS_CLIENT) */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_AnsInit(void);

#ifdef CYBLE_ANS_SERVER
    
CYBLE_GATT_ERR_CODE_T CyBle_AnssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* CYBLE_ANS_SERVER */

#ifdef CYBLE_ANS_CLIENT

void CyBle_AnscDiscoverCharacteristicsEventHandler(const CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_AnscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_AnscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_AnscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_AnscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_AnscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_ANS_CLIENT) */
/* DOM-IGNORE-END */

/***************************************
##External data references
***************************************/

#ifdef CYBLE_ANS_SERVER

extern const CYBLE_ANSS_T cyBle_anss;

#endif /* CYBLE_ANS_SERVER */

#ifdef CYBLE_ANS_CLIENT

extern CYBLE_ANSC_T cyBle_ansc;

#endif /* (CYBLE_ANS_CLIENT) */

#endif /* CY_BLE_CYBLE_ANS_H  */


/* [] END OF FILE */
