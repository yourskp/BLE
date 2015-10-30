/******************************************************************************
* Project Name		: PDM_Microphone
* File Name			: PDM.c
* Version 			: 1.0
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.2
* Compiler    		: ARM GCC 4.8.4
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit 
* Owner				: kris@cypress.com
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

/********************************************************************************
*	Description - Routines to implement PDM microphone interface on PSoC 4 device
********************************************************************************/

#include <Configuration.h>
#include <I2S_OUT.h>
#include <PDM.h>
#include <Project.h>

/***************************************
*    Global variables
***************************************/
uint32 PDM_Integrator_diff0_reg = 0;
uint32 PDM_Integrator_diff1_reg = 0;
uint32 PDM_Integrator_diff2_reg = 0;
uint32 PDM_Integrator_diff3_reg = 0;
uint32 PDM_Integrator_diff4_reg = 0;
int32 PDM_Integrator_outBuf[128];
uint32 Integrator[128];

uint8 dataIndex = 0;	/* Data buffer index */

/*******************************************************************************
* Function Name: PDM_Start
********************************************************************************
*
* Summary:
*  Initialises the PDM interface
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void PDM_Start(void)
{
    PDM_Integrator_Start();
    
#if ENABLE_PDM_INPUT    
    /* Route actual PDM GPIO to integrator block */
    PDM_Control_Write(ENABLE_PDM_COMPONENT_MASK);
#else
    /* Simulate PDM input data */
    PDM_Control_Write(ENABLE_PDM_COMPONENT_MASK | ENABLE_PDM_DATA_MASK);
#endif	
}

/*******************************************************************************
* Function Name: PDM_Integrator_Comb_isr
********************************************************************************
*
* Summary:
*  Performs the 5th order comb filtering on the output of the integrator.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
CY_ISR(PDM_Integrator_Comb_isr)
{
	uint32 sum;
	uint32 diff0, diff1, diff2, diff3, diff4;
    int16 cicOutput;
    int32 cicOutputHighRes;
	
#if DEBUG_ENABLE
	Integrator_Int_Write(1);
#endif    
	
	/* integrator output is stored in FIFO 0 register */
#if (PDM_Integrator_LEFT_CHANNEL_ENABLED) 
#if (CY_PSOC4_4100BL || CY_PSOC4_4200BL)            
		sum = PDM_Integrator_INT_L_OUTPUT_REG;
#else
        sum = PDM_Integrator_INT_L_HI16_OUTPUT_REG;
        sum = sum << 16;
        sum = sum | PDM_Integrator_INT_L_LO16_OUTPUT_REG;
#endif    
#endif	
	
#if (PDM_Integrator_RIGHT_CHANNEL_ENABLED)
#if (CY_PSOC4_4100BL || CY_PSOC4_4200BL)     
		sum = PDM_Integrator_INT_R_OUTPUT_REG;
#else
        sum = PDM_Integrator_INT_R_HI16_OUTPUT_REG;
        sum = sum << 16;
        sum = sum | PDM_Integrator_INT_R_LO16_OUTPUT_REG;
#endif
#endif

	diff0 = sum - PDM_Integrator_diff0_reg;
	diff1 = diff0 - PDM_Integrator_diff1_reg;
	diff2 = diff1 - PDM_Integrator_diff2_reg;
	diff3 = diff2 - PDM_Integrator_diff3_reg;
	diff4 = diff3 - PDM_Integrator_diff4_reg;
	
	PDM_Integrator_diff0_reg = sum;
	PDM_Integrator_diff1_reg = diff0;
	PDM_Integrator_diff2_reg = diff1;
	PDM_Integrator_diff3_reg = diff2;
	PDM_Integrator_diff4_reg = diff3;	
	
	/* diff4 has a 31 bit audio result, extract the upper 16 bits of the 31 bit value */
    #if 0
	diff4 = diff4 << 2;
    diff4 = HI16(diff4);
    cicOutput = diff4 - 0x7FFF;
    #else
#if ENABLE_16_BIT_INTERFACE    
    diff4 = diff4 >> 14;
    diff4 = LO16(diff4);
    cicOutput = diff4 - 0x7FFF;
#else
    diff4 = diff4 >> 6;
    cicOutputHighRes = diff4 - 0x7FFFFF;
#endif
    #endif

#if ENABLE_DATA_LOGGING    
  	Integrator[dataIndex] = sum;
    PDM_Integrator_outBuf[dataIndex] = diff4;    	
	dataIndex++;
    
	if (dataIndex == (sizeof(PDM_Integrator_outBuf)/4))
	{
		dataIndex = 0;
	}
#endif
    
#if ENABLE_I2S_OUTPUT
#if ENABLE_16_BIT_INTERFACE    
    
    /* Left channel */
    I2S_WriteByte((HI8(cicOutput)), I2S_TX_LEFT_CHANNEL);
    I2S_WriteByte((LO8(cicOutput)), I2S_TX_LEFT_CHANNEL);
    
    /* Right channel */
    I2S_WriteByte((HI8(cicOutput)), I2S_TX_RIGHT_CHANNEL);
    I2S_WriteByte((LO8(cicOutput)), I2S_TX_RIGHT_CHANNEL);
#else
    /* Left channel */
    I2S_WriteByte((LO8(HI16(cicOutputHighRes))), I2S_TX_LEFT_CHANNEL);
    I2S_WriteByte((HI8(LO16(cicOutputHighRes))), I2S_TX_LEFT_CHANNEL);
    I2S_WriteByte((LO8(LO16(cicOutputHighRes))), I2S_TX_LEFT_CHANNEL);
    
    /* Right channel */
    I2S_WriteByte((LO8(HI16(cicOutputHighRes))), I2S_TX_RIGHT_CHANNEL);
    I2S_WriteByte((HI8(LO16(cicOutputHighRes))), I2S_TX_RIGHT_CHANNEL);
    I2S_WriteByte((LO8(LO16(cicOutputHighRes))), I2S_TX_RIGHT_CHANNEL);
#endif
        
    if(I2S_IsStreaming() == FALSE)
    {
        I2S_EnableTx();
        I2S_Streaming(TRUE);
        I2S_Clock_Start();
    }
#endif
	
#if DEBUG_ENABLE    
	Integrator_Int_Write(0);
#endif
}

/* [] END OF FILE */
