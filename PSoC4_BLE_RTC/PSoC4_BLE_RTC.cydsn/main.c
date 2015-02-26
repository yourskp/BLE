/*****************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Owner - kris@cypress.com
*
* Project Description - This project demonstrates PSoC 4 BLE acting as a  BLE 
* time client that syncs time from an iOS device (iOS support time server out 
* of the box) and runs an RTC in PSoC 4 BLE.
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <BLEConnection.h>
#include <Configuration.h>
#include <device.h>
#include <Init.h>
#include <LowPower.h>
#include <project.h>
#include <RTC.h>
#include <Stdio.h>
#include <Stdlib.h>

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Main function.
*
* Parameters:  
*  None
*
* Return: 
*  Never
*
*******************************************************************************/
int main(void)
{
    InitializeSystem();
	
    while(1)
    {
        /* Handles any pending BLE events and allows data communication over BLE. Must be called in system main loop */
        BLE_Run(); 
        
        /* configure the system in lowest possible mode between BLE and RTC events */
        HandleLowPowerMode();
        
        /* Update current RTC value on the UART console */
        RTC_UI_Update();
    }
}

/* [] END OF FILE */
