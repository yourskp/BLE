/*******************************************************************************
* File Name: CYBLE_HAL_PVT.c
* Version 1.20
*
* Description:
*  This file contains the source code for the HAL section of the BLE component
*
********************************************************************************
* Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CYBLE_HAL_PVT.h"


#if(CYBLE_MODE == CYBLE_HCI)


/*******************************************************************************
* Function Name: CyBLE_Uart_Start
********************************************************************************
*
* Summary:
*  Enables the platform UART Tx and RX interrupts and then enables the UART
*  component.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void CyBLE_Uart_Start (void)
{
    /* Setup ISR */
    CYBLE_uart_isr_StartEx(&CyBLE_Uart_Interrupt);

    CYBLE_HAL_Uart_Start();
}


/*******************************************************************************
* Function Name: CyBLE_Uart_Stop
********************************************************************************
*
* Summary:
*  Disables the UART, clears all pending interrupts and disables the UART Tx
*  and RX interrupts. This will also empty out the FIFOs.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void CyBLE_Uart_Stop (void)
{
    /* Stop interrupt and UART */
    CYBLE_uart_isr_Stop();

    CYBLE_HAL_Uart_Stop();
}


/*******************************************************************************
* Function Name: CyBLE_Uart_Transmit
********************************************************************************
*
* Summary:
*  Sends the data to Uart Tx FIFO. The API handles data length up to the 
*  supported Tx FIFO length of the uart hardware module.
*
* Parameters:
*  uint8 *data: Pointer to the data to send through the UART
*  uint8 length: the length of data to transmit in bytes
*
* Return:
*  None
*
*******************************************************************************/
void CyBLE_Uart_Transmit (const uint8 *dataBuf, uint8 length)
{
    CYBLE_HAL_Uart_SpiUartPutArray(dataBuf, (uint32)length);
}

#endif /* (CYBLE_MODE == CYBLE_HCI) */


/*******************************************************************************
* Function Name: CyBLE_Nvram_Write
********************************************************************************
*
* Summary:
*  This API writes the data to the NVRAM store. It will check the appropriate
*  alignment of a start address and also perform an address range check based
*  on the length before performing the write operation.
*  This function performs memory compare and writes only row where there are new
*  data to write.
*
* Parameters:
*  const uint8 *buffer: Pointer to the buffer containing the data to be stored.
*  const uint8 *varFlash: Pointer to the array or variable in the flash.
*  uint16 length: the length of the data in bytes.
*
* Return:
*  CYRET_SUCCESS    a successful write
*  CYRET_BAD_PARAM  A request to write outside the flash boundary.
*  CYRET_UNKNOWN    Other errors in writing the flash
*
* Side Effects:
*  This API will automatically modify the clock settings for the device.
*  Writing to flash requires changes to be done to the IMO (set to 48 MHz)
*  and HFCLK (source set to IMO) settings. The configuration is restored before
*  returning. This will impact the operation of most of the hardware in the
*  device.
*
*******************************************************************************/
cystatus CyBLE_Nvram_Write (const uint8 buffer[], const uint8 varFlash[], uint16 length)
{
    uint8 writeBuffer[CY_FLASH_SIZEOF_ROW];
    uint32 rowId;
    uint32 dstIndex;
    uint32 srcIndex = 0u;
    cystatus rc = CYRET_SUCCESS;
    uint32 eeOffset;
    uint32 byteOffset;
    uint32 rowsNotEqual;

    eeOffset = (uint32)varFlash;

    /* Make sure, that varFlash[] points to ROM */
    if ((eeOffset + length) < CYBLE_HAL_FLASH_END_ADDR)
    {
        rowId = eeOffset / CY_FLASH_SIZEOF_ROW;
        byteOffset = CY_FLASH_SIZEOF_ROW * rowId;

        while ((srcIndex < length) && (CYRET_SUCCESS == rc))
        {
            rowsNotEqual = 0u;
            /* Copy data to the write buffer either from the source buffer or from the flash */
            for (dstIndex = 0u; dstIndex < CY_FLASH_SIZEOF_ROW; dstIndex++)
            {
                if ((byteOffset >= eeOffset) && (srcIndex < length))
                {
                    /* Detect that row programming is required */
                    if(writeBuffer[dstIndex] != buffer[srcIndex])
                    {
                        writeBuffer[dstIndex] = buffer[srcIndex];
                        rowsNotEqual = 1u;  
                    }
                    srcIndex++;
                }
                else
                {
                    writeBuffer[dstIndex] = CY_GET_XTND_REG8(CYDEV_FLASH_BASE + byteOffset);
                }
                byteOffset++;
            }

            if(rowsNotEqual != 0u)
            {
                /* Write flash row */
                rc = CySysFlashWriteRow(rowId, writeBuffer);
            }

            /* Go to the next row */
            rowId++;
        }
    }
    else
    {
        rc = CYRET_BAD_PARAM;
    }

    /* Mask return codes from flash, if they are not supported */
    if ((CYRET_SUCCESS != rc) && (CYRET_BAD_PARAM != rc))
    {
        rc = CYRET_UNKNOWN;
    }
    
    return (rc);
}

