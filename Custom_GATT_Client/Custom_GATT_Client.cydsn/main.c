/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdbool.h>

/* CapSense slider example server configurations */
#define CAPSENSE_SLIDER_ATT_UUID        (0xCAA2u)

/* Fixed CapSense example BD address */
static CYBLE_GAP_BD_ADDR_T detectedServerAddr = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0};

/* Flag to check if Advertisement packet occurred from CapSense Slider device */
static bool isCapSenseDeviceFound = false;
/* Flag for device which is waiting for scan */
static bool isScanRspOccured = false;
/* Flag to stop client processing */
static bool isClientDone = false;

static bool IsCapSenseSliderSupported(CYBLE_GAPC_ADV_REPORT_T* scanReport)
{
    uint16 byteindex;

    for(byteindex = 0; byteindex <  scanReport->dataLen; )
    {
        /* Each advt data element contains: Length + Type + Data */
        uint8 advStructureSize = scanReport->data[byteindex];
        if(advStructureSize)
        {
            /* Check the type of Adv Data */
            uint8 advStructureType =
                           scanReport->data[byteindex + sizeof(advStructureSize)];
            /* Slider UUID can be available in service data structure */
            if(CYBLE_GAP_ADV_SRVC_DATA_128UUID == advStructureType)
            {                  
                /* Point to Adv structure data */
                uint8 UuidIndex = byteindex + sizeof(advStructureSize) + sizeof(advStructureType);
                /* Parse the list of UUIDs and check for CapSense service */
                while(UuidIndex < (byteindex + advStructureSize))
                {
                    if(memcmp(cyBle_customCServ[0].uuid, &scanReport->data[UuidIndex], CYBLE_GATT_128_BIT_UUID_SIZE) == 0u)
                    {
                        /* Check if adv structure contains the service data */
                        UuidIndex += CYBLE_GATT_128_BIT_UUID_SIZE;
                        if(UuidIndex < (byteindex + advStructureSize))
                        {
                            /* Return success, if service data matches to slider ATT UUID */
                            if(CAPSENSE_SLIDER_ATT_UUID == CyBle_Get16ByPtr(&scanReport->data[UuidIndex]))
                            {
                                return true;
                            }
                        }
                    }
                    /* Check next UUID */
                    UuidIndex += CYBLE_GATT_16_BIT_UUID_SIZE;
                }
            }
            /* Move to next AD structure */
            byteindex += (advStructureSize + sizeof(uint8));
        }
        else
        {
            return false;
        }
    }
    return false;
}

static void FilterScanResponsePackets(CYBLE_GAPC_ADV_REPORT_T* scanReport)
{
    if(NULL == scanReport)
    {
        return;
    }

    /* Check if device contains CapSense service */
    if(IsCapSenseSliderSupported(scanReport))
    {
        /* Copy the detected BD addr and set the device found flag */
        memcpy(detectedServerAddr.bdAddr, scanReport->peerBdAddr, sizeof(detectedServerAddr.bdAddr));
        detectedServerAddr.type = scanReport->peerAddrType;
        isCapSenseDeviceFound = true;
        isScanRspOccured = false; /* Wait for scan response */
    }

    /* Check if the scan reponse occured for detected CapSense device */
    if(((CYBLE_GAPC_SCAN_RSP == scanReport->eventType) && 
         (!memcmp(detectedServerAddr.bdAddr, scanReport->peerBdAddr, sizeof(detectedServerAddr.bdAddr)))))
    {
        if(isCapSenseDeviceFound)
        {
            CyBle_GapcStopScan();
            isScanRspOccured = true;
            isCapSenseDeviceFound = false;
        }
    }
}

