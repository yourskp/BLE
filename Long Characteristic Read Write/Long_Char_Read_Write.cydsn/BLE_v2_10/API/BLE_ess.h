/*******************************************************************************
File Name: CYBLE_ess.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for the Environmental Sensing
 Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_ESS_H)
#define CY_BLE_CYBLE_ESS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Conditional Compilation Parameters
***************************************/
#ifdef CYBLE_ESS_CLIENT

#define CYBLE_ES_TOTAL_CHAR_COUNT            (`$EsscCharCount`)

#endif /* CYBLE_ESS_CLIENT */

#define CYBLE_ESS_3BYTES_LENGTH              (0x03u)


/***************************************
##Data Struct Definition
***************************************/

/* ESS Characteristic indexes */
typedef enum
{
    CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED,          /* Descriptor Value Changed Characteristic index */
    CYBLE_ESS_APPARENT_WIND_DIR,                 /* Apparent Wind Direction Characteristic index */
    CYBLE_ESS_APPARENT_WIND_SPEED,               /* Apparent Wind Speed Characteristic index */
    CYBLE_ESS_DEW_POINT,                         /* Dew Point Characteristic index */
    CYBLE_ESS_ELEVATION,                         /* Elevation Characteristic index */
    CYBLE_ESS_GUST_FACTOR,                       /* Gust Factor Characteristic index */
    CYBLE_ESS_HEAT_INDEX,                        /* Heat Index Characteristic index */
    CYBLE_ESS_HUMIDITY,                          /* Humidity Characteristic index */
    CYBLE_ESS_IRRADIANCE,                        /* Irradiance Characteristic index */
    CYBLE_ESS_POLLEN_CONCENTRATION,              /* Pollen Concentration Characteristic index */
    CYBLE_ESS_RAINFALL,                          /* Rainfall Characteristic index */
    CYBLE_ESS_PRESSURE,                          /* Pressure Characteristic index */
    CYBLE_ESS_TEMPERATURE,                       /* Temperature Characteristic index */
    CYBLE_ESS_TRUE_WIND_DIR,                     /* True Wind Direction Characteristic index */
    CYBLE_ESS_TRUE_WIND_SPEED,                   /* True Wind Speed Characteristic index */
    CYBLE_ESS_UV_INDEX,                          /* UV Index Characteristic index */
    CYBLE_ESS_WIND_CHILL,                        /* Wind Chill Characteristic index */
    CYBLE_ESS_BAROMETRIC_PRESSURE_TREND,         /* Barometric Pressure trend Characteristic index */
    CYBLE_ESS_MAGNETIC_DECLINATION,              /* Magnetic Declination Characteristic index */
    CYBLE_ESS_MAGNETIC_FLUX_DENSITY_2D,          /* Magnetic Flux Density 2D Characteristic index */
    CYBLE_ESS_MAGNETIC_FLUX_DENSITY_3D,          /* Magnetic Flux Density 3D Characteristic index */
    CYBLE_ESS_CHAR_COUNT                         /* Total count of ESS characteristics */
}CYBLE_ESS_CHAR_INDEX_T;

/* ESS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_ESS_CCCD,                              /* Client Characteristic Configuration Descriptor index */
    CYBLE_ESS_CHAR_EXTENDED_PROPERTIES,          /* Characteristic Extended Properties Descriptor index */
    CYBLE_ESS_ES_MEASUREMENT_DESCR,              /* ES Measurement Descriptor index */
    CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1,        /* ES Trigger Settings Descriptor #1 index */
    CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR2,        /* ES Trigger Settings Descriptor #2 index */
    CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR3,        /* ES Trigger Settings Descriptor #3 index */
    CYBLE_ESS_ES_CONFIG_DESCR,                   /* ES Configuration Descriptor index */
    CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR,       /* Characteristic User Description Descriptor index */
    CYBLE_ESS_VRD,                               /* Valid Range Descriptor index */
    CYBLE_ESS_DESCR_COUNT                        /* Total count of descriptors */
}CYBLE_ESS_DESCR_INDEX_T;

#ifdef CYBLE_ESS_SERVER

