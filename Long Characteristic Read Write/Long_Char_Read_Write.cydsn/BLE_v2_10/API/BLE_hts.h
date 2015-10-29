/*******************************************************************************
File Name: CYBLE_hts.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for Health Thermometer Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_HTS_H)
#define CY_BLE_CYBLE_HTS_H

#include "`$INSTANCE_NAME`_gatt.h"


/*************************************** 
##Data Struct Definition
***************************************/

/* HTS Characteristic indexes */
typedef enum
{
    CYBLE_HTS_TEMP_MEASURE,                         /* Temperature Measurement characteristic index */
    CYBLE_HTS_TEMP_TYPE,                            /* Temperature Type characteristic index */
    CYBLE_HTS_INTERM_TEMP,                          /* Intermediate Temperature characteristic index*/
    CYBLE_HTS_MEASURE_INTERVAL,                     /* Measurement Interval characteristic index */
    CYBLE_HTS_CHAR_COUNT                            /* Total count of HTS characteristics */

}CYBLE_HTS_CHAR_INDEX_T;

/* HTS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_HTS_CCCD,                                  /* Client Characteristic Configuration descriptor index */
    CYBLE_HTS_VRD,                                   /* Valid Range descriptor index */
    CYBLE_HTS_DESCR_COUNT                            /* Total count of descriptors */
}CYBLE_HTS_DESCR_INDEX_T;

/* HTS Characteristic with descriptors */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                          /* Handle of characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_HTS_DESCR_COUNT];  /* Handle of descriptor */
} CYBLE_HTSS_CHAR_T;

/* Structure with Health Thermometer Service attribute handles */
typedef struct
{
    /* Health Thermometer Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /* Health Thermometer Service Characteristic handles */
    CYBLE_HTSS_CHAR_T charInfo[CYBLE_HTS_CHAR_COUNT];
} CYBLE_HTSS_T;


#ifdef CYBLE_HTS_CLIENT

/* HTS Characteristic with descriptors */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_HTS_DESCR_COUNT]; /* Handle of descriptor */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;                        /* Handle of Report characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;                          /* End handle of characteristic */
    uint8  properties;                                              /* Properties for value field */
} CYBLE_HTSC_CHAR_T;
    
/* Structure with discovered attributes information of Health Thermometer Service */
typedef struct
{
    /* Characteristics handles array */
    CYBLE_HTSC_CHAR_T charInfo[CYBLE_HTS_CHAR_COUNT];
} CYBLE_HTSC_T;

#endif /* CYBLE_HTS_CLIENT */

/* HTS Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_HTS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_HTS_CHAR_VALUE_T;

/* HTS Characteristic descriptor value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_HTS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_HTS_DESCR_INDEX_T descrIndex;                     /* Index of descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_HTS_DESCR_VALUE_T;


/* The IEEE-11073 FLOAT-Type is defined as a 32-bit value 
 with a 24-bit mantissa and an 8-bit exponent. */
typedef struct
{
  int8  exponent;                                            /* Base 10 exponent */
  int32 mantissa;                                            /* Mantissa, should be using only 24 bits */
} CYBLE_HTS_FLOAT32;


/*************************************** 
##API Constants
***************************************/

/* Valid Range descriptor length */
#define CYBLE_HTS_VRD_LEN                               (0x04u)


/* Temperature Type measurement indicates where the temperature was measured */
typedef enum
{
    CYBLE_HTS_TEMP_TYPE_ARMPIT = 0x01u,     /* Armpit */
    CYBLE_HTS_TEMP_TYPE_BODY,               /* Body (general) */
    CYBLE_HTS_TEMP_TYPE_EAR,                /* Ear (usually ear lobe) */
    CYBLE_HTS_TEMP_TYPE_FINGER,             /* Finger */
    CYBLE_HTS_TEMP_TYPE_GI_TRACT,           /* Gastro-intestinal Tract */
    CYBLE_HTS_TEMP_TYPE_MOUTH,              /* Mouth */
    CYBLE_HTS_TEMP_TYPE_RECTUM,             /* Rectum */
    CYBLE_HTS_TEMP_TYPE_TOE,                /* Toe */
    CYBLE_HTS_TEMP_TYPE_TYMPANUM            /* Tympanum (ear drum) */
}CYBLE_HTS_TEMP_TYPE_T;

