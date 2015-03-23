/*******************************************************************************
* File Name: CYBLE_HAL_INT.c
* Version 1.20
*
* Description:
*  This file contains the source code for the Interrupt Service Routine for the
*  HAL section of the BLE component
*
********************************************************************************
* Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CYBLE_HAL_PVT.h"


/*******************************************************************************
* Function Name: CyBLE_Bless_Interrupt
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the BLE sub-system.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
CY_ISR(CyBLE_Bless_Interrupt)
{
    #if(CYBLE_STACK_MODE_DEBUG)
        CyBle_ApplCallback(CYBLE_DEBUG_EVT_BLESS_INT, NULL);
    #endif /* CYBLE_STACK_MODE_DEBUG */

    /* Call stack manager bless function handler */
    CyBLE_pf_bless_event_hdlr();
    /* Host stack takes care of clearing interrupts */
}


#if(CYBLE_MODE == CYBLE_HCI)


/*******************************************************************************
* Function Name: CyBLE_Uart_Interrupt
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the UART.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
CY_ISR(CyBLE_Uart_Interrupt)
{
    uint8  uartTxStatus = CyBLE_INTR_TX_SUCCESS;
    uint32 srcInterrupt = 0u;

    uint8 length = 0u;
    uint8 srcCount = 0u;
    uint8 uartRxStatus = CyBLE_INTR_RX_SUCCESS;
    uint8 receivedData[CYBLE_HAL_Uart_FIFO_SIZE] = {0u};

    /* Evaluate RX interrupt event */
    srcInterrupt = CYBLE_HAL_Uart_GetRxInterruptSourceMasked();

    if(0u != (srcInterrupt & CYBLE_HAL_Uart_INTR_RX_NOT_EMPTY))
    {
        if(0u != (srcInterrupt & CYBLE_HAL_Uart_INTR_RX_PARITY_ERROR))
        {
            uartRxStatus |= CyBLE_INTR_RX_PARITY_ERROR;
        }
        if(0u != (srcInterrupt & CYBLE_HAL_Uart_INTR_RX_FRAME_ERROR))
        {
            uartRxStatus |= CyBLE_INTR_RX_FRAME_ERROR;
        }
        if(0u != (srcInterrupt & CYBLE_HAL_Uart_INTR_RX_OVERFLOW))
        {
            uartRxStatus |= CyBLE_INTR_RX_OVERFLOW;
        }
        if(uartRxStatus == CyBLE_INTR_RX_SUCCESS)
        {
            length = (uint8)CYBLE_HAL_Uart_SpiUartGetRxBufferSize();
            for(srcCount = 0u; srcCount < length; srcCount++)
            {
                receivedData[srcCount] = (uint8)CYBLE_HAL_Uart_SpiUartReadRxData();
            }
        }
        else
        {
            CYBLE_HAL_Uart_SpiUartClearRxBuffer();
        }
        for(uartTxStatus = 0u; uartTxStatus < length; uartTxStatus++)
        {
            CyBLE_pf_handle_uart_rx_data(receivedData[uartTxStatus]);
        }
        CYBLE_HAL_Uart_ClearRxInterruptSource(srcInterrupt);
    }
    else
    {
        /* No RX interrupt. Do nothing. */
    }

    /* Evaluate TX interrupt event in sequence */
    srcInterrupt = CYBLE_HAL_Uart_GetTxInterruptSourceMasked();

    /* Stack manager TX uart complete */
    if(0u != (srcInterrupt & CYBLE_HAL_Uart_INTR_TX_UART_DONE))
    {
        if(0u != (srcInterrupt & CYBLE_HAL_Uart_INTR_TX_OVERFLOW))
        {
            /*Stack manager TX uart error */
            uartTxStatus |= CyBLE_INTR_TX_OVERFLOW;
        }
        CyBLE_pf_handle_uart_tx_complt();
        CYBLE_HAL_Uart_ClearTxInterruptSource(srcInterrupt);
    }
    else
    {
        /* No TX interrupt. Do nothing. */
    }
}
#endif /* (CYBLE_MODE == CYBLE_HCI) */


/* [] END OF FILE */
