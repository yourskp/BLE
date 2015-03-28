/*******************************************************************************
* File Name: BLE Connection.c
*
* Version: 1.0
*
* Description: This file contains routines that handle all the BLE interface  
*              connection establishment procedures and event handlers.
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <BLE Connection.h>
#include <Configuration.h>
#include <project.h>
#include <RTC.h>
#include <stdio.h>

/***************************************
*    Function declarations
***************************************/
void BLE_StackEventHandler(uint32 event, void* eventParam);
void BLE_Interface_Init(void);
static uint8 BLE_IsDisconnectionRequestPending(void);
static void BLE_SendDisconnection(void);

/***************************************
*    Global variables
***************************************/
uint8 BLE_status;
uint8 encryptionStatus;

#if DISCONNECT_BLE_AFTER_TIME_SYNC
static uint8 sendDisconnection = 0;    
#endif    

/*******************************************************************************
* Function Name: BLE_Interface_Start
********************************************************************************
*
* Summary:
*  Initialization the BLE interface.
*
* Parameters:  
*  None
*
* Return: 
*  state returned by the BLE interface start API
*
*******************************************************************************/
CYBLE_API_RESULT_T BLE_Interface_Start(void)
{
    CYBLE_API_RESULT_T apiResult;
    
    BLE_Interface_Init();

    apiResult = CyBle_Start(BLE_StackEventHandler);
    
    RTC_Interrupt_StartEx(WDT_Handler);
    
    return apiResult;
}

/*******************************************************************************
* Function Name: BLE_Interface_Init
********************************************************************************
*
* Summary:
*  Initialize the BLE interface state data structures.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
void BLE_Interface_Init(void)
{
    BLE_status = BLE_DISCONNECTED;
    encryptionStatus = BLE_LINK_ENCRYPTION_DISABLED;
}

/*******************************************************************************
* Function Name: BLE_Start_GATT_Client
********************************************************************************
*
* Summary:
*  Initialize the BLE GATT client
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
void BLE_Start_GATT_Client(void)
{
#if (RTC_ENABLE)
    CyBle_CtsRegisterAttrCallback(CtsCallBack);
#endif 
}

/*******************************************************************************
* Function Name: BLE_Engine_Start
********************************************************************************
*
* Summary:
*  Application level API for starting the BLE interface. The API internally calls
*  other BLE interface init APIs to setup the system.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
CYBLE_API_RESULT_T BLE_Engine_Start(void)
{
    CYBLE_API_RESULT_T apiResult;
    
    apiResult = BLE_Interface_Start();
    
    BLE_Start_GATT_Client();
    
    return apiResult;
}

/*******************************************************************************
* Function Name: BLE_Engine_Reinit
********************************************************************************
*
* Summary:
*  Reinitialize the BLE engine after peer device is disconnected.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
void BLE_Engine_Reinit(void)
{
    BLE_Interface_Init();
    
    BLE_Start_GATT_Client();    
}

/*******************************************************************************
* Function Name: BLE_Run
********************************************************************************
*
* Summary:
*  BLE interface processing engine. This API should be continuously called by 
*  the application in its main loop.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
uint8 BLE_Run(void)
{
#if (BLE_GATT_CLIENT_ENABLE)
    CYBLE_API_RESULT_T status;
    
    switch(BLE_status)
    {
        case BLE_CONNECTED:
            status = StartTimeServiceDiscovery();
            if(CYBLE_ERROR_OK == status)
            {            
                BLE_status = BLE_DISCOVEER_GATT_SERVICES;

            }
            else
            {
                BLE_status = BLE_INVALID_OPERATION;   
            }
        break;
            
        case BLE_READ_TIME:
            if(CyBle_GetClientState() == CYBLE_CLIENT_STATE_DISCOVERED)
            {
                status = SyncTimeFromBleTimeServer();
                if(CYBLE_ERROR_OK == status)
                {
                    BLE_status = BLE_IDLE;

                }
                else
                {
                    BLE_status = BLE_INVALID_OPERATION;

                }
            }
        break;
            
        case BLE_INITIATE_AUTHENTICATION:
            CyBle_GapAuthReq(cyBle_connHandle.bdHandle, &cyBle_authInfo);
            BLE_status = BLE_AUTHENTICATION_IN_PROGRESS;
        break;    
        
        default:
        
        break;
    }
#endif
    
    CyBle_ProcessEvents();
    
    /* Store bonding data to flash only when all debug information has been sent */
    if((cyBle_pendingFlashWrite != 0u) 
#if(CONSOLE_LOG_ENABLED)    
       &&((Debug_Console_SpiUartGetTxBufferSize() + Debug_Console_GET_TX_FIFO_SR_VALID) == 0u)
#endif    
    )
    {
        status = CyBle_StoreBondingData(0u);
#if(CONSOLE_LOG_ENABLED)        
        printf("Store bonding data, status: %x \r\n", status);
#endif        
    }
    
