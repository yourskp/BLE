/*******************************************************************************
* File Name: Sleep.c  
* Version 2.10
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Sleep.h"

#define SetP4PinDriveMode(shift, mode)  \
    do { \
        Sleep_PC =   (Sleep_PC & \
                                (uint32)(~(uint32)(Sleep_DRIVE_MODE_IND_MASK << (Sleep_DRIVE_MODE_BITS * (shift))))) | \
                                (uint32)((uint32)(mode) << (Sleep_DRIVE_MODE_BITS * (shift))); \
    } while (0)


/*******************************************************************************
* Function Name: Sleep_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None 
*  
*******************************************************************************/
void Sleep_Write(uint8 value) 
{
    uint8 drVal = (uint8)(Sleep_DR & (uint8)(~Sleep_MASK));
    drVal = (drVal | ((uint8)(value << Sleep_SHIFT) & Sleep_MASK));
    Sleep_DR = (uint32)drVal;
}


/*******************************************************************************
* Function Name: Sleep_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  Sleep_DM_STRONG     Strong Drive 
*  Sleep_DM_OD_HI      Open Drain, Drives High 
*  Sleep_DM_OD_LO      Open Drain, Drives Low 
*  Sleep_DM_RES_UP     Resistive Pull Up 
*  Sleep_DM_RES_DWN    Resistive Pull Down 
*  Sleep_DM_RES_UPDWN  Resistive Pull Up/Down 
*  Sleep_DM_DIG_HIZ    High Impedance Digital 
*  Sleep_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void Sleep_SetDriveMode(uint8 mode) 
{
	SetP4PinDriveMode(Sleep__0__SHIFT, mode);
}


/*******************************************************************************
* Function Name: Sleep_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro Sleep_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 Sleep_Read(void) 
{
    return (uint8)((Sleep_PS & Sleep_MASK) >> Sleep_SHIFT);
}


/*******************************************************************************
* Function Name: Sleep_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 Sleep_ReadDataReg(void) 
{
    return (uint8)((Sleep_DR & Sleep_MASK) >> Sleep_SHIFT);
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(Sleep_INTSTAT) 

    /*******************************************************************************
    * Function Name: Sleep_ClearInterrupt
    ********************************************************************************
    *
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 Sleep_ClearInterrupt(void) 
    {
		uint8 maskedStatus = (uint8)(Sleep_INTSTAT & Sleep_MASK);
		Sleep_INTSTAT = maskedStatus;
        return maskedStatus >> Sleep_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 


/* [] END OF FILE */
