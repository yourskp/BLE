/*
File Name: CYBLE_cps.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for Cycling Power Service.


Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*/


#if !defined(CY_BLE_CYBLE_CPS_H)
#define CY_BLE_CYBLE_CPS_H

#include "`$INSTANCE_NAME`_gatt.h"


/* ##Data Struct Definition */

/* Characteristic indexes */
typedef enum
{
    CYBLE_CPS_POWER_MEASURE,                        /* Cycling Power Measurement characteristic index */ 
    CYBLE_CPS_POWER_FEATURE,                        /* Cycling Power Feature characteristic index */ 
    CYBLE_CPS_SENSOR_LOCATION,                      /* Sensor Location characteristic index */ 
    CYBLE_CPS_POWER_VECTOR,                         /* Cycling Power Vector characteristic index */ 
    CYBLE_CPS_POWER_CP,                             /* Cycling Power Control Point characteristic index */ 
    CYBLE_CPS_CHAR_COUNT                            /* Total count of CPS characteristics */
}CYBLE_CPS_CHAR_INDEX_T;

/* Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_CPS_CCCD,                                  /* Client Characteristic Configuration descriptor index */
    CYBLE_CPS_SCCD,                                  /* Handle of the Server Characteristic Configuration descriptor */ 
    CYBLE_CPS_DESCR_COUNT                            /* Total count of descriptors */
}CYBLE_CPS_DESCR_INDEX_T;

/* Characteristic with descriptors */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle;                          /* Handle of characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_CPS_DESCR_COUNT];  /* Handle of descriptor */
} CYBLE_CPSS_CHAR_T;

/* Structure with Cycling Power Service attribute handles */
typedef struct
{
    /* Cycling Power Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /* Cycling Power Service Characteristic handles */
    CYBLE_CPSS_CHAR_T charInfo[CYBLE_CPS_CHAR_COUNT];
} CYBLE_CPSS_T;


#ifdef CYBLE_CPS_CLIENT

/* Characteristic with descriptors */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T descrHandle[CYBLE_CPS_DESCR_COUNT]; /* Handles of descriptors */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;                        /* Handle of characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;                          /* End handle of characteristic */
    uint8  properties;                                              /* Properties for value field */
} CYBLE_CPSC_CHAR_T;
    
/* Structure with discovered attributes information of Cycling Power Service */
typedef struct
{
    /* Characteristics handles array */
    CYBLE_CPSC_CHAR_T charInfo[CYBLE_CPS_CHAR_COUNT];
} CYBLE_CPSC_T;

#endif /* CYBLE_CPS_CLIENT */

typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_CPS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_CPS_CHAR_VALUE_T;

typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /* Peer device handle */
    CYBLE_CPS_CHAR_INDEX_T charIndex;                       /* Index of service characteristic */
    CYBLE_CPS_DESCR_INDEX_T descrIndex;                     /* Index of descriptor */
    CYBLE_GATT_VALUE_T *value;                              /* Characteristic value */
} CYBLE_CPS_DESCR_VALUE_T;

CYBLE_CYPACKED typedef struct
{
    uint16 year;
    uint8  month;
    uint8  day;
    uint8  hours;
    uint8  minutes;
    uint8  seconds;
}CYBLE_CYPACKED_ATTR CYBLE_CPS_DATE_TIME_T;

CYBLE_CYPACKED typedef struct
{
    uint16 crankLength;                             /* In millimeters with a resolution of 1/2 millimeter */
    uint16 chainLength;                             /* In millimeters with a resolution of 1 millimeter */
    uint16 chainWeight;                             /* In grams with a resolution of 1 gram */
    uint16 spanLength;                              /* In millimeters with a resolution of 1 millimeter */
    CYBLE_CPS_DATE_TIME_T factoryCalibrationDate;   /* Use the same format as the Date Time characteristic */
    uint8 samplingRate;                             /* In Hertz with a resolution of 1 Hertz */
    int16 offsetCompensation;                       /* Either the raw force in Newton or the raw torque in 1/32 Newton 
                                                meter based on the server capabilities. 0xFFFF means "Not Available" */
}CYBLE_CYPACKED_ATTR CYBLE_CPS_CP_ADJUSTMENT_T;

