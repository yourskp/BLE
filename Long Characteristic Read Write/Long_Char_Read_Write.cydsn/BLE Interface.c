/******************************************************************************
* Project Name		: Long Characteristic Read Write
* File Name			: BLE Interface.c
* Version 			: 1.0
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.2
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

/********************************************************************************
* Description -  This file contains routines that handle all the BLE interface  
* connection establishment procedures and event handlers.
********************************************************************************/

#include <BLE Interface.h>
#include <Configuration.h>
#include <project.h>
#include <stdio.h>
#include <stdbool.h>

/***************************************
*    Function declarations
***************************************/
void BLE_StackEventHandler(uint32 event, void* eventParam);

/***************************************
*    Global variables
***************************************/
uint8 updatedString[MAX_LENGTH_OF_CUSTOM_CHAR_IN_THIS_PROJECT];
uint8 stringIndex;
bool gattDbUpdateScheduled = false;

/*******************************************************************************
* Function Name: BLE_Engine_Start
********************************************************************************
*
* Summary:
*  Application level API for starting the BLE interface. The API internally calls
*  other BLE interface init APIs to setup the system.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
CYBLE_API_RESULT_T BLE_Engine_Start(void)
{
    CYBLE_API_RESULT_T apiResult;
    
    apiResult = CyBle_Start(BLE_StackEventHandler);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        CYASSERT(0);    
    }
    
#if (CONSOLE_LOG_ENABLE)
    Console_UartPutString("Initial value of custom characteristic in the GATT DB is:\r\n"); 
    BLE_DisplayCustomCharacteristicValue();
    Console_UartPutString("\r\n\n");
#endif    

    return apiResult;
}

/*******************************************************************************
* Function Name: BLE_Run
********************************************************************************
*
* Summary:
*  BLE interface processing engine. This API should be continuously called by 
*  the application in its main loop.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
void BLE_Run(void)
{
    CyBle_ProcessEvents();
    
    /* GATT DB update due to an execute request would now be complete. Read the updated GATT DB value and 
     * dispplay it on the console */
    if(gattDbUpdateScheduled)
    {
#if (CONSOLE_LOG_ENABLE)
        Console_UartPutString("Updated GATT DB value after Prepare/Execute Write operation is:\r\n"); 
        BLE_DisplayCustomCharacteristicValue();
        Console_UartPutString("\r\n\n");
#endif    
        gattDbUpdateScheduled = false;
    }
}

/*******************************************************************************
* Function Name: BLE_StackEventHandler
********************************************************************************
*
* Summary:
*   BLE stack generic event handler routine for handling connection, discovery, 
*   security etc. events.
*
* Parameters:  
*  event - event that triggered this callback
*  eventParam - parameters for the event.
*
* Return: 
*  None
*******************************************************************************/
void BLE_StackEventHandler(uint32 event, void* eventParam)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *prepWriteParam;
    CYBLE_GATT_HANDLE_VALUE_OFFSET_PARAM_T *prepWriteData;
    uint8 index;
    
#if SEND_EXECUTE_WRITE_ERROR_RESPONSE
    CYBLE_GATTS_EXEC_WRITE_REQ_T *execWriteParam;
