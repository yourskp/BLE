/*******************************************************************************
File Name: CYBLE_ess.c
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 Contains the source code for Environmental Sensing Service.

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`_eventHandler.h"


#ifdef CYBLE_ESS_SERVER
/* Generated code */
`$EssServer`
/* End of generated code */

static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_esssReqHandle;

#if(CYBLE_GAP_ROLE_PERIPHERAL)
    static CYBLE_GAPP_DISC_DATA_T esssAdvertisementData;
#endif /* (CYBLE_GAP_ROLE_PERIPHERAL) */

#endif /* CYBLE_ESS_SERVER */

#ifdef CYBLE_ESS_CLIENT
/* Generated code */
`$EssClient`
/* End of generated code */

static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_esscReqHandle;

/* Read Long Descriptors variables */
static uint8 * cyBle_esscRdLongBuffPtr = NULL;
static uint8 cyBle_esscRdLongBuffLen = 0u;
static uint8 cyBle_esscCurrLen = 0u;

uint8 activeCharIndex = 0u;
uint8 activeCharInstance = 0u;
uint8 prevCharInstIndex;

#endif /* (CYBLE_ESS_CLIENT) */

static CYBLE_CALLBACK_T CyBle_EssApplCallback = NULL;


/******************************************************************************
##Function Name: CyBle_EssInit
*******************************************************************************

Summary:
 This function initializes the Environmental Sensing Service.

Parameters:
 None.

Return:
 None.

******************************************************************************/
void CyBle_EssInit(void)
{
#ifdef CYBLE_ESS_SERVER

    cyBle_esssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_ESS_SERVER */

#ifdef CYBLE_ESS_CLIENT

    activeCharIndex = 0u;
    activeCharInstance = 0u;
    prevCharInstIndex = 0u;

    cyBle_esscRdLongBuffPtr = NULL;
    cyBle_esscRdLongBuffLen = 0u;
    cyBle_esscCurrLen = 0u;

    CyBle_EssClearClientDiscInfo();

    cyBle_esscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_ESS_CLIENT */
}


/******************************************************************************
##Function Name: CyBle_EssRegisterAttrCallback
*******************************************************************************

Summary:
 Registers a callback function for service specific attribute operations.

Parameters:
 callbackFunc:  An application layer event callback function to receive
                events from the BLE Component. The definition of
                CYBLE_CALLBACK_T for HTS Service is,

                typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode,
                                                   void *eventParam)

                * eventCode:  Indicates the event that triggered this callback
                  (e.g. CYBLE_EVT_ESSS_NOTIFICATION_ENABLED).
                * eventParam: Contains the parameters corresponding to the
                  current event. (e.g. Pointer to CYBLE_ESS_CHAR_VALUE_T
                  structure that contains details of the characteristic
                  for which the notification enabled event was triggered).

Return:
 None.

******************************************************************************/
void CyBle_EssRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_EssApplCallback = callbackFunc;
}


#ifdef CYBLE_ESS_SERVER

/******************************************************************************
##Function Name: CyBle_EssGet24ByPtr
*******************************************************************************

Summary:
 Returns a three-bytes value by using a pointer to the LSB. The value is
 returned as uint32.

Parameters:
 ptr: The pointer to the LSB of three-byte data (little-endian).

Return:
 uint32 value: Three-byte data.

******************************************************************************/
uint32 CyBle_EssGet24ByPtr(const uint8 ptr[])
{
    return (((uint32) ptr[0u]) | ((uint32)(((uint32) ptr[1u]) << 8u)) | ((uint32)((uint32) ptr[2u]) << 16u));
}


/******************************************************************************
##Function Name: CyBle_EssGet32ByPtr
*******************************************************************************

Summary:
 Returns a four-byte value by using a pointer to the LSB.

Parameters:
 ptr: The pointer to the LSB of four-byte data (little-endian).

Return:
 uint32 value: Four-byte data.

******************************************************************************/
uint32 CyBle_EssGet32ByPtr(const uint8 ptr[])
{
    return (((uint32) ptr[0u]) | ((uint32) (((uint32) ptr[1u]) << 8u)) | ((uint32)((uint32) ptr[2u]) << 16u) |
        ((uint32)((uint32) ptr[3u]) << 24u));
}


/******************************************************************************
##Function Name: CyBle_EsssCheckIfInRange
*******************************************************************************

Summary:
 Checks if the "value" is in the range of "min" and "max". As the ESS
 characteristics operate with signed or unsigned value types and the value in
 the GATT database are stored in unsigned variables. Need to clearly identify
 the signedness of each "value", "min" and "max". After that the values can
 be properly compared. The signedness of each parameter is encoded in "state"
 and has the following meaning:
    State 0: All three are positive (all three are of unsigned type)
    State 1: Min - positive, Max - positive, Value - negative
    State 2: Min - positive, Max - negative, Value - positive
    State 3: Min - positive, Max - negative, Value - negative
    State 4: Min - negative, Max - positive, Value - positive
    State 5: Min - negative, Max - positive, Value - negative
    State 6: Min - negative, Max - negative, Value - positive
    State 7: All three are negative

Parameters:
 min - Minimum inclusive limit.
 max - Minimum inclusive limit.
 value - The value to be checked if it is in the range.
 state - The state as shown above.

Return:
 A return value is of type CYBLE_GATT_ERR_CODE_T.
  * CYBLE_GATT_ERR_NONE - "value" is in the range.
  * CYBLE_GATT_ERR_OUT_OF_RANGE - "value" is not in the range.

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_EsssCheckIfInRange(uint32 min, uint32 max, uint32 value, uint8 state)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;

    switch(state)
    {
        /* All values are positive */
        case CYBLE_ESS_STATE_0:
            if((min > value) || (max < value))
            {
                gattErr = CYBLE_GATT_ERR_OUT_OF_RANGE;
            }
            break;

        /* "Not in range" cases. Min and Max are negative and value is positive or
        * vice versa.
        */
        case CYBLE_ESS_STATE_1:
        case CYBLE_ESS_STATE_6:
            gattErr = CYBLE_GATT_ERR_OUT_OF_RANGE;
            break;

        /* Min is negative, Max and value are positive */
        case CYBLE_ESS_STATE_4:
            if(max < value)
            {
                gattErr = CYBLE_GATT_ERR_OUT_OF_RANGE;
            }
            break;

        /* Max is positive, Max and value are negative */
        case CYBLE_ESS_STATE_5:
            if(min < value)
            {
                gattErr = CYBLE_GATT_ERR_OUT_OF_RANGE;
            }
            break;

        /* All values are negative */
        case CYBLE_ESS_STATE_7:
            if((min < value) || (max > value))
            {
                gattErr = CYBLE_GATT_ERR_OUT_OF_RANGE;
            }
            break;

        /* Invalid cases. Handled by customizer. */
        case CYBLE_ESS_STATE_2:
        case CYBLE_ESS_STATE_3:
        default:
            gattErr = CYBLE_GATT_ERR_OUT_OF_RANGE;
            break;
    }

    return (gattErr);
}


