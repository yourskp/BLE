/*******************************************************************************
File Name: CYBLE_bts.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the source code for the Bootloader service.

********************************************************************************
Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_eventHandler.h"


uint8  cyBle_cmdReceivedFlag = 0u;
uint16 cyBle_cmdLength = 0u;
uint8  *cyBle_btsBuffPtr;

/* Generated code */
`$BtsServer`


/******************************************************************************
##Function Name: CyBle_BtsWriteEventHandler
*******************************************************************************

Summary:
 Handles a Write Request event for the Bootloader Service.

Parameters:
 CYBLE_GATTS_WRITE_REQ_PARAM_T * eventParam: The pointer to data that came
                                             with a Write Request for BTS.

Return:
 Return a value of type CYBLE_GATT_ERR_CODE_T:
  * CYBLE_GATT_ERR_NONE - A function finished successfully.
  * CYBLE_GATT_ERR_INVALID_HANDLE - The Handle of the Bootloader
                                    Characteristic's Client Configuration
                                    Characteristic Descriptor is not valid.
  * CYBLE_GATT_ERR_WRITE_NOT_PERMITTED - Write operation is not permitted on 
                                         this attribute.
  * CYBLE_GATT_ERR_UNLIKELY_ERROR - An Internal Stack error occurred.
  * CYBLE_GATT_ERR_INVALID_OFFSET - Offset value is invalid.
  * CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED - The notification property of the
                                           Bootloader Characteristic's Client
                                           Configuration Characteristic
                                           Descriptor is disabled.
******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_BtsWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    
    /* Client Characteristic Configuration descriptor Write Request */
    if(eventParam->handleValPair.attrHandle == cyBle_btss.btServiceInfo[0u].btServiceCharDescriptors[0u])
    {
        /* Verify that notification property is enabled for Bootloader Characteristic */
        if(CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_btss.btServiceInfo[0u].btServiceCharHandle))
        {   
            gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
        }
        else
        {
            gattErr = CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED;
        }

        /* Indicate that request was handled */
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
    else if(eventParam->handleValPair.attrHandle == cyBle_btss.btServiceInfo[0u].btServiceCharHandle)
    {
        /* Write value to GATT database */
        gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
        
        cyBle_btsBuffPtr = 
                CYBLE_GATT_DB_ATTR_GET_ATTR_GEN_PTR(cyBle_btss.btServiceInfo[0u].btServiceCharHandle);

        /* Extract length of command data and add control bytes to data 
        * length to get command length.
        */
        cyBle_cmdLength = ((((uint16) cyBle_btsBuffPtr[CYBLE_BTS_COMMAND_DATA_LEN_OFFSET + 1u]) << 8u) | 
                            (uint16) cyBle_btsBuffPtr[CYBLE_BTS_COMMAND_DATA_LEN_OFFSET]) +
                                CYBLE_BTS_COMMAND_CONTROL_BYTES_NUM;

        /* Set flag for bootloader to know that command is received from host */
        cyBle_cmdReceivedFlag = 1u;

        /* Indicate that request was handled */
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
    else
    {
    }
    
    return(gattErr);
}