static void CapSenseClientEventHandler(uint32 event, void *eventParam)
{
    switch(event)
    {
        case CYBLE_EVT_STACK_ON:    
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            isClientDone = false;
            (void)CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
            break;

        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            {
                /* Check if the peer device meets our filtering requirements */
                FilterScanResponsePackets((CYBLE_GAPC_ADV_REPORT_T *)eventParam);
            }
            break;

        case CYBLE_EVT_GATTC_ERROR_RSP:
            {
                /* This event indicates some failure on GATT operation */
                /* Disconnect the device and look for next device */
                CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            }
            break;
            
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:

            break;

        case CYBLE_EVT_HARDWARE_ERROR:
        case CYBLE_EVT_HCI_STATUS:
            {
                CYASSERT(0);
            }
            break;
            
        case CYBLE_EVT_TIMEOUT:
            /* Issue disconnect on any GATT timeout */
            if(CYBLE_GATT_RSP_TO == (CYBLE_TO_REASON_CODE_T)*(uint8 *)eventParam)
            {
                /* Close the connection for any time out errors */
                CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            }
            break;
            
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            {
                /* CapSense value change notification occurred, Handle the LED brightness */
                CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *CapsenseValue = 
                    (CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *)eventParam;

                /* Accept only slider notifications to update the LED brightness */
                if(cyBle_customCServ[0].customServChar[0].customServCharHandle == CapsenseValue->handleValPair.attrHandle)
                {
                    /* Ignore 0xFF and update other values */
                    if((uint8)0xFF != (uint8)*(CapsenseValue->handleValPair.value.val))
                    {
                        /* Update LED brightness with the recevied value */
                    }
                }
            }
            break;

        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ:
            {
                /* Reject connection parameter request from slider, to get faster notifications */
                 if(CYBLE_ERROR_OK != CyBle_L2capLeConnectionParamUpdateResponse(
                     cyBle_connHandle.bdHandle, CYBLE_L2CAP_CONN_PARAM_REJECTED))
                 {
                     CYASSERT(0);
                 }
            }
            break;
    }
}

void ProcessClient(void)
{
    if(!isClientDone)
    {
        /* If the Central device is in disconnected state, process scan progress results and send a connect request to
         * device of intereset */
        if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
        {
            if(isScanRspOccured)
            {
                isScanRspOccured = false;
                if(CYBLE_ERROR_OK != CyBle_GapcConnectDevice(&detectedServerAddr))
                {
                    CYASSERT(0);
                }
            }
        }
        else if(CYBLE_CLIENT_STATE_CONNECTED == CyBle_GetClientState())
        {
            /* Discover all server DB items */
            if(CYBLE_ERROR_OK != CyBle_GattcStartDiscovery(cyBle_connHandle)) 
            {
                /* Assert if unable to start discovery */
                CYASSERT(0);
            }
        }
        else if(CYBLE_CLIENT_STATE_DISCOVERED == CyBle_GetClientState())
        {
            /* Enable notification on server device */
            uint16 attrValue = CYBLE_CCCD_NOTIFICATION;
            CYBLE_API_RESULT_T status;
            CYBLE_GATTC_WRITE_REQ_T writeReqParam;

            /* CapSense slider Descriptor handle to set notifications */
            writeReqParam.attrHandle = cyBle_customCServ[0].customServChar[0].customServCharDesc[0].descHandle;

            writeReqParam.value.len = sizeof(attrValue);
            writeReqParam.value.val = (uint8*)&attrValue;

            status = CyBle_GattcWriteCharacteristicDescriptors(
                                cyBle_connHandle, &writeReqParam);

            if(CYBLE_ERROR_OK == status)
            {
                isClientDone = 1;
            }
            else
            {
                /* Disconnect if GATT write fails */
                (void)CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
            }
        }
    }
}

int main(void)
{
    CYBLE_API_RESULT_T apiResult;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    /* Register callback to handle CapSense client mode */
    apiResult = CyBle_Start(CapSenseClientEventHandler);
    
    if(CYBLE_ERROR_OK != apiResult)
    {
        CYASSERT(0); /* Something went wrong, debug */
    }
    
    for(;;)
    {
        CyBle_ProcessEvents();
        
        ProcessClient();
    }
}

/* [] END OF FILE */
