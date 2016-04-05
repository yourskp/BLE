/*****************************************************************************
* File Name: main.c
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


/*****************************************************************************
* Included headers
*****************************************************************************/
#include <main.h>
#include <BLEApplications.h>


/*****************************************************************************
* Global Definitions
*****************************************************************************/

uint8 SensorDacValues[NUMBER_OF_SENSORS] = {DAC_INITIAL_VALUE};
uint16 RawValue[NUMBER_OF_SENSORS];
int16 AverageSignalValue[NUMBER_OF_SENSORS];
uint16 MaximumSensorSignal[NUMBER_OF_SENSORS] = {30,30,90,280,450,650,980,1200,1280,1075,980};   // This is the actual signal level that is fully active on each sensor
uint32 SensorBaseCalibration[NUMBER_OF_SENSORS] = {44990,44900,44680,44515,44720,44550,44550,44420,44400,44300,44800};    // initial empty tank sensor values at nominal room temp
uint16 SensorScaling[NUMBER_OF_SENSORS] = {130,130,40,22,21,18,11,7,7,11,16};   // values that scale the signal up to a max value of 10000 for linearization of level calculation
uint16 ScaledSensorValue[NUMBER_OF_SENSORS];    // This is the normalized value of each sensor signal. The nominal max value is SIGNAL_MAX.
float InvMaxLevel = 1.0 / (float)( (NUMBER_OF_SENSORS-1) * SIGNAL_MAX );
int32 SensorIntermediate[NUMBER_OF_SENSORS];   // high resolution filter data
int32 SensorFiltered[NUMBER_OF_SENSORS] = {0};    // high resolution of sensor filtered output
int32 WorkingLevel[NUMBER_OF_SENSORS];

// Calibration values are stored in the last block of flash memory
// FlashArray is a pointer to the base of that block and can be used as an array of size CY_FLASH_SIZEOF_ROW
uint8 * FlashArray = (uint8 *)FLASH_CALIBRATION_BASE;   // points to calibration data in flash



/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* System entrance point. This calls the initializing function and continuously
* process BLE and CapSense events.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main()
{
	/* This function will initialize the system resources such as BLE and CapSense */
    InitializeSystem();

    for(;;)
    {
        
#if TUNER_ENABLE
        CapSense_TunerComm();   // this routine reads all sensors and communicates them to tuner
#else  // end TUNER_ENABLE = 1 conditional, do this when = 0
        CapSense_ScanEnabledWidgets();  // read all the sensors
#endif  // end TUNER_ENABLE conditional


        //Process event callback to handle BLE events. The events generated and 
		// used for this application are inside the 'CustomEventHandler' routine
        CyBle_ProcessEvents();
		
		if(TRUE == deviceConnected)
		{
			// Send CapSense Slider data when respective notification is enabled 
			if(TRUE == sendCapSenseSliderNotifications)
			{
                
//???                if(PercentFull != LastPercentFull)
//???	            {
                    // send the BLE notification with the level
            	SendCapSenseNotification(PercentFull);

//???                }	

			}
            
#if KRIS_CHANGES            
            if(TRUE == sendTemperatureNotifications && CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_FREE)
			{
                 SendTemperatureNotification((uint8)DieTemperature);
            }
            
            if(TRUE == sendBatteryNotifications && CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_FREE)
			{
                SendBatteryNotification(BatteryVoltage);
            }
#endif            
                // Save last level
            LastPercentFull = PercentFull;
                // Do the level sensing after the value is sent. This enables timely response
			PercentFull = MeasureLevel();
            
                // Measure the temperature battery voltage
            ReadTemperatureAndBattery();
		}
 
    }	
}

/*******************************************************************************
* Function Name: MeasureLevel
********************************************************************************
* Summary:
* This function turns on the ADC, reads the battery voltage and temperaure channels, 
* then calculates the values to send back.
*
* Formats of values:
*       Battery Voltage is calculated as an unsigned in in millivolts.
*
*       Temperature is the Die Temperature and is a signed 8 bit value
*
* Parameters:
*       void
*
* Return:
*       none
*
********************************************************************************/

