/*****************************************************************************
* File Name: BleApplications.h
*
* Version: 1.0
*
* Description:
*   This file implements the BLE capability in the PSoC 4 Level Sensor.
*
* Hardware Dependency:
*   Chemical Tank Rev A PCB
*
******************************************************************************
* Copyright (2016), Intermountain Water Softeners
******************************************************************************
* This software is owned by Intermountain Water Softeners (Intermountain) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of (Intermountain).
*
*****************************************************************************/

#if !defined(_BLE_APPLICATIONS_H)
#define _BLE_APPLICATIONS_H

/*****************************************************************************
* Included headers
*****************************************************************************/
#include <project.h>


/*****************************************************************************
* Macros 
*****************************************************************************/
#define CAPSENSE_SERVICE_INDEX          (CYBLE_CAPSENSE_SERVICE_SERVICE_INDEX)
#define CAPSENSE_SLIDER_CHAR_INDEX      (CYBLE_CAPSENSE_SERVICE_CAPSENSE_SLIDER_CHARACTERISTIC_CHAR_INDEX)
#define CAPSENSE_SLIDER_CHAR_HANDLE		(CYBLE_CAPSENSE_SERVICE_CAPSENSE_SLIDER_CHARACTERISTIC_CHAR_HANDLE)

#define CAPSENSE_CCC_HANDLE				(CYBLE_CAPSENSE_SERVICE_CAPSENSE_SLIDER_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)

#define CAPSENSE_SLIDER_CCC_INDEX		(CYBLE_CAPSENSE_SERVICE_CAPSENSE_SLIDER_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX)
#define CCC_DATA_INDEX					(0u)

#define BLE_STATE_ADVERTISING			(0x01)
#define BLE_STATE_CONNECTED				(0x02)
#define BLE_STATE_DISCONNECTED			(0x00)

#define MTU_XCHANGE_DATA_LEN			(0x0020)
    
#define KRIS_CHANGES                    1


/*****************************************************************************
* Extern variables
*****************************************************************************/
extern uint8 deviceConnected;
extern uint8 sendCapSenseSliderNotifications;
extern uint8 sendTemperatureNotifications;
extern uint8 sendBatteryNotifications;


/*****************************************************************************
* Public functions
*****************************************************************************/
void CustomEventHandler(uint32 event, void * eventParam);
void SendCapSenseNotification(uint8 CapSenseSliderData);
void SendBatteryNotification(uint16 batteryData);
void SendTemperatureNotification(uint8 temperatureData);

#endif  /* #if !defined(_BLE_APPLICATIONS_H) */

/* [] END OF FILE */
