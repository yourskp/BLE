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
uint16 currentButtonPosition = NO_FINGER_ON_BUTTON;
uint16 previousButtonPosition = NO_FINGER_ON_BUTTON;
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
#if CAPSENSE_SLIDER                
                CapSense_ScanWidget(CapSense_LINEARSLIDER0__LS); /* Initiate the sensor scan */
#else
                CapSense_ScanEnabledWidgets(); /* Initiate the sensor scan */
#endif    
                uiState = UI_SCANNING_STATE;
            break;
            
            case UI_SCANNING_STATE:
                if(CapSense_IsBusy() == 0)
                {
                    CapSense_UpdateEnabledBaselines(); /* Keep the baseline updated with current performance numbers */
                    
#if CAPSENSE_SLIDER                     
                    currentSliderPosition = CapSense_GetCentroidPos(CapSense_LINEARSLIDER0__LS);
                    
                    if(currentSliderPosition != NO_FINGER_ON_SLIDER)
                    {
                        currentSliderPosition =  currentSliderPosition / CENTROID_DIVIDE_FACTOR;
                    }
                    
                    uiState = UI_RESULT_STATE;  
#else
                    if(CapSense_CheckIsWidgetActive(CapSense_NOTE1__BTN))
                    {
                        currentButtonPosition = NOTE1_VALUE;    
                    }
                    else if(CapSense_CheckIsWidgetActive(CapSense_NOTE2__BTN))
                    {
                        currentButtonPosition = NOTE2_VALUE;    
                    }
                    else if(CapSense_CheckIsWidgetActive(CapSense_NOTE3__BTN))
                    {
                        currentButtonPosition = NOTE3_VALUE;    
                    }
                    else if(CapSense_CheckIsWidgetActive(CapSense_NOTE4__BTN))
                    {
                        currentButtonPosition = NOTE4_VALUE;    
                    }
                    else if(CapSense_CheckIsWidgetActive(CapSense_NOTE5__BTN))
                    {
                        currentButtonPosition = NOTE5_VALUE;    
                    }
                    else
                    {
                        currentButtonPosition = NO_FINGER_ON_SLIDER; 
                    }
                    
                    uiState = UI_RESULT_STATE;
#endif    
                }
            break;
            
            case UI_RESULT_STATE:
#if CAPSENSE_SLIDER                           
                if(currentSliderPosition != previousSliderPosition)
                {
                    uiChangeStatus = currentSliderPosition;
                    previousSliderPosition = currentSliderPosition;
                }
#else
                if(currentButtonPosition != previousButtonPosition)
                {
                    uiChangeStatus = currentButtonPosition;
                    previousButtonPosition = currentButtonPosition;
                }
#endif    
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
