/*******************************************************************************
File Name: CYBLE_wpts.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for Wireless Power Transfer
 Service.

********************************************************************************
Copyright 2015, Cypress Semiconductor Corporation. All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_WPTS_H)
#define CY_BLE_CYBLE_WPTS_H

#include "`$INSTANCE_NAME`_gatt.h"


/* ##Data Structure Definition */

/* WPTS Characteristic indexes */
typedef enum
{
    CYBLE_WPTS_PRU_CONTROL,                     /* PRU Control Characteristic index */
    CYBLE_WPTS_PTU_STATIC_PAR,                  /* PTU Static Parameter Characteristic index */
    CYBLE_WPTS_PRU_ALERT,                       /* PRU Alert Characteristic index */
    CYBLE_WPTS_PRU_STATIC_PAR,                  /* PRU Static Parameter Characteristic index */
    CYBLE_WPTS_PRU_DYNAMIC_PAR,                 /* PRU Dynamic Parameter Characteristic index */
    CYBLE_WPTS_CHAR_COUNT                       /* Total count of WPTS Characteristics */
}CYBLE_WPTS_CHAR_INDEX_T;

/* WPTS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_WPTS_CCCD,                            /* Client Characteristic Configuration Descriptor index */
    CYBLE_WPTS_DESCR_COUNT                      /* Total count of Descriptors */
}CYBLE_WPTS_DESCR_INDEX_T;

/* Characteristic with descriptors */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                          /* Handle of characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_WPTS_DESCR_COUNT]; /* Handle of descriptor */
} CYBLE_WPTSS_CHAR_T;

/* WPTS Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;             /* Peer device handle */
    CYBLE_WPTS_CHAR_INDEX_T charIndex;          /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                  /* Characteristic value */
} CYBLE_WPTS_CHAR_VALUE_T;

/* WPTS Characteristic descriptor value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;             /* Peer device handle */
    CYBLE_WPTS_CHAR_INDEX_T charIndex;          /* Index of service characteristic */
    CYBLE_WPTS_DESCR_INDEX_T descrIndex;        /* Index of descriptor */
    CYBLE_GATT_VALUE_T *value;                 	/* Characteristic value */
} CYBLE_WPTS_DESCR_VALUE_T;

#ifdef CYBLE_WPTS_SERVER

/* Structure with Wireless Power Transfer Service attribute handles */
typedef struct
{
    /* Wireless Power Transfer Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;

    /* Wireless Power Transfer Characteristic handles */
    CYBLE_WPTSS_CHAR_T charInfo[CYBLE_WPTS_CHAR_COUNT];
} CYBLE_WPTSS_T;

#endif /* CYBLE_WPTS_SERVER */

#ifdef CYBLE_WPTS_CLIENT

typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_WPTS_DESCR_COUNT];/* Handles of descriptors */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;                        /* Handle of characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;                          /* End handle of a characteristic */
    uint8  properties;                                              /* Properties for value field */
} CYBLE_WPTSC_CHAR_T;

/* WPTS Characteristic with descriptors */
typedef struct
{
    /* Wireless Power Transfer Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    /* Wireless Power Transfer Service characteristics info structure */
    CYBLE_WPTSC_CHAR_T charInfo[CYBLE_WPTS_CHAR_COUNT];
} CYBLE_WPTSC_T;

#endif /* CYBLE_WPTS_CLIENT */


/* ##API Constants */

/* WPT service handle offsets */
#define CYBLE_WPTS_WPT_SERVICE_OFFSET                       (0x00u)
#define CYBLE_WPTS_PRU_CONTROL_CHAR_DECL_OFFSET             (0x01u)
#define CYBLE_WPTS_PRU_CONTROL_CHAR_VALUE_OFFSET            (0x02u)
#define CYBLE_WPTS_PTU_STATIC_PAR_CHAR_DECL_OFFSET          (0x03u)
#define CYBLE_WPTS_PTU_STATIC_PAR_CHAR_VALUE_OFFSET         (0x04u)
#define CYBLE_WPTS_PRU_ALERT_PAR_CHAR_DECL_OFFSET           (0x05u)
#define CYBLE_WPTS_PRU_ALERT_PAR_CHAR_VALUE_OFFSET          (0x06u)
#define CYBLE_WPTS_PRU_ALERT_PAR_CHAR_CCCD_OFFSET           (0x07u)
#define CYBLE_WPTS_PRU_STATIC_PAR_CHAR_DECL_OFFSET          (0x08u)
#define CYBLE_WPTS_PRU_STATIC_PAR_CHAR_VALUE_OFFSET         (0x09u)
#define CYBLE_WPTS_PRU_DYNAMIC_PAR_CHAR_DECL_OFFSET         (0x0Au)
#define CYBLE_WPTS_PRU_DYNAMIC_PAR_CHAR_VALUE_OFFSET        (0x0Bu)


