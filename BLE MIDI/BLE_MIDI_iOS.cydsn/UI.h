/*******************************************************************************
* File Name: UI.h
*
* Description:
*  This file contains CapSense UI routines declarations and constants
* 
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef UI_H
#define UI_H
    
#include <MIDI.h>
    
/***************************************
*       UI Enums
***************************************/
typedef enum
{
    UI_IDLE_STATE,
    UI_SCANNING_STATE,
    UI_RESULT_STATE
} UI_STATE;

/***************************************
*       UI Constants
***************************************/
#define NO_FINGER_ON_SLIDER                         0xFFFF
#define NO_FINGER_ON_BUTTON                         0xFFFF
#define NO_UI_STATE_CHANGE                          0xFFEE

#define MAX_SLIDER_CENTROID                         (100u)
#define NUMBER_OF_CENTROID_SEGMENTS                 (10)
#define CENTROID_DIVIDE_FACTOR                      (MAX_SLIDER_CENTROID/NUMBER_OF_CENTROID_SEGMENTS)

#define NOTE1_VALUE                                 (0u)
#define NOTE2_VALUE                                 (1u)
#define NOTE3_VALUE                                 (2u)
#define NOTE4_VALUE                                 (3u)
#define NOTE5_VALUE                                 (4u)

#define IsUIEnabled()                               IsMidiEnabled()

/***************************************
*       Function Prototypes
***************************************/
void UI_Start(void);
uint16 UI_Run(void);
    
#endif

/* [] END OF FILE */
