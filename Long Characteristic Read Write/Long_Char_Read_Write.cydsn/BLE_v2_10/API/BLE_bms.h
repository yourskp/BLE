/*******************************************************************************
File Name: CYBLE_bms.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in the 
 Bond Management Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BLE_CYBLE_BMS_H)
#define CY_BLE_CYBLE_BMS_H

#include "`$INSTANCE_NAME`_gatt.h"
    

/***************************************    
##Data Types
***************************************/

/* Service Characteristics indexes */
typedef enum
{
    CYBLE_BMS_BMCP,        /* Bond Management Control Point characteristic index */
	CYBLE_BMS_BMFT,        /* Bond Management Feature characteristic index */
    CYBLE_BMS_CHAR_COUNT   /* Total count of BMS characteristics */
}CYBLE_BMS_CHAR_INDEX_T;

/* Service Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_BMS_CEPD,           /* Characteristic Extended Properties descriptor index */
    CYBLE_BMS_DESCR_COUNT     /* Total count of CGMS descriptors */
}CYBLE_BMS_DESCR_INDEX_T;

#ifdef CYBLE_BMS_SERVER
/* Characteristic with descriptors type */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                             /* Handle of Characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_BMS_DESCR_COUNT];    /* Handles of Descriptors */
} CYBLE_BMSS_CHAR_T;
    
/* Structure with Service attribute handles */
typedef struct
{
    /* Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;                      
    
    /* Service characteristics info array */
    CYBLE_BMSS_CHAR_T charInfo[CYBLE_BMS_CHAR_COUNT];
} CYBLE_BMSS_T;
#endif /* CYBLE_BMS_SERVER */

#ifdef CYBLE_BMS_CLIENT
/* Client Characteristic structure type */
typedef struct
{
    /* Properties for value field */
    uint8  properties;

    /* Handle of Server database attribute value entry */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;
    
    /* Characteristics descriptors handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_BMS_DESCR_COUNT];
    
    /* Characteristic End Handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;
}CYBLE_BMSC_CHAR_T;

/* Service structure type */
typedef struct
{
    /* Characteristics handle + properties array */
    CYBLE_BMSC_CHAR_T charInfo[CYBLE_BMS_CHAR_COUNT];
}CYBLE_BMSC_T;

#endif /* CYBLE_BMS_CLIENT */

/* Service Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                        /* Peer device handle */
    CYBLE_BMS_CHAR_INDEX_T charIndex;                      /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                             /* Characteristic value */
    CYBLE_GATT_ERR_CODE_T gattErrorCode;                   /* GATT error code for checking the authorization code */
} CYBLE_BMS_CHAR_VALUE_T;

/* Service Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_BMS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_BMS_DESCR_INDEX_T descrIndex;                     /* Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Descriptor value */
} CYBLE_BMS_DESCR_VALUE_T;


/***************************************
##API Constants
***************************************/
    
/* Bond Management Control Point characteristic supports Op Codes */
#define CYBLE_BMS_BMCP_OPC_RD (0x03u) /* Delete bond of requesting device (LE transport only). */
#define CYBLE_BMS_BMCP_OPC_AB (0x06u) /* Delete all bonds on server (LE transport only). */
#define CYBLE_BMS_BMCP_OPC_BA (0x09u) /* Delete all but the active bond on server (LE transport only). */

/* Bond Management Control Point characteristic supports Op Codes */
#define CYBLE_BMS_BMFT_RD (0x00000010u) /* Delete bond  of current connection (LE transport only) supported. */
#define CYBLE_BMS_BMFT_RC (0x00000020u) /* Authorization Code required for feature above. */
#define CYBLE_BMS_BMFT_AB (0x00000400u) /* Remove all bonds on server (LE transport only) supported. */
#define CYBLE_BMS_BMFT_AC (0x00000800u) /* Authorization Code required for feature above. */
#define CYBLE_BMS_BMFT_BA (0x00010000u) /* Remove all but the active bond on server (LE transport only) supported. */
#define CYBLE_BMS_BMFT_BC (0x00020000u) /* Authorization Code required for feature above. */

#define CYBLE_BMS_FLAG_LW (0x01u)       /* Long Write Procedure */


/***************************************
##Function Prototypes
***************************************/

void CyBle_BmsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_BMS_SERVER

CYBLE_API_RESULT_T CyBle_BmssSetCharacteristicValue(CYBLE_BMS_CHAR_INDEX_T charIndex,
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BmssGetCharacteristicValue(CYBLE_BMS_CHAR_INDEX_T charIndex, 
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BmssSetCharacteristicDescriptor(CYBLE_BMS_CHAR_INDEX_T charIndex,
                                                    CYBLE_BMS_DESCR_INDEX_T descrIndex,
                                                    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BmssGetCharacteristicDescriptor(CYBLE_BMS_CHAR_INDEX_T charIndex,
                                                    CYBLE_BMS_DESCR_INDEX_T descrIndex,
                                                    uint8 attrSize, uint8 *attrValue);
#endif /* CYBLE_BMS_SERVER */

#ifdef CYBLE_BMS_CLIENT

CYBLE_API_RESULT_T CyBle_BmscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
                                                        CYBLE_BMS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_BmscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_BMS_CHAR_INDEX_T charIndex,
                                                            uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BmscReliableWriteCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_BMS_CHAR_INDEX_T charIndex,
                                                            uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BmscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                        CYBLE_BMS_CHAR_INDEX_T charIndex, 
                                                            CYBLE_BMS_DESCR_INDEX_T descrIndex);
#endif /* CYBLE_BMS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_BmsInit(void);

#ifdef CYBLE_BMS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_BmssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_BmssPrepareWriteRequestEventHandler(CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam);
void CyBle_BmssExecuteWriteRequestEventHandler(CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam);

#endif /* CYBLE_BMS_SERVER */

#ifdef CYBLE_BMS_CLIENT

void CyBle_BmscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_BmscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_BmscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_BmscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_BmscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);
void CyBle_BmscExecuteWriteResponseEventHandler(const CYBLE_GATTC_EXEC_WRITE_RSP_T *eventParam);

#endif /* CYBLE_BMS_CLIENT */

/* DOM-IGNORE-END */


/***************************************
## External data references
***************************************/

#ifdef CYBLE_BMS_SERVER
    /* CGM Service GATT DB handles structure */
    extern const CYBLE_BMSS_T  cyBle_bmss;
#endif /* CYBLE_BMS_SERVER */

#ifdef CYBLE_BMS_CLIENT
    /* CGM Service server's GATT DB handles structure */
    extern CYBLE_BMSC_T        cyBle_bmsc;
#endif /* CYBLE_BMS_CLIENT */

#endif /* CY_BLE_CYBLE_BMS_H */

/* [] END OF FILE */
