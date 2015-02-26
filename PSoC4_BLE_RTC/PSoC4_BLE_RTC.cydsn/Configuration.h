/*******************************************************************************
* File Name: Configuration.h
*
* Version: 1.0
*
* Description:
*  Firmware feature enable/disable constants
*
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CONFIGURATION_H)
#define CONFIGURATION_H
    
/* BLE interface feature flag */
    
/* BLE GATT Client feature flags */    
#define BLE_GATT_CLIENT_ENABLE                      (1u)
#define RTC_ENABLE                                  (1u)
    
#define CONSOLE_LOG_ENABLED                         (1u)       
    
#define TRUE 1
#define FALSE 0
    
#endif
