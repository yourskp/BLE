/*******************************************************************************
File Name: CYBLE_custom.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants for the Custom Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_CUSTOM_H)
#define CY_BLE_CYBLE_CUSTOM_H

#include "`$INSTANCE_NAME`_gatt.h"


/***************************************
##Conditional Compilation Parameters
***************************************/

/* Maximum supported Custom Services */
#define CYBLE_CUSTOMS_SERVICE_COUNT                  (`$CustomSCount`)
#define CYBLE_CUSTOMC_SERVICE_COUNT                  (`$CustomCCount`)
#define CYBLE_CUSTOM_SERVICE_CHAR_COUNT              (`$CustomMaxCharacteristicCount`)
#define CYBLE_CUSTOM_SERVICE_CHAR_DESCRIPTORS_COUNT  (`$CustomMaxDescriptorCount`)

/* Below are the indexes and handles of the defined Custom Services and their characteristics */
`$CustomDefines`

#if(CYBLE_CUSTOMS_SERVICE_COUNT != 0u)
    #define CYBLE_CUSTOM_SERVER
#endif /* (CYBLE_CUSTOMS_SERVICE_COUNT != 0u) */
    
#if(CYBLE_CUSTOMC_SERVICE_COUNT != 0u)
    #define CYBLE_CUSTOM_CLIENT
#endif /* (CYBLE_CUSTOMC_SERVICE_COUNT != 0u) */

/***************************************
##Data Struct Definition
***************************************/

#ifdef CYBLE_CUSTOM_SERVER

/* Contains information about Custom Characteristic structure */
typedef struct
{
    /* Custom Characteristic handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServCharHandle;
    /* Custom Characteristic Descriptors handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServCharDesc[CYBLE_CUSTOM_SERVICE_CHAR_DESCRIPTORS_COUNT];
} CYBLE_CUSTOMS_INFO_T;

/* Structure with Custom Service attribute handles. */
typedef struct
{
    /* Handle of a Custom Service */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServHandle;
    
    /* Information about Custom Characteristics */
    CYBLE_CUSTOMS_INFO_T customServInfo[CYBLE_CUSTOM_SERVICE_CHAR_COUNT];
} CYBLE_CUSTOMS_T;


#endif /* (CYBLE_CUSTOM_SERVER) */

/* DOM-IGNORE-BEGIN */
/* The custom Client functionality is not functional in current version of 
* the component.
*/
#ifdef CYBLE_CUSTOM_CLIENT

typedef struct
{
    /* Custom Descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T descHandle;
	/* Custom Descriptor 128 bit UUID */
	const void *uuid;           
    /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */
	uint8 uuidFormat;
   
} CYBLE_CUSTOMC_DESC_T;

typedef struct
{
    /* Characteristic handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServCharHandle;
	/* Characteristic end handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServCharEndHandle;
	/* Custom Characteristic UUID */
	const void *uuid;           
    /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */
	uint8 uuidFormat;
    /* Properties for value field */
    uint8  properties;
	/* Number of descriptors */
    uint8 descCount;
    /* Characteristic Descriptors */
    CYBLE_CUSTOMC_DESC_T * customServCharDesc;
} CYBLE_CUSTOMC_CHAR_T;

/* Structure with discovered attributes information of Custom Service */
typedef struct
{
    /* Custom Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T customServHandle;
	/* Custom Service UUID */
	const void *uuid;           
    /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */
	uint8 uuidFormat;
	/* Number of characteristics */
    uint8 charCount;
    /* Custom Service Characteristics */
    CYBLE_CUSTOMC_CHAR_T * customServChar;
} CYBLE_CUSTOMC_T;

#endif /* (CYBLE_CUSTOM_CLIENT) */
/* DOM-IGNORE-END */


#ifdef CYBLE_CUSTOM_SERVER

extern const CYBLE_CUSTOMS_T cyBle_customs[CYBLE_CUSTOMS_SERVICE_COUNT];

#endif /* (CYBLE_CUSTOM_SERVER) */

/* DOM-IGNORE-BEGIN */
#ifdef CYBLE_CUSTOM_CLIENT

extern CYBLE_CUSTOMC_T cyBle_customc[CYBLE_CUSTOMC_SERVICE_COUNT];

#endif /* (CYBLE_CUSTOM_CLIENT) */
/* DOM-IGNORE-END */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_CustomInit(void);

#ifdef CYBLE_CUSTOM_CLIENT

void CyBle_CustomcDiscoverServiceEventHandler(const CYBLE_DISC_SRVC128_INFO_T *discServInfo);
void CyBle_CustomcDiscoverCharacteristicsEventHandler(uint16 discoveryService, const CYBLE_DISC_CHAR_INFO_T *discCharInfo);
CYBLE_GATT_ATTR_HANDLE_RANGE_T CyBle_CustomcGetCharRange(uint8 incrementIndex);
void CyBle_CustomcDiscoverCharDescriptorsEventHandler(const CYBLE_DISC_DESCR_INFO_T *discDescrInfo);

#endif /* (CYBLE_CUSTOM_CLIENT) */

/* DOM-IGNORE-END */

/***************************************
##External data references 
***************************************/

#ifdef CYBLE_CUSTOM_CLIENT

extern CYBLE_CUSTOMC_T cyBle_customCServ[CYBLE_CUSTOMC_SERVICE_COUNT];

#endif /* (CYBLE_CUSTOM_CLIENT) */


/* DOM-IGNORE-BEGIN */
/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/
#define customServiceCharHandle         customServCharHandle
#define customServiceCharDescriptors    customServCharDesc
#define customServiceHandle             customServHandle
#define customServiceInfo               customServInfo
/* DOM-IGNORE-END */


#endif /* CY_BLE_CYBLE_CUSTOM_H  */

/* [] END OF FILE */
