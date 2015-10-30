/*******************************************************************************
* File Name: `$INSTANCE_NAME`.h
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This header file contains definitions associated with the PDM CIC Filter component.
*
* Note:
* 
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined (`$INSTANCE_NAME`_H)
#define `$INSTANCE_NAME`_H

#include "cyfitter.h"
#include "cytypes.h"   	

/* Conditional compilation parameter for including interrupt related API's */
#define `$INSTANCE_NAME`_INTERRUPT_ENABLED  0

/* Decimation counter period is one less than decimation rate */
#define `$INSTANCE_NAME`_CNT7_PERIOD       (((uint8)(`$DecimationRate`)) - 1)

#define `$INSTANCE_NAME`_MIC_CHANNEL       `$MicrophoneChannel`

#define `$INSTANCE_NAME`_RIGHT_CHANNEL      0u
#define `$INSTANCE_NAME`_LEFT_CHANNEL       1u
#define `$INSTANCE_NAME`_STEREO_CHANNEL     2u

#define `$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED  (`$INSTANCE_NAME`_MIC_CHANNEL != `$INSTANCE_NAME`_LEFT_CHANNEL)
#define `$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED   (`$INSTANCE_NAME`_MIC_CHANNEL != `$INSTANCE_NAME`_RIGHT_CHANNEL)

/***************************************
*             Registers
***************************************/

/* Address of registers holding the CIC output for each microphone channel. Note that these registers
   hold 16-bit values */
#if (`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
	#define `$INSTANCE_NAME`_RIGHT_LO16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_Comb_R_genblk2_dp32_u0__16BIT_F0_REG
	#define `$INSTANCE_NAME`_RIGHT_HI16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_Comb_R_genblk2_dp32_u2__16BIT_F0_REG
#endif

#if (`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)
	#define `$INSTANCE_NAME`_LEFT_LO16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_Comb_L_genblk2_dp32_u0__16BIT_F0_REG
	#define `$INSTANCE_NAME`_LEFT_HI16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_Comb_L_genblk2_dp32_u2__16BIT_F0_REG
#endif

/* Count7 decimation rate counter period register */
#define `$INSTANCE_NAME`_CNT7_PERIOD_REG   (* (reg8 *) `$INSTANCE_NAME`_Counter7__PERIOD_REG)

#if(`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)
	/* Address of registers holding the Integrator output. Note that these registers
	   hold 16-bit values */
	#define `$INSTANCE_NAME`_INT_L_LO16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_genblk2_dp32_l_u0__16BIT_F0_REG
	#define `$INSTANCE_NAME`_INT_L_HI16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_genblk2_dp32_l_u2__16BIT_F0_REG
#if (CY_PSOC4_4100BL || CY_PSOC4_4200BL)        
	#define `$INSTANCE_NAME`_INT_L_OUTPUT_REG_PTR       `$INSTANCE_NAME`_genblk2_dp32_l_u0__32BIT_F0_REG
#endif    
	
	#define `$INSTANCE_NAME`_INT_L_LO16_OUTPUT_REG      (*(uint16 *)`$INSTANCE_NAME`_genblk2_dp32_l_u0__16BIT_F0_REG)
	#define `$INSTANCE_NAME`_INT_L_HI16_OUTPUT_REG      (*(uint16 *)`$INSTANCE_NAME`_genblk2_dp32_l_u2__16BIT_F0_REG)
#if (CY_PSOC4_4100BL || CY_PSOC4_4200BL)    
	#define `$INSTANCE_NAME`_INT_L_OUTPUT_REG           (*(uint32 *)`$INSTANCE_NAME`_genblk2_dp32_l_u0__32BIT_F0_REG)
#endif    
#endif

#if(`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
	/* Address of registers holding the Integrator output. Note that these registers
	   hold 16-bit values */
	#define `$INSTANCE_NAME`_INT_R_LO16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_genblk3_dp32_r_u0__16BIT_F0_REG
	#define `$INSTANCE_NAME`_INT_R_HI16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_genblk3_dp32_r_u2__16BIT_F0_REG
#if (CY_PSOC4_4100BL || CY_PSOC4_4200BL) 	
	#define `$INSTANCE_NAME`_INT_R_OUTPUT_REG           (*(uint32 *)`$INSTANCE_NAME`_genblk2_dp32_r_u0__32BIT_F0_REG)
#endif    
#endif

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
    uint8 Cnt7Period;
} `$INSTANCE_NAME`_BACKUP_STRUCT;

/***************************************
*        Function Prototypes
***************************************/

/* Starts all the components associated with the PDM CIC */
void `$INSTANCE_NAME`_Start(void);

/* Stops all the components associated with the PDM CIC */
void `$INSTANCE_NAME`_Stop(void);

/* Saves the current component configuration */
void `$INSTANCE_NAME`_SaveConfig(void);

/* Restores the last saved component configuration */
void `$INSTANCE_NAME`_RestoreConfig(void);

/* Prepares the component for low power mode entry */
void `$INSTANCE_NAME`_Sleep(void);

/* Restores the component after low power mode exit */
void `$INSTANCE_NAME`_Wakeup(void);

/* Conditional compilation of Interrupt related API's */
#if (`$INSTANCE_NAME`_INTERRUPT_ENABLED)
uint8 `$INSTANCE_NAME`_ReadInterruptStatus(void);
void  `$INSTANCE_NAME`_EnableInterruptGeneration(void);
void  `$INSTANCE_NAME`_DisableInterruptGeneration(void);
#endif

/* Integrator control register mask definitions */
#define `$INSTANCE_NAME`_INT_ENABLE_MASK		    (`$INSTANCE_NAME`_Int_Ctrl_Reg_Sync_ctrl_reg__0__MASK)

#if (`$INSTANCE_NAME`_INTERRUPT_ENABLED)
	/* Status Register mask definitions */
	#define `$INSTANCE_NAME`_INTEGRATOR_R_FIFO_OVERFLOW_MASK    (`$INSTANCE_NAME`_Status_Reg_sts_intr_sts_reg__1__MASK)
	#define `$INSTANCE_NAME`_COMB_R_FIFO_OVERFLOW_MASK          (`$INSTANCE_NAME`_Status_Reg_sts_intr_sts_reg__3__MASK)		
	#define `$INSTANCE_NAME`_INTEGRATOR_L_FIFO_OVERFLOW_MASK    (`$INSTANCE_NAME`_Status_Reg_sts_intr_sts_reg__0__MASK)
	#define `$INSTANCE_NAME`_COMB_L_FIFO_OVERFLOW_MASK          (`$INSTANCE_NAME`_Status_Reg_sts_intr_sts_reg__2__MASK)	

#endif

/* Component disabled state value */
#define `$INSTANCE_NAME`_DISABLED                      (0x00u)

#define `$INSTANCE_NAME`_IS_ENABLE   \
				((`$INSTANCE_NAME`_Int_Ctrl_Reg_Control & `$INSTANCE_NAME`_INT_ENABLE_MASK) != 0)					

#endif /* `$INSTANCE_NAME`_H */

/* [] END OF FILE */
