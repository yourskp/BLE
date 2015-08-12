/*******************************************************************************
File Name: CYBLE.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the function prototypes and constants available to the BLE component.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_H)
#define CY_BLE_CYBLE_H

#include "cytypes.h"
#include "CyLib.h"
#include "CyFlash.h"

/***************************************
##Stack includes
***************************************/

#include "`$INSTANCE_NAME`_Stack.h"
#include "`$INSTANCE_NAME`_StackHostMain.h"
#include "`$INSTANCE_NAME`_StackGap.h"
#include "`$INSTANCE_NAME`_StackGattServer.h"
#include "`$INSTANCE_NAME`_StackGattClient.h"
#include "`$INSTANCE_NAME`_StackGattDb.h"
#include "`$INSTANCE_NAME`_StackL2cap.h"


/***************************************
##Conditional Compilation Parameters
***************************************/
    
#if defined(__GNUC__) || defined(__ARMCC_VERSION)
    #define CYBLE_FLASH_ROW_ALIGNED __attribute__ ((aligned(CY_FLASH_SIZEOF_ROW)))
    #define CYBLE_CYPACKED 
    #define CYBLE_CYPACKED_ATTR __attribute__ ((packed))
    #define CYBLE_CYALIGNED_BEGIN
    #define CYBLE_CYALIGNED_END __attribute__ ((aligned))
#elif defined(__ICCARM__)
    #define CYBLE_CYPACKED __packed
    #define CYBLE_CYPACKED_ATTR 
    #define CYBLE_CYALIGNED_BEGIN _Pragma("data_alignment=4")
    #define CYBLE_CYALIGNED_END
    /* For IAR data alignment is handled directly prior structure declaration */
#else
    #error Unsupported toolchain
#endif

#define CYBLE_MODE                                  (`$Mode`u)
#define CYBLE_PROFILE                               (0u)
#define CYBLE_HCI                                   (1u)
#define CYBLE_MODE_PROFILE                          (CYBLE_MODE == CYBLE_PROFILE)
#define CYBLE_MODE_HCI                              (CYBLE_MODE == CYBLE_HCI)

#define CYBLE_STACK_MODE                            (`$StackMode`u)
#define CYBLE_STACK_DEBUG                           (2u)
#define CYBLE_STACK_RELEASE                         (3u)
#define CYBLE_STACK_MODE_DEBUG                      (CYBLE_STACK_MODE == CYBLE_STACK_DEBUG)
#define CYBLE_STACK_MODE_RELEASE                    (CYBLE_STACK_MODE == CYBLE_STACK_RELEASE)

#if(CYBLE_MODE_PROFILE)
    
#define CYBLE_GAP_ROLE                              (`$GAP_ROLE`u)
#define CYBLE_GAP_HCI                               (0x00u)
#define CYBLE_GAP_PERIPHERAL                        (0x01u)
#define CYBLE_GAP_CENTRAL                           (0x02u)
#define CYBLE_GAP_BOTH                              (CYBLE_GAP_CENTRAL | CYBLE_GAP_PERIPHERAL)
#define CYBLE_GAP_BROADCASTER                       (0x04u)
#define CYBLE_GAP_OBSERVER                          (0x08u)

#define CYBLE_GAP_ROLE_PERIPHERAL                   (0u != (CYBLE_GAP_ROLE & CYBLE_GAP_PERIPHERAL))
#define CYBLE_GAP_ROLE_CENTRAL                      (0u != (CYBLE_GAP_ROLE & CYBLE_GAP_CENTRAL))
#define CYBLE_GAP_ROLE_OBSERVER                     (0u != (CYBLE_GAP_ROLE & CYBLE_GAP_OBSERVER))
#define CYBLE_GAP_ROLE_BROADCASTER                  (0u != (CYBLE_GAP_ROLE & CYBLE_GAP_BROADCASTER))

