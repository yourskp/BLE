/*******************************************************************************
* File Name: User_Button.c  
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
#include "User_Button.h"

#define SetP4PinDriveMode(shift, mode)  \
    do { \
        User_Button_PC =   (User_Button_PC & \
                                (uint32)(~(uint32)(User_Button_DRIVE_MODE_IND_MASK << (User_Button_DRIVE_MODE_BITS * (shift))))) | \
                                (uint32)((uint32)(mode) << (User_Button_DRIVE_MODE_BITS * (shift))); \
    } while (0)


/*******************************************************************************
* Function Name: User_Button_Write
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
void User_Button_Write(uint8 value) 
{
    uint8 drVal = (uint8)(User_Button_DR & (uint8)(~User_Button_MASK));
    drVal = (drVal | ((uint8)(value << User_Button_SHIFT) & User_Button_MASK));
    User_Button_DR = (uint32)drVal;
}


/*******************************************************************************
* Function Name: User_Button_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  User_Button_DM_STRONG     Strong Drive 
*  User_Button_DM_OD_HI      Open Drain, Drives High 
*  User_Button_DM_OD_LO      Open Drain, Drives Low 
*  User_Button_DM_RES_UP     Resistive Pull Up 
*  User_Button_DM_RES_DWN    Resistive Pull Down 
*  User_Button_DM_RES_UPDWN  Resistive Pull Up/Down 
*  User_Button_DM_DIG_HIZ    High Impedance Digital 
*  User_Button_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void User_Button_SetDriveMode(uint8 mode) 
{
	SetP4PinDriveMode(User_Button__0__SHIFT, mode);
}


/*******************************************************************************
* Function Name: User_Button_Read
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
*  Macro User_Button_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 User_Button_Read(void) 
{
    return (uint8)((User_Button_PS & User_Button_MASK) >> User_Button_SHIFT);
}


/*******************************************************************************
* Function Name: User_Button_ReadDataReg
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
uint8 User_Button_ReadDataReg(void) 
{
    return (uint8)((User_Button_DR & User_Button_MASK) >> User_Button_SHIFT);
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(User_Button_INTSTAT) 

    /*******************************************************************************
    * Function Name: User_Button_ClearInterrupt
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
    uint8 User_Button_ClearInterrupt(void) 
    {
		uint8 maskedStatus = (uint8)(User_Button_INTSTAT & User_Button_MASK);
		User_Button_INTSTAT = maskedStatus;
        return maskedStatus >> User_Button_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 


/* [] END OF FILE */