/* ##API Constants */

/* Control Point Procedure Timeout */
#define CYBLE_CPS_CP_PROCEDURE_TIMEOUT               (30u)          /* Seconds */

#define CYBLE_CPS_SAMLING_RATE_DEFAULT               (25u)          /* In Hertz with a resolution of 1 Hertz */

/* ConnIntv = 1/SampleRate(Hz)*1000ms/1.25ms(1 frame is 1.25 ms) = 800 / SampleRate */
#define CYBLE_CPS_SAMLING_RATE_TO_CONN_INTV          (800u)         

/* Server state flags */
#define CYBLE_CPSS_FLAG_CP_PROCEDURE_IN_PROGRESS     (0x01u)
#define CYBLE_CPSS_FLAG_PV_PROCEDURE_IN_PROGRESS     (0x02u)
#define CYBLE_CPSS_FLAG_BROADCAST_IN_PROGRESS        (0x04u)
#define CYBLE_CPSS_FLAG_START_BROADCAST              (0x08u)
#define CYBLE_CPSS_FLAG_STOP_BROADCAST               (0x10u)

/* Client state flags */
#define CYBLE_CPSC_FLAG_CP_PROCEDURE_IN_PROGRESS     (0x01u)
#define CYBLE_CPSC_FLAG_OBSERVE_IN_PROGRESS          (0x02u)
#define CYBLE_CPSC_FLAG_START_OBSERVE                (0x04u)
#define CYBLE_CPSC_FLAG_STOP_OBSERVE                 (0x08u)

/* Cycling Power Feature bits */
#define CYBLE_CPS_CPF_PEDAL_BIT                     (0x01u << 0u)   /* Pedal Power Balance Supported */
#define CYBLE_CPS_CPF_TORQUE_BIT                    (0x01u << 1u)   /* Accumulated Torque Supported */
#define CYBLE_CPS_CPF_WHEEL_BIT                     (0x01u << 2u)   /* Wheel Revolution Data Supported */
#define CYBLE_CPS_CPF_CRANK_BIT                     (0x01u << 3u)   /* Crank Revolution Data Supported */
#define CYBLE_CPS_CPF_MAGNITUDES_BIT                (0x01u << 4u)   /* Extreme Magnitudes Supported */
#define CYBLE_CPS_CPF_ANGLES_BIT                    (0x01u << 5u)   /* Extreme Angles Supported */
#define CYBLE_CPS_CPF_DEAD_SPOT_BIT                 (0x01u << 6u)   /* Top and Bottom Dead Spot Angles Supported */
#define CYBLE_CPS_CPF_ENERGY_BIT                    (0x01u << 7u)   /* Accumulated Energy Supported */
#define CYBLE_CPS_CPF_OFFSET_INDICATOR_BIT          (0x01u << 8u)   /* Offset Compensation Indicator Supported */
#define CYBLE_CPS_CPF_OFFSET_BIT                    (0x01u << 9u)   /* Offset Compensation Supported */
#define CYBLE_CPS_CPF_CPM_CONTENT_MASKING_BIT       (0x01u << 10u)  /* Cycling Power Measurement Characteristic 
                                                                           Content Masking Supported */