#if(CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_BROADCASTER)
    #define CYBLE_FAST_ADV_INT_MIN                  (`$AdvFastIntervalMin`)
    #define CYBLE_FAST_ADV_INT_MAX                  (`$AdvFastIntervalMax`)
    #define CYBLE_FAST_ADV_TIMEOUT                  (`$AdvFastTimeout`)
    #define CYBLE_SLOW_ADV_ENABLED                  (`$AdvReducedEnabled`)
    #define CYBLE_SLOW_ADV_INT_MIN                  (`$AdvReducedIntervalMin`)
    #define CYBLE_SLOW_ADV_INT_MAX                  (`$AdvReducedIntervalMax`)
    #define CYBLE_SLOW_ADV_TIMEOUT                  (`$AdvReducedTimeout`)
    #define CYBLE_GAPP_CONNECTION_INTERVAL_MIN      (`$PeripheralConnectionIntervalMin`)
    #define CYBLE_GAPP_CONNECTION_INTERVAL_MAX      (`$PeripheralConnectionIntervalMax`)
    #define CYBLE_GAPP_CONNECTION_SLAVE_LATENCY     (`$PeripheralConnectionSlaveLatency`)
    #define CYBLE_GAPP_CONNECTION_TIME_OUT          (`$PeripheralConnectionTimeout`)
#endif /* CYBLE_GAP_ROLE_PERIPHERAL */

#if(CYBLE_GAP_ROLE_CENTRAL || CYBLE_GAP_ROLE_OBSERVER)
    #define CYBLE_FAST_SCAN_INTERVAL                (`$ScanFastInterval`)
    #define CYBLE_FAST_SCAN_WINDOW                  (`$ScanFastWindow`)
    #define CYBLE_FAST_SCAN_TIMEOUT                 (`$ScanFastTimeout`)
    #define CYBLE_SLOW_SCAN_ENABLED                 (`$ScanReducedEnabled`)
    #define CYBLE_SLOW_SCAN_INTERVAL                (`$ScanReducedInterval`)
    #define CYBLE_SLOW_SCAN_WINDOW                  (`$ScanReducedWindow`)
    #define CYBLE_SLOW_SCAN_TIMEOUT                 (`$ScanReducedTimeout`)
    #define CYBLE_GAPC_CONNECTION_INTERVAL_MIN      (`$ConnectionIntervalMin`)
    #define CYBLE_GAPC_CONNECTION_INTERVAL_MAX      (`$ConnectionIntervalMax`)
    #define CYBLE_GAPC_CONNECTION_SLAVE_LATENCY     (`$ConnectionSlaveLatency`)
    #define CYBLE_GAPC_CONNECTION_TIME_OUT          (`$ConnectionTimeout`)
#endif /* CYBLE_GAP_ROLE_CENTRAL */

#define SILICON_GENERATED_DEVICE_ADDRESS            (`$SiliconDeviceAddressEnabled`)

/*  Selected IO capability  
    CYBLE_GAP_IOCAP_DISPLAY_ONLY
    CYBLE_GAP_IOCAP_DISPLAY_YESNO
    CYBLE_GAP_IOCAP_KEYBOARD_ONLY
    CYBLE_GAP_IOCAP_NOINPUT_NOOUTPUT
    CYBLE_GAP_IOCAP_KEYBOARD_DISPLAY
*/
#define CYBLE_IO_CAPABILITY                         (`$SecurityIOCapability`)
#define CYBLE_PAIRING_METHOD                        (`$SecurityPairingMethod`)
#define CYBLE_BONDING_REQUIREMENT                   (`$SecurityBonding`)

/* Tx Power Level */
#define CYBLE_TX_POWER_LEVEL_ADV                    (`$TxPowerLevelAdv`)
#define CYBLE_TX_POWER_LEVEL_CONN                   (`$TxPowerLevelConn`)

`$AdvDefines`

/* Align buffer size value to 4 */
#define CYBLE_ALIGN_TO_4(x)                 ((((x) & 3u) == 0u) ? (x) : (((x) - ((x) & 3u)) + 4u))

/* Stack buffers count */
#define CYBLE_GATT_MTU_BUF_COUNT            (CYBLE_GATT_MIN_NO_OF_ATT_MTU_BUFF)
#define CYBLE_STACK_BUF_COUNT               (6u)

/* GATT MTU Size */
#define CYBLE_GATT_MTU                      (`$MTUSize`)
#define CYBLE_GATT_MTU_PLUS_L2CAP_MEM_EXT   CYBLE_ALIGN_TO_4(CYBLE_GATT_MTU + CYBLE_MEM_EXT_SZ + CYBLE_L2CAP_HDR_SZ)

