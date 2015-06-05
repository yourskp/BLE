/*******************************************************************************
* File Name: common.h
*
* Description:
*  Common BLE application header.
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#ifndef CONFIGURATION_H
#define CONFIGURATION_H
    
#include <Cytypes.h>    
    
/***************************************
*    Firmware Configuration Flags
***************************************/
#define CAPSENSE_ENABLED            (1u)
#define CAPSENSE_SLIDER             (0u)  
#define LOGGING_ENABLED             (1u)

/***************************************
*        API Constants
***************************************/
#define LED_ON                      (0u)
#define LED_OFF                     (1u)
    
#define LED_NO_ALERT                (0u)
#define LED_MILD_ALERT              (100u)
#define LED_HIGH_ALERT              (200u)

#endif

/* [] END OF FILE */
