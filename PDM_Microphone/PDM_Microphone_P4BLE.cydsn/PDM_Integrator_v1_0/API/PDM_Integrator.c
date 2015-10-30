/*******************************************************************************
* File Name: `$INSTANCE_NAME`.c
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file contains the function definitions associated with the PDM CIC
*  filter component
*
* Note:
*
*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`.h"
#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"
#include "Project.h"

CY_ISR_PROTO(`$INSTANCE_NAME`_Comb_isr);

uint8 `$INSTANCE_NAME`_initVar = 0u;

static `$INSTANCE_NAME`_BACKUP_STRUCT  `$INSTANCE_NAME`_backup =
{
    /* Component disabled by deafult */
    `$INSTANCE_NAME`_DISABLED,

    /* Decimation counter period */
    `$INSTANCE_NAME`_CNT7_PERIOD,

};

/***************************************
*        Function Prototypes
***************************************/

static void `$INSTANCE_NAME`_Integrator_Init(void);

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Start
********************************************************************************
*
* Summary:
*  Starts the PDM CIC Filter component
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_initVar - is used to indicate initial configuration
*  of this component.  The variable is initialized to zero and set to 1
*  the first time `$INSTANCE_NAME`_Start() is called. This allows for component
*  initialization without re-initialization in all subsequent calls
*  to the `$INSTANCE_NAME`_Start() routine.
*
*
*******************************************************************************/
void `$INSTANCE_NAME`_Start(void)
{
    if(0u == `$INSTANCE_NAME`_initVar)
    {
        /* Set init flag */
        `$INSTANCE_NAME`_initVar = 1u;
    }
	
	`$INSTANCE_NAME`_Integrator_Init();	
	
	Integrator_Interrupt_StartEx(`$INSTANCE_NAME`_Comb_isr);
	
	/* Enable interrupt status generation if required */
	#if (`$INSTANCE_NAME`_INTERRUPT_ENABLED)
		`$INSTANCE_NAME`_ReadInterruptStatus(); /* Clear the status register before enabling interrupt */
		`$INSTANCE_NAME`_EnableInterruptGeneration();
	#endif
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Stop
********************************************************************************
*
* Summary:
*  Stops the PDM CIC Filter component
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  None.
*
*
*******************************************************************************/
void `$INSTANCE_NAME`_Stop(void)
{	
	/* Disable interrupt status generation if required */
	#if (`$INSTANCE_NAME`_INTERRUPT_ENABLED)
		`$INSTANCE_NAME`_DisableInterruptGeneration();
		`$INSTANCE_NAME`_ReadInterruptStatus(); /* Clear the status register after disabling interrupt */
	#endif
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SaveConfig
********************************************************************************
*
* Summary:
*  Saves PDM CIC filter configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_SaveConfig(void)
{
	`$INSTANCE_NAME`_backup.Cnt7Period = `$INSTANCE_NAME`_CNT7_PERIOD_REG;
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores PDM CIC filter configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_RestoreConfig(void)
{
	`$INSTANCE_NAME`_CNT7_PERIOD_REG = `$INSTANCE_NAME`_backup.Cnt7Period;
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Sleep
********************************************************************************
*
* Summary:
*  Prepares PDM CIC component to sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Sleep(void)
{
    /* Get component state */
    //`$INSTANCE_NAME`_backup.enableState = ((uint8) `$INSTANCE_NAME`_IS_ENABLE);

    /* Save registers configuration */
    `$INSTANCE_NAME`_SaveConfig();

    /* Stop component */
    `$INSTANCE_NAME`_Stop();
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Wakeup
********************************************************************************
*
* Summary:
*  Prepares PDM CIC component for wakeup.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
/* Restores the component after low power mode exit */
void `$INSTANCE_NAME`_Wakeup(void)
{
    /* Restore registers values */
    `$INSTANCE_NAME`_RestoreConfig();

    //if(0u != `$INSTANCE_NAME`_backup.enableState)
    {
    	/* Start component */
    	`$INSTANCE_NAME`_Start();
    }
}

/* Conditional compilation of Interrupt related API's */
#if (`$INSTANCE_NAME`_INTERRUPT_ENABLED)

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_ReadInterruptStatus
	********************************************************************************
	*
	* Summary:
	*  Returns the current value of the PDM CIC status register
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  uint8 - Status register value
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	uint8 `$INSTANCE_NAME`_ReadInterruptStatus()
	{
		//return(`$INSTANCE_NAME`_Status_Reg_Read());
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_EnableInterruptGeneration
	********************************************************************************
	*
	* Summary:
	*  Enables interrupt signal generation by the status register
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	void  `$INSTANCE_NAME`_EnableInterruptGeneration()
	{
		//`$INSTANCE_NAME`_Status_Reg_InterruptEnable();
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_DisableInterruptGeneration
	********************************************************************************
	*
	* Summary:
	*  Disables interrupt signal generation by the status register
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	void  `$INSTANCE_NAME`_DisableInterruptGeneration()
	{
		`$INSTANCE_NAME`_Status_Reg_InterruptDisable();
	}

#endif /* (`$INSTANCE_NAME`_INTERRUPT_ENABLED) */

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Integrator_Init
********************************************************************************
*
* Summary:
*  Initializes the integrator
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  None.
*
*
*******************************************************************************/
static void `$INSTANCE_NAME`_Integrator_Init(void)
{
	uint8 enableInterrupts;	
	
	enableInterrupts = CyEnterCriticalSection();
	
	#if(`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)	
		/* Reset the F0, F1 FIFO buffer pointers and status signals */
		CY_SET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u0__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u1__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u2__DP_AUX_CTL_REG) | 0x03 );	
		CY_SET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u3__DP_AUX_CTL_REG) | 0x03 );	
		
		CY_SET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u0__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u1__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u2__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk2_dp32_l_u3__DP_AUX_CTL_REG) & 0xFC );	
	#endif /* (`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED) */
	
	#if(`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
		/* Reset the F0, F1 FIFO buffer pointers and status signals */
		CY_SET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u0__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u1__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u2__DP_AUX_CTL_REG) | 0x03 );	
		CY_SET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u3__DP_AUX_CTL_REG) | 0x03 );	
		
		CY_SET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u0__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u1__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u2__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_genblk3_dp32_r_u3__DP_AUX_CTL_REG) & 0xFC );	
	#endif /* (`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED) */
	
	/* Enable the decimator counter module */
	CY_SET_REG8(`$INSTANCE_NAME`_Counter7__CONTROL_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Counter7__CONTROL_AUX_CTL_REG) | 0x20);
	
	CyExitCriticalSection(enableInterrupts);	
}

/* [] END OF FILE */
