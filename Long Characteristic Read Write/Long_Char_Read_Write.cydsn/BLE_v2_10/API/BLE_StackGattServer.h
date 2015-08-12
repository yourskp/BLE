/*******************************************************************************
File Name: CYBLE_StackGattServer.h
Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`

Description:
 This file contains the GATT Server routines

Related Document:
 BLE Standard Spec - CoreV4.1, CSS, CSAs, ESR05, ESR06

********************************************************************************
Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
You may use this file only in accordance with the license, terms, conditions,
disclaimers, and limitations in the end user license agreement accompanying
the software package with which this file was provided.
*******************************************************************************/


#ifndef CY_BLE_CYBLE_STACK_GATT_SERVER_INTERNAL_H
#define CY_BLE_CYBLE_STACK_GATT_SERVER_INTERNAL_H


/***************************************
##Common stack includes
***************************************/

#include "`$INSTANCE_NAME`_StackGatt.h"


/***************************************
##Constants
***************************************/

#ifdef ATT_QUEUED_WRITE_SUPPORT
#define CYBLE_GATTS_PREP_WRITE_SUPPORT					0x01u
#define CYBLE_GATTS_PREP_WRITE_NOT_SUPPORT				0x00u
#define CYBLE_GATTS_FIRST_PREP_WRITE_REQ                1u
#endif


/***************************************
##Exported structures
***************************************/

/* Handle value notification data to be sent to Client */
typedef CYBLE_GATT_HANDLE_VALUE_PAIR_T CYBLE_GATTS_HANDLE_VALUE_NTF_T;

/* GATT handle value indication parameter type */
typedef CYBLE_GATT_HANDLE_VALUE_PAIR_T CYBLE_GATTS_HANDLE_VALUE_IND_T;

/* GATT Server Error Response parameter type */
typedef struct
{
    /* Handle in which error is generated */
    CYBLE_GATT_DB_ATTR_HANDLE_T    	attrHandle;

    /* Opcode which has resulted in Error 
       Information on ATT/GATT opcodes is available in the 
       Bluetooth specification.
    */
    uint8              				opcode;

    /* Error Code describing cause of error */
    CYBLE_GATT_ERR_CODE_T			errorCode;
	
}CYBLE_GATTS_ERR_PARAM_T;

/* Read response parameter to be sent to Client */
typedef CYBLE_GATT_VALUE_T CYBLE_GATTS_READ_RSP_PARAM_T;

/* Prepare write request parameter received from Client */
typedef struct
{
	/* Connection handle */
	CYBLE_CONN_HANDLE_T 						connHandle; 			

	/* Base address of the queue where data is queued, Queue is of type 
	   CYBLE_GATT_HANDLE_VALUE_OFFSET_PARAM_T. 
	   Each baseAddr[currentPrepWriteReqCount-1].handleValuePair.value.val
	   provides the current data and baseAddr[0].handleValuePair.value.val
	   provides the base address of the data buffer where full value will be stored.
	   Application can calculate the total length based on each each array element.
	   i.e total length up current request = baseAddr[0].handleValuePair.value.len+
	   ....+baseAddr[currentPrepWriteReqCount-1].handleValuePair.value.len
	*/
    CYBLE_GATT_HANDLE_VALUE_OFFSET_PARAM_T 		*baseAddr;	

	/* Current count of prepare request from remote. This parameter can be used 
	   to access the data from 'baseAddr[]'. Array index will range from 0 to 
	   currentPrepWriteReqCount - 1 */	
	uint8	  									currentPrepWriteReqCount;

	/* Application provide GATT error code for the procedure. This is an o/p parameter */
    uint8 										gattErrorCode;	

}CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T;