#if DISCONNECT_BLE_AFTER_TIME_SYNC    
    if(BLE_IsDisconnectionRequestPending())
    {
        BLE_SendDisconnection();
    }
#endif    
    
    return BLE_status;
}

/*******************************************************************************
* Function Name: BLE_StackEventHandler
********************************************************************************
*
* Summary:
*   BLE stack generic event handler routine for handling connection, discovery, 
*   security etc. events.
*
* Parameters:  
*  event - event that triggered this callback
*  eventParam - parameters for the event.
*
* Return: 
*  None
*******************************************************************************/
void BLE_StackEventHandler(uint32 event, void* eventParam)
{
#if (RESTART_ADV_ON_DISCONNECTION)    
    CYBLE_API_RESULT_T apiResult;
#endif    
    CYBLE_GATTC_ERR_RSP_PARAM_T *errorResponse;
    
    switch (event)
	{
        /**********************************************************
        *                       General Events
        ***********************************************************/
		case CYBLE_EVT_STACK_ON: /* This event is received when component is Started */
            /* Enter in to discoverable mode so that remote can search it. */
            (void) CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);   
            break;
            
		case CYBLE_EVT_TIMEOUT:            
#if (BLE_GATT_CLIENT_ENABLE)
            if(*(uint8 *)eventParam == CYBLE_GATT_RSP_TO && BLE_status == BLE_DISCOVEER_GATT_SERVICES)
            {
                /* The peer device didn't respond to service discovery, enable RTC in free run mode if configured */
                BLE_status = BLE_TIME_SERVER_ABSENT;
#if (RTC_ENABLE)
                RTC_Start();
#endif /* End of #if (RTC_ENABLE) */

#if DISCONNECT_BLE_AFTER_TIME_SYNC               
                BLE_RequestDisconnection();
#endif 
            }
#endif /* End of #if (BLE_GATT_CLIENT_ENABLE) */    

			break;     
        /**********************************************************
        *                       GAP Events
        ***********************************************************/

        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            /* we initiated the authentication with the iOS device and the authentication is now complete. Proceed
             * to characteristic value read after this */
            BLE_status = BLE_READ_TIME;
            break;    
            
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
#if RESTART_ADV_ON_DISCONNECTION            
            BLE_Engine_Reinit(); /* Re-initialize application data structures */
            /* Put the device to discoverable mode so that remote can search it. */
            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            
            if(apiResult != CYBLE_ERROR_OK)
            {
                CYASSERT(0);    
            }
#endif            
            break;    
        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
            BLE_status = BLE_CONNECTED;
            break;
            
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            BLE_status = BLE_DISCONNECTED;
            break;
            
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            if(cyBle_ctsc.currTimeCharacteristics[CYBLE_CTS_CURRENT_TIME].valueHandle == 0x0000)
            {
                BLE_status = BLE_TIME_SERVER_ABSENT;
                
#if (RTC_ENABLE)/* If the time server is absent, let the RTC run in free run mode */
                RTC_Start();
#endif

#if DISCONNECT_BLE_AFTER_TIME_SYNC               
                BLE_RequestDisconnection();
#endif 
            }
            else
            {
                BLE_status = BLE_READ_TIME;
            }
            break;
            
        case CYBLE_EVT_GATTC_ERROR_RSP:
            errorResponse = (CYBLE_GATTC_ERR_RSP_PARAM_T*) eventParam;
            
            /* characteristic read requires an authenticated link */
            if(errorResponse -> errorCode == CYBLE_GATT_ERR_INSUFFICIENT_AUTHENTICATION)
            {
                BLE_status = BLE_INITIATE_AUTHENTICATION;
            }
            break;   
            
        default:            
			break;
	}
}

#if DISCONNECT_BLE_AFTER_TIME_SYNC
/*******************************************************************************
* Function Name: BLE_RequestDisconnection
********************************************************************************
*
* Summary:
*   Request the BLE run routine to issue a disconnection request to the peer 
*   device.
*
* Parameters:  
*  None
*
* Return: 
*  None
*******************************************************************************/
void BLE_RequestDisconnection(void)
{
    sendDisconnection = 1;    
}

/*******************************************************************************
* Function Name: BLE_SendDisconnection
********************************************************************************
*
* Summary:
*   Initiates a BLE disconnection event to the peer device.
*
* Parameters:  
*  None
*
* Return: 
*  None
*******************************************************************************/
static void BLE_SendDisconnection(void)
{
    CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
    sendDisconnection = 0;    
}

/*******************************************************************************
* Function Name: BLE_IsDisconnectionRequestPending
********************************************************************************
*
* Summary:
*   Checks if disconnection request from other blocks are pending or not.
*
* Parameters:  
*  None
*
* Return: 
*  1 - request is penidng, 0 - request not pending.
*******************************************************************************/
static uint8 BLE_IsDisconnectionRequestPending(void)
{
    return sendDisconnection;    
}
#endif    
/* [] END OF FILE */
