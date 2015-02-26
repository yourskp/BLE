/*******************************************************************************
* File Name: BLEConnection.h
*
* Version: 1.0
*
* Description:
*  Header file for BLE interface connection related constants
*
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(BLE_CONNECTION_H)
#define BLE_CONNECTION_H
    
#include <project.h>
#include <Configuration.h>    
    
typedef enum
{
    BLE_INVALID_OPERATION,
    BLE_DISCONNECTED,
    BLE_CONNECTED,
    BLE_DISCOVEER_GATT_SERVICES,
    BLE_READ_TIME,
    BLE_INITIATE_AUTHENTICATION,
    BLE_AUTHENTICATION_IN_PROGRESS,
    BLE_TIME_SERVER_ABSENT,
    BLE_IDLE,
} RTC_STATE;

/* External references */
extern volatile uint8 eventHandlerStatus;
extern volatile uint8 rfActive;
  

#define BLE_CONTINUOUS_ADVERTISEMENT                (0u)
#define BLE_LINK_ENCRYPTION_DISABLED                (0u)
#define BLE_LINK_ENCRYPTION_ENABLED                 (1u)
#define BLE_EVENT_PENDING                           (0x01)

CYBLE_API_RESULT_T BLE_Engine_Start(void);
void BLE_Engine_Reinit(void);
uint8 BLE_Run(void);

/* In-line macros */
#define SetCustomEvenStatusPending()                (eventHandlerStatus = eventHandlerStatus | BLE_EVENT_PENDING)
#define ClearCustomEvenStatusPending()              (eventHandlerStatus = eventHandlerStatus & ~BLE_EVENT_PENDING)
#define IsCustomEventStatusPending()                ((eventHandlerStatus & BLE_EVENT_PENDING) != 0)
#define IsBlessActive()                             (rfActive)
#define ClearBlessActive()                          (rfActive = FALSE)

#endif /* End of #if !defined(BLE_CONNECTION_H) */   

/* [] END OF FILE */