#define CYBLE_CPS_CPF_MULTIPLE_SENSOR_BIT           (0x01u << 11u)  /* Multiple Sensor Locations Supported */
#define CYBLE_CPS_CPF_CRANK_LEN_ADJ_BIT             (0x01u << 12u)  /* Crank Length Adjustment Supported */
#define CYBLE_CPS_CPF_CHAIN_LEN_ADJ_BIT             (0x01u << 13u)  /* Chain Length Adjustment Supported */
#define CYBLE_CPS_CPF_CHAIN_WEIGHT_ADJ_BIT          (0x01u << 14u)  /* Chain Weight Adjustment Supported */
#define CYBLE_CPS_CPF_SPAN_LEN_ADJ_BIT              (0x01u << 15u)  /* Span Length Adjustment Supported */
#define CYBLE_CPS_CPF_SENSOR_MEASURE_BIT            (0x01u << 16u)  /* Sensor Measurement Context */
#define CYBLE_CPS_CPF_SENSOR_MEASURE_FORCE          (0x00u << 16u)  /* Sensor Measurement Context: Force based */
#define CYBLE_CPS_CPF_SENSOR_MEASURE_TORQUE         (0x01u << 16u)  /* Sensor Measurement Context: Torque based */
#define CYBLE_CPS_CPF_INSTANTANEOUS_DIRECTION_BIT   (0x01u << 17u)  /* Instantaneous Measurement Direction Supported */
#define CYBLE_CPS_CPF_CALL_DATE_BIT                 (0x01u << 18u)  /* Factory Calibration Date Supported */


/* Cycling Power Measurement bits */
#define CYBLE_CPS_CPM_PEDAL_PRESENT_BIT             (0x01u << 0u)   /* Pedal Power Balance Present */
#define CYBLE_CPS_CPM_PEDAL_REFERENCE_BIT           (0x01u << 1u)   /* Pedal Power Balance Reference */
#define CYBLE_CPS_CPM_TORQUE_PRESENT_BIT            (0x01u << 2u)   /* Accumulated Torque Present */
#define CYBLE_CPS_CPM_TORQUE_SOURCE_BIT             (0x01u << 3u)   /* Accumulated Torque Source */
#define CYBLE_CPS_CPM_TORQUE_SOURCE_WHEEL           (0x00u << 3u)   /* Accumulated Torque Source: Wheel Based */
#define CYBLE_CPS_CPM_TORQUE_SOURCE_CRANK           (0x01u << 3u)   /* Accumulated Torque Source: Crank Based */
#define CYBLE_CPS_CPM_WHEEL_BIT                     (0x01u << 4u)   /* Wheel Revolution Data Present */
#define CYBLE_CPS_CPM_CRANK_BIT                     (0x01u << 5u)   /* Crank Revolution Data Present */
#define CYBLE_CPS_CPM_FORCE_MAGNITUDES_BIT          (0x01u << 6u)   /* Extreme Force Magnitudes Present */
#define CYBLE_CPS_CPM_TORQUE_MAGNITUDES_BIT         (0x01u << 7u)   /* Extreme Torque Magnitudes Present */
#define CYBLE_CPS_CPM_ANGLES_BIT                    (0x01u << 8u)   /* Extreme Angles Present */
#define CYBLE_CPS_CPM_TOP_DEAD_SPOT_BIT             (0x01u << 9u)   /* Top Dead Spot Angle Present */
#define CYBLE_CPS_CPM_BOTTOM_DEAD_SPOT_BIT          (0x01u << 10u)  /* Bottom Dead Spot Angle Present */
#define CYBLE_CPS_CPM_ENERGY_BIT                    (0x01u << 11u)  /* Accumulated Energy Present */
#define CYBLE_CPS_CPM_OFFSET_INDICATOR_BIT          (0x01u << 12u)  /* Offset Compensation Indicator */