/* Execute Write result */
typedef struct
{
	/* Connection handle */
	CYBLE_CONN_HANDLE_T 						connHandle; 

	/* Base address of the queue where data is queued. Queue is of type 
	   CYBLE_GATT_HANDLE_VALUE_OFFSET_PARAM_T. 
	   baseAddr[0].handleValuePair.value.val
	   provides the base address of the total data stored in prepare write
	   queue internally by stack. 
	   Application can calculate the total length based on each each array element.
	   i.e total length = baseAddr[0].handleValuePair.value.len+
	   ....+baseAddr[prepWriteReqCount-1].handleValuePair.value.len
	*/	
	CYBLE_GATT_HANDLE_VALUE_OFFSET_PARAM_T	  	*baseAddr;

	/* Total count of prepare request from remote. This parameter can be used 
	   to access the data from 'baseAddr[]'. array index will range from 0 to 
	   prepWriteReqCount - 1 */	
	uint8	  									prepWriteReqCount;

	/* Execute write flag received from remote */
    uint8 										execWriteFlag;

	/* Attribute Handle at which error occured. This is an o/p param  */
    CYBLE_GATT_DB_ATTR_HANDLE_T	  				attrHandle;     
	
	/* Application provide GATT error code for the procedure. This is an o/p param */
    uint8 										gattErrorCode;

}CYBLE_GATTS_EXEC_WRITE_REQ_T;

/* Write request parameter received from Client */
typedef struct
{
	/* Connection handle */
	CYBLE_CONN_HANDLE_T 				connHandle; 

	/* handle value pair */
    CYBLE_GATT_HANDLE_VALUE_PAIR_T    	handleValPair; 
	
}CYBLE_GATTS_WRITE_REQ_PARAM_T;

/* Write command request parameter received from Client */
typedef CYBLE_GATTS_WRITE_REQ_PARAM_T  CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T;

/* Signed Write command request parameter received from Client */
typedef CYBLE_GATTS_WRITE_REQ_PARAM_T  CYBLE_GATTS_SIGNED_WRITE_CMD_REQ_PARAM_T;

/* Prepare write response parameter to be sent to Client */
typedef CYBLE_GATT_HANDLE_VALUE_OFFSET_PARAM_T CYBLE_GATTS_PREP_WRITE_RSP_PARAM_T;

/* Event parameters for characteristic read value access
 * even generated by BLE Stack upon an access of Characteristic value
 * read for the characteristic definition which has 
 * CYBLE_GATT_DB_ATTR_CHAR_VAL_RD_EVENT property set.
 **/
typedef struct
{
    /* Connection handle */
    CYBLE_CONN_HANDLE_T             connHandle;

    /* Attribute Handle*/
    CYBLE_GATT_DB_ATTR_HANDLE_T     attrHandle;

    /* Output Param: Profile/Service specific error code,
     * profile or application need to change this 
     * to service specific error based on service/profile
     * requirements. */
    CYBLE_GATT_ERR_CODE_T           gattErrorCode;

}CYBLE_GATTS_CHAR_VAL_READ_REQ_T;


/***************************************
##Exported APIs
***************************************/

/******************************************************************************
##Function Name: CyBle_GattsNotification
*******************************************************************************

Summary:
 This function sends a notification to the peer device when the GATT Server
 is configured to notify a Characteristic Value to the GATT Client without
 expecting any Attribute Protocol layer acknowledgement that the notification
 was successfully received. This is a non-blocking function.

 On enabling notification successfully for a specific attribute, if the GATT server has an
 updated value to be notified to the GATT Client, it sends out a 'Handle Value
 Notification' which results in CYBLE_EVT_GATTC_HANDLE_VALUE_NTF event at the
 GATT Client's end.

 Refer to Bluetooth 4.1 core specification, Volume 3, Part G, section 4.10 for
 more details on notifications.
 
Parameters:
 connHandle: Connection handle to identify the peer GATT entity, of type
             CYBLE_CONN_HANDLE_T.
 ntfParam: Pointer to structure of type CYBLE_GATTS_HANDLE_VALUE_NTF_T which is same as 
		 CYBLE_GATT_HANDLE_VALUE_PAIR_T.

Return:
 CYBLE_API_RESULT_T : Return value indicates if the function succeeded or
 failed. Following are the possible error codes.
 <table>
 Errors codes                           Description
 ------------                           -----------
  CYBLE_ERROR_OK                        On successful operation
  CYBLE_ERROR_INVALID_PARAMETER         'connHandle' value does not
                                         represent any existing entry
                                         in the Stack
  CYBLE_ERROR_INVALID_OPERATION         This operation is not permitted
  CYBLE_ERROR_MEMORY_ALLOCATION_FAILED  Memory allocation failed
 </table>

******************************************************************************/
CYBLE_API_RESULT_T CyBle_GattsNotification
(
	CYBLE_CONN_HANDLE_T					connHandle, 
	CYBLE_GATTS_HANDLE_VALUE_NTF_T		* ntfParam
);