void ReadTemperatureAndBattery(void)
{
    int16 AdcResult;
    
        // Pull the bottom of the voltage divider low for measurement
    AdcGnd_Write(LOW);
    
    ADC_Start();    // power on the ADC
    ADC_EnableInjection();  // the injection or temperature channel needs to be enabled before every scan
    ADC_StartConvert();
        // wait until all channels are done including the diode reading
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT_INJ);
    
    // Read the ADC for battery voltage. The voltage divider is 1/6 so multiply
    AdcResult = 6*ADC_GetResult16(AdcBatteryChannel);
        // convert to millivolts
    BatteryVoltage = ADC_CountsTo_mVolts(AdcBatteryChannel, AdcResult);
    
        // Read the ADC for temperature
    AdcResult = ADC_GetResult16(AdcDiodeChannel);
    DieTemperature = (int8)DieTemp_CountsTo_Celsius(AdcResult);
    
    ADC_Stop(); // stop the ADC to save power
    AdcGnd_Write(HIGH); // reduce current by releasing bottom of voltage divider
}

/*******************************************************************************
* Function Name: MeasureLevel
********************************************************************************
* Summary:
* This function scans the sensors and calculates the liquid level, 
* then triggers separate routine for BLE notification.
*
* Parameters:
*  void
*
* Return:
*       unsigned char Value from 0-100
*
*******************************************************************************/
uint8 MeasureLevel(void)
{
	uint32 Level;   // Present slider position read by CapSense
    int n;
    static uint8 HowFull = 0;  // final percentage full
    int32 SensorFraction;
    int32 SensorDiff;
    
#if TUNER_ENABLE
    CapSense_TunerComm();   // this routine reads all sensors and communicates them to tuner
#else  // end TUNER_ENABLE = 1 conditional, do this when = 0
    CapSense_ScanEnabledWidgets();  // read all the sensors
#endif  // end TUNER_ENABLE conditional

    // Wait for CapSense scanning to be complete
	while(CapSense_IsBusy())
    {
    }
    
/* IMPORTANT NOTE:
        The CapSense_sensorSignal[] only gets updated by baseline update calls. 
        This program never updates the baseline because it depends on the production
        baselines stored in flash for calculating levels. Therefore, CapSense_sensorSignal[]  
        is modified in this function.
*/

    for( n = 0; n < NUMBER_OF_SENSORS; ++n )
    {    // create the raw signal
        if(CapSense_sensorBaseline[n] > CapSense_sensorRaw[n])
        {
            SensorIntermediate[n] = 0;
        }
        else
        {
            // keep as uint becaue negative values are set to zero above
            CapSense_sensorSignal[n] = CapSense_sensorRaw[n] - CapSense_sensorBaseline[n];
        }
        // now filter it.
        SensorIntermediate[n] = (int32)(CapSense_sensorSignal[n]<<SENSOR_FRACTIONAL_SHIFT);    //used to keep resolution in filter
        // take difference of last output and new signal.
        SensorDiff = SensorIntermediate[n] - SensorFiltered[n];
        // Final output is a portion of difference added to last output
        SensorFraction = SensorDiff >> SENSOR_IIR_FRACTION;
        SensorFiltered[n] = SensorFiltered[n] + SensorFraction;
        CapSense_sensorSignal[n] = (uint16)(SensorFiltered[n] >> SENSOR_FRACTIONAL_SHIFT);
    }
    
    Level = 0;
    // start at 1 because first sensor is bad
    for( n = 1; n < NUMBER_OF_SENSORS; ++n )
    {
        WorkingLevel[n] = (int32)CapSense_sensorSignal[n]*SensorScaling[n];
        
        if( WorkingLevel[n] > SIGNAL_MAX ) // clip the signal
            WorkingLevel[n] = SIGNAL_MAX;
        
        Level += WorkingLevel[n];
    }

    // Each of 10 signals can be as high as 10,000
    // When added together, that gives 100,000 maximum
    // dividing that by 1000 give 100 maximum which is exactly what we want!
    HowFull = (uint8)(Level / 1000);
    
    return(HowFull);
}