/* ##Function Prototypes */

void CyBle_WptsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_WPTS_SERVER

CYBLE_API_RESULT_T CyBle_WptssSetCharacteristicValue(CYBLE_WPTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WptssGetCharacteristicValue(CYBLE_WPTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WptssSetCharacteristicDescriptor(CYBLE_WPTS_CHAR_INDEX_T charIndex,
    CYBLE_WPTS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WptssGetCharacteristicDescriptor(CYBLE_WPTS_CHAR_INDEX_T charIndex,
    CYBLE_WPTS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WptssSendNotification(CYBLE_CONN_HANDLE_T connHandle, CYBLE_WPTS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WptssSendIndication(CYBLE_CONN_HANDLE_T connHandle, CYBLE_WPTS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_WPTS_SERVER */

#ifdef CYBLE_WPTS_CLIENT

void CyBle_WptscDiscovery(CYBLE_GATT_DB_ATTR_HANDLE_T servHandle);
CYBLE_API_RESULT_T CyBle_WptscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_WPTS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 * attrValue);
CYBLE_API_RESULT_T CyBle_WptscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_WPTS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_WptscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_WPTS_CHAR_INDEX_T charIndex, CYBLE_WPTS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_WptscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_WPTS_CHAR_INDEX_T charIndex, CYBLE_WPTS_DESCR_INDEX_T descrIndex);

#endif /* CYBLE_WPTS_CLIENT */


/* ##Private Function Prototypes */

/* DOM-IGNORE-BEGIN */
void CyBle_WptsInit(void);

#ifdef CYBLE_WPTS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_WptssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_WptssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);

#endif /* CYBLE_WPTS_SERVER */

#ifdef CYBLE_WPTS_CLIENT

void CyBle_WptscDiscoverCharacteristicsEventHandler(const CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_WptscDiscoverCharDescriptorsEventHandler(const CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_WptscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_WptscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_WptscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_WptscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_WptscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_WPTS_CLIENT */

/* DOM-IGNORE-END */

/* ##Macro Functions */

#ifdef CYBLE_WPTS_CLIENT

#define CyBle_WptscGetCharacteristicValueHandle(charIndex)    \
    (((charIndex) >= CYBLE_WPTS_CHAR_COUNT) ? \
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE : \
            cyBle_wptsc.charInfo[charIndex].valueHandle)

#define CyBle_WptscGetCharacteristicDescriptorHandle(charIndex, descrIndex)    \
    ((((charIndex) >= CYBLE_WPTS_CHAR_COUNT) || ((descrIndex) >= CYBLE_WPTS_DESCR_COUNT)) ? \
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE : \
            cyBle_wptsc.charInfo[charIndex].descrHandle[descrIndex])

#endif /* (CYBLE_CPS_CLIENT) */


/***************************************
##External data references
***************************************/

#ifdef CYBLE_WPTS_SERVER

extern const CYBLE_WPTSS_T cyBle_wptss;

#endif /* CYBLE_WPTS_SERVER */

#ifdef CYBLE_WPTS_CLIENT

extern CYBLE_WPTSC_T cyBle_wptsc;
extern const CYBLE_UUID128_T cyBle_wptscCharUuid128[CYBLE_WPTS_CHAR_COUNT];

#endif /* CYBLE_WPTS_CLIENT */


#endif /* CY_BLE_CYBLE_WPTS_H */

/* [] END OF FILE */
