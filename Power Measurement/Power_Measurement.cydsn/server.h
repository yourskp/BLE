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

void StartAdvertisement(void);
void ServerDebugOut(uint32 event, void* eventParam);

/* [] END OF FILE */