/* Cycling Control Point Mask of Cycling Power Measurement characteristic content */
#define CYBLE_CPS_CP_PEDAL_PRESENT_BIT              (0x01u << 0u)   /* Pedal Power Balance Turn off */
#define CYBLE_CPS_CP_TORQUE_PRESENT_BIT             (0x01u << 1u)   /* Accumulated Torque Turn off */
#define CYBLE_CPS_CP_WHEEL_BIT                      (0x01u << 2u)   /* Wheel Revolution Data Turn off */
#define CYBLE_CPS_CP_CRANK_BIT                      (0x01u << 3u)   /* Crank Revolution Data Turn off */
#define CYBLE_CPS_CP_MAGNITUDES_BIT                 (0x01u << 4u)   /* Extreme Magnitudes Turn off */
#define CYBLE_CPS_CP_ANGLES_BIT                     (0x01u << 5u)   /* Extreme Angles Turn off */
#define CYBLE_CPS_CP_TOP_DEAD_SPOT_BIT              (0x01u << 6u)   /* Top Dead Spot Angle Turn off */
#define CYBLE_CPS_CP_BOTTOM_DEAD_SPOT_BIT           (0x01u << 7u)   /* Bottom Dead Spot Angle Turn off */
#define CYBLE_CPS_CP_ENERGY_BIT                     (0x01u << 8u)   /* Accumulated Energy Turn off */
#define CYBLE_CPS_CP_ENERGY_RESERVED                (0xFE00u)       /* Reserved bits */

/* Cycling Power Vector bits */
#define CYBLE_CPS_CPV_CRANK_DATA_BIT                (0x01u << 0u)   /* Crank Revolution Data Present */
#define CYBLE_CPS_CPV_FIRST_CRANK_MEASURE_BIT       (0x01u << 1u)   /* First Crank Measurement Angle Present */
#define CYBLE_CPS_CPV_INST_FORCE_MAGN_BIT           (0x01u << 2u)   /* Instantaneous Force Magnitude Array Present */
#define CYBLE_CPS_CPV_INST_TORQUE_MAGN_BIT          (0x01u << 3u)   /* Instantaneous Torque Magnitude Array Present */
#define CYBLE_CPS_CPV_INST_MEASURE_DIR_MASK         (0x03u << 4u)   /* Instantaneous Measurement Direction */
#define CYBLE_CPS_CPV_INST_MEASURE_DIR_TANGENTIAL   (0x01u << 4u)   /* Instantaneous Measurement Direction: Tangential*/
#define CYBLE_CPS_CPV_INST_MEASURE_DIR_RADIAL       (0x02u << 4u)   /* Instantaneous Measurement Direction: Radial */
#define CYBLE_CPS_CPV_INST_MEASURE_DIR_LATERAL      (0x03u << 4u)   /* Instantaneous Measurement Direction: Lateral */

/* Op Codes of the Cycling Power Control Point characteristic */
typedef enum
{
    CYBLE_CPS_CP_OC_SCV = 1u,  /* Set Cumulative Value */
    CYBLE_CPS_CP_OC_USL,       /* Update Sensor Location */
    CYBLE_CPS_CP_OC_RSSL,      /* Request Supported Sensor Locations */
    CYBLE_CPS_CP_OC_SCRL,      /* Set Crank Length */
    CYBLE_CPS_CP_OC_RCRL,      /* Request Crank Length */
    CYBLE_CPS_CP_OC_SCHL,      /* Set Chain Length */
    CYBLE_CPS_CP_OC_RCHL,      /* Request Chain Length */
    CYBLE_CPS_CP_OC_SCHW,      /* Set Chain Weight */
    CYBLE_CPS_CP_OC_RCHW,      /* Request Chain Weight */
    CYBLE_CPS_CP_OC_SSL,       /* Set Span Length */
    CYBLE_CPS_CP_OC_RSL,       /* Request Span Length */
    CYBLE_CPS_CP_OC_SOC,       /* Start Offset Compensation */
    CYBLE_CPS_CP_OC_MCPMCC,    /* Mask Cycling Power Measurement Characteristic Content */
    CYBLE_CPS_CP_OC_RSR,       /* Request Sampling Rate */
    CYBLE_CPS_CP_OC_RFCD,      /* Request Factory Calibration Date */
    CYBLE_CPS_CP_OC_RC = 32u,  /* Response Code */
} CYBLE_CPS_CP_OC_T;


