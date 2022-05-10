

#include <Arduino.h>
#include "cnst.h"
#include "wifi_c.h"
#include "can.h"
#include "ina.h"
#include  "util.h"
#include "tacho.h"
#include "param.h"
#include "motor.h"
#include <ArduinoJson.h>
 
PARAM Param; 
SM_CAN CAN;
utility UTIL;
stepper D_left,D_right;
frequency flows[4];
INA219 Cur_sen;

void can_core (void *parameters){     while (1) {CAN.check_CAN();}}

void current_core (void *parameters){     while (1) {Cur_sen.measure();}}

void flow0_core (void *parameters){ while (1) {flows[0].measure();}}
void flow1_core (void *parameters){ while (1) {flows[1].measure();}}
void flow2_core (void *parameters){ while (1) {flows[2].measure();}}
void flow3_core (void *parameters){ while (1) {flows[3].measure();}}

#define R_SHUNT 1
#define V_SHUNT_MAX 0.05
#define V_BUS_MAX 16
#define I_MAX_EXPECTED 25

#define PWM_Res   8
#define PWM_Freq  50

void IRAM_ATTR L_FLT_int(){}
void IRAM_ATTR R_FLT_int(){}

void setup() {

 Serial.begin(115200);
 while(!Serial);
 Delay(500);
 pinMode(BLINK_LED_PIN,OUTPUT);
 pinMode(SELETRON_RIGHT_SIG,OUTPUT); SELETRON_RIGHT(OFF);
 pinMode(SELETRON_LEFT_SIG,OUTPUT);  SELETRON_LEFT(OFF);
 pinMode(STEP_RESET,OUTPUT);

  pinMode(L_FLT, INPUT);
  attachInterrupt(L_FLT, L_FLT_int, RISING);
  pinMode(R_FLT, INPUT);
  attachInterrupt(R_FLT, R_FLT_int, RISING);
 
 ledcSetup(0, PWM_Freq, PWM_Res); 
 ledcAttachPin(MAIN_PUMP_PWM,0);
 ledcWrite(0, 0);
 
  PRINTS("FileFS=",FS_Name);
  Delay(500);
 if(!FileFS.begin())        Serial.println("FileFS Mount Failed");
  
 UTIL.reset_reason();
 UTIL.listDir("/",2);
 Param.restor_param() ;
 Serial.println("startting...");

 CAN.begin();
 start_wifi();
 
 Cur_sen.begin();
//GAIN_1_40MV = 0,        GAIN_2_80MV = 1,        GAIN_4_160MV = 2,        GAIN_8_320MV = 3
// ADC_9BIT    ADC_10BIT   ADC_11BIT   ADC_12BIT    ADC_2SAMP   ADC_4SAMP  ADC_8SAMP   ADC_16SAMP  ADC_32SAMP   ADC_64SAMP   ADC_128SAMP 
 Cur_sen.configure(INA219::RANGE_16V, INA219::GAIN_1_40MV, INA219::ADC_12BIT, INA219::ADC_128SAMP, INA219::CONT_SH);
 Cur_sen.calibrate(R_SHUNT, V_SHUNT_MAX, V_BUS_MAX, I_MAX_EXPECTED);


 D_left.setup(  DOSING_LEFT_SIG,1,1000);
 D_right.setup(DOSING_RIGHT_SIG,3,1000);
 
 flows[0].setup(PULSE_A, 0, 5, 0,FALLING);
 flows[1].setup(PULSE_B, 1, 5, 0,FALLING);
 flows[2].setup(PULSE_C, 2, 5, 0,FALLING);
 flows[3].setup(PULSE_D, 3, 5, 0,FALLING);
PRINTF("remaining stack:%d Heap before malloc:%d\n",uxTaskGetStackHighWaterMark(NULL),xPortGetFreeHeapSize());
xTaskCreatePinnedToCore(can_core, "check_CAN", 2048,NULL, 1,NULL, 1);     
xTaskCreatePinnedToCore(flow0_core, "check_flow0_core", 2048,NULL, 1,NULL, 1); 
xTaskCreatePinnedToCore(flow1_core, "check_flow1_core", 2048,NULL, 1,NULL, 1); 
xTaskCreatePinnedToCore(flow2_core, "check_flow2_core", 2048,NULL, 1,NULL, 1); 
xTaskCreatePinnedToCore(flow3_core, "check_flow3_core", 2048,NULL, 1,NULL, 1); 
xTaskCreatePinnedToCore(current_core, "current_core", 2048,NULL, 1,NULL, 1); 

  Param.acceleration_left=false;
  Param.acceleration_right=false;
  Param.num_nozzel=0;
  Param.main_pump_active=false;
xTaskCreatePinnedToCore(accerate_core, "accerate_core", 2048,NULL, 1,NULL, 1);


}

void loop() {
 
if(Param.MainWSclinet) CAN.send_web();
if(Param.this_CAN_SSR>0) CAN.senStatus();



}
