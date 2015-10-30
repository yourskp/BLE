/*******************************************************************************
File Name: CYBLE_cts.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the function prototypes and constants used in
 the Current Time Service of the BLE Component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_CTS_H)
#define CY_BLE_CYBLE_CTS_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Data Struct Definition
***************************************/

/* Service Characteristics indexes */
typedef enum
{
    CYBLE_CTS_CURRENT_TIME,            /* Current Time characteristic index */
    CYBLE_CTS_LOCAL_TIME_INFO,         /* Local Time Information characteristic index */
    CYBLE_CTS_REFERENCE_TIME_INFO,     /* Reference Time Information characteristic index */
    CYBLE_CTS_CHAR_COUNT               /* Total count of Current Time Service characteristics */
}CYBLE_CTS_CHAR_INDEX_T;

/* Service Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_CTS_CURRENT_TIME_CCCD,       /* Current Time Client Characteristic configuration descriptor index */
    CYBLE_CTS_COUNT                    /* Total count of Current Time Service characteristic descriptors */
}CYBLE_CTS_CHAR_DESCRIPTORS_T;

/* Current Time Characteristic structure */
typedef struct
{
    uint8 yearLow;          /* LSB of current year */
    uint8 yearHigh;         /* MSB of current year */
    uint8 month;            /* Current month */
    uint8 day;              /* Current day */
    uint8 hours;            /* Current time - hours */
    uint8 minutes;          /* Current time - minutes */
    uint8 seconds;          /* Current time - seconds */
    uint8 dayOfWeek;        /* Current day of week */
    uint8 fractions256;     /* The value of 1/256th of second */
    uint8 adjustReason;     /* Reason of Current Time service characteristics change */
} CYBLE_CTS_CURRENT_TIME_T;

/* Local Time Information Characteristic structure */
typedef struct
{
    int8 timeZone;           /* Current Time Zone */
    uint8 dst;               /* Daylight Saving Time value */
} CYBLE_CTS_LOCAL_TIME_INFO_T;

/* Reference Time Information Characteristic structure */
typedef struct
{
    uint8 timeSource;              /* Time update source */
    uint8 timeAccuracy;            /* Time accuracy */
    uint8 daysSinceUpdate;         /* Days since last time update */
    uint8 hoursSinseUpdate;        /* Hours since last time update */
} CYBLE_CTS_REFERENCE_TIME_INFO_T;

/* Current Time Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T         connHandle;             /* Connection handle */
    CYBLE_CTS_CHAR_INDEX_T      charIndex;              /* Characteristic index of Current Time Service */
    CYBLE_GATT_VALUE_T         *value;                  /* Pointer to value of Current Time Service characteristic */
} CYBLE_CTS_CHAR_VALUE_T;

/* Current Time Service Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T             connHandle;         /* Connection handle */
    CYBLE_CTS_CHAR_INDEX_T          charIndex;          /* Characteristic index of Current Time Service */
    CYBLE_CTS_CHAR_DESCRIPTORS_T    descrIndex;         /* Characteristic index Descriptor of Current Time Service */
    CYBLE_GATT_VALUE_T             *value;              /* Pointer to value of Current Time Service characteristic */
} CYBLE_CTS_DESCR_VALUE_T;

#ifdef CYBLE_CTS_SERVER

/* Structure with Current Time Service attribute handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T    serviceHandle;           /* Current Time Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T    currTimeCharHandle;      /* Current Time Characteristic handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T    currTimeCccdHandle;      /* Current Time Client Characteristic Configuration 
                                                               Characteristic handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T    localTimeInfCharHandle;  /* Local Time Information Characteristic handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T    refTimeInfCharHandle;    /* Reference Time Information Characteristic handle */
} CYBLE_CTSS_T;

#endif /* (CYBLE_CTS_SERVER) */

#ifdef CYBLE_CTS_CLIENT