/******************************************************************************
##Function Name: CyBle_GattsIndication
*******************************************************************************

Summary:
 This function sends an indication to the peer device when the GATT Server is
 configured to indicate a Characteristic Value to the GATT Client and expects
 an Attribute Protocol layer acknowledgement that the indication was 
 successfully received. This is a non-blocking function.

 On enabling indication successfully, if the GATT server has an updated value to be 
 indicated to the GATT Client, it sends out a 'Handle Value Indication' which
 results in CYBLE_EVT_GATTC_HANDLE_VALUE_IND event at the GATT Client's end.

 Refer to Bluetooth 4.1 core specification, Volume 3, Part G, section 4.11 for
 more details on Indications.

Parameters:
 connHandle: Connection handle to identify the peer GATT entity, of type 
              CYBLE_CONN_HANDLE_T.
 indParam: Pointer to structure of type CYBLE_GATTS_HANDLE_VALUE_IND_T which is same as 
		 CYBLE_GATT_HANDLE_VALUE_PAIR_T.

Return:
 CYBLE_API_RESULT_T : Return value indicates if the function succeeded or
 failed. Following are the possible error codes.
 <table>
 Errors codes                           Description
 ------------                           -----------
  CYBLE_ERROR_OK                        On successful operation
  CYBLE_ERROR_INVALID_PARAMETER         'connHandle' value does not
                                         represent any existing entry
                                         in the Stack
  CYBLE_ERROR_INVALID_OPERATION         This operation is not permitted
  CYBLE_ERROR_MEMORY_ALLOCATION_FAILED  Memory allocation failed
 </table>

******************************************************************************/
CYBLE_API_RESULT_T CyBle_GattsIndication
(
	CYBLE_CONN_HANDLE_T					connHandle, 
	CYBLE_GATTS_HANDLE_VALUE_IND_T		* indParam
);


/******************************************************************************
##Function Name: CyBle_GattsErrorRsp
*******************************************************************************

Summary:
 This function sends an error response to the peer device. The Error Response
 is used to state that a given request cannot be performed, and to provide the
 reason as defined in 'CYBLE_GATT_ERR_CODE_T'. This is a non-blocking function.

 Note that the 'Write Command' initiated by GATT Client does not generate an
 'Error Response' from the GATT Server's end. The GATT Client gets 
 CYBLE_EVT_GATTC_ERROR_RSP event on receiving error response.

 Refer Bluetooth 4.1 core specification, Volume 3, Part F, section 3.4.1.1 for
 more details on Error Response operation.
 
Parameters:
 connHandle: Connection handle to identify the peer GATT entity, of type
              CYBLE_CONN_HANDLE_T.
 errRspParam: Pointer to structure of type CYBLE_GATTS_ERR_PARAM_T.

Return:
 CYBLE_API_RESULT_T : Return value indicates if the function succeeded or
 failed. Following are the possible error codes.
 <table>
 Errors codes                           Description
 ------------                           -----------
  CYBLE_ERROR_OK                        On successful operation
  CYBLE_ERROR_INVALID_PARAMETER         'connHandle' value does not
                                         represent any existing entry
                                         in the Stack
  CYBLE_ERROR_INVALID_OPERATION         This operation is not permitted
  CYBLE_ERROR_MEMORY_ALLOCATION_FAILED  Memory allocation failed
 </table>

******************************************************************************/
CYBLE_API_RESULT_T CyBle_GattsErrorRsp
(
	CYBLE_CONN_HANDLE_T			connHandle, 
	CYBLE_GATTS_ERR_PARAM_T		* errRspParam
);


