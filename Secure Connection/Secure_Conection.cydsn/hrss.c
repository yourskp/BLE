/*******************************************************************************
* File Name: hrss.c
*
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file contains HRS service related code.
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "hrss.h"

/* Heart Rate Measurement characteristic data structure */
CYBLE_HRS_HRM_T hrsHeartRate;
uint8 heartRateSimulation;

/* Heart Rate Service callback */
void HeartRateCallBack(uint32 event, void* eventParam)
{
    if(0u != eventParam)
    {
        /* This dummy operation is to avoid warning about unused eventParam */
    }

    switch(event)
    {
        case CYBLE_EVT_HRSS_NOTIFICATION_ENABLED:
            heartRateSimulation = ENABLED;
            printf("Heart Rate Measurement Notification is Enabled \r\n");
            break;

        case CYBLE_EVT_HRSS_NOTIFICATION_DISABLED:
            heartRateSimulation = DISABLED;
            printf("Heart Rate Measurement Notification is Disabled \r\n");
            break;

        default:
            printf("Unknown HRS event: %lx \r\n", event);
            break;
    }
}

/*******************************************************************************
* Function Name: HrsInit
********************************************************************************
*
* Summary:
*  Initializes the Heart Rate application global variables.
*
* Parameters:
*  None
*
* Return:
*  None.
*
*******************************************************************************/

void HrsInit(void)
{
    CyBle_HrsRegisterAttrCallback(HeartRateCallBack);

    hrsHeartRate.flags = 0u;
    hrsHeartRate.heartRateValue = 0u;
}

/***************************************
*        Functions
***************************************/

/*******************************************************************************
* Function Name: HrssSendHeartRateNtf
********************************************************************************
*
* Summary:
*  Packs the Heart Rate Measurement characteristic structure into the
*  uint8 array prior to sending it to the collector. Also clears the
*  CYBLE_HRS_HRM_HRVAL16, CYBLE_HRS_HRM_ENEXP and CYBLE_HRS_HRM_RRINT flags.
*
* Parameters:
*  attHandle:  Pointer to the handle which consists of device ID and ATT
*              connection ID.
*
* Return:
*  CYBLE_API_RESULT_T: API result will state if API succeeded
*                      (CYBLE_ERROR_OK)
*                      or failed with error codes.
*
*******************************************************************************/
void HrssSendHeartRateNtf()
{
    uint8 pdu[CYBLE_HRS_HRM_CHAR_LEN];
    uint8 nextPtr;

    /* Flags field is always the first byte */
    pdu[0u] = hrsHeartRate.flags & (uint8) ~CYBLE_HRS_HRM_HRVAL16;

    /* If the Heart Rate value exceeds one byte */
    if(hrsHeartRate.heartRateValue > 0x00FFu)
    {
        /* then set the CYBLE_HRS_HRM_HRVAL16 flag */
        pdu[0u] |= CYBLE_HRS_HRM_HRVAL16;
        /* and set the full 2-bytes Heart Rate value */
        CyBle_Set16ByPtr(&pdu[1u], hrsHeartRate.heartRateValue);
        /* The next data will be located beginning from 3rd byte */
        nextPtr = 3u;
    }
    else
    {
        /* Else leave the CYBLE_HRS_HRM_HRVAL16 flag remains being cleared */
        /* and set only LSB of the  Heart Rate value */
        pdu[1u] = (uint8) hrsHeartRate.heartRateValue;
        /* The next data will be located beginning from 2nd byte */
        nextPtr = 2u;
    }
	
	do
    {
        CyBle_ProcessEvents();
    }
    while(CyBle_GattGetBusStatus() == CYBLE_STACK_STATE_BUSY);

    apiResult = CyBle_HrssSendNotification(cyBle_connHandle, CYBLE_HRS_HRM, nextPtr, pdu);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        printf("HrssSendHeartRateNtf API Error: %x \r\n", apiResult);
    }
    else
    {
        printf("Heart Rate Notification is sent successfully, Heart Rate = %d \r\n", hrsHeartRate.heartRateValue);
    }
}

/*******************************************************************************
* Function Name: SimulateHeartRate
********************************************************************************
*
* Summary:
*   Simulates all constituents of the Heart Rate Measurement characterstic.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void SimulateHeartRate(void)
{
    static uint16 heartRate             = SIM_HEART_RATE_MIN;

    /* Heart Rate simulation */
    heartRate += SIM_HEART_RATE_INCREMENT;
    if(heartRate > SIM_HEART_RATE_MAX)
    {
        heartRate = SIM_HEART_RATE_MIN;
    }

    HrssSetHeartRate(heartRate);
    
    HrssSendHeartRateNtf();
}

/* [] END OF FILE */
