/*******************************************************************************
File Name: CYBLE_lns.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in
 the Location And Navigation Profile of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_LNS_H)
#define CY_BLE_CYBLE_LNS_H

#include "`$INSTANCE_NAME`_gatt.h"
    
#define CYBLE_LNS_FLAG_PROCESS      (0x01u)


/***************************************
##Data Types
***************************************/

/* LNS Service Characteristics indexes */
typedef enum
{
    CYBLE_LNS_FT,           /* Location and Navigation Feature characteristic index */
    CYBLE_LNS_LS,           /* Location and Speed characteristic index */
    CYBLE_LNS_PQ,           /* Position Quality characteristic index */
    CYBLE_LNS_CP,           /* Location and Navigation Control Point characteristic index */
    CYBLE_LNS_NV,           /* Navigation characteristic index */
    CYBLE_LNS_CHAR_COUNT    /* Total count of LNS characteristics */
}CYBLE_LNS_CHAR_INDEX_T;

/* LNS Service Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_LNS_CCCD,           /* Client Characteristic Configuration descriptor index */
    CYBLE_LNS_DESCR_COUNT     /* Total count of LNS descriptors */
}CYBLE_LNS_DESCR_INDEX_T;


#ifdef CYBLE_LNS_SERVER
/* Location and Navigation Server Characteristic structure type */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                          /* Handle of characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_LNS_DESCR_COUNT];  /* Handle of descriptor */
}CYBLE_LNSS_CHAR_T;
    
/* Structure with Location and Navigation Service attribute handles */
typedef struct
{
    /* Location and Navigation Service handle*/
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /* Location and Navigation Service characteristics info array */
    CYBLE_LNSS_CHAR_T charInfo[CYBLE_LNS_CHAR_COUNT];
} CYBLE_LNSS_T;
#endif /* CYBLE_LNS_SERVER */

#ifdef CYBLE_LNS_CLIENT
/* Location and Navigation Client Characteristic structure type */
typedef struct
{
    /* Properties for value field */
    uint8  properties;

    /* Handle of server database attribute value entry */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;
    
    /* Location and Navigation client char. descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_LNS_DESCR_COUNT];
    
    /* Characteristic End Handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
}CYBLE_LNSC_CHAR_T;

/* Structure with discovered attributes information of Location and Navigation Service */
typedef struct
{
    CYBLE_LNSC_CHAR_T charInfo[CYBLE_LNS_CHAR_COUNT];   /* Characteristics handle + properties array */
}CYBLE_LNSC_T;

#endif /* CYBLE_LNS_CLIENT */

/* LNS Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_LNS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_LNS_CHAR_VALUE_T;

/* LNS Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_LNS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_LNS_DESCR_INDEX_T descrIndex;                     /* Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Descriptor value */
} CYBLE_LNS_DESCR_VALUE_T;


/***************************************
##Function Prototypes
***************************************/

void CyBle_LnsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_LNS_SERVER

CYBLE_API_RESULT_T CyBle_LnssSetCharacteristicValue(CYBLE_LNS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_LnssGetCharacteristicValue(CYBLE_LNS_CHAR_INDEX_T charIndex, 
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_LnssGetCharacteristicDescriptor(CYBLE_LNS_CHAR_INDEX_T charIndex, 
                                                        CYBLE_LNS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_LnssSendNotification(CYBLE_CONN_HANDLE_T connHandle, 
                                                CYBLE_LNS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_LnssSendIndication(CYBLE_CONN_HANDLE_T connHandle, 
                                                CYBLE_LNS_CHAR_INDEX_T charIndex,
                                                uint8 attrSize, uint8 *attrValue);
#endif /* CYBLE_LNS_SERVER */

#ifdef CYBLE_LNS_CLIENT

CYBLE_API_RESULT_T CyBle_LnscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_LNS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_LnscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
                                                        CYBLE_LNS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_LnscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_LNS_CHAR_INDEX_T charIndex,
                                                        CYBLE_LNS_DESCR_INDEX_T descrIndex,
                                                        uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_LnscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_LNS_CHAR_INDEX_T charIndex,
                                                        CYBLE_LNS_DESCR_INDEX_T descrIndex);
#endif /* CYBLE_LNS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_LnsInit(void);

#ifdef CYBLE_LNS_SERVER

void CyBle_LnssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
CYBLE_GATT_ERR_CODE_T CyBle_LnssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* CYBLE_LNS_SERVER */

#ifdef CYBLE_LNS_CLIENT

void CyBle_LnscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T * discCharInfo);
void CyBle_LnscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_LnscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_LnscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_LnscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_LnscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_LnscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_LNS_CLIENT */

/* Internal macros */
#ifdef CYBLE_LNS_SERVER
#define CYBLE_LNS_IS_PQ_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_lnss.charInfo[CYBLE_LNS_PQ].charHandle)
    
#define CYBLE_LNS_IS_CP_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_lnss.charInfo[CYBLE_LNS_CP].charHandle)

#define CYBLE_LNS_IS_NV_SUPPORTED\
    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_lnss.charInfo[CYBLE_LNS_NV].charHandle)
    
#define CYBLE_LNS_IS_PROCEDURE_IN_PROGRESS\
    (0u != (CYBLE_LNS_FLAG_PROCESS & cyBle_lnsFlag))
#endif /* CYBLE_LNS_SERVER */
/* DOM-IGNORE-END */


/***************************************
## External data references
***************************************/

#ifdef CYBLE_LNS_SERVER
    /* Location and Navigation Service GATT DB handles structure */
    extern const CYBLE_LNSS_T                                   cyBle_lnss;
    extern uint8 cyBle_lnsFlag;
#endif /* CYBLE_LNSC_SERVER */

#ifdef CYBLE_LNS_CLIENT
    /* Location and Navigation Service server's GATT DB handles structure */
    extern CYBLE_LNSC_T                                         cyBle_lnsc;
#endif /* CYBLE_LNS_CLIENT */

#endif /* CY_BLE_CYBLE_LNS_H */

/* [] END OF FILE */