#endif
    
    (void) eventParam;
    
    switch(event)
    {
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
#if (CONSOLE_LOG_ENABLE)
        Console_UartPutString("Peer device disconnected\r\n\n"); 
#endif
        /* fall through to advertisement start API */

        case CYBLE_EVT_STACK_ON:
            /* Put the device into discoverable mode so that remote can search it. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            
#if (CONSOLE_LOG_ENABLE)
        Console_UartPutString("Advertising\r\n\n"); 
#endif
            
            if(apiResult != CYBLE_ERROR_OK)
            {
                CYASSERT(0);    
            }
        break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
#if (CONSOLE_LOG_ENABLE)
        Console_UartPutString("Peer device connected\r\n\n"); 
#endif
        break;

        case CYBLE_EVT_GATTS_PREP_WRITE_REQ:
        prepWriteParam = (CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T*) eventParam;
        
        /* Check if this is the first prepare write request */
        if(prepWriteParam->currentPrepWriteReqCount == 1)
        {
            /* Let the BLE stack know that this application suports prepare and execute write operation */
            CyBle_GattsPrepWriteReqSupport(CYBLE_GATTS_PREP_WRITE_SUPPORT);
            
            /* Reset the string storage index */
            stringIndex = 0;
        }
        
        prepWriteData = (CYBLE_GATT_HANDLE_VALUE_OFFSET_PARAM_T*)prepWriteParam->baseAddr;
        
        /* Check if this prepare write request is for the characteristic of interest */
        
        if(prepWriteData[prepWriteParam->currentPrepWriteReqCount-1].handleValuePair.attrHandle 
            == CYBLE_LONG_READ_WRITE_DEMO_LONG_WRITE_CHARACTERISTIC_CHAR_HANDLE)
        {
            /* What you do here is optional. Here are the possible options:
             * 1. Get the data that is received as part of Prepare Write request 
             * 2. Get the data and verify if the data is valid. If valid, then do nothing. If it's invalid, then 
             *    let the BLE stack know to send an error response to prepare write request
             * 3. Any other custom handling that your application desires */
            
            /* In this example, we'll just copy the data */
            memcpy(&updatedString[stringIndex], 
                    prepWriteData[prepWriteParam->currentPrepWriteReqCount-1].handleValuePair.value.val, 
                    prepWriteData[prepWriteParam->currentPrepWriteReqCount-1].handleValuePair.value.len);
            
            stringIndex = stringIndex + 
                        prepWriteData[prepWriteParam->currentPrepWriteReqCount-1].handleValuePair.value.len;
                        
#if (CONSOLE_LOG_ENABLE)
            Console_UartPutString("Prepare Write request received from the peer device\r\n"); 
            
            for(index = 0; 
                index <  prepWriteData[prepWriteParam->currentPrepWriteReqCount-1].handleValuePair.value.len; 
                index++)
            {
                Console_UartPutChar(prepWriteData[prepWriteParam->currentPrepWriteReqCount-1].handleValuePair.value.val[index]);    
            }
            
            Console_UartPutString("\r\n");
            
#if SEND_PREPARE_WRITE_ERROR_RESPONSE
            /* If we were to report an error on Prepare Write (for example, data validattion failed), then do 
             * the following. The value of the error parameter depends on the type of validation required */
            prepWriteParam->gattErrorCode = CYBLE_GATT_ERR_INVALID_HANDLE;
    
#if (CONSOLE_LOG_ENABLE)
            Console_UartPutString("Responded with an error message for Prepare Write request\r\n\n"); 
#endif
#endif
#endif             
        }
   
        break;

        case CYBLE_EVT_GATTS_EXEC_WRITE_REQ:
        
        /* Unlike normal writes, execute write also triggers the BLE stack to update the GATT db with the
         * queued prepare write request data. Explicit GATT DB update in the application code is not required */
        
#if SEND_EXECUTE_WRITE_ERROR_RESPONSE
        execWriteParam = (CYBLE_GATTS_EXEC_WRITE_REQ_T*) eventParam;

        /* If we were to report an error on Execute Write (for example, data validattion failed), then set the 
         * corresponding error message as shown below. The value of the error parameter depends on the type 
         * of application layer validation required */
        execWriteParam->gattErrorCode = CYBLE_GATT_ERR_INVALID_HANDLE;

        /* FYI - GATT DB will not be updated with the queued Prepare Write Request data and the queued data will
         * be flushed by the BLE component */
    
#if (CONSOLE_LOG_ENABLE)
            Console_UartPutString("\r\nResponded with an error message for Execute Write request\r\n\n");
#endif

#else
#if (CONSOLE_LOG_ENABLE)
        Console_UartPutString("\r\nUpdated data received from the peer device is:\r\n"); 

        for(index = 0; index < stringIndex; index++)
        {
            Console_UartPutChar(updatedString[index]);
        }
        
        Console_UartPutString("\r\nThe above data will be updated in the GATT DB immediately after this event\r\n\n"); 
#endif      
            
        /* Let the application main loop know that an execute write event is triggered and next time through
         * the main loop, GATT DB will be updated with the complete queued prepare write data */
        gattDbUpdateScheduled = true;
#endif /* End of #if SEND_EXECUTE_WRITE_ERROR_RESPONSE */
        break;
            
        default:
        break;
    }
}

#if (CONSOLE_LOG_ENABLE)
/*******************************************************************************
* Function Name: BLE_DisplayCharacteristicValue
********************************************************************************
*
* Summary:
*  This routie will display the value stored in GATT DB for the custom 
*  characteristic in this project.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
void BLE_DisplayCustomCharacteristicValue(void)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T executeWriteAttrib;
    uint8 updatedAttribute[MAX_LENGTH_OF_CUSTOM_CHAR_IN_THIS_PROJECT];
    uint8 index;

    executeWriteAttrib.attrHandle = CYBLE_LONG_READ_WRITE_DEMO_LONG_WRITE_CHARACTERISTIC_CHAR_HANDLE;
    executeWriteAttrib.value.val = &updatedAttribute[0];
    executeWriteAttrib.value.len = MAX_LENGTH_OF_CUSTOM_CHAR_IN_THIS_PROJECT;
    
    CyBle_GattsReadAttributeValue(&executeWriteAttrib, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
    
    for(index = 0; index < MAX_LENGTH_OF_CUSTOM_CHAR_IN_THIS_PROJECT; index++)
    {
        Console_UartPutChar(updatedAttribute[index]);
    }
}
#endif /* End of #if (CONSOLE_LOG_ENABLE) */

/* [] END OF FILE */
