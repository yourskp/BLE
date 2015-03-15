/*******************************************************************************
* File Name: WriteUserSFlash.h
*
* Version: 1.0
*
* Description:
*  This file contains the headers and constants for user SFLASH write API.
*
* Owner:
*  sey@cypress.com, kris@cypress.com
*
* Hardware Dependency:
*  1. PSoC 4 BLE device
*
* Code Tested With:
*  1. PSoC Creator 3.1 SP1
*  2. ARM GCC 4.8.4
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef WRITE_USER_SFLASH_H
#define WRITE_USER_SFLASH_H

#include <project.h>

#define LOAD_FLASH				0x80000004
#define WRITE_USER_SFLASH_ROW	0x80000018
	
uint32 WriteUserSFlashRow(uint8 userrow_num, uint32 *write_value);   

#endif

/* [] END OF FILE */