/* Structure with discovered attributes information of Current Time Service */
typedef struct
{
    /* Structure with Characteristic handles + properties of Current Time Service */
    CYBLE_SRVR_CHAR_INFO_T      currTimeCharacteristics[CYBLE_CTS_CHAR_COUNT];
    /* Current Time Client Characteristic Configuration handle of Current Time Service */
    CYBLE_GATT_DB_ATTR_HANDLE_T currTimeCccdHandle;
} CYBLE_CTSC_T;

#endif /* (CYBLE_CTS_CLIENT) */


/***************************************
##Function Prototypes
***************************************/

void CyBle_CtsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);

/* GATT Server API functions */
#ifdef CYBLE_CTS_SERVER

CYBLE_API_RESULT_T CyBle_CtssSetCharacteristicValue(CYBLE_CTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CtssGetCharacteristicValue(CYBLE_CTS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CtssGetCharacteristicDescriptor(CYBLE_CTS_CHAR_INDEX_T charIndex,
                                                         CYBLE_CTS_CHAR_DESCRIPTORS_T descrIndex,
                                                         uint8 attrSize,
                                                         uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CtssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
                                              CYBLE_CTS_CHAR_INDEX_T charIndex,
                                              uint8 attrSize,
                                              uint8 *attrValue);

#endif /* (CYBLE_CTS_SERVER) */

/* GATT Client API functions */
#ifdef CYBLE_CTS_CLIENT
    
CYBLE_API_RESULT_T CyBle_CtscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_CTS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_CtscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                         CYBLE_CTS_CHAR_INDEX_T charIndex,
                                                         CYBLE_CTS_CHAR_DESCRIPTORS_T descrIndex,
                                                         uint8 attrSize,
                                                         uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_CtscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
                                                   CYBLE_CTS_CHAR_INDEX_T charIndex,
                                                   uint8 descrIndex);

#endif /* (CYBLE_CTS_CLIENT) */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_CtsInit(void);

#ifdef CYBLE_CTS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_CtssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* (CYBLE_CTS_SERVER) */

#ifdef CYBLE_CTS_CLIENT

void CyBle_CtscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_CtscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T * discDescrInfo);
void CyBle_CtscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_CtscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T* eventParam);
void CyBle_CtscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_CtscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_CTS_CLIENT) */
/* DOM-IGNORE-END */


/***************************************
##API Constants
***************************************/

#define CYBLE_CTS_CURRENT_TIME_SIZE              (10u)
#define CYBLE_CTS_LOCAL_TIME_INFO_SIZE           (2u)
#define CYBLE_CTS_REFERENCE_TIME_INFO_SIZE       (4u)
#define CYBLE_CTS_CLIENT_CONFIG_DESCR_SIZE       (2u)

/* CTS GATT error codes */
#define CYBLE_GATT_ERR_CTS_DATA_FIELD_IGNORED    ((CYBLE_GATT_ERR_CODE_T) (0x80u))


/***************************************
##Current Time ranges
***************************************/

/* The range of "Year" is from 1582 to 9999. "Year" is split into two bytes and
   the following values define the maximum and minimum allowed years. */
#define CYBLE_CTS_YEAR_MAX                      (9999u)
#define CYBLE_CTS_YEAR_MIN                      (1582u)

/* Range of "Month" is 0 to 12  */
#define CYBLE_CTS_MONTH_MAX                      (12u)

/* Range of "Month" is 0 to 31  */
#define CYBLE_CTS_DAY_MAX                        (31u)

/* Range of "Hours" is 0 to 24  */
#define CYBLE_CTS_HOURS_MAX                      (23u)

/* Range of "Minutes" is 0 to 59  */
#define CYBLE_CTS_MINUTES_MAX                    (59u)

/* Range of "Seconds" is 0 to 59  */
#define CYBLE_CTS_SECONDS_MAX                    (CYBLE_CTS_MINUTES_MAX)

/* Range of "dayOfWeek" is 1 to 7  */
#define CYBLE_CTS_DAY_OF_WEEK_MAX                (7u)

