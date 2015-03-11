/******************************************************************************
* Project Name		: BLE_HID_Audio_Control
* File Name			: ConsumerControl.h
* Version 			: 1.0
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.1 SP1
* Compiler    		: ARM GCC 4.8.4
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit 
* Owner				: kris@cypress.com
*
********************************************************************************
* Copyright (2015), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/

/******************************************************************************
* Description - Header file for HID consumer control routines and constants
*******************************************************************************/

#if !defined(CONSUMER_CONTROL_H)
#define CONSUMER_CONTROL_H
    
/******************************************************************************
*                           Header Files
*******************************************************************************/   
#include <cytypes.h>    
    
/******************************************************************************
*                               Enums 
*******************************************************************************/
typedef enum
{
    NO_KEY_PRESSED,
    PLAY_PAUSE_PRESSED = 2,
    NEXT_TRACK_PRESSED = 4,
    PREVIOUS_TRACK_PRESSED = 6,
    VOLUME_INCREMENT_PRESSED = 8,
    VOLUME_DECREMENT_PRESSED = 10,
    INVALID_STATE
} CONSUMER_CONTROL_EVENTS;

/******************************************************************************
*                               Constants 
*******************************************************************************/
#define CONSUMER_TIMEOUT                        (100u)
#define CONSUMER_SERVICE_INDEX                  (0u)
#define CONSUMER_CONTROL_DATA_SIZE              (1u)

#define CONSUMER_CONTROL_SUMULATION_ENABLE      (1u)
#define CONSUMER_CONTROL_SUMULATION_DISABLE     (0u)

#define SEND_PLAY_PAUSE                         (1u)
#define SEND_NEXT_TRACK                         (2u)
#define SEND_PREVIOUS_TRACK                     (4u)
#define SEND_VOLUME_INCREMENT                   (8u)
#define SEND_VOLUME_DECREMENT                   (16u)

/******************************************************************************
*                               Functions 
*******************************************************************************/
void SetConsumerNotification(uint8 notificationState);
void SimulateConsumerControl(void);

#endif

/* [] END OF FILE */