/*******************************************************************************
* Function Name: InitializeSystem
********************************************************************************
* Summary:
* Start the components and initialize system.
*
*   On exit; 
*       - CapSense_sensorBaseline[] has been updated with the baseline value for each sensor
*       - SensorDacValues[] has the DAC values to achieve the correct sensor range
*       - Calibrated values are stored in flash on the first powerup
*       - Calibrated values are read from flash and put in proper arrays on all subsequent powerups
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void InitializeSystem(void)
{
    uint16 m, Sensor;
    
	/* Enable global interrupt mask */
	CyGlobalIntEnable;

#if TUNER_ENABLE
    CapSense_TunerStart();  //Start Tuner operation. this includes all of the capsense functionality
#else  // end TUNER_ENABLE = 1 conditional, do this when = 0
    EZI2C_Start();
    CapSense_Start();    // Start CapSense component
    
    // Does not seem to work without explicitly enabling each button
    // enable all of the buttons for scanning
    for( m = 0; m < NUMBER_OF_SENSORS; ++m)
    {
        CapSense_EnableWidget(m);
    }
#endif  // end TUNER_ENABLE conditional

    // Flash is zeroed when programmed, therefore, a 0 means it has not been calibrated
    // using the MSB because there is a possibility that the LSB could be zero
    if( FlashArray[1] == 0 ) 
    {
        SetSensorDacs();    // sets operating point of the sensor dacs for best sensitivity
        SaveInitialCalibration();   // saves all the initial calibration points to flash during production
    }
    else
    {    // if sensors have been calibrated, read values for DAC's and baselines from flash
        
        // put calibrated baselines into working baselines
        for(Sensor = 0; Sensor < NUMBER_OF_SENSORS; ++Sensor )
        {
            m = Sensor * 2;
            CapSense_sensorBaseline[Sensor] = (uint16)FlashArray[m];
            CapSense_sensorBaseline[Sensor] += (uint16)FlashArray[m+1]<<8;
        }

        // put calibration values into DACs
        for(m = 0; m < NUMBER_OF_SENSORS; ++m)
        {
            CapSense_SetModulationIDAC( m, FlashArray[NUMBER_OF_SENSORS*2+m]);
        }
        // read calibration temperature
        CalibrationTemp = FlashArray[NUMBER_OF_SENSORS*3];
        CalibrationTemp += FlashArray[NUMBER_OF_SENSORS*3+1]<<8;

    }

	/* Start BLE component and register the CustomEventHandler function. This 
	 * function exposes the events from BLE component for application use */
    CyBle_Start(CustomEventHandler);
}





/***************************************************************
    Run on the first power-up of the board. It calibrates all the
    initial values to an empty tank and nominal temperature.
    All of the values are then saved to flash.
*****************************************************************/

char SaveInitialCalibration(void)
{
    int16 AdcResult;
    char ReturnCode;

    ADC_Start();
    // start the ADC conversion
    ADC_StartConvert();
    
    // wait until the diode reading conversion is complete
    ADC_IsEndConversion(ADC_RETURN_STATUS_INJ);
    
    // Read the ADC
    AdcResult = ADC_GetResult16(AdcDiodeChannel);
    
    // save the temperature that calibration was run
    DieTemperature = (int16)DieTemp_CountsTo_Celsius(AdcResult);
    
    ADC_Stop(); // stop the ADC to save power
    
    // Save the base sensor readings
    ReturnCode = WriteFlash( NUMBER_OF_SENSORS, SensorBaseCalibration);
    return(ReturnCode);

}



