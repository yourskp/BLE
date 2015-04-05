/******************************************************************************
* Project Name		: ADD YOUR PROJECT NAME HERE
* File Name			: main.c
* Version 			: 1.0
* Device Used		: CY8C4247LQI-BL483
* Software Used		: ADD PSOC CREATOR VERSION USED FOR YOUR PROJECT
* Compiler    		: ARM GCC 4.8.4
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit 
* Owner				: ADD YOUR EMAIL ID HERE
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
*                           THEORY OF OPERATION
*******************************************************************************
* ADD A SHORT DESCRIPTION OF WHAT THIS PROJECT DEMONSTRATES. KEEP THIS TO 
* <10 LINES
*******************************************************************************
*                                 NOTES
*******************************************************************************
* ADD ANYTHING THE USER SHOULD BE AWARE OF.
* EXAMPLE - THIS PROJECT WILL ONLY WORK WITH IOS DEVICES THAT SUPPORTS CURRENT 
* TIME SERVICE (CTS). DELETE THIS SECTION IF NOT USED.
*
******************************************************************************
*                         HARDWARE CONFIGURATION
*******************************************************************************
* ADD REQUIRED HARDWARE CONNECTION DETAILS TO TEST THIS EXAMPLE PROJECT
******************************************************************************/

/******************************************************************************
*                           Header Files
*******************************************************************************/
#include <Application.h>
#include <BLE Interface.h>
#include <Configuration.h>
#include <Init.h>
#include <LowPower.h>
#include <project.h>
/* ADD ADDITIONAL INCLUDES HERE */

/******************************************************************************
*                           Extern Declarations
*******************************************************************************/
/* ADD EXTERN DECLARATIONS HERE */
/* USE THIS IF IT'S UNAVOIDABLE */

/******************************************************************************
*                           Global Variables
*******************************************************************************/
/* ADD GLOBAL VARIABLES HERE */
/* KEEP THE GLOBALS TO A MINIMUM AND ALWAYS ABSTRACT THEM WITH A GETTER/SETTER  
 * FUNCTIONS (AVOID EXTERNS)*/

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*        System entry point. This calls the BLE and other peripheral Component
* APIs for achieving the desired system behaviour
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main()
{
    /* This function will initialize all the Components and system variables */
	InitializeSystem();

    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1)
    {
        /***********************************************************************
        *  Process all the pending BLE events in the stack
        ***********************************************************************/
        BLE_Run();
         
        /***********************************************************************
        *  Put BLE sub system in DeepSleep mode when it is idle
        ***********************************************************************/ 
        BLE_ManagePower();
    
        /***********************************************************************
        *  Run your application specific code here
        ***********************************************************************/  
        if(Application_GetPowerState() == ACTIVE)
        {
            /* ADD YOUR APPLICATION MAIN LOOP CODE IN THIS FUNCTION */
            Application_Run(); 
        }
        
        /***********************************************************************
        *  Process application and system power modes 
        ***********************************************************************/
        Application_ManagePower();   
        
        System_ManagePower();
        
    }	/* End of while(1) */
    
} /* End of main */

/* [] END OF FILE */