/******************************************************************************
##Function Name: CyBle_EssHandleRangeCheck
*******************************************************************************

Summary:
 Performs an extraction of the characteristic value ranges then compares it to
 the value received from the client and returns a result of the comparison.

Parameters:
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance number of the characteristic specified by
               "charIndex".
 length: The length of a buffer to store the main and may ranges. Can be 2,4,6
         or 8 bytes.
 type: Identifies the type of the value pointed by "pValue" (0 - the value
       is of unsigned type, 1 - the value is of signed type).
 pValue: The pointer to the value to be validated if it is in the range.


Return:
 A return value is of type CYBLE_GATT_ERR_CODE_T.
  * CYBLE_GATT_ERR_NONE - The value stored in "pValue" is in ranges specified
                          by Valid Range Descriptor of the characteristic 
                          addressed by "charIndex" and "charInstance".
  * CYBLE_GATT_ERR_UNLIKELY_ERROR - Failed to read Valid Range Descriptor 
                                    value.
  * CYBLE_GATT_ERR_OUT_OF_RANGE - The value stored in "pValue" is not in the
                                  valid ranges.

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_EssHandleRangeCheck(CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, uint16 length,
    uint8 type, const uint8 pValue[])
{
    uint8  state = 0u;
    uint8  skipSignCheck = 0u;
    uint8  tmpBuff[8u];
    uint32 maxLimit = 0u;
    uint32 minLimit = 0u;
    uint32 operand = 0u;
    uint32 u32Sign = 0u;
    CYBLE_GATT_ERR_CODE_T apiResult = CYBLE_GATT_ERR_UNLIKELY_ERROR;
    
    if(CYBLE_ERROR_OK == CyBle_EsssGetCharacteristicDescriptor(charIndex, charInstance, CYBLE_ESS_VRD, length, tmpBuff))
    {
        switch(length)
        {
            case CYBLE_ESS_2BYTES_LENGTH:
                operand =  (uint32)(pValue[0u]);
                minLimit = (uint32)(tmpBuff[0u]);
                maxLimit = (uint32)(tmpBuff[1u]);
                u32Sign = CYBLE_ESS_U8_SIGN_BIT;
                break;
            case CYBLE_ESS_4BYTES_LENGTH:
                operand =  (uint32) CyBle_EssGet16ByPtr(&pValue[0u]);
                minLimit = (uint32) CyBle_EssGet16ByPtr(&tmpBuff[0u]);
                maxLimit = (uint32) CyBle_EssGet16ByPtr(&tmpBuff[2u]);
                u32Sign = CYBLE_ESS_U16_SIGN_BIT;
                break;
            case CYBLE_ESS_6BYTES_LENGTH:
                operand =  (uint32) CyBle_EssGet24ByPtr(&pValue[0u]);
                minLimit = (uint32) CyBle_EssGet24ByPtr(&tmpBuff[0u]);
                maxLimit = (uint32) CyBle_EssGet24ByPtr(&tmpBuff[3u]);
                u32Sign = CYBLE_ESS_U24_SIGN_BIT;
                break;
            case CYBLE_ESS_8BYTES_LENGTH:
                operand =  (uint32) CyBle_EssGet32ByPtr(&pValue[0u]);
                minLimit = (uint32) CyBle_EssGet32ByPtr(&tmpBuff[0u]);
                maxLimit = (uint32) CyBle_EssGet32ByPtr(&tmpBuff[4u]);
                break;

            default:
                skipSignCheck = 1u;
                break;
        }

        if(0u == skipSignCheck)
        {
            if(type != CYBLE_ESS_UNSIGNED_TYPE)
            {
                state = CYBLE_ESS_IS_NEGATIVE(minLimit, u32Sign) << 1u;
                state = (state | CYBLE_ESS_IS_NEGATIVE(maxLimit, u32Sign)) << 1u;
                state |= CYBLE_ESS_IS_NEGATIVE(operand, u32Sign);
            }
            else
            {
                state = 0u;
            }
        }

        /* Compare the operand to the limits with respect to the state
        * and return the results. 
        */
        apiResult = CyBle_EsssCheckIfInRange(minLimit, maxLimit, operand, state);
    }

    return(apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsssWriteEventHandler
*******************************************************************************

Summary:
 Handles a Write Request event for Environmental Sensing Service.

Parameters:
 eventParam: The pointer to the data structure specified by an event.

Return:
 A return value is of type CYBLE_GATT_ERR_CODE_T.
  * CYBLE_GATT_ERR_NONE - Write is successful.
  * CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED - Request is not supported.
  * CYBLE_GATT_ERR_INVALID_HANDLE - 'handleValuePair.attrHandle' is not valid.
  * CYBLE_GATT_ERR_WRITE_NOT_PERMITTED - Write operation is not permitted on
                                         this attribute.
  * CYBLE_GATT_ERR_INVALID_OFFSET - the offset value is invalid.
  * CYBLE_GATT_ERR_UNLIKELY_ERROR - Some other error occurred.
  * CYBLE_GATT_ERR_CONDITION_NOT_SUPPORTED - The condition in ES Trigger Settings
                                             Descriptor is not supported.
  * CYBLE_GATT_ERR_WRITE_REQ_REJECTED - A write request was rejected.

******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_EsssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    uint8 i;
    uint8 j;
    uint32 event = (uint32) CYBLE_EVT_ESSS_DESCR_WRITE;
    uint8 foundItem = 0u;
    CYBLE_ESSS_CHAR_T *essCharInfoPtr;
    CYBLE_ESS_DESCR_VALUE_T wrDescrReqParam;
    CYBLE_GATT_DB_ATTR_HANDLE_T tmpHandle;
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;

    tmpHandle = eventParam->handleValPair.attrHandle;

    if(NULL != CyBle_EssApplCallback)
    {
        wrDescrReqParam.descrIndex = CYBLE_ESS_DESCR_COUNT;
        
        /* Go through all possible Service characteristics.
        * Exit the handler in following conditions:
        * 1) No more characteristic left;
        * 2) Characteristic or descriptor was successfully written;
        * 3) Error occurred while writing characteristic or descriptor.
        */
        for(i = 0u; ((i < ((uint8) CYBLE_ESS_CHAR_COUNT)) && (0u == foundItem) && (CYBLE_GATT_ERR_NONE == gattErr));
            i++)
        {
            /* Check if characteristic is enabled. If the pointer to the characteristic
            * is not "NULL", there is at least one instance of the characteristic in
            * the ES Service.
            */
            if(NULL != cyBle_esss.charInfoAddr[i].charInfoPtr)
            {
                for(j = 0u; j < cyBle_esssCharInstances[i]; j++)
                {
                    essCharInfoPtr = &cyBle_esss.charInfoAddr[i].charInfoPtr[j];

                    /* Client Characteristic Configuration Descriptor Write Request handling */
                    if(tmpHandle == essCharInfoPtr->descrHandle[CYBLE_ESS_CCCD])
                    {
                        /* Verify that optional notification property is enabled for characteristic */
                        if(CYBLE_IS_NOTIFICATION_SUPPORTED(essCharInfoPtr->charHandle))
                        {
                            if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                            {
                                event = (uint32)CYBLE_EVT_ESSS_NOTIFICATION_ENABLED;
                            }
                            else
                            {
                                event = (uint32)CYBLE_EVT_ESSS_NOTIFICATION_DISABLED;
                            }

                            /* Value is NULL for descriptors */
                            wrDescrReqParam.value = NULL;
                            wrDescrReqParam.descrIndex = CYBLE_ESS_CCCD;
                            foundItem = CYBLE_ESS_DESCRIPTOR_ITEM;

                        #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
                            (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
                            /* Set flag to store bonding data to flash */
                            cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
                        #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
                        }
                        else if(CYBLE_IS_INDICATION_SUPPORTED(essCharInfoPtr->charHandle))
                        {
                            if(CYBLE_IS_INDICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                            {
                                event = (uint32)CYBLE_EVT_ESSS_INDICATION_ENABLED;
                            }
                            else
                            {
                                event = (uint32)CYBLE_EVT_ESSS_INDICATION_DISABLED;
                            }

                            /* Value is NULL for descriptors */
                            wrDescrReqParam.value = NULL;
                            wrDescrReqParam.descrIndex = CYBLE_ESS_CCCD;
                            foundItem = CYBLE_ESS_DESCRIPTOR_ITEM;

                        #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
                            (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
                            /* Set flag to store bonding data to flash */
                            cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
                        #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
                        }
                        else
                        {
                            gattErr = (CYBLE_GATT_ERR_CODE_T) CYBLE_GATT_ERR_WRITE_REQ_REJECTED;
                        }
                    }
                    else if(tmpHandle == essCharInfoPtr->descrHandle[CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1])
                    {
                        wrDescrReqParam.descrIndex = CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1;
                        foundItem = CYBLE_ESS_DESCRIPTOR_ITEM;
                    }
                    else if(tmpHandle == essCharInfoPtr->descrHandle[CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR2])
                    {
                        wrDescrReqParam.descrIndex = CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR2;
                        foundItem = CYBLE_ESS_DESCRIPTOR_ITEM;
                    }
                    else if(tmpHandle == essCharInfoPtr->descrHandle[CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR3])
                    {
                        wrDescrReqParam.descrIndex = CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR3;
                        foundItem = CYBLE_ESS_DESCRIPTOR_ITEM;
                    }
                    else if(tmpHandle == essCharInfoPtr->descrHandle[CYBLE_ESS_ES_CONFIG_DESCR])
                    {
                        wrDescrReqParam.descrIndex = CYBLE_ESS_ES_CONFIG_DESCR;
                        foundItem = CYBLE_ESS_DESCRIPTOR_ITEM;
                    }
                    else if(tmpHandle == essCharInfoPtr->descrHandle[CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR])
                    {
                        wrDescrReqParam.descrIndex = CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR;
                        foundItem = CYBLE_ESS_DESCRIPTOR_ITEM;
                    }
                    else
                    {
                        /* No handle match was found */
                    }

                    /* Verify if requested handle was found and successfully handled */
                    if((gattErr == CYBLE_GATT_ERR_NONE) && (0u != foundItem))
                    {
                        switch(wrDescrReqParam.descrIndex)
                        {
                        case CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1:
                        case CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR2:
                        case CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR3:
                            if(eventParam->handleValPair.value.val[0u] > CYBLE_ESS_TRIG_WHILE_EQUAL_NOT_TO)
                            {
                                /* Trigger condition is not supported */
                                gattErr = (CYBLE_GATT_ERR_CODE_T) CYBLE_GATT_ERR_CONDITION_NOT_SUPPORTED;
                            }
                            /* If Valid Range Descriptor for the characteristic is present, then
                            * check the characteristic value range. The ranges should be properly
                            * set in the server or otherwise the server will constantly return an
                            * "Out of range" error.
                            */
                            else if((CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
                                essCharInfoPtr->descrHandle[CYBLE_ESS_VRD]) &&
                                    (eventParam->handleValPair.value.len > 1u)&& 
                                        (eventParam->handleValPair.value.val[0u] > CYBLE_ESS_TRIG_WHILE_LESS_THAN))
                            {
                                /* The following "switch" groups characteristics are based on their value
                                * type.
                                */
                                switch((CYBLE_ESS_CHAR_INDEX_T) i)
                                {
                                    /* uint8 */
                                    case CYBLE_ESS_BAROMETRIC_PRESSURE_TREND:
                                    case CYBLE_ESS_GUST_FACTOR:
                                    case CYBLE_ESS_UV_INDEX:
                                        gattErr = CyBle_EssHandleRangeCheck((CYBLE_ESS_CHAR_INDEX_T) i, j,
                                            CYBLE_ESS_2BYTES_LENGTH, CYBLE_ESS_UNSIGNED_TYPE,
                                                &eventParam->handleValPair.value.val[1u]);
                                        break;

                                    /* uint16 */
                                    case CYBLE_ESS_APPARENT_WIND_DIR:
                                    case CYBLE_ESS_APPARENT_WIND_SPEED:
                                    case CYBLE_ESS_HUMIDITY:
                                    case CYBLE_ESS_IRRADIANCE:
                                    case CYBLE_ESS_MAGNETIC_DECLINATION:
                                    case CYBLE_ESS_RAINFALL:
                                    case CYBLE_ESS_TRUE_WIND_DIR:
                                    case CYBLE_ESS_TRUE_WIND_SPEED:
                                        gattErr = CyBle_EssHandleRangeCheck((CYBLE_ESS_CHAR_INDEX_T) i, j,
                                            CYBLE_ESS_4BYTES_LENGTH, CYBLE_ESS_UNSIGNED_TYPE,
                                                &eventParam->handleValPair.value.val[1u]);
                                        break;

                                    /* uint24 */
                                    case CYBLE_ESS_POLLEN_CONCENTRATION:
                                        gattErr = CyBle_EssHandleRangeCheck((CYBLE_ESS_CHAR_INDEX_T) i, j,
                                            CYBLE_ESS_6BYTES_LENGTH, CYBLE_ESS_UNSIGNED_TYPE,
                                                &eventParam->handleValPair.value.val[1u]);
                                        break;

                                    /* uint32 */
                                    case CYBLE_ESS_PRESSURE:
                                        gattErr = CyBle_EssHandleRangeCheck((CYBLE_ESS_CHAR_INDEX_T) i, j,
                                            CYBLE_ESS_8BYTES_LENGTH, CYBLE_ESS_UNSIGNED_TYPE,
                                                &eventParam->handleValPair.value.val[1u]);
                                        break;

                                    /* int8 */
                                    case CYBLE_ESS_DEW_POINT:
                                    case CYBLE_ESS_HEAT_INDEX:
                                    case CYBLE_ESS_WIND_CHILL:
                                        gattErr = CyBle_EssHandleRangeCheck((CYBLE_ESS_CHAR_INDEX_T) i, j,
                                            CYBLE_ESS_2BYTES_LENGTH, CYBLE_ESS_SIGNED_TYPE,
                                                &eventParam->handleValPair.value.val[1u]);
                                        break;

                                    /* int16 */
                                    case CYBLE_ESS_TEMPERATURE:
                                        gattErr = CyBle_EssHandleRangeCheck((CYBLE_ESS_CHAR_INDEX_T) i, j,
                                            CYBLE_ESS_4BYTES_LENGTH, CYBLE_ESS_SIGNED_TYPE,
                                                &eventParam->handleValPair.value.val[1u]);
                                        break;

                                    /* int24 */
                                    case CYBLE_ESS_ELEVATION:
                                        gattErr = CyBle_EssHandleRangeCheck((CYBLE_ESS_CHAR_INDEX_T) i, j,
                                            CYBLE_ESS_6BYTES_LENGTH, CYBLE_ESS_SIGNED_TYPE,
                                                &eventParam->handleValPair.value.val[1u]);
                                        break;

                                    case CYBLE_ESS_MAGNETIC_FLUX_DENSITY_2D:
                                        /* No validation required */
                                        break;

                                    case CYBLE_ESS_MAGNETIC_FLUX_DENSITY_3D:
                                        /* No validation required */
                                        break;

                                    default:
                                        /* Invalid characteristic */
                                        gattErr = CYBLE_GATT_ERR_UNLIKELY_ERROR;
                                        break;
                                }
                            }
                            else /* No error */
                            {
                                if(CYBLE_GATT_DB_ATTR_GET_ATTR_GEN_MAX_LEN(
                                    essCharInfoPtr->descrHandle[wrDescrReqParam.descrIndex]) >=
                                        eventParam->handleValPair.value.len)
                                {
                                    /* Set new length of the descriptor value */
                                    CYBLE_GATT_DB_ATTR_SET_ATTR_GEN_LEN(
                                        essCharInfoPtr->descrHandle[wrDescrReqParam.descrIndex],
                                            eventParam->handleValPair.value.len);
                                }
                                else
                                {
                                    gattErr = CYBLE_GATT_ERR_INVALID_ATTRIBUTE_LEN;
                                }
                            }
                            break;
                        case CYBLE_ESS_ES_CONFIG_DESCR:
                            if(eventParam->handleValPair.value.val[0u] > CYBLE_ESS_CONF_BOOLEAN_OR)
                            {
                                /* Trigger Logic value is not supported */
                                gattErr = (CYBLE_GATT_ERR_CODE_T) CYBLE_GATT_ERR_WRITE_REQ_REJECTED;
                            }
                            break;
                        case CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR:
                            /* The ESS spec. states are the following: "The Server may also choose to reject
                            * a write request to the Characteristic User Description if it determines that
                            * the contents of the new value are unsuitable, such as a string containing
                            * characters in a language that the implementation does not support."
                            * This validation is omitted in the current version of the service.
                            */

                            if(CYBLE_GATT_DB_ATTR_GET_ATTR_GEN_MAX_LEN(
                                essCharInfoPtr->descrHandle[CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR]) >=
                                    eventParam->handleValPair.value.len)
                            {
                                /* Set new length of the descriptor value */
                                CYBLE_GATT_DB_ATTR_SET_ATTR_GEN_LEN(
                                    essCharInfoPtr->descrHandle[CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR],
                                        eventParam->handleValPair.value.len);
                            }
                            else
                            {
                                gattErr = CYBLE_GATT_ERR_INVALID_ATTRIBUTE_LEN;
                            }
                            break;
                        default:
                            break;
                        }

                        if(gattErr == CYBLE_GATT_ERR_NONE)
                        {
                            /* Fill data and pass it to user */
                            wrDescrReqParam.connHandle = eventParam->connHandle;
                            wrDescrReqParam.charIndex = (CYBLE_ESS_CHAR_INDEX_T) i;
                            wrDescrReqParam.charInstance = j;
                            wrDescrReqParam.gattErrorCode = CYBLE_GATT_ERR_NONE;

                            if(wrDescrReqParam.descrIndex != CYBLE_ESS_CCCD)
                            {
                                /* Check if descriptor index is not CCCD index as "event" and
                                * "wrDescrReqParam.value" parameters for CCCD were handled
                                * above.
                                */
                                wrDescrReqParam.value = &eventParam->handleValPair.value;
                            }

                            CyBle_EssApplCallback(event, &wrDescrReqParam);

                            if(wrDescrReqParam.gattErrorCode == CYBLE_GATT_ERR_NONE)
                            {
                                /* Write value to GATT database */
                                gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u,
                                    &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                            }
                        }

                        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                        break;
                    }
                }
            }
        }
    }

    if(CYBLE_GATT_ERR_NONE != gattErr)
    {
        /* Indicate that request was handled */
        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    }

    return (gattErr);
}


/****************************************************************************** 
##Function Name: CyBle_EsssPrepareWriteRequestEventHandler
*******************************************************************************

Summary:
 Handles the Write Request Event for the Environmental Sensing Service.

Parameters:
 eventParam: The pointer to the data that received with a prepare write
              request event for the  Environmental Sensing Service.

Return:
 None.

******************************************************************************/
void CyBle_EsssPrepareWriteRequestEventHandler(CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T *eventParam)
{
    uint8 i;
    uint8 j;
    uint8 exitLoop = 0u;
    CYBLE_ESSS_CHAR_T *essCharInfoPtr;

    if(NULL != CyBle_EssApplCallback)
    {
        for(i = 0u; ((i < ((uint8) CYBLE_ESS_CHAR_COUNT)) && (exitLoop == 0u)); i++)
        {
            /* Check if characteristic is enabled. If the pointer to the characteristic
            * is not "NULL", there is at least one instance of the characteristic in
            * the ES Service.
            */
            if(NULL != cyBle_esss.charInfoAddr[i].charInfoPtr)
            {
                for(j = 0u; ((j < cyBle_esssCharInstances[i]) && (exitLoop == 0u)); j++)
                {
                    essCharInfoPtr = &cyBle_esss.charInfoAddr[i].charInfoPtr[j];

                    /* Need to check following conditions: 1) if requested handle is the handle of
                    * Characteristic User Description Descriptor; 2) and if there is no active
                    * requests; 3) or there is an active request but the requested handle is
                    * different handle. the thord condition means that previous request resulted
                    * with an error so need to hanle that.
                    */
                    if(eventParam->baseAddr[eventParam->currentPrepWriteReqCount - 1u].handleValuePair.attrHandle ==
                        essCharInfoPtr->descrHandle[CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR])
                    {
                        if(cyBle_esssReqHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
                        {
                            /* Send Prepare Write Response which identifies acknowledgment for
                            * long characteristic value write.
                            */
                            CyBle_GattsPrepWriteReqSupport(CYBLE_GATTS_PREP_WRITE_SUPPORT);

                            cyBle_esssReqHandle =
                            eventParam->baseAddr[eventParam->currentPrepWriteReqCount - 1u].handleValuePair.attrHandle;
                        }
                        /* Indicate that request was handled */
                        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;

                        /* Set the flag to exit the loop */
                        exitLoop = 1u;
                    }
                }
            }
        }
    }
}


/****************************************************************************** 
##Function Name: CyBle_EsssExecuteWriteRequestEventHandler
*******************************************************************************

Summary:
 Handles the Execute Write Request Event for the Environmental Sensing Service.

Parameters:
 eventParam: The pointer to the data that came with a write request for the
             Environmental Sensing Service.

Return:
 None.

******************************************************************************/
void CyBle_EsssExecuteWriteRequestEventHandler(CYBLE_GATTS_EXEC_WRITE_REQ_T *eventParam)
{
    uint8 i;
    uint8 j;
    uint8 count;
    uint8 exitLoop = 0u;
    uint16 length = 0u;
    CYBLE_GATT_VALUE_T locDescrValue;
    CYBLE_ESS_DESCR_VALUE_T wrDescrReqParam;

    if(NULL != CyBle_EssApplCallback)
    {
        for(i = 0u; ((i < ((uint8) CYBLE_ESS_CHAR_COUNT)) && (exitLoop == 0u)); i++)
        {
            /* Check if characteristic is enabled. If the pointer to the characteristic
            * is not "NULL", there is at least one instance of the characteristic in
            * the ES Service.
            */
            if(NULL != cyBle_esss.charInfoAddr[i].charInfoPtr)
            {
                for(j = 0u; ((j < cyBle_esssCharInstances[i]) && (exitLoop == 0u)); j++)
                {
                    if((eventParam->baseAddr[0u].handleValuePair.attrHandle == 
                        cyBle_esss.charInfoAddr[i].charInfoPtr[j].descrHandle[CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR]) &&
                            (cyBle_esssReqHandle ==
                                eventParam->baseAddr[0u].handleValuePair.attrHandle))
                    {
                        for(count = 0u; count < eventParam->prepWriteReqCount; count++)
                        {
                            length += eventParam->baseAddr[count].handleValuePair.value.len;
                        }

                        if(CYBLE_GATT_DB_ATTR_GET_ATTR_GEN_MAX_LEN(cyBle_esssReqHandle) >= length)
                        {
                            /* Fill data and pass it to user */
                            wrDescrReqParam.connHandle = eventParam->connHandle;
                            wrDescrReqParam.charIndex = (CYBLE_ESS_CHAR_INDEX_T) i;
                            wrDescrReqParam.charInstance = j;
                            wrDescrReqParam.gattErrorCode = CYBLE_GATT_ERR_NONE;
                            locDescrValue = eventParam->baseAddr[0u].handleValuePair.value;
                            wrDescrReqParam.value = &locDescrValue;
                            wrDescrReqParam.value->len = length;
                            wrDescrReqParam.descrIndex = CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR;

                            CyBle_EssApplCallback(CYBLE_EVT_ESSS_DESCR_WRITE, &wrDescrReqParam);

                            if(wrDescrReqParam.gattErrorCode == CYBLE_GATT_ERR_NONE)
                            {
                                CYBLE_GATT_DB_ATTR_SET_ATTR_GEN_LEN(cyBle_esssReqHandle, length);
                            }
                        }
                        else
                        {
                            wrDescrReqParam.gattErrorCode = CYBLE_GATT_ERR_INVALID_ATTRIBUTE_LEN;
                        }

                        /* Pass user error code to Stack */
                        eventParam->gattErrorCode = wrDescrReqParam.gattErrorCode;

                        /* Set the flag to exit the loop */
                        exitLoop = 1u;

                        /* Indicate that request was handled */
                        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;

                        /* Clear requested handle */
                        cyBle_esssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                    }
                }
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_EsssConfirmationEventHandler
*******************************************************************************

Summary:
 Handles a Value Confirmation request event from the BLE stack.

Parameters:
 eventParam - The pointer to a structure of type CYBLE_CONN_HANDLE_T.

Return:
 None.

******************************************************************************/
void CyBle_EsssConfirmationEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    CYBLE_ESS_CHAR_VALUE_T locCharValue;

    if((NULL != CyBle_EssApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_esssReqHandle))
    {
        /* Only Descriptor Value Change Characteristic has Indication property.
        * Check if the requested handle is the handle of Descriptor Value Change
        * handle.
        */
        if(cyBle_esssReqHandle ==
            cyBle_esss.charInfoAddr[CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED].charInfoPtr[0u].charHandle)
        {
            /* Fill in event data and inform Application about
            * successfully confirmed indication.
            */
            locCharValue.connHandle = *eventParam;
            locCharValue.charIndex = CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED;
            locCharValue.value = NULL;

            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            cyBle_esssReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            CyBle_EssApplCallback((uint32)CYBLE_EVT_ESSS_INDICATION_CONFIRMATION, &locCharValue);
        }
    }
}


#if(CYBLE_GAP_ROLE_PERIPHERAL)

/******************************************************************************
##Function Name: CyBle_EsssSetChangeIndex
*******************************************************************************

Summary:
 Performs write operation of two-byte pseudo-random change index to the
 advertisement packet. The "Service Data" field should be selected in the
 component customizer GUI and contain a two-byte initial change index value
 and in opposite case the function will always return
 "CYBLE_ERROR_INVALID_OPERATION".

Parameters:
 essIndex: A two-byte pseudo-random change index to be written to the
           advertisement data.

Return:
 A return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_OPERATION - The change index is not present in the 
                                   advertisement data or its length is not
                                   equal to two bytes.


******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsssSetChangeIndex(uint16 essIndex)
{
    uint8 flag = 0u;
    uint8 adLength = 0u;
    uint8 byteCounter = 0u;
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_OPERATION;

    while((byteCounter < cyBle_discoveryData.advDataLen) && (0u == flag))
    {
        adLength = cyBle_discoveryData.advData[byteCounter];
        
        if(0u != adLength)
        {
            /* Increment byte counter so that it points to AD type field */
            byteCounter++;
            
            if(cyBle_discoveryData.advData[byteCounter] == CYBLE_ESS_SERVICE_DATA_AD_TYPE)
            {
                /* Start of a "Service Data" AD type was found. Set flag and exit the loop. */
                flag = 1u;
            }
            else
            {
                byteCounter += adLength;
            }
        }
        else
        {
            /* End of advertisement data structure was encountered so exit loop. */
            break;
        }
    }

    if(0u != flag)
    {
        /* Check if length is proper */
        if(adLength == CYBLE_ESS_SERVICE_DATA_LENGTH)
        {
            /* Increment byte counter to point to service UUID */
            byteCounter++;

            /* Check if UUID is ESS UUID */
            if(CyBle_Get16ByPtr(&cyBle_discoveryData.advData[byteCounter]) == CYBLE_UUID_ENVIRONMENTAL_SENSING_SERVICE)
            {
                /* Increment byte counter so that it points to change index value */
                byteCounter+=2u;
                CyBle_Set16ByPtr(&cyBle_discoveryData.advData[byteCounter], essIndex);

                if(CyBle_GetState() == CYBLE_STATE_ADVERTISING)
                {
                    /* The Stack stires the pointer to the advertisement data so
                    * need to have a different location with the advertisemt data when
                    * device in the advertisement mode.
                    */
                    memcpy((void *) &esssAdvertisementData, (void *) &cyBle_discoveryData,
                        sizeof(CYBLE_GAPP_DISC_DATA_T));
                    apiResult = CyBle_GapUpdateAdvData(&esssAdvertisementData, NULL);
                }
            }
        }
    }
    
    return (apiResult);
}

#endif /* (CYBLE_GAP_ROLE_PERIPHERAL) */


/******************************************************************************
##Function Name: CyBle_EsssSetCharacteristicValue
*******************************************************************************

Summary:
 Sets the characteristic value of the service in the local database.

Parameters:
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance number of the characteristic specified by
               "charIndex".
 attrSize: The size (in Bytes) of the characteristic value attribute.
 attrValue: The pointer to the characteristic value data that should be
            stored in the GATT database.

Return:
 A return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is
                                              absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsssSetCharacteristicValue(CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    /* Validate range of ESS characteristics */
    if(charIndex < CYBLE_ESS_CHAR_COUNT)
    {
        /* Check if requested characteristic is present in Service.
        * There are three conditions which should be checked: 1) The pointer to
        * "CYBLE_ESSS_CHAR_T" is not NULL, 2) The handle of the characteristic is
        * a valid handle, 3) The requested instance is a valid characteristic
        * instance.
        */
        if((NULL != cyBle_esss.charInfoAddr[charIndex].charInfoPtr) &&
            (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
                cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].charHandle) &&
                    (cyBle_esssCharInstances[charIndex] > charInstance))
        {
            CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

            locHandleValuePair.attrHandle =
                cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].charHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;

            /* Store data in database */
            if(CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED) ==
                CYBLE_GATT_ERR_NONE)
            {
                apiResult = CYBLE_ERROR_OK;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsssGetCharacteristicValue
*******************************************************************************

Summary:
 Gets the characteristic value of the service, which is a value identified by
 charIndex.

Parameters:
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance number of the characteristic specified by
               "charIndex".
 attrSize: The size of the characteristic value attribute.
 attrValue: The pointer to the location where characteristic value data
            should be stored.

Return:
 A return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is
                                              absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsssGetCharacteristicValue(CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(charIndex < CYBLE_ESS_CHAR_COUNT)
    {
        /* Check if requested characteristic is present in Service. There are three
        * conditions to be checked: 1) The pointer to "CYBLE_ESSS_CHAR_T" record in
        * the cyBle_esss struct is not NULL, 2) The handle of the characteristic is a 
        * valid handle, 3) The requested instance is a valid characteristic instance.
        */
        if((NULL != cyBle_esss.charInfoAddr[charIndex].charInfoPtr) &&
            (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
                cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].charHandle) &&
                    (cyBle_esssCharInstances[charIndex] > charInstance))
        {
            CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

            locHandleValuePair.attrHandle = cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].charHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;

            /* Read characteristic value from database */
            if( CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED) ==
                CYBLE_GATT_ERR_NONE)
            {
                apiResult = CYBLE_ERROR_OK;
                
            /* Set new length of the descriptor value */
            CYBLE_GATT_DB_ATTR_SET_ATTR_GEN_LEN(cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].charHandle,
                                                attrSize);
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsssSetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sets the characteristic descriptor of the specified characteristic.

Parameters:
 charIndex: The index of the service characteristic.
 charInstance: The instance number of the characteristic specified by
               "charIndex".
 descrIndex: The index of the service characteristic descriptor of type 
             CYBLE_ESS_DESCR_INDEX_T. 
 attrSize: The size of the characteristic descriptor attribute.
 attrValue: The pointer to the descriptor value data to 
            be stored in the GATT database.

Return:
 A return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is
                                              absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsssSetCharacteristicDescriptor(CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex, uint16 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if((charIndex < CYBLE_ESS_CHAR_COUNT) && (cyBle_esssCharInstances[charIndex] >= charInstance) &&
        (CYBLE_ESS_DESCR_COUNT > descrIndex))
    {
        /* Check if requested descriptor is present in Service.
        * There are three conditions to be checked: 1) The pointer to
        * "CYBLE_ESSS_CHAR_T" is not NULL, 2) The handle of the characteristic is
        * a valid handle, 3) The handle of the descriptor is a valid handle.
        */
        if((NULL != cyBle_esss.charInfoAddr[charIndex].charInfoPtr) &&
            (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
                cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].charHandle) &&
                    (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
                        cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex]))
        {
            CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

            locHandleValuePair.attrHandle =
                cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex];
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;
                        
            /* Read characteristic value from database */
            if(CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED) ==
                CYBLE_GATT_ERR_NONE)
            {
                apiResult = CYBLE_ERROR_OK;
                
                /* Set new length of the descriptor value */
                CYBLE_GATT_DB_ATTR_SET_ATTR_GEN_LEN(cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].
                                                    descrHandle[descrIndex],
                                                    attrSize);
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsssGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Gets the characteristic descriptor of the specified characteristic.

Parameters:
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance number of the characteristic specified by
               "charIndex".
 descrIndex: The index of the service characteristic descriptor of type 
             CYBLE_ESS_DESCR_INDEX_T. 
 attrSize: The size of the characteristic descriptor attribute.
 attrValue: The pointer to the location where characteristic descriptor
             value data should be stored.

Return:
 A return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request handled successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is
                                              absent.
******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsssGetCharacteristicDescriptor(CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex, uint16 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if((charIndex < CYBLE_ESS_CHAR_COUNT) && (cyBle_esssCharInstances[charIndex] >= charInstance) &&
        (CYBLE_ESS_DESCR_COUNT > descrIndex))
    {
        /* Check if requested descriptor is present in Service.
        * There are three conditions to be checked: 1) The pointer to
        * "CYBLE_ESSS_CHAR_T" is not NULL, 2) The handle of the characteristic is
        * a valid handle, 3) The handle of the descriptor is a valid handle.
        */
        if((NULL != cyBle_esss.charInfoAddr[charIndex].charInfoPtr) &&
            (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
                cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].charHandle) &&
                        (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
                            cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex]))
        {
            CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

            locHandleValuePair.attrHandle =
                cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex];
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;

            if(CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED) ==
                CYBLE_GATT_ERR_NONE)
            {
                apiResult = CYBLE_ERROR_OK;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }


    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsssSendNotification
*******************************************************************************

Summary:
 Sends a notification with a characteristic value of the Environmental Sensing
 Service, which is a value specified by charIndex, to the client's device.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance number of the characteristic specified by 
               "charIndex".
 attrSize: The size of the characteristic value attribute.
 attrValue: The pointer to the characteristic value data that should be
              sent to the client's device.

Return:
 A return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_NTF_DISABLED - A notification is not enabled by the client.
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is
                                              absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_INVALID_PARAMETER;

    if(charIndex < CYBLE_ESS_CHAR_COUNT)
    {
        if((NULL != cyBle_esss.charInfoAddr[charIndex].charInfoPtr) && 
            (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != 
                cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[CYBLE_ESS_CCCD]))
        {
            /* Send notification if it is enabled and connected */
            if(CYBLE_STATE_CONNECTED != CyBle_GetState())
            {
                apiResult = CYBLE_ERROR_INVALID_STATE;
            }
            
            else if(!CYBLE_IS_NOTIFICATION_ENABLED(
                cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[CYBLE_ESS_CCCD]))
            {
                apiResult = CYBLE_ERROR_NTF_DISABLED;
            }
            else
            {
                CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;

                /* Fill all fields of write request structure ... */
                ntfReqParam.attrHandle = cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].charHandle;
                ntfReqParam.value.val = attrValue;
                ntfReqParam.value.len = attrSize;

                /* Send notification to client using previously filled structure */
                apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsssSendIndication
*******************************************************************************

Summary:
 Sends an indication with a characteristic value of the Environmental Sensing
 Service, which is a value specified by charIndex, to the client's device.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic.
 charInstance: The instance number of the characteristic specified by 
               "charIndex".
 attrSize: The size of the characteristic value attribute.
 attrValue: The pointer to the characteristic value data that should be
              sent to the client's device.

Return:
 A return value is of type CYBLE_API_RESULT_T.
  * CYBLE_ERROR_OK - The request handled successfully
  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted
  * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
  * CYBLE_ERROR_IND_DISABLED - Indication is not enabled by the client.
  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is
                                              absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsssSendIndication(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, uint8 attrSize, uint8 *attrValue)
{
    /* Store new data in database */
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;

    if(charIndex == CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED)
    {
        /* Send indication if it is enabled and connected */
        if(CYBLE_STATE_CONNECTED != CyBle_GetState())
        {
            apiResult = CYBLE_ERROR_INVALID_STATE;
        }
        else if(!CYBLE_IS_INDICATION_ENABLED(
            cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[CYBLE_ESS_CCCD]))
        {
            apiResult = CYBLE_ERROR_IND_DISABLED;
        }
        else
        {
            if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
                cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].charHandle)
            {
                CYBLE_GATTS_HANDLE_VALUE_IND_T indReqParam;

                /* Fill all fields of write request structure ... */
                indReqParam.attrHandle = cyBle_esss.charInfoAddr[charIndex].charInfoPtr[charInstance].charHandle;
                indReqParam.value.val = attrValue;
                indReqParam.value.len = attrSize;

                /* Send indication to client using previously filled structure */
                apiResult = CyBle_GattsIndication(connHandle, &indReqParam);
                /* Save handle to support service specific value confirmation response from client */
                if(apiResult == CYBLE_ERROR_OK)
                {
                    cyBle_esssReqHandle = indReqParam.attrHandle;
                }
            }
            else
            {
                apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
            }
        }
    }
    return (apiResult);
}


#endif /* CYBLE_ESS_SERVER */


#ifdef CYBLE_ESS_CLIENT

/******************************************************************************
##Function Name: CyBle_EssClearClientDiscInfo
*******************************************************************************

Summary:
 Clears the client discovery info, a previously discovered characteristic, and
 descriptor handles of the Environmental Sensing Service.

Parameters:
 None.

Return:
 None.

******************************************************************************/
void CyBle_EssClearClientDiscInfo(void)
{
    uint8 i;
    uint8 j;

    for(i = (uint8) CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED; (i < ((uint8) CYBLE_ESS_CHAR_COUNT)); i++)
    {
        for(j = 0u; j < cyBle_esscCharInstances[i]; j++)
        {
            (void) memset(&cyBle_essc.charInfoAddr[i].charInfoPtr[j], 0, sizeof(CYBLE_ESSC_CHAR_T));
        }
    }
}


/******************************************************************************
##Function Name: CyBle_EsscDiscoverCharacteristicsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
 event. Based on the service UUID, an appropriate data structure is populated
 using the data received as part of the callback.

Parameters:
 discCharInfo: The pointer to a characteristic information structure.

Return:
 None.

******************************************************************************/
void CyBle_EsscDiscoverCharacteristicsEventHandler(const CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    uint8 i;
    uint8 j;
    uint8 exitLoop = 0u;

    /* ESS characteristics UUIDs */
    static const CYBLE_UUID16 cyBle_esscCharUuid[CYBLE_ESS_CHAR_COUNT] =
    {
        CYBLE_UUID_CHAR_DESCR_VALUE_CHANGED,
        CYBLE_UUID_CHAR_APPARENT_WIND_DIRECTION,
        CYBLE_UUID_CHAR_APPARENT_WIND_SPEED,
        CYBLE_UUID_CHAR_DEW_POINT,
        CYBLE_UUID_CHAR_ELEVATION,
        CYBLE_UUID_CHAR_GUST_FACTOR,
        CYBLE_UUID_CHAR_HEAT_INDEX,
        CYBLE_UUID_CHAR_HUMIDITY,
        CYBLE_UUID_CHAR_IRRADIANCE,
        CYBLE_UUID_CHAR_POLLEN_CONCENTRATION,
        CYBLE_UUID_CHAR_RAINFALL,
        CYBLE_UUID_CHAR_PRESSURE,
        CYBLE_UUID_CHAR_THEMPERATURE,
        CYBLE_UUID_CHAR_TRUE_WIND_DIRECTION,
        CYBLE_UUID_CHAR_TRUE_WIND_SPEED,
        CYBLE_UUID_CHAR_UV_INDEX,
        CYBLE_UUID_CHAR_WIND_CHILL,
        CYBLE_UUID_CHAR_BAR_PRESSURE_TREND,
        CYBLE_UUID_CHAR_MAGNETIC_DECLINATION,
        CYBLE_UUID_CHAR_MAGNETIC_FLUX_DENSITY_2D,
        CYBLE_UUID_CHAR_MAGNETIC_FLUX_DENSITY_3D
    };

    static CYBLE_GATT_DB_ATTR_HANDLE_T *essLastEndHandle = NULL;

    /* Update last characteristic endHandle to declaration handle of this characteristic */
    if(essLastEndHandle != NULL)
    {
        *essLastEndHandle = discCharInfo->charDeclHandle - 1u;
        essLastEndHandle = NULL;
    }

    /* Search through all available characteristics */
    for(i = (uint8) CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED; ((i < (uint8) CYBLE_ESS_CHAR_COUNT) && (0u == exitLoop)); i++)
    {
        for(j = 0u; j < cyBle_esscCharInstances[i]; j++)
        {
            if(cyBle_esscCharUuid[i] == discCharInfo->uuid.uuid16)
            {
                if(NULL != cyBle_essc.charInfoAddr[i].charInfoPtr)
                {
                    if(cyBle_essc.charInfoAddr[i].charInfoPtr[j].valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
                    {
                        cyBle_essc.charInfoAddr[i].charInfoPtr[j].valueHandle = discCharInfo->valueHandle;
                        cyBle_essc.charInfoAddr[i].charInfoPtr[j].properties = discCharInfo->properties;
                        essLastEndHandle = &cyBle_essc.charInfoAddr[i].charInfoPtr[j].endHandle;
                        exitLoop = 1u;
                        break;
                    }
                }
            }
        }
    }

    /* Initialize characteristic endHandle to Service endHandle.
       Characteristic endHandle will be updated to the declaration
       handler of the following characteristic,
       in the following characteristic discovery procedure. */
    if(essLastEndHandle != NULL)
    {
        *essLastEndHandle = cyBle_serverInfo[cyBle_disCount].range.endHandle;
    }
}


/******************************************************************************
##Function Name: CyBle_EsscDiscoverCharDescriptorsEventHandler
*******************************************************************************

Summary:
 This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event.
 Based on the descriptor UUID, an appropriate data structure is populated using
 the data received as part of the callback.

Parameters:
 discDescrInfo: The pointer to a descriptor information structure.

Return:
 None.

******************************************************************************/
void CyBle_EsscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    CYBLE_ESS_DESCR_INDEX_T locDescIndex;

    if(NULL != cyBle_essc.charInfoAddr[activeCharIndex].charInfoPtr)
    {
        switch(discDescrInfo->uuid.uuid16)
        {
        case CYBLE_UUID_CHAR_CLIENT_CONFIG:
            locDescIndex = CYBLE_ESS_CCCD;
            break;
        case CYBLE_UUID_CHAR_VALID_RANGE:
            locDescIndex = CYBLE_ESS_VRD;
            break;
        case CYBLE_UUID_CHAR_USER_DESCRIPTION:
            locDescIndex = CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR;
            break;
        case CYBLE_UUID_CHAR_ES_CONFIGURATION:
            locDescIndex = CYBLE_ESS_ES_CONFIG_DESCR;
            break;
        case CYBLE_UUID_CHAR_EXTENDED_PROPERTIES:
            locDescIndex = CYBLE_ESS_CHAR_EXTENDED_PROPERTIES;
            break;
        case CYBLE_UUID_CHAR_ES_MEASUREMENT:
            locDescIndex = CYBLE_ESS_ES_MEASUREMENT_DESCR;
            break;
        case CYBLE_UUID_CHAR_ES_TRIGGER_SETTING:
            if(cyBle_essc.charInfoAddr[activeCharIndex].charInfoPtr[activeCharInstance].
                    descrHandle[CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1] == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                locDescIndex = CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1;
            }
            else if(cyBle_essc.charInfoAddr[activeCharIndex].
                    charInfoPtr[activeCharInstance].descrHandle[CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR2] ==
                        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                locDescIndex = CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR2;
            }
            else
            {
                locDescIndex = CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR3;
            }
            break;
        default:
            /* Not supported descriptor */
            locDescIndex = CYBLE_ESS_DESCR_COUNT;
            break;
        }

        if(locDescIndex < CYBLE_ESS_DESCR_COUNT)
        {
            if(cyBle_essc.charInfoAddr[activeCharIndex].charInfoPtr[activeCharInstance].descrHandle[locDescIndex] ==
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                cyBle_essc.charInfoAddr[activeCharIndex].charInfoPtr[activeCharInstance].descrHandle[locDescIndex] =
                    discDescrInfo->descrHandle;

            }
            else    /* Duplication of descriptor */
            {
                CyBle_ApplCallback(CYBLE_EVT_GATTC_DESCR_DUPLICATION, &discDescrInfo->uuid);
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_EsscNotificationEventHandler
*******************************************************************************

Summary:
 Handles a notification event for the Environmental Sensing Service.

Parameters:
 eventParam: The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_EsscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    uint8 j;
    CYBLE_ESS_CHAR_INDEX_T locCharIndex;
    CYBLE_ESS_CHAR_VALUE_T notifValue;

    if(NULL != CyBle_EssApplCallback)
    {
        for(locCharIndex = CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED; locCharIndex < CYBLE_ESS_CHAR_COUNT; locCharIndex++)
        {
            if(NULL != cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr)
            {
                for(j = 0u; j < cyBle_esscCharInstances[locCharIndex]; j++)
                {
                    if(cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].valueHandle ==
                        eventParam->handleValPair.attrHandle)
                    {
                        notifValue.connHandle = eventParam->connHandle;
                        notifValue.charIndex = locCharIndex;
                        notifValue.value = &eventParam->handleValPair.value;
                        CyBle_EssApplCallback((uint32) CYBLE_EVT_ESSC_NOTIFICATION, &notifValue);
                        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                        break;
                    }
                }
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_EsscIndicationEventHandler
*******************************************************************************

Summary:
 Handles an indication event for Environmental Sensing Service.

Parameters:
 eventParam: The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_EsscIndicationEventHandler(CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T *eventParam)
{
    CYBLE_ESS_CHAR_VALUE_T indicationValue;

    if(NULL != CyBle_EssApplCallback)
    {
        if(cyBle_essc.charInfoAddr[CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED].charInfoPtr[0u].valueHandle ==
            eventParam->handleValPair.attrHandle)
        {
            indicationValue.connHandle = eventParam->connHandle;
            indicationValue.charIndex = CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED;
            indicationValue.value = &eventParam->handleValPair.value;
            CyBle_EssApplCallback((uint32) CYBLE_EVT_ESSC_INDICATION, &indicationValue);
            cyBle_eventHandlerFlag &= (uint8) ~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_EsscReadLongRespEventHandler
*******************************************************************************

Summary:
 Handles a Read Long Response event for the Environmental Sensing Service.

Parameters:
 eventParam: The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_EsscReadLongRespEventHandler(const CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    uint8 j;
    uint8 i;
    uint16 mtuSize;
    uint8 isReqEnded = 0u;
    CYBLE_GATT_VALUE_T rdValue;
    CYBLE_ESS_CHAR_INDEX_T locCharIndex;
    CYBLE_ESS_DESCR_VALUE_T rdDescrValue;

    if(NULL != CyBle_EssApplCallback)
    {
        /* Go trough all characteristics */
        for(locCharIndex = CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED; locCharIndex < CYBLE_ESS_CHAR_COUNT; locCharIndex++)
        {
            /* Check if specific characteristic exist */
            if(NULL != cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr)
            {
                /* Go trough all characteristic instances */
                for(j = 0u; j < cyBle_esscCharInstances[locCharIndex]; j++)
                {
                    /* Read Long Request is only supported for Characteristic User
                    * Description descriptor. Check if requested handle equals 
                    *the descriptor.
                    */
                    if((cyBle_esscReqHandle ==
                cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].descrHandle[CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR])
                    && ( cyBle_esscReqHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE))
                    {
                        /* Update user buffer with received data */
                        for(i = 0u; (i < eventParam->value.len) && (cyBle_esscCurrLen < cyBle_esscRdLongBuffLen); i++)
                        {
                            cyBle_esscRdLongBuffPtr[cyBle_esscCurrLen] = eventParam->value.val[i];
                            cyBle_esscCurrLen++;
                        }

                        (void) CyBle_GattGetMtuSize(&mtuSize);

                        /* If the received data length is less than the MTU size, the Read Long
                        * request is completed or the provided user's buffer is full.
                        */
                        if(((mtuSize - 1u) > eventParam->value.len))
                        {
                            cyBle_esscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                        }
                        else if(cyBle_esscCurrLen == cyBle_esscRdLongBuffLen)
                        {
                            CyBle_GattcStopCmd();
                        }
                        else
                        {
                            isReqEnded = 1u;
                        }

                        /* If the buffer is full, then stop any remaining read long requests */
                        if(0u == isReqEnded)
                        {
                            rdValue.val = cyBle_esscRdLongBuffPtr;
                            rdValue.len = cyBle_esscCurrLen;

                            rdDescrValue.connHandle = eventParam->connHandle;
                            rdDescrValue.charIndex = locCharIndex;
                            rdDescrValue.value = &rdValue;
                            rdDescrValue.charInstance = j;
                            rdDescrValue.descrIndex = CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR;
                            CyBle_EssApplCallback((uint32) CYBLE_EVT_ESSC_READ_DESCR_RESPONSE, &rdDescrValue);
                        }

                        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                        break;
                    }
                }
            }
        }
    }
}


/******************************************************************************
##Function Name: CyBle_EsscReadResponseEventHandler
*******************************************************************************

Summary:
 Handles a Read Response event for the Environmental Sensing Service.

Parameters:
 eventParam: The pointer to the data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_EsscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    uint8 j;
    uint8 locReqHandle = 0u;
    CYBLE_ESS_CHAR_INDEX_T locCharIndex;

    if((NULL != CyBle_EssApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_esscReqHandle))
    {
        for(locCharIndex = CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED; (locCharIndex < CYBLE_ESS_CHAR_COUNT) && 
            (locReqHandle == 0u); locCharIndex++)
        {
            if(NULL != cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr)
            {
                for(j = 0u; ((j < cyBle_esscCharInstances[locCharIndex]) && (locReqHandle == 0u)); j++)
                {
                    if(cyBle_esscReqHandle == cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].valueHandle)
                    {
                        CYBLE_ESS_CHAR_VALUE_T locCharValue;

                        locCharValue.connHandle = eventParam->connHandle;
                        locCharValue.charIndex = locCharIndex;
                        locCharValue.charInstance = j;
                        locCharValue.value = &eventParam->value;
                        cyBle_esscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                        CyBle_EssApplCallback((uint32) CYBLE_EVT_ESSC_READ_CHAR_RESPONSE, &locCharValue);
                        locReqHandle = 1u;
                    }
                    else
                    {
                        CYBLE_ESS_DESCR_INDEX_T locDescIndex;

                        for(locDescIndex = CYBLE_ESS_CCCD; (locDescIndex < CYBLE_ESS_DESCR_COUNT) &&
                           (locReqHandle == 0u); locDescIndex++)
                        {
                            if(cyBle_esscReqHandle ==
                                cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].descrHandle[locDescIndex])
                            {
                                CYBLE_ESS_DESCR_VALUE_T locDescrValue;

                                locDescrValue.connHandle = eventParam->connHandle;
                                locDescrValue.charIndex = locCharIndex;
                                locDescrValue.descrIndex = locDescIndex;
                                locDescrValue.charInstance = j;
                                locDescrValue.value = &eventParam->value;
                                cyBle_esscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                                CyBle_EssApplCallback((uint32) CYBLE_EVT_ESSC_READ_DESCR_RESPONSE, &locDescrValue);
                                locReqHandle = 1u;
                            }
                        }
                    }
                }
            }
        }
        if(locReqHandle != 0u)
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_EsscWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles a Write Response event for the Environmental Sensing Service.

Parameters:
 eventParam: The pointer to a data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_EsscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    uint8 j;
    uint8 locReqHandle = 0u;
    CYBLE_ESS_CHAR_INDEX_T locCharIndex;
    CYBLE_ESS_DESCR_VALUE_T locDescrValue;

    /* Check if service handler was registered and request handle is
    * valid.
    */
    if((NULL != CyBle_EssApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_esscReqHandle))
    {
        for(locCharIndex = CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED; (locCharIndex < CYBLE_ESS_CHAR_COUNT) &&
            (locReqHandle == 0u); locCharIndex++)
        {
            /* If this condition is false, this means that the current characteristic
            * is not included in the GUI, in other words support for the characteristic is
            * not enabled.
            */
            if(NULL != cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr)
            {
                for(j = 0u; ((j < cyBle_esscCharInstances[locCharIndex]) && (locReqHandle == 0u)); j++)
                {
                    /* Check if requested descriptor handle is in
                    * characteristic range.
                    */
                    if((cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].valueHandle < cyBle_esscReqHandle) &&
                        (cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].endHandle >= cyBle_esscReqHandle))
                    {
                        if(cyBle_esscReqHandle ==
                            cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].descrHandle[CYBLE_ESS_CCCD])
                        {
                            locDescrValue.descrIndex = CYBLE_ESS_CCCD;
                        }
                        else
                        if(cyBle_esscReqHandle ==
                cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].descrHandle[CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1])
                        {
                            locDescrValue.descrIndex = CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1;
                        }
                        else
                        if(cyBle_esscReqHandle ==
                cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].descrHandle[CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR2])
                        {
                            locDescrValue.descrIndex = CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR2;
                        }
                        else if(cyBle_esscReqHandle ==
                cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].descrHandle[CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR3])
                        {
                            locDescrValue.descrIndex = CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR3;
                        }
                        else if(cyBle_esscReqHandle ==
                            cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].descrHandle[CYBLE_ESS_ES_CONFIG_DESCR])
                        {
                            locDescrValue.descrIndex = CYBLE_ESS_ES_CONFIG_DESCR;
                        }
                        else if(cyBle_esscReqHandle ==
                cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].descrHandle[CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR])
                        {
                            locDescrValue.descrIndex = CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR;
                        }
                        else
                        {
                            /* Should never enter here */
                        }

                        locDescrValue.connHandle = *eventParam;
                        locDescrValue.charIndex = locCharIndex;
                        locDescrValue.charInstance = j;
                        locDescrValue.value = NULL;
                        cyBle_esscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                        CyBle_EssApplCallback((uint32) CYBLE_EVT_ESSC_WRITE_DESCR_RESPONSE, &locDescrValue);
                        locReqHandle = 1u;
                    }
                }
            }
        }

        if(locReqHandle != 0u)
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_EsscExecuteWriteResponseEventHandler
*******************************************************************************

Summary:
 Handles a Execute Write Response event for the Environmental Sensing Service.

Parameters:
 eventParam: The pointer to a data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_EsscExecuteWriteResponseEventHandler(const CYBLE_GATTC_EXEC_WRITE_RSP_T *eventParam)
{
    uint8 j;
    uint8 locReqHandle = 0u;
    CYBLE_ESS_CHAR_INDEX_T locCharIndex;
    CYBLE_ESS_DESCR_VALUE_T locDescrValue;

    /* Check if service handler was registered and request handle is
    * valid.
    */
    if((NULL != CyBle_EssApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_esscReqHandle))
    {
        for(locCharIndex = CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED; (locCharIndex < CYBLE_ESS_CHAR_COUNT) &&
            (locReqHandle == 0u); locCharIndex++)
        {
            /* If this condition is false, this means that the current characteristic
            * is not included in the GUI, in other words the
            * characteristic support is not enabled.
            */
            if(NULL != cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr)
            {
                for(j = 0u; ((j < cyBle_esscCharInstances[locCharIndex]) && (locReqHandle == 0u)); j++)
                {
                    if(cyBle_esscReqHandle ==
                cyBle_essc.charInfoAddr[locCharIndex].charInfoPtr[j].descrHandle[CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR])
                    {
                        locDescrValue.connHandle = eventParam->connHandle;
                        locDescrValue.charIndex = locCharIndex;
                        locDescrValue.charInstance = j;
                        locDescrValue.value = NULL;
                        locDescrValue.descrIndex = CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR;
                        cyBle_esscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                        CyBle_EssApplCallback((uint32) CYBLE_EVT_ESSC_WRITE_DESCR_RESPONSE, &locDescrValue);
                        locReqHandle = 1u;
                    }
                }
            }
        }

        if(locReqHandle != 0u)
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
##Function Name: CyBle_EsscErrorResponseEventHandler
*******************************************************************************

Summary:
 Handles an Error Response event for the Environmental Sensing Service.

Parameters:
 eventParam: The pointer to a data structure specified by an event.

Return:
 None.

******************************************************************************/
void CyBle_EsscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((NULL != eventParam) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_esscReqHandle))
    {
        cyBle_esscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


/******************************************************************************
##Function Name: CyBle_EsscSetCharacteristicValue
*******************************************************************************

Summary:
 Sends a request to set a characteristic value of the service, which is a
 value identified by charIndex, to the server's device.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance number of the characteristic specified by
               "charIndex".
 attrSize: The size of the characteristic value attribute.
 attrValue: The pointer to the characteristic value data that should be
            sent to the server device.

Return:
 A return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                              the particular characteristic.
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                    characteristic.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance, uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T writeReqParam;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_ESS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
        cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].valueHandle)
    {
        writeReqParam.attrHandle = cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].valueHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &writeReqParam);
        /* Save handle to support service specific write response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_esscReqHandle = writeReqParam.attrHandle;
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsscGetCharacteristicValue
*******************************************************************************

Summary:
 This function is used to read a characteristic value, which is a value
 identified by charIndex, from the server.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance number of the characteristic specified by
               "charIndex".

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The read request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
                                             the particular characteristic.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
 * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
                                    characteristic.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is 
                                             absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_ESS_CHAR_INDEX_T charIndex,
    uint8 charInstance)
{
    CYBLE_API_RESULT_T apiResult;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if(charIndex >= CYBLE_ESS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
        cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].valueHandle)
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle,
                                            cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].valueHandle);

        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_esscReqHandle = cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].valueHandle;
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsscSetCharacteristicDescriptor
*******************************************************************************

Summary:
 This function is used to write the characteristic descriptor to the server,
 which is identified by charIndex and descrIndex.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic. Starts with zero.
 descrIndex: The index of the service characteristic descriptor.
 charInstance: The instance number of the characteristic specified by
               "charIndex".
 attrSize: The size of the characteristic value attribute.
 attrValue: The pointer to the characteristic descriptor value data that
            should be sent to the server device.

Return:
 A return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_INVALID_STATE - The state is not valid.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on
                                   the specified attribute.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional Characteristic Descriptor
                                             is absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex, uint8 attrSize,
        uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;

    if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
        cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex])
    {
        if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
        {
            apiResult = CYBLE_ERROR_INVALID_STATE;
        }
        else if((charIndex >= CYBLE_ESS_CHAR_COUNT) || (descrIndex >= CYBLE_ESS_DESCR_COUNT))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
        else if((descrIndex == CYBLE_ESS_VRD) || (descrIndex == CYBLE_ESS_ES_MEASUREMENT_DESCR))
        {
            apiResult = CYBLE_ERROR_INVALID_OPERATION;
        }
        else
        {
         /* Fill all fields of write request structure ... */
            writeReqParam.attrHandle = cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex];
            writeReqParam.value.val = attrValue;
            writeReqParam.value.len = attrSize;

            /* ... and send request to server's device. */
            apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);

            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_esscReqHandle = writeReqParam.attrHandle;
            }
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsscGetCharacteristicDescriptor
*******************************************************************************

Summary:
 Sends a request to get the characteristic descriptor of the specified 
 characteristic of the service.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance number of the characteristic specified by
               "charIndex".
 descrIndex: The index of the service characteristic descriptor.

Return:
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_INVALID_STATE - The state is not valid.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on
                                    the specified attribute.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional Characteristic Descriptor
                                             is absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
        cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex])
    {

        if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
        {
            apiResult = CYBLE_ERROR_INVALID_STATE;
        }
        else if((charIndex >= CYBLE_ESS_CHAR_COUNT) || (descrIndex >= CYBLE_ESS_DESCR_COUNT))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
        else
        {
            apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle,
                                    cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex]);

            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_esscReqHandle = cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex];
            }
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }

    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsscSetLongCharacteristicDescriptor
*******************************************************************************

Summary:
 This function is used to write a long characteristic descriptor to the server,
 which is identified by charIndex and descrIndex.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic. Starts with zero.
 descrIndex: The index of the service characteristic descriptor.
 charInstance: The instance number of the characteristic specified by
               "charIndex".
 attrSize: The size of the characteristic value attribute.
 attrValue: The pointer to the characteristic descriptor value data that
            should be sent to the server device.

Return:
 Return value is of type CYBLE_API_RESULT_T.
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_INVALID_STATE - The state is not valid.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on
                                   the specified attribute.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic Descriptor
                                             is absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsscSetLongCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex, uint16 attrSize,
        uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_PREP_WRITE_REQ_T writeReqParam;

    if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
        cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex])
    {
        if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
        {
            apiResult = CYBLE_ERROR_INVALID_STATE;
        }
        else if((charIndex >= CYBLE_ESS_CHAR_COUNT) || (descrIndex >= CYBLE_ESS_DESCR_COUNT))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
        else if(descrIndex == CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR)
        {
         /* Fill all fields of write request structure ... */
            writeReqParam.handleValuePair.attrHandle =
                cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex];
            writeReqParam.handleValuePair.value.val = attrValue;
            writeReqParam.handleValuePair.value.len = attrSize;
            writeReqParam.offset = 0u;

            /* ... and send request to server's device. */
            apiResult = CyBle_GattcWriteLongCharacteristicValues(connHandle, &writeReqParam);

            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_esscReqHandle = writeReqParam.handleValuePair.attrHandle;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_INVALID_OPERATION;
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    return (apiResult);
}