/* Current Time update reasons */
#define CYBLE_CTS_REASON_UNKNOWN                 (0u)
#define CYBLE_CTS_MANUAL_UPDATE                  (1u)
#define CYBLE_CTS_EXTERNAL_REF_UPDATE            (2u)
#define CYBLE_CTS_TIME_ZONE_CHANGE               (4u)
#define CYBLE_CTS_DST_CHANGE                     (8u)

#define CYBLE_CTS_MAX_DAYS_SINCE_UPDATE          (255u)
#define CYBLE_CTS_MAX_HOURS_SINCE_UPDATE         (CYBLE_CTS_MAX_DAYS_SINCE_UPDATE)

/* DST offset values */
#define CYBLE_CTS_STANDARD_TIME                  (0u)
#define CYBLE_CTS_DAYLIGHT_TIME_0_5              (2u)
#define CYBLE_CTS_DAYLIGHT_TIME_1_0              (4u)
#define CYBLE_CTS_DAYLIGHT_TIME_2_0              (8u)
#define CYBLE_CTS_DST_UNKNOWN                    (255u)

#define CYBLE_CTS_UTC_OFFSET_MAX                 (56)
#define CYBLE_CTS_UTC_OFFSET_MIN                 (-48)
#define CYBLE_CTS_UTC_OFFSET_UNKNOWN             (-127)

/* Time update source constants */
#define CYBLE_CTS_TIME_SRC_UNKNOWN               (0u)
#define CYBLE_CTS_TIME_SRC_NTP                   (1u)
#define CYBLE_CTS_TIME_SRC_GPS                   (2u)
#define CYBLE_CTS_TIME_SRC_RADIO                 (3u)
#define CYBLE_CTS_TIME_SRC_MANUAL                (4u)
#define CYBLE_CTS_TIME_SRC_ATOMIC_CLK            (5u)
#define CYBLE_CTS_TIME_SRC_CELL_NET              (6u)

/* Client Characteristic Configuration Descriptor constants */
#define CYBLE_CTS_CLIENT_CONFIG_MASK             (0x03u)
#define CYBLE_CTS_CLIENT_CONFIG_NTF_ENABLED      (0x01u)
#define CYBLE_CTS_CLIENT_CONFIG_IND_ENABLED      (0x02u)

#define CYBLE_CTS_INVALID_CHAR_INDEX             (0xFFu)

#define CYBLE_CTS_8_BIT_OFFSET                   (8u)

#ifdef CYBLE_CTS_SERVER

/* Performs validation of 'adjReason' if it has invalid value, it will be set to
   "Unknown" reason. */
#define CYBLE_CTS_CHECK_ADJUST_REASON(adjReason)    ((adjReason) = \
    (((adjReason) == CYBLE_CTS_REASON_UNKNOWN) ? CYBLE_CTS_REASON_UNKNOWN :\
    (((adjReason) == CYBLE_CTS_MANUAL_UPDATE) ? CYBLE_CTS_MANUAL_UPDATE :\
    (((adjReason) == CYBLE_CTS_EXTERNAL_REF_UPDATE) ? CYBLE_CTS_EXTERNAL_REF_UPDATE :\
    (((adjReason) == CYBLE_CTS_TIME_ZONE_CHANGE) ? CYBLE_CTS_TIME_ZONE_CHANGE :\
    (((adjReason) == CYBLE_CTS_DST_CHANGE) ? CYBLE_CTS_DST_CHANGE : \
    CYBLE_CTS_REASON_UNKNOWN))))))

#endif /* (CYBLE_CTS_SERVER) */


/***************************************
##Variables with external linkage
***************************************/

#ifdef CYBLE_CTS_SERVER

extern const CYBLE_CTSS_T cyBle_ctss;

#endif /* (CYBLE_CTS_SERVER) */

#ifdef CYBLE_CTS_CLIENT

extern CYBLE_CTSC_T cyBle_ctsc;

#endif /* (CYBLE_CTS_CLIENT) */

#endif /* !defined(CY_BLE_CYBLE_CTS_H) */


/* [] END OF FILE */