char WriteFlash( uint8 NumberOfValues, uint32 *InputArray )
{
    int i, ArrayIndex;
    uint8 FlashRowArray[CY_FLASH_SIZEOF_ROW];   // buffer used to pass data to flash writing routine
    char ReturnCode;
    
    // format the 16 bit Sensor Values into 8 bit for writing to flash
    for(i=0; i < NumberOfValues; ++i )
    {
       ArrayIndex = i*2;
       FlashRowArray[ArrayIndex] = (uint8)InputArray[i];         // LSB
       FlashRowArray[ArrayIndex+1] = (uint8)(InputArray[i]>>8);    // MSB
    }
    
    ArrayIndex = i*2;
    
    // append the DAC values to the array being written to flash
    for( i = 0; i < NumberOfValues; ++i )
    {
        FlashRowArray[ArrayIndex] = SensorDacValues[i];
        ++ArrayIndex;
    }

    // append the die temperature to the array. Used for temperature compensation
    FlashRowArray[i] = (uint8)DieTemperature;         // LSB
    FlashRowArray[i+1] = (uint8)(DieTemperature>>8);    // MSB

    ReturnCode = CySysFlashWriteRow( (CY_FLASH_NUMBER_ROWS-1), FlashRowArray);
    
    return(ReturnCode);
}

/***************************************************************
Function that sets the nominal operating point of the Sensors.
This is done to optimize the sensitivity of each sensor.

*****************************************************************/
void SetSensorDacs()
{
    static int AverageCount, SensorNumber, CalibratedTotal, Calibrated;
    static int i;
    static int CalibrationNotDone;
    
    AverageCount = 0;
    SensorNumber = 0;
    CalibratedTotal = 0;
    CalibrationNotDone = 1;
    Calibrated = 0;
    
    // set all the DAC values to a point known to give a high sensor reading
    // this seems to happen normally, but just making sure
    for(i = 0; i < NUMBER_OF_SENSORS; ++i)
    {
        CapSense_SetModulationIDAC( i, SensorDacValues[i]);
    }
    
    while( CalibrationNotDone )
    {
        
#if TUNER_ENABLE
        CapSense_TunerComm();   // this routine reads all sensors and communicates them to tuner
#else  // end TUNER_ENABLE = 1 conditional, do this when = 0
        CapSense_ScanEnabledWidgets();  // read all the sensors
#endif  // end TUNER_ENABLE conditional

        while(CapSense_IsBusy());   // wait for scanning to complete
        
        //********* section that adjusts the sensors to the right range *****************
        // after the tuner or scan call, sensors have all been read. 
        // If calibration is not completed, go into the routine to set the DAC's
        if( Calibrated != 1 )
        {   // still more to calibrate
            CalibratedTotal = 0;
            for(SensorNumber = 0; SensorNumber < NUMBER_OF_SENSORS; ++SensorNumber)
            {
                RawValue[SensorNumber] = CapSense_ReadSensorRaw(SensorNumber);
                // Read the sensor value for the current sensor and compare to target
                if( RawValue[SensorNumber] < SENSOR_TARGET_VALUE )
                {
                    // getting here means that the value is set properly
                    // Start working on the new sensor
                    ++CalibratedTotal;
                }
                else
                {
                    // Reduce the DAC value which drives up the raw counts
                    ++SensorDacValues[SensorNumber];
                    CapSense_SetModulationIDAC(SensorNumber, SensorDacValues[SensorNumber]);
                }
            }
            if( CalibratedTotal >= NUMBER_OF_SENSORS)
                Calibrated = 1;
        }
        else if( AverageCount < AVERAGE_COUNT )   // after sensors are in the right range, sample 16 times to get a good average
        {
            for( SensorNumber = 0; SensorNumber < NUMBER_OF_SENSORS; ++SensorNumber )
            {   // sum the samples
//??? used for testing
                SensorBaseCalibration[SensorNumber] += RawValue[SensorNumber];
            }
            ++AverageCount;
        }
        else if( CalibrationNotDone )
        {   // average baseline and save as in-use baseline
            for( SensorNumber = 0; SensorNumber < NUMBER_OF_SENSORS; ++SensorNumber )
            {
                // set the baseline
                SensorBaseCalibration[SensorNumber] = SensorBaseCalibration[SensorNumber] >> AVERAGE_SHIFT;
                // set the operational baseline to the what was just calculated
                CapSense_sensorBaseline[SensorNumber] = (uint16)SensorBaseCalibration[SensorNumber];
            }
            CalibrationNotDone = 0;
        }
    }   //************ End sensor self calibration
}



/* [] END OF FILE */
