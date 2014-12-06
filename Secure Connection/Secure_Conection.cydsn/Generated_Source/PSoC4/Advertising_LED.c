/*******************************************************************************
* File Name: Advertising_LED.c  
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
#include "Advertising_LED.h"

#define SetP4PinDriveMode(shift, mode)  \
    do { \
        Advertising_LED_PC =   (Advertising_LED_PC & \
                                (uint32)(~(uint32)(Advertising_LED_DRIVE_MODE_IND_MASK << (Advertising_LED_DRIVE_MODE_BITS * (shift))))) | \
                                (uint32)((uint32)(mode) << (Advertising_LED_DRIVE_MODE_BITS * (shift))); \
    } while (0)


/*******************************************************************************
* Function Name: Advertising_LED_Write
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
void Advertising_LED_Write(uint8 value) 
{
    uint8 drVal = (uint8)(Advertising_LED_DR & (uint8)(~Advertising_LED_MASK));
    drVal = (drVal | ((uint8)(value << Advertising_LED_SHIFT) & Advertising_LED_MASK));
    Advertising_LED_DR = (uint32)drVal;
}


/*******************************************************************************
* Function Name: Advertising_LED_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  Advertising_LED_DM_STRONG     Strong Drive 
*  Advertising_LED_DM_OD_HI      Open Drain, Drives High 
*  Advertising_LED_DM_OD_LO      Open Drain, Drives Low 
*  Advertising_LED_DM_RES_UP     Resistive Pull Up 
*  Advertising_LED_DM_RES_DWN    Resistive Pull Down 
*  Advertising_LED_DM_RES_UPDWN  Resistive Pull Up/Down 
*  Advertising_LED_DM_DIG_HIZ    High Impedance Digital 
*  Advertising_LED_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void Advertising_LED_SetDriveMode(uint8 mode) 
{
	SetP4PinDriveMode(Advertising_LED__0__SHIFT, mode);
}


/*******************************************************************************
* Function Name: Advertising_LED_Read
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
*  Macro Advertising_LED_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 Advertising_LED_Read(void) 
{
    return (uint8)((Advertising_LED_PS & Advertising_LED_MASK) >> Advertising_LED_SHIFT);
}


/*******************************************************************************
* Function Name: Advertising_LED_ReadDataReg
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
uint8 Advertising_LED_ReadDataReg(void) 
{
    return (uint8)((Advertising_LED_DR & Advertising_LED_MASK) >> Advertising_LED_SHIFT);
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(Advertising_LED_INTSTAT) 

    /*******************************************************************************
    * Function Name: Advertising_LED_ClearInterrupt
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
    uint8 Advertising_LED_ClearInterrupt(void) 
    {
		uint8 maskedStatus = (uint8)(Advertising_LED_INTSTAT & Advertising_LED_MASK);
		Advertising_LED_INTSTAT = maskedStatus;
        return maskedStatus >> Advertising_LED_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 


/* [] END OF FILE */