/* Response Code of the Cycling Power Control Point characteristic */
typedef enum
{
    CYBLE_CPS_CP_RC_SUCCESS = 1u,       /* Response for successful operation. */
    CYBLE_CPS_CP_RC_NOT_SUPPORTED,      /* Response if unsupported Op Code is received */
    CYBLE_CPS_CP_RC_INVALID_PARAMETER,  /* Response if Parameter received does not meet the requirements of the 
                                           service or is outside of the supported range of the Sensor */
    CYBLE_CPS_CP_RC_OPERATION_FAILED,   /* Response if the requested procedure failed */
} CYBLE_CPS_CP_RC_T;

/* Sensor Location characteristic value */
typedef enum
{
    CYBLE_CPS_SL_OTHER,
    CYBLE_CPS_SL_TOP_OF_SHOE,
    CYBLE_CPS_SL_IN_SHOE,
    CYBLE_CPS_SL_HIP,
    CYBLE_CPS_SL_FRONT_WHEEL,
    CYBLE_CPS_SL_LEFT_CRANK,
    CYBLE_CPS_SL_RIGHT_CRANK,
    CYBLE_CPS_SL_LEFT_PEDAL,
    CYBLE_CPS_SL_RIGHT_PEDAL,
    CYBLE_CPS_SL_FRONT_HUB,
    CYBLE_CPS_SL_REAR_DROPOUT,
    CYBLE_CPS_SL_CHAINSTAY,
    CYBLE_CPS_SL_REAR_WHEEL,
    CYBLE_CPS_SL_REAR_HUB,
    CYBLE_CPS_SL_CHEST,
    CYBLE_CPS_SL_COUNT
} CYBLE_CPS_SL_VALUE_T;


#define CYBLE_CPSS_BROADCAST_ADVERT_INTERVAL_OFFSET (0x05u)
#define CYBLE_CPSS_BROADCAST_DATA_LEN_OFFSET        (0x07u)
#define CYBLE_CPSS_BROADCAST_DATA_OFFSET            (0x0Bu)

#define CYBLE_CPSS_BROADCAST_DATA_LEN_MIN           (0x07u)
#define CYBLE_CPSS_BROADCAST_DATA_HEADER_LEN        (0x03u)
#define CYBLE_CPSS_BROADCAST_PACKETS                (0x03u)
#define CYBLE_CPSS_BROADCAST_HEADER_LEN             (CYBLE_GAP_ADV_FLAGS_PACKET_LENGTH + \
                                                     CYBLE_GAP_ADV_ADVERT_INTERVAL_PACKET_LENGTH + \
                                                     CYBLE_CPSS_BROADCAST_DATA_HEADER_LEN + \
                                                     CYBLE_CPSS_BROADCAST_PACKETS)

#define CYBLE_CPSC_BROADCAST_DATA_OFFSET            (0x04u)
/* ##Function Prototypes */

void CyBle_CpsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

#ifdef CYBLE_CPS_SERVER