/* ESS Characteristic with descriptors */
typedef struct
{
    /* Handles of Characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;

    /* Array of Descriptor handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_ESS_DESCR_COUNT];
} CYBLE_ESSS_CHAR_T;

/* Structure to hold pointer to CYBLE_ESSS_CHAR_T */
typedef struct
{
    /* Pointer to CYBLE_ESSS_CHAR_T which holds information about specific
    * ES Characteristic
    */
    CYBLE_ESSS_CHAR_T *charInfoPtr;
} CYBLE_ESSS_CHAR_INFO_PTR_T;

/* Structure with Environmental Sensing Service attribute handles */
typedef struct
{
    /* Environmental Sensing Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;

    /* Environmental Sensing Service Array with pointers to
    * Characteristic handles.
    */
    CYBLE_ESSS_CHAR_INFO_PTR_T charInfoAddr[CYBLE_ESS_CHAR_COUNT];
} CYBLE_ESSS_T;

#endif /* CYBLE_ESS_SERVER */

#ifdef CYBLE_ESS_CLIENT

/* ESS Characteristic with descriptors */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;                        /* Handle of characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;                          /* End handle of characteristic */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_ESS_DESCR_COUNT]; /* Array of Descriptor handles */
    uint8  properties;                                              /* Properties for value field */
} CYBLE_ESSC_CHAR_T;

/* Structure to hold pointer to CYBLE_ESSC_CHAR_T */
typedef struct
{
    /* Pointer to CYBLE_ESSC_CHAR_T which holds information about specific
    * ES Characteristic.
    */
    CYBLE_ESSC_CHAR_T *charInfoPtr;
} CYBLE_ESSC_CHAR_INFO_PTR_T;

/* Structure with discovered attributes information of Health Thermometer Service */
typedef struct
{
    /* Environmental Sensing Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;

    /* Environmental Sensing Service Array with pointers to
    * characteristic information.
    */
    CYBLE_ESSC_CHAR_INFO_PTR_T charInfoAddr[CYBLE_ESS_CHAR_COUNT];
} CYBLE_ESSC_T;

#endif /* CYBLE_ESS_CLIENT */

/* ESS Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_ESS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    uint8 charInstance;                                     /* Instance of specific service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_ESS_CHAR_VALUE_T;

/* ESS Characteristic descriptor value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_ESS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    uint8 charInstance;                                     /* Instance of specific service characteristic */
    CYBLE_ESS_DESCR_INDEX_T descrIndex;                     /* Index of descriptor */
    CYBLE_GATT_ERR_CODE_T gattErrorCode;                       /* Error code received from application (optional) */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_ESS_DESCR_VALUE_T;


/***************************************
##API Constants
***************************************/
/* ESS specific GATT errors  */
#define CYBLE_GATT_ERR_WRITE_REQ_REJECTED                   (0x80u)
#define CYBLE_GATT_ERR_CONDITION_NOT_SUPPORTED              (0x81u)

#define CYBLE_ESS_UNSIGNED_TYPE                             (0u)
#define CYBLE_ESS_SIGNED_TYPE                               (1u)

/* ES Trigger Descriptor "Condition" field constants */
#define CYBLE_ESS_TRIG_TRIGGER_INACTIVE                     (0x00u)
#define CYBLE_ESS_TRIG_USE_FIXED_TIME_INTERVAL              (0x01u)
#define CYBLE_ESS_TRIG_NO_LESS_THEN_TIME_INTERVAL           (0x02u)
#define CYBLE_ESS_TRIG_WHEN_CHANGED                         (0x03u)
#define CYBLE_ESS_TRIG_WHILE_LESS_THAN                      (0x04u)
#define CYBLE_ESS_TRIG_WHILE_LESS_OR_EQUAL                  (0x05u)
#define CYBLE_ESS_TRIG_WHILE_GREATER_THAN                   (0x06u)
#define CYBLE_ESS_TRIG_WHILE_GREATER_OR_EQUAL               (0x07u)
#define CYBLE_ESS_TRIG_WHILE_EQUAL_TO                       (0x08u)
#define CYBLE_ESS_TRIG_WHILE_EQUAL_NOT_TO                   (0x09u)

/* ES Measurement Descriptor "Sampling Function" field constants */
#define CYBLE_ESS_MEASURE_SF_UNSPECIFIED                    (0x00u)
#define CYBLE_ESS_MEASURE_SF_INSTANTANEOUS                  (0x01u)
#define CYBLE_ESS_MEASURE_SF_ARITHM_MEAN                    (0x02u)
#define CYBLE_ESS_MEASURE_SF_RMS                            (0x03u)
#define CYBLE_ESS_MEASURE_SF_MAXIMUM                        (0x04u)
#define CYBLE_ESS_MEASURE_SF_MINIMUM                        (0x05u)
#define CYBLE_ESS_MEASURE_SF_ACCUMULATED                    (0x06u)
#define CYBLE_ESS_MEASURE_SF_COUNT                          (0x07u)

