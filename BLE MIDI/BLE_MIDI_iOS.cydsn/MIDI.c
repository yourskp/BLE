/*******************************************************************************
* File Name: ias.c
*
* Description:
*  This file contains Immediate Alert Service callback handler function.
* 
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <MIDI.h>
#include <project.h>
#include <UI.h>

static volatile uint8 enableMidi = 0;
static MIDI_BUFFER midiBuffer;
static uint16 previousUiChangeStatus = NO_FINGER_ON_SLIDER;

/*******************************************************************************
* Function Name: IasEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component,
*  which are specific to Immediate Alert Service.
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
void MIDIEventHandler(uint32 event, void *eventParam)
{
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T midiNotificationCccdHandle;
    uint8 midiCccdUpdate = 0x01;
    
    switch(event)
    {
        case CYBLE_EVT_GATTS_WRITE_REQ:
            /* This event is received when iOS device sends a Write command on an Attribute */
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            
        	/* When this event is triggered, the peripheral has received a write command on the custom characteristic */
        	/* Check if command is for correct attribute and update the flag for sending Notifications */
            if(CYBLE_MIDI_SERVICE_MIDI_IO_CHARATERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == wrReqParam->handleValPair.attrHandle)
        	{
        		/* Extract the Write value sent by the Client for CapSense Slider CCCD */
                enableMidi = wrReqParam->handleValPair.value.val[CYBLE_MIDI_SERVICE_MIDI_IO_CHARATERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX];
        		
        		/* Update CCCD handle with notification status data*/
        		midiNotificationCccdHandle.attrHandle = CYBLE_MIDI_SERVICE_MIDI_IO_CHARATERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
        		midiNotificationCccdHandle.value.val = &midiCccdUpdate;
        		midiNotificationCccdHandle.value.len = sizeof(midiCccdUpdate);
        		
        		/* Report data to BLE component for sending data when read by Central device */
        		CyBle_GattsWriteAttributeValue(&midiNotificationCccdHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
            }
            
            CyBle_GattsWriteRsp(cyBle_connHandle);
        break;
            
        case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
           ServiceMidiOutData(eventParam);
        break;
    }
}

/*******************************************************************************
* Function Name: Midi_Start
********************************************************************************
*
* Summary:
*  This function initializes the MIDI data structures.
*
* Parameters:
*  void
*
* Return:
*  None
*******************************************************************************/
void Midi_Start(void)
{
    uint8 index;
    
    midiBuffer.packetIndex = 0;
    
    for(index = 0; index < MAX_NUMBER_OF_MIDI_PACKETS; index++)
    {
        midiBuffer.midiPacket[index].dataIndex = 0;
        midiBuffer.midiPacket[index].packetStatus = MIDI_PACKET_INVALID;
    }
}

/*******************************************************************************
* Function Name: IsMidiEnabled
********************************************************************************
*
* Summary:
*  This function returns the state of the BLE MIDI interface.
*
* Parameters:
*  void
*
* Return:
*  uint8 - status of the MIDI interface. 
*  Returns 1 if MIDI IO Char CCCD is enabled
*  Returns 0 if MIDI IO Char CCCD is disabled
*
*******************************************************************************/
uint8 IsMidiEnabled(void)
{
    return enableMidi;    
}

/*******************************************************************************
* Function Name: ServiceMidiOutData
********************************************************************************
*
* Summary:
*  This function services the MIDI out messages received from the iOS/Mac through
*  a MIDI IO Characteristic write.
*
* Parameters:
*  eventParam -  MIDI note write event data
*
* Return:
*  None
*
*******************************************************************************/
void ServiceMidiOutData(void *eventParam)
{
    CYBLE_GATTS_WRITE_REQ_PARAM_T *midiOutEvent = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T midiOutData;
    uint8 midiDataLength;
    
    midiOutData = midiOutEvent->handleValPair;
    
    /* This write event is for the MIDI IO characteristic */
    if(CYBLE_MIDI_SERVICE_MIDI_IO_CHARATERISTIC_CHAR_HANDLE == midiOutData.attrHandle)
    {
        midiDataLength = midiOutData.value.len;
        
        if(midiDataLength == MIDI_FULL_EVENT_SIZE)
        {
            switch(midiOutData.value.val[MIDI_STATUS_INDEX])
            {
                case MIDI_NOTE_ON_CHANNEL_0:
                    UIOutput_UpdateMailbox(midiOutData.value.val[MIDI_NOTE_VELOCITY_INDEX]);
                break;
                
                case MIDI_NOTE_OFF_CHANNEL_0:
                    UIOutput_UpdateMailbox(0);
                break;
            }
        }
    }
}

