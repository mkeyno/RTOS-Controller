

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
//uint64_t time;
void can_core (void *parameters){     while (1) {CAN.check_CAN();}}
/*
void link_core (void *parameters){     while (1) {
  Delay(100);
   if(Param.status==CEASE){
                            twai_message_t check;
                            check.identifier=5000;
                            check.data_length_code=8;
                            if (twai_transmit(&check, pdMS_TO_TICKS(1)) != ESP_OK) CAN.resetTWAI();

                              }
  
  }}
void accerate_coreA(void *parameters){     while (1) {
  
if(Param.acceleration_left){
                            Param.currenst_f_l+=5;
                            if(Param.currenst_f_l>=Param.target_f_l){ PRINTF("left_f>%d\n",Param.target_f_l); Param.acceleration_left=false; }
                            D_left.set_speed(Param.currenst_f_l); Delay(5); D_left.start();
                            Delay(5);
                          }

if(Param.acceleration_right){
                            Param.currenst_f_r+=5;
                            if(Param.currenst_f_r>=Param.target_f_r){ PRINTF("right_f>%d\n",Param.target_f_r); Param.acceleration_right=false; }
                            D_right.set_speed(Param.currenst_f_r); Delay(5); D_right.start();
                            Delay(5);
                          }
   
  //if(!Param.main_pump_active && D_left.isRunning()  && D_right.isRunning() ) {Param.acceleration_right=false; Param.acceleration_left=false;  D_left.stop(); Delay(2) ;  D_right.stop(); PRINTLN("cut out"); }
    Delay(2) ;                     

}}
void accerate_core (void *parameters){     while (1) {
  
if(Param.acceleration_left){
                           // 
                           if(Param.num_nozzel==1 && Param.done_halfing_l){ Param.target_f_l=Param.target_f_l/2; Param.done_halfing_l=false; PRINTLN("left half");}

                               Param.currenst_f_l+=5;
                            if(Param.currenst_f_l>=Param.target_f_l){ PRINTF("left_f>%d\n",Param.target_f_l); Param.acceleration_left=false; }
                            D_left.set_speed(Param.currenst_f_l); Delay(5); D_left.start();

                            if(Param.num_nozzel==0) {Param.acceleration_left=false;  D_left.stop();}
                            Delay(5);
                          }

if(Param.acceleration_right){
                            if(Param.num_nozzel==1 && Param.done_halfing_r){ Param.target_f_r=Param.target_f_r/2; Param.done_halfing_r=false; PRINTLN("right half");}

                               Param.currenst_f_r+=5;
                            if(Param.currenst_f_r>=Param.target_f_r){ PRINTF("right_f>%d\n",Param.target_f_r); Param.acceleration_right=false; }
                            D_right.set_speed(Param.currenst_f_r); Delay(5); D_right.start();

                            if(Param.num_nozzel==0) {Param.acceleration_right=false;  D_right.stop();}
                            Delay(5);
                          }
   
  //if(!Param.main_pump_active && D_left.isRunning()  && D_right.isRunning() ) {Param.acceleration_right=false; Param.acceleration_left=false;  D_left.stop(); Delay(2) ;  D_right.stop(); PRINTLN("cut out"); }
    Delay(2) ;                     


}}
void heart_bit (void *parameters){     while (1) {
  Delay(5);
 //  ets_printf( "%d,%d, %d \n",Param.status,millis()-Param.last_heart_bit,Param.last_heart_bit);
   
  if((millis()-Param.last_heart_bit)>5000 && Param.status==RUNNING) {   
                                                                      PRINTLN("Shut down every things") ; //UTIL.shut_down();  
                                                                      SELETRON_LEFT(OFF);
                                                                      SELETRON_RIGHT(OFF);
                                                                      ledcWrite(0, 0);
                                                                      D_left.stop(); //13
                                                                      D_right.stop();//16
                                                                      Param.status=CEASE;
                                                                      Param.information["RUNNING"]=CEASE;

                                                                      }
  
  }}
*/
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

void IRAM_ATTR L_FLT_int()
{
//DISABLE_STEPPER;
//Delay(10);
//  Param.acceleration_left=true;Param.currenst_f_l=400;
  ets_printf("\t\t\t\tfalt left\n");
 // ENABLE_STEPPER;

}
void IRAM_ATTR R_FLT_int()
{
 /// DISABLE_STEPPER;
 // Delay(10);
 // Param.acceleration_right=true;Param.currenst_f_r=400;
  ets_printf("\t\t\t\tfalt right\n");
 // ENABLE_STEPPER
}


void setup() {

 Serial.begin(115200);
 while(!Serial);
 Delay(500);
 pinMode(BLINK_LED_PIN,OUTPUT);
 pinMode(SELETRON_RIGHT_SIG,OUTPUT); SELETRON_RIGHT(OFF);
 pinMode(SELETRON_LEFT_SIG,OUTPUT);  SELETRON_LEFT(OFF);
 pinMode(STEP_RESET,OUTPUT);
 //pinMode(16, GPIO_MODE_INPUT_OUTPUT);
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
//xTaskCreatePinnedToCore(heart_bit, "heart_bit", 1024,NULL, 1,NULL, 1);
//xTaskCreatePinnedToCore(link_core, "link_core", 1024,NULL, 1,NULL, 1);

/*
  PRINTF("Total heap: %d\n", ESP.getHeapSize());
  PRINTF("Free heap: %d\n", ESP.getFreeHeap());//log_d
  PRINTF("Total PSRAM: %d\n", ESP.getPsramSize());
  PRINTF("Free PSRAM: %d\n", ESP.getFreePsram());
  PRINTF("psram Found : %d\n", psramFound());
*/
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