/* GATT Maximum attribute length */
#define CYBLE_GATT_MAX_ATTR_LEN             (`$GattDBMaxValueLen` == 0u ? 1u : `$GattDBMaxValueLen`)
#define CYBLE_GATT_MAX_ATTR_LEN_PLUS_L2CAP_MEM_EXT \
                                    CYBLE_ALIGN_TO_4(CYBLE_GATT_MAX_ATTR_LEN + CYBLE_MEM_EXT_SZ + CYBLE_L2CAP_HDR_SZ)

/* L2CAP MTU Size */
#define CYBLE_L2CAP_MTU                     (`$L2capMtuSize`u)
#define CYBLE_L2CAP_MTU_PLUS_L2CAP_MEM_EXT  CYBLE_ALIGN_TO_4(CYBLE_L2CAP_MTU + CYBLE_MEM_EXT_SZ + CYBLE_L2CAP_HDR_SZ)

/* L2CAP PMS Size */
#define CYBLE_L2CAP_MPS                     (`$L2capMpsSize`u)
#define CYBLE_L2CAP_MPS_PLUS_L2CAP_MEM_EXT  CYBLE_ALIGN_TO_4(CYBLE_L2CAP_MPS + CYBLE_MEM_EXT_SZ + CYBLE_L2CAP_HDR_SZ)

#define CYBLE_L2CAP_PSM_PLUS_L2CAP_MEM_EXT  CYBLE_ALIGN_TO_4(CYBLE_L2CAP_PSM_SIZE + CYBLE_MEM_EXT_SZ)

#define CYBLE_L2CAP_CBFC_PLUS_L2CAP_MEM_EXT CYBLE_ALIGN_TO_4(CYBLE_L2CAP_CBFC_CHANNEL_SIZE + CYBLE_MEM_EXT_SZ)

#define CYBLE_L2CAP_LOGICAL_CHANNEL_COUNT   (`$L2capNumChannels`u) 

#define CYBLE_L2CAP_PSM_COUNT               (`$L2capNumPsm`u)

#endif /* CYBLE_MODE_PROFILE */

/* RAM memory size required for stack */
#if(CYBLE_MODE_PROFILE)
    #define CYBLE_STACK_RAM_SIZE   CYBLE_ALIGN_TO_4(CYBLE_DEFAULT_RAM_SIZE_SOC +\
              (CYBLE_GATT_MTU_PLUS_L2CAP_MEM_EXT * CYBLE_GATT_MTU_BUF_COUNT) +\
               CYBLE_GATT_MAX_ATTR_LEN_PLUS_L2CAP_MEM_EXT +\
              (CYBLE_L2CAP_PSM_PLUS_L2CAP_MEM_EXT * CYBLE_L2CAP_PSM_COUNT) +\
              (CYBLE_L2CAP_CBFC_PLUS_L2CAP_MEM_EXT * CYBLE_L2CAP_LOGICAL_CHANNEL_COUNT) +\
              (CYBLE_L2CAP_MTU_PLUS_L2CAP_MEM_EXT * CYBLE_L2CAP_LOGICAL_CHANNEL_COUNT) +\
              (CYBLE_L2CAP_MPS_PLUS_L2CAP_MEM_EXT * CYBLE_L2CAP_LOGICAL_CHANNEL_COUNT *\
              (CYBLE_L2CAP_MTU / CYBLE_L2CAP_MPS)) +\
              (CYBLE_STACK_BUFFER_MGR_UTIL_RAM_SZ * CYBLE_STACK_BUF_COUNT))
#else
    #define CYBLE_STACK_RAM_SIZE     (CYBLE_DEFAULT_RAM_SIZE_HCI)
#endif /* CYBLE_MODE_PROFILE */  

#if(CYBLE_MODE_PROFILE)
    #define CYBLE_STACK_FLASH_SIZE   (9u + 89u*4u)
#endif /* (CYBLE_MODE_PROFILE) */


/***************************************
##Enumerated Types and Structs
***************************************/