/*******************************************************************************
* Function Name: Midi_Run
********************************************************************************
*
* Summary:
*  This function services the MIDI IN messages received from the UI (CapSense)
*  and initiates a notification transfer
*
* Parameters:
*  uiChangeStatus - Indicates the current state of the UI element. Based on the 
*  state of the UI, corresponding MIDI messages are generated and notifications
*  are initiated.
*
* Return:
*  None
*
*******************************************************************************/
void Midi_Run(uint16 uiChangeStatus)
{
    Midi_GenerateMessages(uiChangeStatus);
    
    Midi_TransferMessages();
}

/*******************************************************************************
* Function Name: Midi_GenerateMessages
********************************************************************************
*
* Summary:
*  This function services the MIDI IN messages received from the UI (CapSense)
*  and creates corresponding MIDI messages
*
* Parameters:
*  uiChangeStatus - Indicates the current state of the UI element. Based on the 
*  state of the UI, corresponding MIDI messages are generated and store in the 
*  MIDI IN buffer.
*
* Return:
*  None
*
*******************************************************************************/
static void Midi_GenerateMessages(uint16 uiChangeStatus)
{
    /* There was a change in UI, generate the corresponding MIDI message */
    if(uiChangeStatus != NO_UI_STATE_CHANGE)
    {
        if(previousUiChangeStatus != NO_FINGER_ON_SLIDER || uiChangeStatus == NO_FINGER_ON_SLIDER)
        {
            Midi_UpdateBufferPointers();
            
            /* If a note ON was sent earlier, send a note off message before beginning the new note */
            midiBuffer.midiPacket[midiBuffer.packetIndex].midiNotificationPacket\
            [midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex++] = Midi_GetTimeStamp(LOW);
            
            midiBuffer.midiPacket[midiBuffer.packetIndex].midiNotificationPacket\
            [midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex++] = MIDI_NOTE_ON_CHANNEL_0;
            
            midiBuffer.midiPacket[midiBuffer.packetIndex].midiNotificationPacket\
            [midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex++] = (uint8)previousUiChangeStatus + MIDI_NOTE_OFFSET;
            
            midiBuffer.midiPacket[midiBuffer.packetIndex].midiNotificationPacket\
            [midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex++] = MIDI_NOTE_VELOCITY_OFF;
        }
        
        if(uiChangeStatus != NO_FINGER_ON_SLIDER)
        {
            Midi_UpdateBufferPointers();
            
            /* Send a new note ON message */
            midiBuffer.midiPacket[midiBuffer.packetIndex].midiNotificationPacket\
            [midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex++] = Midi_GetTimeStamp(LOW);
            
            midiBuffer.midiPacket[midiBuffer.packetIndex].midiNotificationPacket\
            [midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex++] = MIDI_NOTE_ON_CHANNEL_0;
            
            midiBuffer.midiPacket[midiBuffer.packetIndex].midiNotificationPacket\
            [midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex++] = (uint8)uiChangeStatus + MIDI_NOTE_OFFSET;
            
            midiBuffer.midiPacket[midiBuffer.packetIndex].midiNotificationPacket\
            [midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex++] = MIDI_NOTE_VELOCITY_ON;
        }
        
        previousUiChangeStatus = uiChangeStatus;
    }
}

