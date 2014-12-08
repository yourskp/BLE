/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This is the source code for the PSoC 4 BLE Find Me Target lab.
*
* Hardware Dependency:
*  1. PSoC 4 BLE device
*  2. CY8CKIT-042 BLE Pioneer Kit
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#include <Project.h>
#include <stdio.h>

extern CYBLE_GAPP_DISC_PARAM_T cyBle_discoveryParam;

/***************************************
*        API Constants
***************************************/
#define NO_ALERT                                (0u)
#define MILD_ALERT                              (1u)
#define HIGH_ALERT                              (2u)

#define LED_ON                                  (0u)
#define LED_OFF                                 (1u)

#define BLINK_TIMEOUT                           (100u)
#define RSSI_THRESHOLD                          (-60)

#define ADV_STATE_GENERAL                       1
#define ADV_STATE_WHITELIST                     2

#define SCAN_ANY_CONNECT_ANY                    0
#define SCAN_WHITELIST_CONNECT_WHITELIST        3

/***************************************
*        Function Prototypes
***************************************/
void StackEventHandler(uint32 event, void* eventParam);
void LlsEventHandler(uint32 event, void* eventParam);
void StartAdvertisement(uint8 advType);

/***************************************
*        Global variables
***************************************/
uint8 linkAlertLevel = NO_ALERT;
uint8 advState = ADV_STATE_GENERAL;

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Main function.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
int main()
{
    CYBLE_LP_MODE_T lpMode;
    CYBLE_BLESS_STATE_T blessState;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
  
    CyGlobalIntEnable; 

    /* ADD_CODE_TO - Start the BLE component and register StackEventHandler function */
    apiResult = CyBle_Start(StackEventHandler);

    if(apiResult != CYBLE_ERROR_OK)
    {
        /* BLE stack initialization failed, check your configuration */
        CYASSERT(0);
    }

    /* Register LLS event handler function */
    CyBle_LlsRegisterAttrCallback(LlsEventHandler);
        
    while(1)
    {
        /* Process all the pending BLE tasks. This single API call will service all 
         * the BLE stack events. This API MUST be called at least once per BLE connection interval */
        CyBle_ProcessEvents();

        if(CyBle_GetState() != CYBLE_STATE_INITIALIZING) /* Waiting for BLE component initialization */
        {
            static uint8 blinkTimeout = BLINK_TIMEOUT;

            /* Update link loss alert LED status based on IAS Alert level characteristic */
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED || 
                (CyBle_GetState() == CYBLE_STATE_CONNECTED && CyBle_GetRssi() < RSSI_THRESHOLD))
            {
                switch(linkAlertLevel)
                {
                    case NO_ALERT:
                        Link_Alert_LED_Write(LED_OFF);
                    break;

                    case MILD_ALERT:
                    if(--blinkTimeout == 0)
                    {
                        Link_Alert_LED_Write(Link_Alert_LED_Read() ^ 0x01);
                        blinkTimeout = BLINK_TIMEOUT;
                    }
                    break;

                    case HIGH_ALERT:
                        Link_Alert_LED_Write(LED_ON);
                    break;
                }
            }
            else
            {
                Link_Alert_LED_Write(LED_OFF);
            }
        }
        
        lpMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
        CyGlobalIntDisable;
        blessState = CyBle_GetBleSsState();

        if(lpMode == CYBLE_BLESS_DEEPSLEEP) 
        {   
            if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
            {
                /* Put the device into the Deep Sleep mode only when all debug information has been sent */
                if((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) == 0u)
                {
                    CySysPmDeepSleep();
                }
                else
                {
                    CySysPmSleep();
                }
            }
        }
        else
        {
            if(blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
            {
                CySysPmSleep();
            }
        }
        CyGlobalIntEnable;
        
        if((cyBle_pendingFlashWrite != 0u) &&
                    ((UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) == 0u))
        {
            apiResult = CyBle_StoreBondingData(0u);
            printf("Store bonding data, status: %x \r\n", apiResult);
        }
    }
}

/*******************************************************************************
* Function Name: StackEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component.
*
* Parameters:  
*  uint8 event:       Event from the CYBLE component
*  void* eventParams: A structure instance for corresponding event type. The 
*                     list of event structure is described in the component 
*                     datasheet.
*
* Return: 
*  None
*
*******************************************************************************/
void StackEventHandler(uint32 event, void *eventParam)
{
    switch(event)
    {
        /* Mandatory events to be handled by Find Me Target design */
        
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        case CYBLE_EVT_STACK_ON:
            StartAdvertisement(ADV_STATE_GENERAL);
            linkAlertLevel = NO_ALERT;
            Advertising_LED_Write(LED_ON);
            Disconnection_LED_Write(LED_OFF);
            advState = ADV_STATE_GENERAL;
        break;
          
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
          /* Indicate successful BLE link establishment 
           * by turning off Advertising_LED and Disconnection_LED */
            linkAlertLevel = NO_ALERT;
            Advertising_LED_Write(LED_OFF);
        break; 

        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED) /* ADV timed out */
            {
                if(advState == ADV_STATE_GENERAL)
                {
                    Advertising_LED_Write(LED_OFF);
                    CyDelay(100);
                    Advertising_LED_Write(LED_ON);
                    StartAdvertisement(ADV_STATE_WHITELIST);
                    advState = ADV_STATE_WHITELIST;
                }
                else
                {
                    /* No connection established, shut down ADV */
                    Advertising_LED_Write(LED_OFF);
                    Disconnection_LED_Write(LED_ON);
                }
            }
        break;
            
        default:
    	break;
    }
}

/*******************************************************************************
* Function Name: LlsEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component,
*  which are specific to Link Loss Service.
*
* Parameters:  
*  uint8 event:       Write Command event from the CYBLE component.
*  void* eventParams: A structure instance of CYBLE_GATT_HANDLE_VALUE_PAIR_T
*                     type.
*
* Return: 
*  None
*
*******************************************************************************/
void LlsEventHandler(uint32 event, void *eventParam)
{
    /* Alert Level Characteristic write event */
    if(event == CYBLE_EVT_LLSS_WRITE_CHAR_REQ)
    {
        CyBle_LlssGetCharacteristicValue(CYBLE_LLS_ALERT_LEVEL, sizeof(linkAlertLevel), &linkAlertLevel);                
    }
}

/* For GCC compiler revise _write() function for printf functionality */
int _write(int file, char *ptr, int len)
{
    int i;
    file = file;
    for (i = 0; i < len; i++)
    {
        UART_UartPutChar(*ptr++);
    }
    return len;
}

/*******************************************************************************
* Function Name: StartAdvertisement
********************************************************************************
*
* Summary:
*   Initiates the advertisement procedure.
*   Prints the Device Address.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
*******************************************************************************/
void StartAdvertisement(uint8 advType)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GAP_BD_ADDR_T localAddr;
    uint16 i;
    
    if(advType == ADV_STATE_GENERAL)
    {
        cyBle_discoveryParam.advFilterPolicy = SCAN_ANY_CONNECT_ANY;
    }
    else
    {
        cyBle_discoveryParam.advFilterPolicy  = SCAN_WHITELIST_CONNECT_WHITELIST;
    }
    
    apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
    
    if(apiResult != CYBLE_ERROR_OK)
    {
        printf("StartAdvertisement API Error: %x \r\n", (int) apiResult);
    }
    else
    {
        printf("Start Advertisement with addr: ");
        CyBle_GetDeviceAddress(&localAddr);
        for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
        {
            printf("%2.2x", localAddr.bdAddr[i-1]);
        }
        printf("\r\n");
    }
}

/* [] END OF FILE */