/*******************************************************************************
* Function Name: CyBLE_Nvram_Erase
********************************************************************************
*
* Summary:
*  This API erases the data from the NVRAM store. This API will perform an 
*  address range check based on the length before performing erase operation.
*
* Parameters:
*  const uint8 *varFlash: the pointer to the array or variable in the flash.
*  uint16 length: the length of the data in bytes.
*
* Return:
*  CYRET_SUCCESS    a successful write
*  CYRET_BAD_PARAM  A request to write outside the flash boundary.
*  CYRET_UNKNOWN    Other errors in writing the flash
*
*******************************************************************************/
cystatus CyBLE_Nvram_Erase (const uint8 *varFlash, uint16 length)
{
    const uint8 *tempFlash;
    uint8 flashBuffer[CY_FLASH_SIZEOF_ROW] = {0u};
    uint16 lengthFlash;
    uint16 lengthRemaining;
    cystatus rc = CYRET_UNKNOWN;

    tempFlash = varFlash;
    lengthRemaining = length;

    while(lengthRemaining > 0u)
    {
        if(lengthRemaining < CY_FLASH_SIZEOF_ROW)
        {
            lengthFlash = lengthRemaining;
        }
        else
        {
            lengthFlash = CY_FLASH_SIZEOF_ROW;
        }
        rc = CyBLE_Nvram_Write(flashBuffer, tempFlash, lengthFlash);
        if(rc != CYRET_SUCCESS)
        {
            break;
        }
        tempFlash += CY_FLASH_SIZEOF_ROW;
        lengthRemaining -= lengthFlash;
    }

    return rc;
}


/*******************************************************************************
* Function Name: CyBLE_Bless_LlRegRead
********************************************************************************
*
* Summary:
*  This API reads the content of the BLESS Link Layer register from the stack.
*
* Parameters:
*  uint32 *blessAddr: the pointer to the BLESS link layer address.
*  uint16 *regValue: the pointer to the buffer space to copy the read value.
*
* Return:
*  None
*
*******************************************************************************/
void CyBLE_Bless_LlRegRead (const reg32 *blessAddr, uint16 *regValue)
{
    *regValue = (uint16)CY_GET_REG32(blessAddr);
}


/*******************************************************************************
* Function Name: CyBLE_Bless_LlRegWrite
********************************************************************************
*
* Summary:
*  This API writes to the BLESS Link Layer register from the stack.
*
* Parameters:
*  uint32 *blessAddr: the pointer to the BLESS link layer address.
*  uint16 regValue: the pointer to the value to be written to the BLESS LL reg.
*
* Return:
*  None
*
*******************************************************************************/
void CyBLE_Bless_LlRegWrite (reg32 *blessAddr, const uint16 regValue)
{
    CY_SET_REG32(blessAddr, regValue);
}


/*******************************************************************************
* Function Name: CyBLE_Bless_RfRegRead
********************************************************************************
*
* Summary:
*  This API reads the content of the BLESS RF register from the stack.
*
* Parameters:
*  uint32 *blessAddr: the pointer to BLESS RF register address.
*  uint16 *regValue: the pointer to the buffer space to copy the read value.
*
* Return:
*  None
*
*******************************************************************************/
void CyBLE_Bless_RfRegRead (const reg32 *blessAddr, uint16 *regValue)
{
    *regValue = (uint16)CY_GET_REG32(blessAddr);
}


/*******************************************************************************
* Function Name: CyBLE_Bless_RfRegWrite
********************************************************************************
*
* Summary:
*  This API writes to the BLESS RF register from the stack.
*
* Parameters:
*  uint32 *blessAddr: the pointer to the BLESS RF address.
*  uint16 regValue: the pointer to the value to be written to the BLESS RF reg.
*
* Return:
*  None
*
*******************************************************************************/
void CyBLE_Bless_RfRegWrite (reg32 *blessAddr, const uint16 regValue)
{
     CY_SET_REG32(blessAddr, regValue);
}


void CyBleHal_DelayUs(uint16 delayVal)
{
    CyDelayUs(delayVal);
}

void CyBleHal_DelayMs(uint32 delayVal)
{
    CyDelay(delayVal);
}

void CyBleHal_EnableGlobalInterrupts(void)
{
    CYBLE_bless_isr_Enable();
}

void CyBleHal_DisableGlobalInterrupts(void)
{
    CYBLE_bless_isr_Disable();
}

void CyBle_HalInit(void)
{
    /* For all we know the interrupt is active. */
    CYBLE_bless_isr_Disable();

    /* Set the ISR to point to the BLE_bless_isr Interrupt. */
    CYBLE_bless_isr_SetVector(&CyBLE_Bless_Interrupt);

    /* Set the priority. */
    CYBLE_bless_isr_SetPriority((uint8)CYBLE_bless_isr_INTC_PRIOR_NUMBER);
}

/* Start Interrupt Controller API. */
void CYBLE_BlessStart(void)
{
    CYBLE_bless_isr_StartEx(&CyBLE_Bless_Interrupt);
}

/* [] END OF FILE */