/* Health Thermometer measurement flag bits */
#define CYBLE_HTS_MEAS_FLAG_TEMP_UNITS_BIT              (0x01u << 0u)
#define CYBLE_HTS_MEAS_FLAG_TIME_STAMP_BIT              (0x01u << 1u)
#define CYBLE_HTS_MEAS_FLAG_TEMP_TYPE_BIT               (0x01u << 2u)


/***************************************
##Function Prototypes
***************************************/

void CyBle_HtsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_HTS_SERVER

CYBLE_API_RESULT_T CyBle_HtssSetCharacteristicValue(CYBLE_HTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HtssGetCharacteristicValue(CYBLE_HTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HtssSetCharacteristicDescriptor(CYBLE_HTS_CHAR_INDEX_T charIndex,
    CYBLE_HTS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HtssGetCharacteristicDescriptor(CYBLE_HTS_CHAR_INDEX_T charIndex,
    CYBLE_HTS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HtssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_HTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HtssSendIndication(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_HTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_HTS_SERVER */

#ifdef CYBLE_HTS_CLIENT

CYBLE_API_RESULT_T CyBle_HtscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_HTS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HtscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_HTS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_HtscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_HTS_CHAR_INDEX_T charIndex, CYBLE_HTS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HtscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_HTS_CHAR_INDEX_T charIndex, CYBLE_HTS_DESCR_INDEX_T descrIndex);

#endif /* (CYBLE_HTS_CLIENT) */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_HtsInit(void);

#ifdef CYBLE_HTS_SERVER
    
CYBLE_GATT_DB_ATTR_HANDLE_T CyBle_HtssGetCharacteristicDescriptorHandle(
    CYBLE_HTS_CHAR_INDEX_T charIndex, CYBLE_HTS_DESCR_INDEX_T descrIndex);
CYBLE_GATT_ERR_CODE_T CyBle_HtssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_HtssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);

#endif /* CYBLE_HTS_SERVER */

#ifdef CYBLE_HTS_CLIENT

void CyBle_HtscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_HtscDiscoverCharDescriptorsEventHandler(CYBLE_HTS_CHAR_INDEX_T discoveryCharIndex, 
                                                   CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_HtscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_HtscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_HtscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_HtscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_HtscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_HTS_CLIENT) */


/***************************************
##Macro Functions
***************************************/

#ifdef CYBLE_HTS_CLIENT

#define CyBle_HtscCheckCharHandle(handle)\
    do {\
        if((handle).valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)\
        {\
            (handle).valueHandle = discCharInfo->valueHandle;\
            (handle).properties = discCharInfo->properties;\
        }\
        else\
        {\
            CyBle_ApplCallback((uint32)CYBLE_EVT_GATTC_CHAR_DUPLICATION,\
                                      &(discCharInfo->uuid.uuid16));\
        }\
    } while(0)

#define CyBle_HtscGetCharacteristicValueHandle(charIndex)    \
    (((charIndex) >= CYBLE_HTS_CHAR_COUNT) ? \
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE : \
            cyBle_htsc.charInfo[charIndex].valueHandle)

#define CyBle_HtscGetCharacteristicDescriptorHandle(charIndex, descrIndex)    \
    ((((charIndex) >= CYBLE_HTS_CHAR_COUNT) || ((descrIndex) >= CYBLE_HTS_DESCR_COUNT)) ? \
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE : \
            cyBle_htsc.charInfo[charIndex].descrHandle[descrIndex])

#endif /* (CYBLE_HTS_CLIENT) */
/* DOM-IGNORE-END */


/***************************************
##External data references
***************************************/

#ifdef CYBLE_HTS_SERVER

extern const CYBLE_HTSS_T cyBle_htss;

#endif /* CYBLE_HTS_SERVER */

#ifdef CYBLE_HTS_CLIENT

extern CYBLE_HTSC_T cyBle_htsc;

#endif /* (CYBLE_HTS_CLIENT) */


#endif /* CY_BLE_CYBLE_HTS_H  */


/* [] END OF FILE */