/*******************************************************************************
* Function Name: Midi_TransferMessages
********************************************************************************
*
* Summary:
*  This function fetches the MIDI messages from the MIDI buffer and triggers
*  a BLE notification.
*
* Parameters:
*  void
*
* Return:
*  None
*
*******************************************************************************/
static void Midi_TransferMessages(void)
{
    uint8 index;
    
    /* Parse the buffers and extract messages */
    for(index = 0; index < MAX_NUMBER_OF_MIDI_PACKETS; index++)
    {
        /* Valid packet is waiting to be scheduled for notification */
        if(midiBuffer.midiPacket[index].packetStatus == MIDI_PACKET_VALID && midiBuffer.midiPacket[index].dataIndex > 1)
        {
        	CYBLE_GATTS_HANDLE_VALUE_NTF_T		midiNotification;	
        	
        	/* If stack is not busy, then send the notification */
        	if( CyBle_GattGetBusStatus() == CYBLE_STACK_STATE_FREE &&
                ((midiBuffer.midiPacket[index].dataIndex >= (CYBLE_GATT_MTU - MAX_MIDI_PACKET_LENGTH) ) ||
                CyBle_GetBleSsState() != CYBLE_BLESS_STATE_EVENT_CLOSE) )
        	{
                CYBLE_API_RESULT_T result;
                
        		/* Update notification handle with CapSense slider data*/
        		midiNotification.attrHandle = CYBLE_MIDI_SERVICE_MIDI_IO_CHARATERISTIC_CHAR_HANDLE;				
        		midiNotification.value.val = &(midiBuffer.midiPacket[index].midiNotificationPacket[0]);
        		midiNotification.value.len = midiBuffer.midiPacket[index].dataIndex;
        		
        		/* Send the updated handle as part of attribute for notifications */
        		result = CyBle_GattsNotification(cyBle_connHandle,&midiNotification);
                
                if(result == CYBLE_ERROR_OK)
                {
                    /* Notification is successfully queued */
                    midiBuffer.midiPacket[index].packetStatus = MIDI_PACKET_INVALID;
                    midiBuffer.midiPacket[index].dataIndex = 0;
                }
                else
                {
                    break;    
                }
        	}
            else
            {
                break;    
            }
        }
    }
}

/*******************************************************************************
* Function Name: Midi_UpdateBufferPointers
********************************************************************************
*
* Summary:
*  This function updates the MIDI buffer pointers
*
* Parameters:
*  void
*
* Return:
*  None
*
*******************************************************************************/
static void Midi_UpdateBufferPointers(void)
{
    if(midiBuffer.midiPacket[midiBuffer.packetIndex].packetStatus == MIDI_PACKET_VALID &&
        midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex >= (CYBLE_GATT_MTU - MAX_MIDI_PACKET_LENGTH + 1))
    {
        midiBuffer.packetIndex++;
        
        if(midiBuffer.packetIndex == MAX_NUMBER_OF_MIDI_PACKETS)
        {
            midiBuffer.packetIndex = 0;
        }
        
        if(midiBuffer.midiPacket[midiBuffer.packetIndex].packetStatus == MIDI_PACKET_VALID)
        {
            /* The updated packet is still valid and is not sent over BLE. Flag this as an overflow condition */
            CYASSERT(0);
        }
    }
    
    if(midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex == 0)
    {
        /* Insert the header byte to the packet structure */
        midiBuffer.midiPacket[midiBuffer.packetIndex].midiNotificationPacket\
        [midiBuffer.midiPacket[midiBuffer.packetIndex].dataIndex++] = MIDI_PACKET_HEADER;
        
        midiBuffer.midiPacket[midiBuffer.packetIndex].packetStatus = MIDI_PACKET_VALID;
    }
}

/*******************************************************************************
* Function Name: Midi_GetTimeStamp
********************************************************************************
*
* Summary:
*  This function returns the timestamp used for stamping each of the MIDI messages
*
* Parameters:
*  byteNumber - Specifies which byte of the time stamp value is required
*
* Return:
*  uint8 - upper or lower byte value of the current timestamp
*
*******************************************************************************/
static uint8 Midi_GetTimeStamp(uint8 byteNumber)
{
    static uint16 timeStamp = 0x80;
    
    if(byteNumber == HIGH)
    {
#if 0        
        timeStamp++;
#endif    
        return (uint8)((timeStamp >> 8) & 0x3F); 
    }
    else if (byteNumber == LOW)
    {
        return (uint8)(timeStamp & 0xFF);
    }
    else
    {
        
    }
    return 0;
}


/* [] END OF FILE */