CYBLE_API_RESULT_T CyBle_CpssSetCharacteristicValue(CYBLE_CPS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CpssGetCharacteristicValue(CYBLE_CPS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CpssSetCharacteristicDescriptor(CYBLE_CPS_CHAR_INDEX_T charIndex,
    CYBLE_CPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CpssGetCharacteristicDescriptor(CYBLE_CPS_CHAR_INDEX_T charIndex,
    CYBLE_CPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CpssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CPS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CpssSendIndication(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CPS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
#if CYBLE_GAP_ROLE_BROADCASTER
CYBLE_API_RESULT_T CyBle_CpssStartBroadcast(uint16 advInterval, uint8 attrSize, uint8 *attrValue);
void CyBle_CpssStopBroadcast(void);
#endif /* CYBLE_GAP_ROLE_BROADCASTER */

#endif /* CYBLE_CPS_SERVER */

#ifdef CYBLE_CPS_CLIENT

CYBLE_API_RESULT_T CyBle_CpscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_CPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CpscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_CPS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_CpscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CPS_CHAR_INDEX_T charIndex, CYBLE_CPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CpscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_CPS_CHAR_INDEX_T charIndex, CYBLE_CPS_DESCR_INDEX_T descrIndex);
CYBLE_API_RESULT_T CyBle_CpscStartObserve(void);
void CyBle_CpscStopObserve(void);

#endif /* (CYBLE_CPS_CLIENT) */


/* ##Private Function Prototypes */
/* DOM-IGNORE-BEGIN */
void CyBle_CpssInit(void);
void CyBle_CpsInit(void);

#ifdef CYBLE_CPS_SERVER
    
CYBLE_GATT_DB_ATTR_HANDLE_T CyBle_CpssGetCharacteristicDescriptorHandle(
    CYBLE_CPS_CHAR_INDEX_T charIndex, CYBLE_CPS_DESCR_INDEX_T descrIndex);
CYBLE_GATT_ERR_CODE_T CyBle_CpssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);
void CyBle_CpssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_CpssConnParamUpdateRspEventHandler(uint16 response);
void CyBle_CpssConnUpdateCompleteEventHandler(const CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *eventParam);
#if CYBLE_GAP_ROLE_BROADCASTER
void CyBle_CpssAdvertisementStartStopEventHandler(void);
#endif /* CYBLE_GAP_ROLE_BROADCASTER */
#define CyBle_CpssTimeOutEventHandler       CyBle_CpssInit
#define CyBle_CpssConnectEventHandler       CyBle_CpssInit
#define CyBle_CpssDisconnectEventHandler    CyBle_CpssStopBroadcast

#endif /* CYBLE_CPS_SERVER */

#ifdef CYBLE_CPS_CLIENT

void CyBle_CpscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_CpscDiscoverCharDescriptorsEventHandler(CYBLE_CPS_CHAR_INDEX_T discoveryCharIndex, 
                                                   CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_CpscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_CpscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam);
void CyBle_CpscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_CpscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_CpscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);
void CyBle_CpscScanStartStopEventHandler(void);
void CyBle_CpscScanProcessEventHandler(CYBLE_GAPC_ADV_REPORT_T *eventParam);
#define CyBle_CpscDisconnectEventHandler    CyBle_CpscStopObserve

#endif /* (CYBLE_CPS_CLIENT) */
/* DOM-IGNORE-END */

/* ##Macro Functions */

#ifdef CYBLE_CPS_CLIENT

#define CyBle_CpscCheckCharHandle(handle)\
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

#define CyBle_CpscGetCharacteristicValueHandle(charIndex)    \
    (((charIndex) >= CYBLE_CPS_CHAR_COUNT) ? \
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE : \
            cyBle_cpsc.charInfo[charIndex].valueHandle)

#define CyBle_CpscGetCharacteristicDescriptorHandle(charIndex, descrIndex)    \
    ((((charIndex) >= CYBLE_CPS_CHAR_COUNT) || ((descrIndex) >= CYBLE_CPS_DESCR_COUNT)) ? \
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE : \
            cyBle_cpsc.charInfo[charIndex].descrHandle[descrIndex])

#endif /* (CYBLE_CPS_CLIENT) */


/* ##External data references */

#ifdef CYBLE_CPS_SERVER

extern const CYBLE_CPSS_T cyBle_cpss;
extern CYBLE_GAPP_DISC_MODE_INFO_T cyBle_cpssBroadcastModeInfo;
extern CYBLE_CPS_CP_ADJUSTMENT_T cyBle_cpssAdjustment;

#endif /* CYBLE_CPS_SERVER */

#ifdef CYBLE_CPS_CLIENT

extern CYBLE_CPSC_T cyBle_cpsc;

#endif /* (CYBLE_CPS_CLIENT) */


#endif /* CY_BLE_CYBLE_CPS_H  */


/* [] END OF FILE */