/* ES Measurement Descriptor "Application" field constants */
#define CYBLE_ESS_MEASURE_APP_UNSPECIFIED                   (0x00u)
#define CYBLE_ESS_MEASURE_APP_AIR                           (0x01u)
#define CYBLE_ESS_MEASURE_APP_WATER                         (0x02u)
#define CYBLE_ESS_MEASURE_APP_BAROMETRIC                    (0x03u)
#define CYBLE_ESS_MEASURE_APP_SOIL                          (0x04u)
#define CYBLE_ESS_MEASURE_APP_INFRARED                      (0x05u)
#define CYBLE_ESS_MEASURE_APP_MAP_DATABASE                  (0x06u)
#define CYBLE_ESS_MEASURE_APP_BAR_ELEVATION_SOURCE          (0x07u)
#define CYBLE_ESS_MEASURE_APP_GPS_ELEVATION_SOURCE          (0x08u)
#define CYBLE_ESS_MEASURE_APP_GPS_MAP_ELEVATION_SOURCE      (0x09u)
#define CYBLE_ESS_MEASURE_APP_VER_ELEVATION_SOURCE          (0x0Au)
#define CYBLE_ESS_MEASURE_APP_ONSHORE                       (0x0Bu)
#define CYBLE_ESS_MEASURE_APP_ONBOARD_VES_VEH               (0x0Cu)
#define CYBLE_ESS_MEASURE_APP_FRONT                         (0x0Du)
#define CYBLE_ESS_MEASURE_APP_BACK_REAR                     (0x0Eu)
#define CYBLE_ESS_MEASURE_APP_UPPER                         (0x0Fu)
#define CYBLE_ESS_MEASURE_APP_LOWER                         (0x10u)
#define CYBLE_ESS_MEASURE_APP_PRIMARY                       (0x11u)
#define CYBLE_ESS_MEASURE_APP_SECONDARY                     (0x12u)
#define CYBLE_ESS_MEASURE_APP_OUTDOOR                       (0x13u)
#define CYBLE_ESS_MEASURE_APP_INDOOR                        (0x14u)
#define CYBLE_ESS_MEASURE_APP_TOP                           (0x15u)
#define CYBLE_ESS_MEASURE_APP_BOTTOM                        (0x16u)
#define CYBLE_ESS_MEASURE_APP_MAIN                          (0x17u)
#define CYBLE_ESS_MEASURE_APP_BACKUP                        (0x18u)
#define CYBLE_ESS_MEASURE_APP_AUXILIARY                     (0x19u)
#define CYBLE_ESS_MEASURE_APP_SUPLEMENTARY                  (0x1Au)
#define CYBLE_ESS_MEASURE_APP_INSIDE                        (0x1Bu)
#define CYBLE_ESS_MEASURE_APP_OUTSIDE                       (0x1Cu)
#define CYBLE_ESS_MEASURE_APP_LEFT                          (0x1Du)
#define CYBLE_ESS_MEASURE_APP_RIGHT                         (0x1Eu)
#define CYBLE_ESS_MEASURE_APP_INTERNAL                      (0x1Fu)
#define CYBLE_ESS_MEASURE_APP_EXTERNAL                      (0x20u)
#define CYBLE_ESS_MEASURE_APP_SOLAR                         (0x21u)

/* ES Configuration Descriptor Trigger Logic value constants */
#define CYBLE_ESS_CONF_BOOLEAN_AND                          (0x00u)
#define CYBLE_ESS_CONF_BOOLEAN_OR                           (0x01u)

/* Descriptor Value Changed Characteristic Flags */
#define CYBLE_ESS_VALUE_CHANGE_SOURCE_CLIENT                (0x01u)
#define CYBLE_ESS_VALUE_CHANGE_ES_TRIGGER                   (0x02u)
#define CYBLE_ESS_VALUE_CHANGE_ES_CONFIG                    (0x04u)
#define CYBLE_ESS_VALUE_CHANGE_ES_MEASUREMENT               (0x08u)
#define CYBLE_ESS_VALUE_CHANGE_USER_DESCRIPTION             (0x10u)

/* Internal Constants */
/* DOM-IGNORE-BEGIN */
#define CYBLE_ESS_U8_SIGN_BIT                               (0x00000080ul)
#define CYBLE_ESS_U16_SIGN_BIT                              (0x00008000ul)
#define CYBLE_ESS_U24_SIGN_BIT                              (0x00800000ul)

#define CYBLE_ESS_STATE_0                                   (0x00u)
#define CYBLE_ESS_STATE_1                                   (0x01u)
#define CYBLE_ESS_STATE_2                                   (0x02u)
#define CYBLE_ESS_STATE_3                                   (0x03u)
#define CYBLE_ESS_STATE_4                                   (0x04u)
#define CYBLE_ESS_STATE_5                                   (0x05u)
#define CYBLE_ESS_STATE_6                                   (0x06u)
#define CYBLE_ESS_STATE_7                                   (0x07u)