/******************************************************************************
##Function Name: CyBle_GattsExchangeMtuRsp
*******************************************************************************

Summary:
This function sends the GATT Server's MTU size to the GATT Client. This
function has to be invoked in response to an Exchange MTU Request received
from the GATT Client. The GATT Server's MTU size should be greater than or 
equal to the default MTU size (23 bytes). This is a non-blocking function.

The peer GATT Client receives CYBLE_EVT_GATTC_XCHNG_MTU_RSP event on executing
this function on the GATT Server.

Refer to Bluetooth 4.1 core specification, Volume 3, Part G, section 4.3.1 for
more details on exchange of MTU.

Parameters:
 connHandle: Connection handle to identify the peer GATT entity, of type 
             CYBLE_CONN_HANDLE_T.
 mtu: Size of MTU, of type uint16

Return:
 CYBLE_API_RESULT_T : Return value indicates if the function succeeded or
 failed. Following are the possible error codes.
 <table>
 Errors codes                           Description
 ------------                           -----------
  CYBLE_ERROR_OK                        On successful operation
  CYBLE_ERROR_INVALID_PARAMETER         'connHandle' value does not
                                         represent any existing entry
                                         in the Stack or, 'mtu' has a
										 value which is greater than
										 that set on calling CyBle_StackInit
										 function
  CYBLE_ERROR_INVALID_OPERATION         This operation is not permitted
  CYBLE_ERROR_MEMORY_ALLOCATION_FAILED  Memory allocation failed
 </table>

******************************************************************************/
CYBLE_API_RESULT_T CyBle_GattsExchangeMtuRsp
(
	CYBLE_CONN_HANDLE_T			connHandle, 
	uint16 						mtu
);


#ifdef ATT_QUEUED_WRITE_SUPPORT

/******************************************************************************
##Function Name: CyBle_GattsPrepWriteReqSupport
*******************************************************************************

Summary:
 This API needs to be called after getting CYBLE_EVT_GATTS_PREP_WRITE_REQ 
 event from the BLE Stack to support prepare write request operation. 
 This API should be called only once during one Long/reliable write session. 
 This needs to be called from the same event call back context. 
 This is a non-blocking function.

 On receiving CYBLE_EVT_GATTS_PREP_WRITE_REQ, returning from the event handler
 without calling this function will result in prepare write response being 
 sent to the peer device rejecting the prepare write operation. 
 CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED error code will be sent to client.

Parameters:
 prepWriteSupport: If prepare write operation  is supported by the application
                    then the application layer should set this variable to 
                    CYBLE_GATTS_PREP_WRITE_SUPPORT. Any other value will 
                    result in the device rejecting the prepare write operation.
                    Allowed values for this parameter
                     * CYBLE_GATTS_PREP_WRITE_SUPPORT
                     * CYBLE_GATTS_PREP_WRITE_NOT_SUPPORT

Return:
 None

******************************************************************************/
void CyBle_GattsPrepWriteReqSupport
(
	uint8	 	prepWriteSupport
);


#endif /* ATT_QUEUED_WRITE_SUPPORT */


/******************************************************************************
##Function Name: CyBle_GattsWriteRsp
*******************************************************************************

Summary:
 This function sends a Write Response from a GATT Server to the GATT Client.
 This is a non-blocking function. This function has to be invoked in
 response to a valid Write Request event from the GATT Client
 (CYBLE_EVT_GATTS_WRITE_REQ) to acknowledge that the attribute has been
 successfully written.

 The Write Response has to be sent after the attribute value is written or 
 saved by the GATT Server. Write Response results in CYBLE_EVT_GATTC_WRITE_RSP
 event at the GATT Client's end.

Parameters:
connHandle: Connection handle to identify the peer GATT entity, of type
	         CYBLE_CONN_HANDLE_T.

Return:
 CYBLE_API_RESULT_T : Return value indicates if the function succeeded or
 failed. Following are the possible error codes.
 <table>
 Errors codes                           Description
 ------------                           -----------
  CYBLE_ERROR_OK                        On successful operation
  CYBLE_ERROR_INVALID_PARAMETER         'connHandle' value does not 
                                         represent any existing entry
                                         in the Stack
  CYBLE_ERROR_INVALID_OPERATION         This operation is not permitted
  CYBLE_ERROR_MEMORY_ALLOCATION_FAILED  Memory allocation failed
 </table>

******************************************************************************/
CYBLE_API_RESULT_T CyBle_GattsWriteRsp
(
	CYBLE_CONN_HANDLE_T	 connHandle 
);


#endif /*CY_BLE_CYBLE_STACK_GATT_SERVER_INTERNAL_H*/


 /*EOF*/
