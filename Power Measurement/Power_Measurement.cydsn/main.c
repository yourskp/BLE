/*******************************************************************************
* File Name: main.c
*
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This project demonstrates the operation of the Heart Rate Profile
*  in Server (Peripheral) role.
*
* Note:
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <Project.h>

#define ENABLE_850_XTAL_STARTUP   1  /* Required to configure slave clock accuracy to 50ppm. Don't disable this */
#define CON_PARAM_UPDATE          1  /* When enabled, updated BLE connection interval to 1 second */
#define SEND_NOTIFICATIONS        0  /* Enables firmware to send HRM notifications when CCCD is enabled */
#define NOTIF_INTERVAL_FOUR_SEC   0  /* Sends HRM notification once in 4 connection intervals. If connection interval 
                                      * is 1 sec, then the firmware sends notifications every 4 seconds */
#define DEBUG_ENABLE              0  /* Enables GPIO toggling on different power modes. Poor man's power profiler */

#if CON_PARAM_UPDATE
    #define NOTIFICATION_OFFSET   10 /* When to start sending notification after CCCD is enabled */
#else
    #define NOTIFICATION_OFFSET   1
#endif    

volatile uint32 mainTimer = 0;
CYBLE_API_RESULT_T apiResult;
uint8 connected;
uint8 connUpdate = 0;
CYBLE_CONN_HANDLE_T connHandle;
uint8 notificationState = 0;
uint8 initCount = 0;    

/*******************************************************************************
* Function Name: AppCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
*******************************************************************************/
void AppCallBack(uint32 event, void* eventParam)
{
    CYBLE_BLESS_CLK_CFG_PARAMS_T clockConfig;
    
    switch(event)
    {
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            connected = 0;
            notificationState = 0;
            initCount = 0;
        case CYBLE_EVT_STACK_ON:
        
            #if ENABLE_850_XTAL_STARTUP
            clockConfig.bleLlClockDiv = CYBLE_LL_ECO_CLK_DIV_2;
            clockConfig.bleLlSca = CYBLE_LL_SCA_031_TO_050_PPM;
            clockConfig.ecoXtalStartUpTime = 15; 
            CyBle_SetBleClockCfgParam(&clockConfig);
            #endif

            /* Put the device into discoverable mode so that remote can search it. */
                apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;
            
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            connected = 1;
            break;
                
        case CYBLE_EVT_GATT_CONNECT_IND:
            connHandle.attId = ((CYBLE_CONN_HANDLE_T *)eventParam)->attId;
            connHandle.bdHandle = ((CYBLE_CONN_HANDLE_T *)eventParam)->bdHandle;
            break;        

        default:
            break;
    }
}

/*******************************************************************************
* Function Name: HeartRateCallBack(uint32 event, void* eventParam)
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the Heart Rate Service.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
*******************************************************************************/
void HeartRateCallBack(uint32 event, void* eventParam)
{
    switch(event)
    {
        case CYBLE_EVT_HRSS_NOTIFICATION_ENABLED:
            #if CON_PARAM_UPDATE
            connUpdate = 1;
            #endif    
            notificationState = 1;
        break;
        
        case CYBLE_EVT_HRSS_NOTIFICATION_DISABLED:
            notificationState = 0;
            connUpdate = 0;
        break;
    }
}

/*******************************************************************************
* Function Name: int main()
********************************************************************************
*
* Summary:
*   Main entry point for the project. 
*
* Parameters: None
*  
*******************************************************************************/
int main()
{
    CYBLE_LP_MODE_T lpMode;
    CYBLE_BLESS_STATE_T blessState;
    uint32 intStatus;
    
    #if SEND_NOTIFICATIONS
    uint8 notifyValue = 60;
    uint8 heartRatePacket[2] = {0x01, 60};
    #endif

    CyGlobalIntEnable;
    
    CySysClkIloStop();
    
    /* Start the BLE component and register generic event handler */
    apiResult = CyBle_Start(AppCallBack);
    
    /* Services initialization */
    CyBle_HrsRegisterAttrCallback(HeartRateCallBack);
    
    /* Set XTAL divider to 3MHz mode */
    CySysClkWriteEcoDiv(CY_SYS_CLK_ECO_DIV8);            
    
    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1)
    {
        if(CyBle_GetState() != CYBLE_STATE_INITIALIZING)
        {
            /* Enter DeepSleep mode between connection intervals */
            lpMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
            
            intStatus = CyEnterCriticalSection();
            
            blessState = CyBle_GetBleSsState();

            if(lpMode == CYBLE_BLESS_DEEPSLEEP)
            {   
                if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
                {
                    #if DEBUG_ENABLE
                    Pin_1_Write(1);
                    #endif
                    
                    CySysPmDeepSleep();
                    
                    #if DEBUG_ENABLE
                    Pin_1_Write(0);
                    #endif
                }
            }
            else
            {
                #if DEBUG_ENABLE
                Pin_2_Write(1);
                #endif
                
                CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_ECO);
                CySysClkImoStop();
                CySysPmSleep();
                CySysClkImoStart();
                CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_IMO);
                
                #if SEND_NOTIFICATIONS
                CyExitCriticalSection(intStatus);
                if(blessState == CYBLE_BLESS_STATE_ECO_STABLE && notificationState)
                {
                    if(initCount < NOTIFICATION_OFFSET) /* Wait for 'X' connection intervals before starting notifications */
                    {
                        initCount++;
                    }
                    else
                    {
                        notifyValue++;
                        
                        #if NOTIF_INTERVAL_FOUR_SEC
                        if(notifyValue % 4 == 0)    
                        #endif    
                        {
                            if(notifyValue > 160)
                            {
                                notifyValue = 60;   
                            }
                            heartRatePacket[1] = notifyValue;
                           
                            CyBle_HrssSendNotification(connHandle, CYBLE_HRS_HRM, sizeof(heartRatePacket), heartRatePacket);
                        }
                    }
                }
                #endif                
                
                #if DEBUG_ENABLE
                Pin_2_Write(0);
                #endif
            }
            CyExitCriticalSection(intStatus);
        }
             
        /*******************************************************************
        *  Process all pending BLE events in the stack
        *******************************************************************/
        CyBle_ProcessEvents();
        
        if(connected && connUpdate)
        {
            CYBLE_API_RESULT_T conResult;
            static CYBLE_GAP_CONN_UPDATE_PARAM_T hrmConnectionParam =
            {
                790,        /* Minimum connection interval of 987 ms */
                800,        /* Maximum connection interval of 1000 ms */
                0,          /* Slave latency */
                500        /* Supervision timeout of 5 seconds */
            };

            conResult = CyBle_L2capLeConnectionParamUpdateRequest(connHandle.bdHandle, &hrmConnectionParam);
            
            if(conResult != CYBLE_ERROR_OK)
            {
                connUpdate = 0;
            }
        }
    }
}

/* [] END OF FILE */
