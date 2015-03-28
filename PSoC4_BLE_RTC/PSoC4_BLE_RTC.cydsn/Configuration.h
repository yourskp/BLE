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
    
/* Project configuration flags */    
#define BLE_GATT_CLIENT_ENABLE                      (1u)
#define RTC_ENABLE                                  (1u)
#define DISCONNECT_BLE_AFTER_TIME_SYNC              (1u)  
#define RESTART_ADV_ON_DISCONNECTION                (0u)  
#define CONSOLE_LOG_ENABLED                         (1u)
#if CONSOLE_LOG_ENABLED    
#define DISPLAY_ON_BUTTON_PRESS                     (1u)    
#endif    
    
#define TRUE 1
#define FALSE 0
    
#endif