/******************************************************************************
##Function Name: CyBle_EsscGetLongCharacteristicDescriptor
*******************************************************************************

Summary:
 Sends a request to read long characteristic descriptor of the specified
 characteristic of the service.

Parameters:
 connHandle: The connection handle.
 charIndex: The index of the service characteristic. Starts with zero.
 charInstance: The instance number of the characteristic specified by
               "charIndex".
 descrIndex: The index of the service characteristic descriptor.
 attrSize: The size of the characteristic value attribute.
 attrValue: The pointer to the buffer where the read long characteristic 
            descriptor value should be stored.

Return:
 * CYBLE_ERROR_OK - The request was sent successfully.
 * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
 * CYBLE_ERROR_INVALID_STATE - The state is not valid.
 * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
 * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on
                                    the specified attribute.
 * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The optional Characteristic Descriptor
                                             is absent.

******************************************************************************/
CYBLE_API_RESULT_T CyBle_EsscGetLongCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_ESS_CHAR_INDEX_T charIndex, uint8 charInstance, CYBLE_ESS_DESCR_INDEX_T descrIndex, uint16 attrSize,
        uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_READ_BLOB_REQ_T readLongdata;

    if(CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE !=
        cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex])
    {
        if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
        {
            apiResult = CYBLE_ERROR_INVALID_STATE;
        }
        else if((charIndex >= CYBLE_ESS_CHAR_COUNT) || (descrIndex >= CYBLE_ESS_DESCR_COUNT))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
        else
        {
            readLongdata.offset = 0u;
            readLongdata.attrHandle = cyBle_essc.charInfoAddr[charIndex].charInfoPtr[charInstance].descrHandle[descrIndex];

            cyBle_esscRdLongBuffLen = attrSize;
            cyBle_esscRdLongBuffPtr = attrValue;

            apiResult = CyBle_GattcReadLongCharacteristicDescriptors(connHandle, &readLongdata);

            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_esscReqHandle = readLongdata.attrHandle;
            }
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }

    return (apiResult);
}

#endif /* CYBLE_ESS_CLIENT */


/* [] END OF FILE */
