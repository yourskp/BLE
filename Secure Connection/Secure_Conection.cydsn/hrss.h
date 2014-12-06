/*******************************************************************************
* File Name: hrss.h

* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  HRS service related code header.
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include "common.h"

/***************************************
*        Constant definitions
***************************************/
/* Definitions for Heart Rate Measurement characteristic */
#define CYBLE_HRS_HRM_HRVAL16           (0x01u)
#define CYBLE_HRS_HRM_CHAR_LEN          (20u)        /* for default 23-byte MTU */

#define SIM_HEART_RATE_MIN              (60u)       /* Minimum simulated heart rate measurement */
#define SIM_HEART_RATE_MAX              (300u)      /* Maximum simulated heart rate measurement */
#define SIM_HEART_RATE_INCREMENT        (2u)        /* Value by which the heart rate is incremented */

#define MAIN_LOOP_SIMULATION_THRESHOLD  (5000u)

/***************************************
*            Data Types
***************************************/
/* Heart Rate Measurement characteristic data structure type */
typedef struct
{
    uint8 flags;
    uint16 heartRateValue;
}CYBLE_HRS_HRM_T;

/***************************************
*        Function Prototypes
***************************************/

void HeartRateCallBack(uint32 event, void* eventParam);
void SimulateHeartRate(void);
void HrsInit(void);
void HrssSendHeartRateNtf(void);

/* Functions generated in macros */

/*******************************************************************************
* Function Name: CyBle_HrssSetHeartRate
********************************************************************************
*
* Summary:
*  Sets Heart Rate value into the Heart Rate Measurument characteristic.
*
* Parameters:
*  uint16 heartRate - Heart Rate value to be set.
*
* Return:
*  None
*
*******************************************************************************/
#define HrssSetHeartRate(heartRate)\
            (hrsHeartRate.heartRateValue = (heartRate))

/***************************************
*      External data references
***************************************/
/* Heart Rate Measurement characteristic data structure */
extern CYBLE_HRS_HRM_T hrsHeartRate;
extern uint8 heartRateSimulation;

/* [] END OF FILE */