/* Advertisement SIG assigned numbers */
typedef enum
{
    /*Flags*/
    CYBLE_GAP_ADV_FLAGS = 0x01u,

    /*Incomplete List of 16-bit Service Class UUIDs*/
    CYBLE_GAP_ADV_INCOMPL_16UUID,

    /*Complete List of 16-bit Service Class UUIDs*/
    CYBLE_GAP_ADV_COMPL_16UUID,

    /*Incomplete List of 32-bit Service Class UUIDs*/
    CYBLE_GAP_ADV_INCOMPL_32_UUID,

    /*Complete List of 32-bit Service Class UUIDs*/
    CYBLE_GAP_ADV_COMPL_32_UUID,

    /*Incomplete List of 128-bit Service Class UUIDs*/
    CYBLE_GAP_ADV_INCOMPL_128_UUID,

    /*Complete List of 128-bit Service Class UUIDs*/
    CYBLE_GAP_ADV_COMPL_128_UUID,

    /*Shortened Local Name*/
    CYBLE_GAP_ADV_SHORT_NAME,

    /*Complete Local Name*/
    CYBLE_GAP_ADV_COMPL_NAME,

    /*Tx Power Level*/
    CYBLE_GAP_ADV_TX_PWR_LVL,

    /*Class of Device*/
    CYBLE_GAP_ADV_CLASS_OF_DEVICE = 0x0Du,

    /*Simple Pairing Hash C*/
    CYBLE_GAP_ADV_SMPL_PAIR_HASH_C,

    /*Simple Pairing Randomizer R*/
    CYBLE_GAP_ADV_SMPL_PAIR_RANDOM_R,

    /*Device ID*/
    CYBLE_GAP_ADV_DEVICE_ID,

    /*Security Manager TK Value*/
    CYBLE_GAP_ADV_SCRT_MNGR_TK_VAL = 0x10u,

    /* Security Manager Out of Band Flags */
    CYBLE_GAP_ADV_SCRT_MNGR_OOB_FLAGS,

    /* Slave Connection Interval Range */
    CYBLE_GAP_ADV_SLAVE_CONN_INTRV_RANGE,

    /* List of 16-bit Service Solicitation UUIDs*/
    CYBLE_GAP_ADV_SOLICIT_16UUID = 0x14u,

    /*List of 128-bit Service Solicitation UUIDs*/
    CYBLE_GAP_ADV_SOLICIT_128UUID,

    /*Service Data - 16-bit UUID*/
    CYBLE_GAP_ADV_SRVC_DATA_16UUID,

    /* Public Target Address */
    CYBLE_GAP_ADV_PUBLIC_TARGET_ADDR,

    /* Random Target Address */
    CYBLE_GAP_ADV_RANDOM_TARGET_ADDR,

    /* Appearance */
    CYBLE_GAP_ADV_APPEARANCE,

    /* Advertising Interval */
    CYBLE_GAP_ADV_ADVERT_INTERVAL,

    /* LE Bluetooth Device Address */
    CYBLE_GAP_ADV_LE_BT_DEVICE_ADDR,

    /* LE Role */
    CYBLE_GAP_ADV_LE_ROLE,

    /* Simple Pairing Hash C-256 */
    CYBLE_GAP_ADV_SMPL_PAIR_HASH_C256,

    /* Simple Pairing Randomizer R-256 */
    CYBLE_GAP_ADV_SMPL_PAIR_RANDOM_R256,

    /*List of 32-bit Service Solicitation UUIDs*/
    CYBLE_GAP_ADV_SOLICIT_32UUID,

    /*Service Data - 32-bit UUID*/
    CYBLE_GAP_ADV_SRVC_DATA_32UUID,

    /*Service Data - 128-bit UUID*/
    CYBLE_GAP_ADV_SRVC_DATA_128UUID,

    /* 3D Information Data */
    CYBLE_GAP_ADV_3D_INFO_DATA = 0x3D

}CYBLE_GAP_ADV_ASSIGN_NUMBERS;


/***************************************
##Type Definitions
***************************************/

/* Event callback function prototype to receive events from BLE component */
typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, void *eventParam);


/***************************************
##API Constants
***************************************/

#define CYBLE_AD_TYPE_MORE16UUID                    (0x02u)
#define CYBLE_AD_TYPE_CMPL16UUID                    (0x03u)
#define CYBLE_AD_TYPE_MORE32UUID                    (0x04u)
#define CYBLE_AD_TYPE_CMPL32UUID                    (0x05u)
#define CYBLE_AD_TYPE_MORE128UUID                   (0x06u)
#define CYBLE_AD_TYPE_CMPL128UUID                   (0x07u)

