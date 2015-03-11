/******************************************************************************
* Project Name		: BLE_HID_Audio_Control
* File Name			: ConsumerControl.c
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
*                           Header Files
*******************************************************************************/
#include <BLE Interface.h>
#include <Configuration.h>
#include <ConsumerControl.h>
#include <Init.h>
#include <LowPower.h>
#include <project.h>
#include <utils.h>

/******************************************************************************
*                           Global variables
*******************************************************************************/ 
uint8 consumerSimulation = CONSUMER_CONTROL_SUMULATION_DISABLE;

/*******************************************************************************
* Function Name: SimulateConsumerControl()
********************************************************************************
*
* Summary:
*   The custom function to simulate play/pause, next track, previous track, 
*   volume up and volume down HID consumer control commnads
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void SimulateConsumerControl(void)
{
    static uint8 consumerState = NO_KEY_PRESSED;
    static uint8 sendCommand = 0;
    uint8 commandValue;
    CYBLE_API_RESULT_T apiResult;
    static uint32 consumerTimer = CONSUMER_TIMEOUT;
    
    if(consumerSimulation == CONSUMER_CONTROL_SUMULATION_ENABLE && --consumerTimer == 0)
    {
        switch (consumerState)
        {
            case PLAY_PAUSE_PRESSED:
                commandValue = SEND_PLAY_PAUSE;
                sendCommand = 1;
            break;
            
            case NEXT_TRACK_PRESSED:
                commandValue = SEND_NEXT_TRACK;
                sendCommand = 1;
            break;
            
            case PREVIOUS_TRACK_PRESSED:
                commandValue = SEND_PREVIOUS_TRACK;
                sendCommand = 1;
            break;
            
            case VOLUME_INCREMENT_PRESSED:
                commandValue = SEND_VOLUME_INCREMENT;
                sendCommand = 1;
            break;
            
            case VOLUME_DECREMENT_PRESSED:
                commandValue = SEND_VOLUME_DECREMENT;
                sendCommand = 1;
            break;
            
            default:
            break;
        }
        
        if(consumerState >= INVALID_STATE)
        {
            consumerState = NO_KEY_PRESSED;
        }
        
        if(sendCommand)
        {
            /* send key press notification */
            apiResult = CyBle_HidssSendNotification(cyBle_connHandle, CONSUMER_SERVICE_INDEX, CYBLE_HIDS_REPORT, CONSUMER_CONTROL_DATA_SIZE, &commandValue);
            if(apiResult != CYBLE_ERROR_OK)
            {
                consumerState = NO_KEY_PRESSED;
            }
            
            sendCommand = 0;
            commandValue = 0;   
            
            /* Send key release notification */
            apiResult = CyBle_HidssSendNotification(cyBle_connHandle, CONSUMER_SERVICE_INDEX, CYBLE_HIDS_REPORT, CONSUMER_CONTROL_DATA_SIZE, &commandValue);
            if(apiResult != CYBLE_ERROR_OK)
            {
                consumerState = NO_KEY_PRESSED;
            }
        }
        
        consumerState++;
        consumerTimer = CONSUMER_TIMEOUT;
    }
}

/*******************************************************************************
* Function Name: SetConsumerNotification()
********************************************************************************
*
* Summary:
*   The custom function to simulate play/pause, next track, previous track, 
*   volume up and volume down HID consumer control commnads
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void SetConsumerNotification(uint8 notificationState)
{
    consumerSimulation = notificationState;
}
/* [] END OF FILE */
