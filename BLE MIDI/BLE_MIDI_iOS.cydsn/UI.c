/*******************************************************************************
* File Name: UI.c
*
* Description:
*  This file contains CapSense UI routines and implementation
* 
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <Configuration.h>
#include <Project.h>
#include <UI.h>

/***************************************
*        Global and Static variables
***************************************/
UI_STATE uiState = UI_IDLE_STATE;
uint16 currentSliderPosition = NO_FINGER_ON_SLIDER;
uint16 previousSliderPosition = NO_FINGER_ON_SLIDER;

/*******************************************************************************
* Function Name: UI_Start
********************************************************************************
* Summary:
*        Start the UI components and systems
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void UI_Start(void)
{
#if CAPSENSE_ENABLED
	/*Initialize CapSense component and initialize baselines*/
	CapSense_Start();
	CapSense_InitializeAllBaselines();
    
    uiState = UI_IDLE_STATE;
#endif
}

/*******************************************************************************
* Function Name: UI_Run
********************************************************************************
* Summary:
*        Runs the user interface engine for the MIDI interface
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
uint16 UI_Run(void)
{
    uint16 uiChangeStatus = NO_UI_STATE_CHANGE;
    
    if(IsUIEnabled())
    {
#if CAPSENSE_ENABLED
        switch(uiState)
        {
            case UI_IDLE_STATE:
                CapSense_ScanWidget(CapSense_LINEARSLIDER0__LS); /* Initiate the sensor scan */
                uiState = UI_SCANNING_STATE;
            break;
            
            case UI_SCANNING_STATE:
                if(CapSense_IsBusy() == 0)
                {
                    CapSense_UpdateEnabledBaselines(); /* Keep the baseline updated with current performance numbers */
                    currentSliderPosition = CapSense_GetCentroidPos(CapSense_LINEARSLIDER0__LS);
                    uiState = UI_RESULT_STATE;  
                }
            break;
            
            case UI_RESULT_STATE:
                if(currentSliderPosition != previousSliderPosition)
                {
                    uiChangeStatus = currentSliderPosition;
                }
                previousSliderPosition = currentSliderPosition;
                uiState = UI_IDLE_STATE;  
            break;
                
            default:
            break;
        }
#endif    
    }
    
    return uiChangeStatus;
}

/* [] END OF FILE */