#define CYBLE_PDU_DATA_LEN_ZERO                     (0x00u)

#define CYBLE_DISCOVERY_IDLE                        (0x00u)
#define CYBLE_DISCOVERY_SERVICE                     (0x01u)
#define CYBLE_DISCOVERY_CHAR                        (0x02u)
#define CYBLE_DISCOVERY_DESCR                       (0x03u)
#define CYBLE_DISCOVERY_DONE                        (0x04u)

/* CyBle_NextCharDscrDiscovery parameters */
#define CYBLE_DISCOVERY_INIT                        (0x00u)
#define CYBLE_DISCOVERY_CONTINUE                    (0x01u)

/* Device address stored by user in ROW4 of the SFLASH */
#define CYBLE_SFLASH_DEVICE_ADDRESS_PTR             ( (CYBLE_GAP_BD_ADDR_T *) (CYREG_SFLASH_MACRO_0_FREE_SFLASH0))
#define CYBLE_SFLASH_DIE_WAFER_REG                  ( *(uint8 *) CYREG_SFLASH_DIE_WAFER)
#define CYBLE_SFLASH_DIE_X_REG                      ( *(uint8 *) CYREG_SFLASH_DIE_X)
#define CYBLE_SFLASH_DIE_Y_REG                      ( *(uint8 *) CYREG_SFLASH_DIE_Y)

#define CYBLE_AD_STRUCTURE_MAX_LENGTH               (31u)

/* AD types for complete and shortened local name */
#define CYBLE_SHORT_LOCAL_NAME                      (8u)
#define CYBLE_COMPLETE_LOCAL_NAME                   (9u)

#define CYBLE_ADVERTISING_FAST                      (0x00u)
#define CYBLE_ADVERTISING_SLOW                      (0x01u)
#define CYBLE_ADVERTISING_CUSTOM                    (0x02u)

#define CYBLE_SCANNING_FAST                         (0x00u)
#define CYBLE_SCANNING_SLOW                         (0x01u)
#define CYBLE_SCANNING_CUSTOM                       (0x02u)

#define CYBLE_BONDING_YES                           (0x00u)
#define CYBLE_BONDING_NO                            (0x01u)

#define CYBLE_PENDING_STACK_FLASH_WRITE_BIT         (0x01u)
#define CYBLE_PENDING_CCCD_FLASH_WRITE_BIT          (0x02u)

/* GAP Advertisement Flags */
#define CYBLE_GAP_ADV_FLAG_LE_LIMITED_DISC_MODE     (0x01u)   /* LE Limited Discoverable Mode. */
#define CYBLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE     (0x02u)   /* LE General Discoverable Mode. */
#define CYBLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED     (0x04u)   /* BR/EDR not supported. */
#define CYBLE_GAP_ADV_FLAG_LE_BR_EDR_CONTROLLER     (0x08u)   /* Simultaneous LE and BR/EDR, Controller. */
#define CYBLE_GAP_ADV_FLAG_LE_BR_EDR_HOST           (0x10u)   /* Simultaneous LE and BR/EDR, Host. */
#define CYBLE_GAP_ADV_FLAGS_PACKET_LENGTH           (0x02u)

/* GAP Advertising interval min and max */
#define CYBLE_GAP_ADV_ADVERT_INTERVAL_MIN           (0x0020u) /* Minimum Advertising interval in 625 us units, i.e. 20 ms. */
#define CYBLE_GAP_ADV_ADVERT_INTERVAL_NONCON_MIN    (0x00A0u) /* Minimum Advertising interval in 625 us units for non connectable mode, i.e. 100 ms. */
#define CYBLE_GAP_ADV_ADVERT_INTERVAL_MAX           (0x4000u) /* Maximum Advertising interval in 625 us units, i.e. 10.24 s. */
#define CYBLE_GAP_ADV_ADVERT_INTERVAL_PACKET_LENGTH (0x03u)

#define CYBLE_GAPC_CONNECTING_TIMEOUT               (30u)     /* Seconds */


/***************************************
##Private Function Prototypes
***************************************/

