/*******************************************************************************
* File Name: WriteUserSFlash.c
*
* Version: 1.0
*
* Description:
*  This file contains the source code for User SFLASH Write API.
*
* Owner:
*  sey@cypress.com, kris@cypress.com
*
* Hardware Dependency:
*  1. PSoC 4 BLE device
*
* Code Tested With:
*  1. PSoC Creator 3.1 SP1
*  2. ARM GCC 4.8.4
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <Project.h>
#include <WriteUserSFlash.h>

#if defined (__GNUC__)
#pragma GCC optimize ("O0")
#endif    
uint32 WriteUserSFlashRow(uint8 userrow_num, uint32 *write_value)
{
    uint8 local_count;
	volatile uint32 retvalue=0;
	volatile uint32 sram_cmd_data_buffer[(CY_FLASH_SIZEOF_ROW/4) + 2];
	volatile uint32 reg1,reg2,reg3,reg4,reg5,reg6;
	
	/* Store the clock settings temporarily */
    reg1 =	CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_SELECT));
    reg2 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_CONFIG));
    reg3 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM4));
    reg4 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM5));
    reg5 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM1));
    reg6 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM2));
	
	/* Initialize the clock necessary for flash programming */
	CY_SET_REG32(CYREG_CPUSS_SYSARG, 0x0000e8b6);
	CY_SET_REG32(CYREG_CPUSS_SYSREQ, 0x80000015);
	
	/******* Initialize SRAM parameters for the LOAD FLASH command ******/
	/* byte 3 (i.e. 00) is the Macro_select */
	/* byte 2 (i.e. 00) is the Start addr of page latch */
	/* byte 1 (i.e. d7) is the key 2  */
	/* byte 0 (i.e. b6) is the key 1  */
  	sram_cmd_data_buffer[0]=0x0000d7b6;
	
	/****** Initialize SRAM parameters for the LOAD FLASH command ******/
	/* byte 3,2 and 1 are null */
	/* byte 0 (i.e. 7F) is the number of bytes to be written */
	sram_cmd_data_buffer[1]=0x0000007F;	 
    
	/* Initialize the SRAM buffer with data bytes */
    for(local_count = 0; local_count < (CY_FLASH_SIZEOF_ROW/4); local_count++)    
	{
		sram_cmd_data_buffer[local_count + 2] = write_value[local_count]; 
	}
	
	/* Write the following to registers to execute a LOAD FLASH bytes */
	CY_SET_REG32(CYREG_CPUSS_SYSARG, &sram_cmd_data_buffer[0]);
	CY_SET_REG32(CYREG_CPUSS_SYSREQ, LOAD_FLASH);
	
    /****** Initialize SRAM parameters for the WRITE ROW command ******/
	/* byte 3 & 2 are null */
	/* byte 1 (i.e. 0xeb) is the key 2  */
	/* byte 0 (i.e. 0xb6) is the key 1  */
	sram_cmd_data_buffer[0] = 0x0000ebb6;
    
	/* byte 7,6 and 5 are null */
	/* byte 4 is desired SFLASH user row 
	 * Allowed values 0 - row 4
	                  1 - row 5 
					  2 - row 6
					  3 - row 7 */
	sram_cmd_data_buffer[1] = (uint32) userrow_num;
	
	/* Write the following to registers to execute a WRITE USER SFLASH ROW command */
	CY_SET_REG32(CYREG_CPUSS_SYSARG, &sram_cmd_data_buffer[0]);
	CY_SET_REG32(CYREG_CPUSS_SYSREQ, WRITE_USER_SFLASH_ROW);
    
	/* Read back SYSARG for the result. 0xA0000000 = SUCCESS; */
	retvalue = CY_GET_REG32(CYREG_CPUSS_SYSARG);
	
	/* Restore the clock settings after the flash programming is done */
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_SELECT),reg1);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_CONFIG),reg2);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM4),reg3);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM5),reg4);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM1),reg5);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM2),reg6);  
	
	return retvalue;
}
#if defined (__GNUC__)
#pragma GCC reset_options
#endif    

/* [] END OF FILE */
