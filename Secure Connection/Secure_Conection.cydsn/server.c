/*******************************************************************************
* File Name: server.c
*
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  Common BLE application code for server devices.
*
*******************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>
#include "common.h"

extern CYBLE_GAPP_DISC_PARAM_T cyBle_discoveryParam;


/*******************************************************************************
* Function Name: StartAdvertisement
********************************************************************************
*
* Summary:
*   Initiates the advertisement procedure.
*   Prints the Device Address.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void StartAdvertisement(uint8 advType)
{
    uint16 i;
    CYBLE_GAP_BD_ADDR_T localAddr;
    
    if(advType == ADV_STATE_GENERAL)
    {
        cyBle_discoveryParam.advFilterPolicy = SCAN_ANY_CONNECT_ANY;
    }
    else
    {
        cyBle_discoveryParam.advFilterPolicy  = SCAN_WHITELIST_CONNECT_WHITELIST;
    }
    
    apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        printf("StartAdvertisement API Error: %x \r\n", (int) apiResult);
    }
    else
    {
        printf("Start Advertisement with addr: ");
        CyBle_GetDeviceAddress(&localAddr);
        for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
        {
            printf("%2.2x", localAddr.bdAddr[i-1]);
        }
        printf("\r\n");
    }
}

/*******************************************************************************
* Function Name: ServerDebugOut
********************************************************************************
*
* Summary:
*   GATT server specific event handler.
*
* Parameters:
*  event - the event code.
*  *eventParam - the event parameters.
*
* Return:
*   None.
*
*******************************************************************************/
void ServerDebugOut(uint32 event, void* eventParam)
{
    uint16 i;
    switch(event)
    {
        case CYBLE_EVT_GATTS_WRITE_REQ:
            printf("EVT_GATT_WRITE_REQ: %x = ",((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.attrHandle);
            for(i = 0; i < ((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.value.len; i++)
            {
                printf("%2.2x ", ((CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam)->handleValPair.value.val[i]);
            }
            printf("\r\n");
            break;

        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            printf("EVT_GATTS_XCNHG_MTU_REQ \r\n");
            break;

        case CYBLE_EVT_GATTS_HANDLE_VALUE_CNF:
            printf("EVT_GATTS_HANDLE_VALUE_CNF \r\n");
            break;

        default:
        #if (0)
            printf("unknown event: %lx \r\n", event);
        #endif
            break;
    }
}


/* [] END OF FILE */
