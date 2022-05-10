#pragma once
#ifndef _CONST_H
#define _CONST_H


#include "Arduino.h"

#ifndef BLINK_LED_PIN
  #define BLINK_LED_PIN   2
#endif

#define PROGRAM_NAME "SMC_DEFOLIATOR_HEAD_MOTOR_CONTROLLER"
#define PROGRAM_VERSION "V_2_1"
#define PROGRAM_DATE "25_1_2022"

#define PULSE_A               GPIO_NUM_18
#define PULSE_B               GPIO_NUM_23
#define PULSE_C               GPIO_NUM_19
#define PULSE_D               GPIO_NUM_25

#define L_FLT                 GPIO_NUM_34
#define R_FLT                 GPIO_NUM_35

#define DOSING_LEFT_SIG       GPIO_NUM_13
#define DOSING_RIGHT_SIG      GPIO_NUM_12

#define MAIN_PUMP_PWM         GPIO_NUM_27

#define STEP_RESET            GPIO_NUM_26

#define SELETRON_LEFT_SIG     GPIO_NUM_33
#define SELETRON_RIGHT_SIG    GPIO_NUM_32

//#define DOSING_LEFT_SIG       GPIO_NUM_17
//#define 
#define OFF LOW

#define RUNNING true
#define CEASE   false

#define SELETRON_LEFT(x)  digitalWrite(SELETRON_LEFT_SIG,x);
#define SELETRON_RIGHT(x) digitalWrite(SELETRON_RIGHT_SIG,x);

#define ENABLE_STEPPER    digitalWrite(STEP_RESET,HIGH);
#define DISABLE_STEPPER   digitalWrite(STEP_RESET,LOW);

#define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())
   
#define  multithread

#define Delay(ms) vTaskDelay(ms / portTICK_PERIOD_MS);
#define BLINK_LED_PIN 2


#define WDT_TIMEOUT 5


#ifdef ESP_IDF_VERSION_MAJOR // IDF 4+
  #if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
    #include "esp32/rom/rtc.h"
  #elif CONFIG_IDF_TARGET_ESP32S2
    #include "esp32s2/rom/rtc.h"
  #elif CONFIG_IDF_TARGET_ESP32C3
    #include "esp32c3/rom/rtc.h"
  #else 
     #error Target CONFIG_IDF_TARGET is not supported
  #endif
#else // ESP32 Before IDF 4.0
  #include "rom/rtc.h"
#endif

#if ( ARDUINO_ESP32C3_DEV )
    // Currently, ESP32-C3 only supporting SPIFFS and EEPROM. Will fix to support LittleFS
    #define USE_LITTLEFS          false
    #define USE_SPIFFS            true
  #else
    #define USE_LITTLEFS    true
    #define USE_SPIFFS      false
#endif

#if USE_LITTLEFS
    // Use LittleFS
    #include "FS.h"

    // Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
    //#if ( ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0) )  //(ESP_ARDUINO_VERSION_MAJOR >= 2)
    #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
      //#warning Using ESP32 Core 1.0.6 or 2.0.0+
      // The library has been merged into esp32 core from release 1.0.6
      #include <LittleFS.h>
      
     // FS* filesystem =      &LittleFS;
      #define FileFS         LittleFS
      #define FS_Name       "LittleFS"
    #else
      #warning Using ESP32 Core 1.0.5-. You must install LITTLEFS library
      // The library has been merged into esp32 core from release 1.0.6
      #include <LITTLEFS.h>             // https://github.com/lorol/LITTLEFS
      
      FS* filesystem =      &LITTLEFS;
      #define FileFS        LITTLEFS
      #define FS_Name       "LittleFS"
    #endif
    
#elif USE_SPIFFS
    #include <SPIFFS.h>
    FS* filesystem =      &SPIFFS;
    #define FileFS        SPIFFS
    #define FS_Name       "SPIFFS"
#else
    // +Use FFat
    #include <FFat.h>
    FS* filesystem =      &FFat;
    #define FileFS        FFat
    #define FS_Name       "FFat"
#endif

 
#define DEBUG 1
#define DEBUG_Serial
#if defined DEBUG_Serial
  #define PRINT(s)     Serial.print(s)  
  #define PRINTS(s,v)  { Serial.print(s); Serial.println(v); }
  #define PRINTLN(s)   Serial.println(s)   
  #define PRINTF(...) Serial.printf( __VA_ARGS__ )
#else  
  #define PRINT(s)
  #define PRINTS(s,v)
  #define PRINTLN(s)
  #define PRINTF(...)
#endif




#endif

