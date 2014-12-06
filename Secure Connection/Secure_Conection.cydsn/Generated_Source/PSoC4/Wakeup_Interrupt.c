/*******************************************************************************
* File Name: Wakeup_Interrupt.c  
* Version 1.70
*
*  Description:
*   API for controlling the state of an interrupt.
*
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include <cydevice_trm.h>
#include <CyLib.h>
#include <Wakeup_Interrupt.h>

#if !defined(Wakeup_Interrupt__REMOVED) /* Check for removal by optimization */

/*******************************************************************************
*  Place your includes, defines and code here 
********************************************************************************/
/* `#START Wakeup_Interrupt_intc` */

/* `#END` */

extern cyisraddress CyRamVectors[CYINT_IRQ_BASE + CY_NUM_INTERRUPTS];

/* Declared in startup, used to set unused interrupts to. */
CY_ISR_PROTO(IntDefaultHandler);


/*******************************************************************************
* Function Name: Wakeup_Interrupt_Start
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void Wakeup_Interrupt_Start(void)
{
    /* For all we know the interrupt is active. */
    Wakeup_Interrupt_Disable();

    /* Set the ISR to point to the Wakeup_Interrupt Interrupt. */
    Wakeup_Interrupt_SetVector(&Wakeup_Interrupt_Interrupt);

    /* Set the priority. */
    Wakeup_Interrupt_SetPriority((uint8)Wakeup_Interrupt_INTC_PRIOR_NUMBER);

    /* Enable it. */
    Wakeup_Interrupt_Enable();
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_StartEx
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it.
*
* Parameters:  
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void Wakeup_Interrupt_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    Wakeup_Interrupt_Disable();

    /* Set the ISR to point to the Wakeup_Interrupt Interrupt. */
    Wakeup_Interrupt_SetVector(address);

    /* Set the priority. */
    Wakeup_Interrupt_SetPriority((uint8)Wakeup_Interrupt_INTC_PRIOR_NUMBER);

    /* Enable it. */
    Wakeup_Interrupt_Enable();
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_Stop
********************************************************************************
*
* Summary:
*   Disables and removes the interrupt.
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
void Wakeup_Interrupt_Stop(void)
{
    /* Disable this interrupt. */
    Wakeup_Interrupt_Disable();

    /* Set the ISR to point to the passive one. */
    Wakeup_Interrupt_SetVector(&IntDefaultHandler);
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_Interrupt
********************************************************************************
*
* Summary:
*   The default Interrupt Service Routine for Wakeup_Interrupt.
*
*   Add custom code between the coments to keep the next version of this file
*   from over writting your code.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
CY_ISR(Wakeup_Interrupt_Interrupt)
{
    /*  Place your Interrupt code here. */
    /* `#START Wakeup_Interrupt_Interrupt` */

    /* `#END` */
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_SetVector
********************************************************************************
*
* Summary:
*   Change the ISR vector for the Interrupt. Note calling Wakeup_Interrupt_Start
*   will override any effect this method would have had. To set the vector 
*   before the component has been started use Wakeup_Interrupt_StartEx instead.
*
* Parameters:
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void Wakeup_Interrupt_SetVector(cyisraddress address)
{
    CyRamVectors[CYINT_IRQ_BASE + Wakeup_Interrupt__INTC_NUMBER] = address;
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_GetVector
********************************************************************************
*
* Summary:
*   Gets the "address" of the current ISR vector for the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Address of the ISR in the interrupt vector table.
*
*******************************************************************************/
cyisraddress Wakeup_Interrupt_GetVector(void)
{
    return CyRamVectors[CYINT_IRQ_BASE + Wakeup_Interrupt__INTC_NUMBER];
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_SetPriority
********************************************************************************
*
* Summary:
*   Sets the Priority of the Interrupt. Note calling Wakeup_Interrupt_Start
*   or Wakeup_Interrupt_StartEx will override any effect this method would 
*   have had. This method should only be called after Wakeup_Interrupt_Start or 
*   Wakeup_Interrupt_StartEx has been called. To set the initial
*   priority for the component use the cydwr file in the tool.
*
* Parameters:
*   priority: Priority of the interrupt. 0 - 3, 0 being the highest.
*
* Return:
*   None
*
*******************************************************************************/
void Wakeup_Interrupt_SetPriority(uint8 priority)
{
	uint8 interruptState;
    uint32 priorityOffset = ((Wakeup_Interrupt__INTC_NUMBER % 4u) * 8u) + 6u;
    
	interruptState = CyEnterCriticalSection();
    *Wakeup_Interrupt_INTC_PRIOR = (*Wakeup_Interrupt_INTC_PRIOR & (uint32)(~Wakeup_Interrupt__INTC_PRIOR_MASK)) |
                                    ((uint32)priority << priorityOffset);
	CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_GetPriority
********************************************************************************
*
* Summary:
*   Gets the Priority of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Priority of the interrupt. 0 - 3, 0 being the highest.
*
*******************************************************************************/
uint8 Wakeup_Interrupt_GetPriority(void)
{
    uint32 priority;
	uint32 priorityOffset = ((Wakeup_Interrupt__INTC_NUMBER % 4u) * 8u) + 6u;

    priority = (*Wakeup_Interrupt_INTC_PRIOR & Wakeup_Interrupt__INTC_PRIOR_MASK) >> priorityOffset;

    return (uint8)priority;
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_Enable
********************************************************************************
*
* Summary:
*   Enables the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void Wakeup_Interrupt_Enable(void)
{
    /* Enable the general interrupt. */
    *Wakeup_Interrupt_INTC_SET_EN = Wakeup_Interrupt__INTC_MASK;
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_GetState
********************************************************************************
*
* Summary:
*   Gets the state (enabled, disabled) of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   1 if enabled, 0 if disabled.
*
*******************************************************************************/
uint8 Wakeup_Interrupt_GetState(void)
{
    /* Get the state of the general interrupt. */
    return ((*Wakeup_Interrupt_INTC_SET_EN & (uint32)Wakeup_Interrupt__INTC_MASK) != 0u) ? 1u:0u;
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_Disable
********************************************************************************
*
* Summary:
*   Disables the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void Wakeup_Interrupt_Disable(void)
{
    /* Disable the general interrupt. */
    *Wakeup_Interrupt_INTC_CLR_EN = Wakeup_Interrupt__INTC_MASK;
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_SetPending
********************************************************************************
*
* Summary:
*   Causes the Interrupt to enter the pending state, a software method of
*   generating the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void Wakeup_Interrupt_SetPending(void)
{
    *Wakeup_Interrupt_INTC_SET_PD = Wakeup_Interrupt__INTC_MASK;
}


/*******************************************************************************
* Function Name: Wakeup_Interrupt_ClearPending
********************************************************************************
*
* Summary:
*   Clears a pending interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void Wakeup_Interrupt_ClearPending(void)
{
    *Wakeup_Interrupt_INTC_CLR_PD = Wakeup_Interrupt__INTC_MASK;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