#define CYBLE_ESS_2BYTES_LENGTH                             (0x02u)
#define CYBLE_ESS_4BYTES_LENGTH                             (0x04u)
#define CYBLE_ESS_6BYTES_LENGTH                             (0x06u)
#define CYBLE_ESS_8BYTES_LENGTH                             (0x08u)

#define CYBLE_ESS_UNDEFINED_ITEM                            (0x00u)
#define CYBLE_ESS_CHARACTERISTIC_ITEM                       (0x01u)
#define CYBLE_ESS_DESCRIPTOR_ITEM                           (0x02u)

#if(CYBLE_GAP_ROLE_PERIPHERAL)
#define CYBLE_ESS_SERVICE_DATA_AD_TYPE                      (0x16u)
#define CYBLE_ESS_SERVICE_DATA_LENGTH                       (0x05u)
#endif /* (CYBLE_GAP_ROLE_PERIPHERAL) */

/* DOM-IGNORE-END */


/***************************************
##Function Prototypes
***************************************/

void CyBle_EssRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_ESS_SERVER

#if(CYBLE_GAP_ROLE_PERIPHERAL)

CYBLE_API_RESULT_T CyBle_EsssSetChangeIndex(uint16 essIndex);

#endif /* (CYBLE_GAP_ROLE_PERIPHERAL) */

CYBLE_API_RESULT_T CyBle_EsssSetCharacteristicValue(CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_EsssGetCharacteristicValue(CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_EsssSetCharacteristicDescriptor(CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex, uint16 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_EsssGetCharacteristicDescriptor(CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex, uint16 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_EsssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_EsssSendIndication(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, uint8 attrSize, uint8 *attrValue);

#endif /* CYBLE_ESS_SERVER */

#ifdef CYBLE_ESS_CLIENT

CYBLE_API_RESULT_T CyBle_EsscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance, uint8 attrSize, uint8 * attrValue);
CYBLE_API_RESULT_T CyBle_EsscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance);
CYBLE_API_RESULT_T CyBle_EsscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex, uint8 attrSize,
        uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_EsscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex);
CYBLE_API_RESULT_T CyBle_EsscSetLongCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex, uint16 attrSize,
        uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_EsscGetLongCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex, uint16 attrSize,
        uint8 *attrValue);

#endif /* CYBLE_ESS_CLIENT */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_EssInit(void);

#ifdef CYBLE_ESS_SERVER
    
uint32 CyBle_EssGet24ByPtr(const uint8 ptr[]);
uint32 CyBle_EssGet32ByPtr(const uint8 ptr[]);
CYBLE_GATT_ERR_CODE_T CyBle_EssHandleRangeCheck(CYBLE_ESS_CHAR_INDEX_T charIndex,  uint8 charInstance, uint16 length,
    uint8 type, const uint8 pValue[]);
CYBLE_GATT_ERR_CODE_T CyBle_EsssCheckIfInRange(uint32 min, uint32 max, uint32 value, uint8 state);
CYBLE_GATT_ERR_CODE_T CyBle_EsssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_EsssPrepareWriteRequestEventHandler(CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam);
void CyBle_EsssExecuteWriteRequestEventHandler(CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam);
void CyBle_EsssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);

#endif /* CYBLE_ESS_SERVER */

#ifdef CYBLE_ESS_CLIENT

void CyBle_EssClearClientDiscInfo(void);
void CyBle_EsscDiscoverCharacteristicsEventHandler(const CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_EsscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_EsscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_EsscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_EsscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_EsscReadLongRespEventHandler(const CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_EsscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_EsscExecuteWriteResponseEventHandler(const CYBLE_GATTC_EXEC_WRITE_RSP_T *eventParam);
void CyBle_EsscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* CYBLE_ESS_CLIENT */
/* DOM-IGNORE-END */


/***************************************
##Macro Functions
***************************************/

#define CyBle_EssGet16ByPtr                              CyBle_Get16ByPtr
#define CYBLE_ESS_IS_NEGATIVE(value, sign)               (((((uint32) (value)) & (sign)) == (sign)) ? (1u) : (0u))


/***************************************
##External data references
***************************************/

#ifdef CYBLE_ESS_SERVER

extern const CYBLE_ESSS_T cyBle_esss;
extern uint8 cyBle_esssCharInstances[CYBLE_ESS_CHAR_COUNT];

#endif /* CYBLE_ESS_SERVER */

#ifdef CYBLE_ESS_CLIENT

extern CYBLE_ESSC_T cyBle_essc;

extern uint8 activeCharIndex;
extern uint8 activeCharInstance;
extern uint8 prevCharInstIndex;
extern uint8 cyBle_esscCharInstances[0x15u];

#endif /* (CYBLE_ESS_CLIENT) */

#endif /* CY_BLE_CYBLE_ESS_H */


/* [] END OF FILE */