/* DOM-IGNORE-BEGIN */
void CyBle_Init(void);
void CyBle_ServiceInit(void);
uint16 CyBle_Get16ByPtr(const uint8 ptr[]);
void CyBle_Set16ByPtr(uint8 ptr[], uint16 value);
/* DOM-IGNORE-END */


/***************************************
##Function Prototypes
***************************************/

CYBLE_API_RESULT_T CyBle_Start(CYBLE_CALLBACK_T  callbackFunc);
void CyBle_Stop(void);

#if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
    CYBLE_API_RESULT_T CyBle_StoreBondingData(uint8 isForceWrite);
#endif /* (CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */

#if(CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_BROADCASTER)
    CYBLE_API_RESULT_T CyBle_GappStartAdvertisement(uint8 advertisingIntervalType);
    void CyBle_GappStopAdvertisement(void);
    void CyBle_ChangeAdDeviceAddress(const CYBLE_GAP_BD_ADDR_T* bdAddr, uint8 dest);
#endif /* CYBLE_GAP_ROLE_PERIPHERAL */

#if(CYBLE_GAP_ROLE_CENTRAL || CYBLE_GAP_ROLE_OBSERVER)
    CYBLE_API_RESULT_T CyBle_GapcStartScan(uint8 scanningIntervalType);
    void CyBle_GapcStopScan(void);
#endif /* CYBLE_GAP_ROLE_CENTRAL || CYBLE_GAP_ROLE_OBSERVER */

#if(CYBLE_GAP_ROLE_CENTRAL)
    CYBLE_API_RESULT_T CyBle_GapcConnectDevice(const CYBLE_GAP_BD_ADDR_T * address);
    CYBLE_API_RESULT_T CyBle_GapcCancelDeviceConnection(void);
#endif /* CYBLE_GAP_ROLE_CENTRAL */


/***************************************
##External data references
***************************************/

extern CYBLE_CALLBACK_T                             CyBle_ApplCallback;
extern uint8                                        cyBle_initVar;
extern uint8                                        cyBle_stackMemoryRam[CYBLE_STACK_RAM_SIZE];
extern volatile uint8                               cyBle_eventHandlerFlag;
extern CYBLE_GAP_BD_ADDR_T                          cyBle_deviceAddress;
extern CYBLE_GAP_BD_ADDR_T                          * cyBle_sflashDeviceAddress;
extern CYBLE_GAP_AUTH_INFO_T                        cyBle_authInfo;

#if(CYBLE_GAP_ROLE_CENTRAL || CYBLE_GAP_ROLE_OBSERVER)
    extern CYBLE_GAPC_DISC_INFO_T                   cyBle_discoveryInfo;
#endif /* CYBLE_GAP_ROLE_CENTRAL || CYBLE_GAP_ROLE_OBSERVER */

#if(CYBLE_GAP_ROLE_CENTRAL)
    extern CYBLE_GAPC_CONN_PARAM_T                  cyBle_connectionParameters;
    extern uint16                                   cyBle_connectingTimeout;
    
#endif /* CYBLE_GAP_ROLE_CENTRAL */

#if(CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_BROADCASTER)
    extern CYBLE_GAPP_DISC_PARAM_T                  cyBle_discoveryParam;
    extern CYBLE_GAPP_DISC_DATA_T                   cyBle_discoveryData;
    extern CYBLE_GAPP_SCAN_RSP_DATA_T               cyBle_scanRspData;
    extern CYBLE_GAPP_DISC_MODE_INFO_T              cyBle_discoveryModeInfo;
#endif /* CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_BROADCASTER */

#if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))

/* This is a two-bit variable that contains status of pending write to flash operation. 
   This variable is initialized to zero in CyBle_Init() API.
   CYBLE_PENDING_CCCD_FLASH_WRITE_BIT flag is set after write to CCCD event. 
   CYBLE_PENDING_STACK_FLASH_WRITE_BIT flag is set after CYBLE_EVT_PENDING_FLASH_WRITE event.
   CyBle_StoreBondingData API should be called to store pending bonding data.
   This API automatically clears pending bits after write operation complete. */
    extern uint8 cyBle_pendingFlashWrite;
#endif  /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

#define CYBLE_TX_POWER_LEVEL                        CYBLE_TX_POWER_LEVEL_ADV



#endif /* CY_BLE_CYBLE_H  */


/* [] END OF FILE */
