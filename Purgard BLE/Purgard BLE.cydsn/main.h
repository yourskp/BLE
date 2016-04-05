/*****************************************************************************
* File Name: main.h
*
* Version: 1.0
*
* Description:
*   This is the top level application for the Purgard Level Sensor on PSoC 4.
*
* Hardware Dependency:
*   Chemical Tank Rev A PCB
*
******************************************************************************
* Copyright (2016), Intermountain water Softeners
******************************************************************************
* This software is owned by Intermountain water Softener (Intermoutain) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited.
*
*****************************************************************************/

#if !defined(_MAIN_H)
#define _MAIN_H

/*****************************************************************************
* Included headers
*****************************************************************************/
#include <project.h>


/*****************************************************************************
* Macros 
*****************************************************************************/

/*****************************************************************************
* Function Prototypes
*****************************************************************************/
void InitializeSystem(void);
uint8 MeasureLevel(void);
extern void CapSense_SetCompensationIDAC();
extern uint8 LevelCalculation(void);
extern char SaveInitialCalibration(void);
extern char WriteFlash(uint8, uint32 * );
void SetSensorDacs();    
extern void ReadTemperatureAndBattery(void);

/*****************************************************************************
* Global Definitions
*****************************************************************************/
    // enable CapSense Tuner (for tuning the sensors)
#define TUNER_ENABLE    0

#define SENSOR_TARGET_VALUE     45000   // target steady state sensor value
#define NUMBER_OF_SENSORS   11
#define DAC_INITIAL_VALUE   2
#define OUTPUT_LEVEL_MAX    100     // reports 100 when the tank is full
#define SIGNAL_MAX          10000   // sensor signals are multiplied by a scaler to achieve this maximum level
#define SENSOR_IIR_FRACTION 4       // the number of shifts instead of a filter coefficient. 4 = 1/16, 2 = 1/4
#define SENSOR_FRACTIONAL_SHIFT 15  // shifts a 16 bit unsigned to a 32 bit signed number
#define AVERAGE_COUNT       64      // number of sampleto average for baseline
#define AVERAGE_SHIFT       6       // number of shifts to normalize the average count

#define FLASH_CALIBRATION_BASE     ( CY_FLASH_BASE + (CY_FLASH_SIZEOF_ROW * (CY_FLASH_NUMBER_ROWS-1)) )
#define AdcBatteryChannel   0
#define AdcDiodeChannel 1

#define CAPSENSE_ENABLED
#define BLE_ENABLED

#define NO_FINGER           (0xFFFFu)

#define SLIDER_MAX_VALUE    (0x0064)

#define TRUE				1
#define FALSE				0
#define ZERO				0
#define LOW                 0
#define HIGH                1


uint8 LastPercentFull;
uint8 PercentFull;   // integer percentage of how full tank is (0-100)
int8 DieTemperature;   // die temperature. funtion returns 32 bits, but that is silly.
int16 CalibrationTemp;  // Temperature that the unit was calibrated. This is the internally read temp sensor.
uint16 BatteryVoltage;  // in Millivolts


#endif  /* #if !defined(_MAIN_H) */

/* [] END OF FILE */
