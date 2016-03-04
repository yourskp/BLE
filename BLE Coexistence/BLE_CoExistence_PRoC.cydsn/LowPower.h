/******************************************************************************
* Project Name		: BLE_CoExistence
* File Name			: LowPower.h
* Version 			: 1.0
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.3 SP1
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
* Description - Header file for low power implementation routines
*******************************************************************************/

#if !defined(LOW_POWER_H)
#define LOW_POWER_H
    
#include <Configuration.h>

/*****************************************************
*                  Enums and macros
*****************************************************/    
typedef enum {
    WAKEUP_SLEEP,
    WAKEUP_DEEPSLEEP,
    ACTIVE,
    SLEEP,
    DEEPSLEEP
} POWER_MODE_T;    
    
#if BLE_COEXISTENCE_ENABLE
#define CYREG_RADIO_TX_RX_MUX_REGISTER          0x40030008
#define RADIO_TX_RX_SEL                         0x0330
#define RADIO_TX_RX_MUX_MASK                    0x00003F3F
#define BLESS_MUX_INPUT_MASK                    0x80000203
    
#define CYREG_BLE_BLELL_CONN_INTR_REG           (* (reg32 *) (CYREG_BLE_BLELL_CONN_INTR))
#define START_CE_MASK                           0x00000008
#define CLOSE_CE_MASK                           0x00000010
    
#define CYREG_BLE_BLELL_ADV_INTR_REG            (* (reg32 *) (CYREG_BLE_BLELL_ADV_INTR))
#define START_ADV_MASK                           0x00000001
#define CLOSE_ADV_MASK                           0x00000002    
    
#define CYREG_BLE_BLELL_CONN_INTR_MASK_REG      (* (reg32 *) (CYREG_BLE_BLELL_CONN_INTR_MASK))
#define START_CE_INTERRUPT_ENABLE_MASK          0x00000008 
    
#define CYREG_BLE_BLELL_ADV_CONFIG_REG          (* (reg32 *) (CYREG_BLE_BLELL_ADV_CONFIG))
#define START_ADV_INTERRUPT_ENABLE_MASK         0x00000001
    
#define CYREG_BLE_BLESS_LL_DSM_INTR_STAT_REG    (* (reg32 *) (CYREG_BLE_BLESS_LL_DSM_INTR_STAT))
    
#define CYREG_BLE_BLELL_EVENT_ENABLE_REG        (* (reg32 *) (CYREG_BLE_BLELL_EVENT_ENABLE))
#define ADVERTISEMENT_INTERRUPT_MASK            0x00000001
#define CONNECTION_INTERRUPT_MASK               0x00000008
    
#define DSM_EXITED_INTERRUPT_MASK               0x00000002    
#endif /* End of #if BLE_COEXISTENCE_ENABLE */

/*****************************************************
*                  Function declarations
*****************************************************/ 
void System_ManagePower();

#endif /* End of #if !defined(LOW_POWER_H) */

/* [] END OF FILE */
