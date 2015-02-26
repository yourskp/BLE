/*******************************************************************************
* File Name: RTC.h
*
* Version: 1.0
*
* Description:
*  Header file for RTC implementaiton on PSoC 4 BLE
*
*
*******************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(RTC_H)
#define RTC_H
    
#include <project.h>    
    
#define CYBLE_INIT                                  (0u)      /* Initialized state */
#define CYBLE_UNCONFIG                              (1u)      /* Unconfigured state */
#define CYBLE_CONFIG                                (2u)      /* Configured state */    
    
#define BLE_CTS_CHARACTERISTIC_VALUE                (0u)
#define BLE_CTS_CHARACTERISTIC_DESCRIPTOR           (1u)
    
/* Leap Year status bit */
#define RTC_STATUS_LY                               (0x02u)

/* AM/PM status bit */
#define RTC_STATUS_AM_PM                            (0x04u)
    
/* Time elapse constants */
#define RTC_MINUTE_ELAPSED                          (59u)
#define RTC_HOUR_ELAPSED                            (59u)
#define RTC_HALF_OF_DAY_ELAPSED                     (12u)
#define RTC_DAY_ELAPSED                             (23u)
#define RTC_WEEK_ELAPSED                            (7u)
#define RTC_YEAR_ELAPSED                            (12u)
#define RTC_DAYS_IN_WEEK                            (7u)
#define RTC_YEAR_LOW_MAX                            (255u)
    
/* Days Of Week definition */
#define RTC_SUNDAY                                  (1u)
#define RTC_MONDAY                                  (2u)
#define RTC_TUESDAY                                 (3u)
#define RTC_WEDNESDAY                               (4u)
#define RTC_THURDAY                                 (5u)
#define RTC_FRIDAY                                  (6u)
#define RTC_SATURDAY                                (7u)
    
/* Month definition */
#define RTC_JANUARY                                 (1u)
#define RTC_DAYS_IN_JANUARY                         (31u)
#define RTC_FEBRUARY                                (2u)
#define RTC_DAYS_IN_FEBRUARY                        (28u)
#define RTC_MARCH                                   (3u)
#define RTC_DAYS_IN_MARCH                           (31u)
#define RTC_APRIL                                   (4u)
#define RTC_DAYS_IN_APRIL                           (30u)
#define RTC_MAY                                     (5u)
#define RTC_DAYS_IN_MAY                             (31u)
#define RTC_JUNE                                    (6u)
#define RTC_DAYS_IN_JUNE                            (30u)
#define RTC_JULY                                    (7u)
#define RTC_DAYS_IN_JULY                            (31u)
#define RTC_AUGUST                                  (8u)
#define RTC_DAYS_IN_AUGUST                          (31u)
#define RTC_SEPTEMBER                               (9u)
#define RTC_DAYS_IN_SEPTEMBER                       (30u)
#define RTC_OCTOBER                                 (10u)
#define RTC_DAYS_IN_OCTOBER                         (31u)
#define RTC_NOVEMBER                                (11u)
#define RTC_DAYS_IN_NOVEMBER                        (30u)
#define RTC_DECEMBER                                (12u)
#define RTC_DAYS_IN_DECEMBER                        (31u)    
    
#define RTC_MONTHS_IN_YEAR                          (12u)
#define RTC_DAYS_IN_YEAR                            (365u)
#define RTC_DAYS_IN_LEAP_YEAR                       (366u)
    
#define RTC_SOURCE_COUNTER                          (0u)
#define RTC_COUNTER_ENABLE                          (1u)
#define RTC_COUNT_PERIOD                            ((uint32)32767)
#define RTC_INTERRUPT_SOURCE                        CY_SYS_WDT_COUNTER0_INT    
    
/* Returns 1 if leap year, otherwise 0 */
#define RTC_LEAP_YEAR(year) \
                    (((((year) % 400u) == 0u) || ((((year) % 4u) == 0u) && \
                    (((year) % 100u) != 0u))) ? 0x01u : 0x00u)

/* Returns 1 if corresponding bit is set, otherwise 0 */
#define RTC_IS_BIT_SET(value, mask) (((mask) == ((value) & (mask))) ? 0x01u : 0x00u)   
                    
void WDT_Handler(void);
extern CYBLE_CTS_CURRENT_TIME_T currentTime;

void RTC_Start(void);
CYBLE_API_RESULT_T StartTimeServiceDiscovery(void); 
CYBLE_API_RESULT_T SyncTimeFromBleTimeServer(void);
void CtsCallBack(uint32 event, void* eventParam);
uint8 RTC_TickExpired(void);
void RTC_UI_Update(void);
    
#endif /* End of #if !defined(RTC_H) */

/* [] END OF FILE */
