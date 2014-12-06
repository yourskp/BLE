/*******************************************************************************
* File Name: server.h
*
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  Common BLE application header for server devices.
*
*******************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>

#define ADV_STATE_GENERAL                       1
#define ADV_STATE_WHITELIST                     2

#define SCAN_ANY_CONNECT_ANY                    0
#define SCAN_WHITELIST_CONNECT_WHITELIST        3

void StartAdvertisement(uint8);
void ServerDebugOut(uint32 event, void* eventParam);

/* [] END OF FILE */
