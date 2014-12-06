/*******************************************************************************
* File Name: common.h
*
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  Common BLE application header.
*
*******************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>

#if(CYBLE_GATT_ROLE_SERVER)
#include "server.h"
#endif /* (CYBLE_GATT_ROLE_SERVER) */
#if(CYBLE_GATT_ROLE_CLIENT)
#include "client.h"
#endif /* (CYBLE_GATT_ROLE_CLIENT) */

#define DEBUG_OUT                   

#define ENABLED                     (1u)
#define DISABLED                    (0u)

#define LED_ON                      (0u)
#define LED_OFF                     (1u)

/***************************************
*        External Function Prototypes
***************************************/
int _write(int file, char *ptr, int len);
void DebugOut(uint32 event, void* eventParam);


/***************************************
* External data references
***************************************/
extern CYBLE_API_RESULT_T apiResult;
extern volatile uint32 mainTimer;

/* [] END OF FILE */
