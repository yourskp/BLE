/*******************************************************************************
File Name: CYBLE_ndcs.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for Next DST Change
 Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BLE_CYBLE_NDCS_H)
#define CY_BLE_CYBLE_NDCS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Data Struct Definition
***************************************/

/* Characteristic indexes */
typedef enum
{
    CYBLE_NDCS_TIME_WITH_DST,                           /* Time with DST Characteristic index */
    CYBLE_NDCS_CHAR_COUNT                               /* Total count of NDCS Characteristics */
} CYBLE_NDCS_CHAR_INDEX_T;

/* Next DST Change Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                     /* Peer device handle */
    CYBLE_NDCS_CHAR_INDEX_T charIndex;                  /* Index of Next DST Change Service Characteristic */
    CYBLE_GATT_VALUE_T *value;                          /* Characteristic value */
} CYBLE_NDCS_CHAR_VALUE_T;

#if defined(CYBLE_NDCS_SERVER)

/* Structure with Device Information Service atribute handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;          /* Handle of the Next DST Change Service */
    CYBLE_GATT_DB_ATTR_HANDLE_T timeWithDst;            /* Handle of the Time with DST Characteristic */
} CYBLE_NDCSS_T;

#endif /* CYBLE_NDCS_SERVER */

#if defined(CYBLE_NDCS_CLIENT)

/* Structure with discovered attributes information of Next DST Change Service */
typedef struct
{
    /* Characteristic handle and properties */
    CYBLE_SRVR_CHAR_INFO_T charInfo[CYBLE_NDCS_CHAR_COUNT];
} CYBLE_NDCSC_T;

#endif /* CYBLE_NDCS_CLIENT */


/***************************************
##Function Prototypes
***************************************/

void CyBle_NdcsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#if defined(CYBLE_NDCS_SERVER)

CYBLE_API_RESULT_T CyBle_NdcssSetCharacteristicValue(CYBLE_NDCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_NdcssGetCharacteristicValue(CYBLE_NDCS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_NDCS_SERVER */

#if defined(CYBLE_NDCS_CLIENT)

CYBLE_API_RESULT_T CyBle_NdcscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_NDCS_CHAR_INDEX_T charIndex);

#endif /* (CYBLE_NDCS_CLIENT) */


/***************************************
##Private Function Prototypes
***************************************/
/* DOM-IGNORE-BEGIN */
void CyBle_NdcsInit(void);

#if defined(CYBLE_NDCS_CLIENT)

void CyBle_NdcscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_NdcscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_NDCS_CLIENT) */
/* DOM-IGNORE-END */


/***************************************
##External data references
***************************************/

#if defined(CYBLE_NDCS_SERVER)
extern const CYBLE_NDCSS_T cyBle_ndcss;
#endif /* CYBLE_NDCS_SERVER */

#if defined(CYBLE_NDCS_CLIENT)
extern CYBLE_NDCSC_T cyBle_ndcsc;
#endif /* (CYBLE_NDCS_CLIENT) */


#endif /* CY_BLE_CYBLE_NDCS_H */


/* [] END OF FILE */