/****************************************************************************** 
##Function Name: CyBle_BtssPrepareWriteRequestEventHandler
*******************************************************************************

Summary:
 Handles a Prepare Write Request event for the Bootloader Service.

Parameters:
 CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam: The pointer to the data
                                                 received with a Prepare Write
                                                 Request event for the
                                                 Bootloader service.

Return:
 None.

******************************************************************************/
void CyBle_BtssPrepareWriteRequestEventHandler(CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam)
{
    if(eventParam->baseAddr[0u].handleValuePair.attrHandle == cyBle_btss.btServiceInfo[0u].btServiceCharHandle)
    {
        if(eventParam->currentPrepWriteReqCount == 1u)
        {
            /* Send Prepare Write Response which identifies acknowledgement for
            *  long characteristic value write.
            */
            CyBle_GattsPrepWriteReqSupport(CYBLE_GATTS_PREP_WRITE_SUPPORT);
            
            cyBle_cmdLength = 0u;
        }

        /* Indicate that request was handled */
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
}


/****************************************************************************** 
##Function Name: CyBle_BtssExecuteWriteRequestEventHandler
*******************************************************************************

Summary:
 Handles an Execute Write Request event for the Bootloader Service.

Parameters:
 CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam: The pointer to the data that came 
                                           with a Write Request for the
                                           Bootloader service.

Return:
 None.

******************************************************************************/
void CyBle_BtssExecuteWriteRequestEventHandler(CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam)
{
    if(eventParam->baseAddr[0u].handleValuePair.attrHandle == cyBle_btss.btServiceInfo[0u].btServiceCharHandle)
    {        
        cyBle_btsBuffPtr = eventParam->baseAddr[0u].handleValuePair.value.val;
        
        /* Extract length of command data and add control bytes to data 
        * length to get command length.
        */
        cyBle_cmdLength = ((((uint16) cyBle_btsBuffPtr[CYBLE_BTS_COMMAND_DATA_LEN_OFFSET + 1u]) << 8u) | 
                            (uint16) cyBle_btsBuffPtr[CYBLE_BTS_COMMAND_DATA_LEN_OFFSET]) +
                            CYBLE_BTS_COMMAND_CONTROL_BYTES_NUM;
        
        if(cyBle_cmdLength > CYBLE_BTS_COMMAND_MAX_LENGTH)
        {
            cyBle_cmdLength = CYBLE_BTS_COMMAND_MAX_LENGTH;
        }

        /* Set flag for bootloader to know that command is received from host */
        cyBle_cmdReceivedFlag = 1u;

        /* Indicate that request was handled */
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }
}


/******************************************************************************
##Function Name: CyBtldrCommStart
*******************************************************************************

Summary:
 This function does nothing but it is required for code consistency.

Parameters:
 None.

Return:
 None.

******************************************************************************/
void CyBtldrCommStart(void)
{
}


/******************************************************************************
##Function Name: CyBtldrCommStop
*******************************************************************************

Summary:
 This function does nothing but it is required for code consistency.

Parameters:
 None.

Return:
 None.

******************************************************************************/
void CyBtldrCommStop(void)
{
}


/******************************************************************************
##Function Name: CyBtldrCommReset
*******************************************************************************

Summary:
 This function does nothing but it is required for code consistency.

Parameters:
 None.

Return:
 None.

******************************************************************************/
void CyBtldrCommReset(void)
{
}


/******************************************************************************
##Function Name: CyBtldrCommWrite
*******************************************************************************

Summary:
 Requests that the provided size (number of bytes) should be written from the
 input data buffer to the host device. This function in turn invokes the
 CyBle_GattsNotification() API to sent the data. If a notification is
 accepted, the function returns CYRET_SUCCESS. The timeOut parameter is ignored
 in this case.

Parameters:
 data: The pointer to the buffer containing data to be written.
 size: The number of bytes from the data buffer to write.
 count: The pointer to where the BLE component will write the the number 
        of written bytes, generally the same as the size.
 timeOut: Ignored. Used for consistency.

Return:
 The return value is of type cystatus:
    * CYRET_SUCCESS - Indicates if a notification is successful.
    * CYRET_LOCKED - Notifications are disabled.
    * CYRET_UNKNOWN - Failed to send notification to the host.

******************************************************************************/
cystatus CyBtldrCommWrite(uint8 *data, uint16 size, uint16 *count, uint8 timeOut)
{
    cystatus status = CYRET_LOCKED;
    uint8 *btCmdCharacteristicPtr;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfParam;

    if(CYBLE_IS_NOTIFICATION_ENABLED(cyBle_btss.btServiceInfo[0u].btServiceCharDescriptors[0u]))
    {
        /* Get pointer to Bootloader Characteristic value to store response there */
        btCmdCharacteristicPtr =
            CYBLE_GATT_DB_ATTR_GET_ATTR_GEN_PTR(cyBle_btss.btServiceInfo[0u].btServiceCharHandle);
        
        /* Prepare and send notification with the bootloader packet emulator
        * response if notifications are enabled.
        */
        (void)memcpy((void *)btCmdCharacteristicPtr, (const void *)data, size);

        /* Fill notification parameters structure to be send with  
        * CyBle_GattsNotification().
        */
        ntfParam.value.len = size;
        ntfParam.value.val = btCmdCharacteristicPtr;
        ntfParam.attrHandle = cyBle_btss.btServiceInfo[0u].btServiceCharHandle;
        
        if(CyBle_GattsNotification(cyBle_connHandle, &ntfParam) == CYBLE_ERROR_OK)
        {
            *count = size;
            status = CYRET_SUCCESS;
        }
        else
        {
            *count = 0u;
            status = CYRET_UNKNOWN;
        }
    }

    /* Unreferenced variable warning workaround */
    if(timeOut != 0u)
    {
    }
    
    return(status);
}


/******************************************************************************
##Function Name: CyBtldrCommRead
*******************************************************************************

Summary:
 Requests that the provided size (number of bytes) is read from the host device
 and stored in the provided data buffer. Once the read is done, the "count" is
 endorsed with the number of bytes written. The timeOut parameter is used to
 provide an upper bound on the time that the function is allowed to operate. If
 the read completes early, it should return success code as soon as possible.
 If the read was not successful before the allocated time has expired, it
 should return an error.

Parameters:
 uint8 *data: The pointer to the buffer to store data from the host controller.
 uint16 size: The number of bytes to read into the data buffer.
 uint16 *count: The pointer to where the BLE component will write the count of the
                number of read bytes.
 uint8 timeOut: The amount of time (in units of 10 milliseconds) for which the
                BLE component should wait before indicating communications
                timed out.

Return:
 The return value is of type cystatus:
    * CYRET_SUCCESS - A command was successfully read.
    * CYRET_BAD_DATA - The size of the command exceeds the buffer.
    * CYRET_TIMEOUT - The host controller did not respond to the read in 10 
                      milliseconds * timeOut milliseconds.

Side Effects:
 CyBle_ProcessEvents() is called as a part of this function.

******************************************************************************/
cystatus CyBtldrCommRead(uint8 *data, uint16 size, uint16 *count, uint8 timeOut)
{
    uint32 timeoutUs;
    cystatus status = CYRET_TIMEOUT;

    /* Convert from 10ms units to 100us units */
    timeoutUs = ((uint32) 100u * timeOut);
    
    while(0u != timeoutUs)
    {
        /* Process BLE events */
        CyBle_ProcessEvents();
        
        if(cyBle_cmdReceivedFlag == 1u)
        {
            /* Clear command receive flag */
            cyBle_cmdReceivedFlag = 0u;

            if(cyBle_cmdLength < size)
            {
                (void)memcpy((void *) data, (const void *) cyBle_btsBuffPtr, cyBle_cmdLength);

                /* Return actual received command length */
                *count = cyBle_cmdLength;
                
                status = CYRET_SUCCESS;
            }
            else
            {
                data = NULL;
                *count = 0u;
                status = CYRET_BAD_DATA;
            }
            break;
        }
        /* Wait 100 us and update timeout counter */
        CyDelayUs(CYBLE_BTS_TIMEOUT_PERIOD);
        timeoutUs -= CYBLE_BTS_TIMEOUT_PERIOD;
    }

    return(status);
}


/* [] END OF FILE */
