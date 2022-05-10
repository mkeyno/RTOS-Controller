
#include "Arduino.h"
#include "motor.h"
#include "param.h"
#ifndef LED_BUILTIN
  #define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
#endif
#include "cnst.h"

extern PARAM Param;

 

 


#define Qot(x) #x
#define P_SET(s,v) PRINT("Set "); PRINT(#s); PRINTS(" to:",v);
#define P_GET(s,v) PRINT("This "); PRINT(#s); PRINTS(" value:",v);
#define SAVE(s,v,t) Param.save_##t(#s,v) ;     Param.this_##s=v;         P_SET(s,v)    ;         

#define READ_d(s)   Param.this_##s = Param.read_d(Qot(s)) ;   
#define READ_s(s)   Param.this_##s = Param.read_s(Qot(s)) ;  
#define READ_f(s)   Param.this_##s = Param.read_f(Qot(s)) ;   

#define P_JASON(s,v) Param.this_##s=v;   Param.information[#s ]=Param.this_##s ;
#define A_qot(s,n) s##n
#define PWM_Res   8
#define PWM_Freq  200

#define RAMP_UP    true
#define RAMP_DOWN false

#define FORWARD  1
#define BACKWARD 0
#define FINISH true

hw_timer_t *timer[3];
hw_timer_t * timer0 = NULL;
hw_timer_t * timer1 = NULL;
portMUX_TYPE  lock0 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE  lock1 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE  lock2 = portMUX_INITIALIZER_UNLOCKED;

bool  Flip=false;
bool don_fix=false,don_fix1=false,don_fix2=false;
/*
 HS_L[]={17,23,18};
 LS_L[]={27,26,33};

 HS_R[]={13,16,19};
 LS_R[]={14,25,32}; 
*/

void ARDUINO_ISR_ATTR timerCB0(){
portENTER_CRITICAL_ISR(&lock0);
  if(Param.MSG[0]==RAMP_UP){
                          if(      Param.current_direction[0]==FORWARD  )  { if(!digitalRead(Param.LS_L[0])) {digitalWrite(Param.LS_L[0],HIGH);  }}///ets_printf("F%dH",Param.LS_L[0]);
                          else if( Param.current_direction[0]==BACKWARD  ) { if(!digitalRead(Param.LS_R[0])) {digitalWrite(Param.LS_R[0],HIGH);}}  ///ets_printf("B%dH",Param.LS_R[0]);

                          if (Param.current_duty[0] <Param.des_duty[0]) {
                                                                          if(Param.current_direction[0]==FORWARD) ledcWrite(Param.PWM_R[0], Param.current_duty[0]);
                                                                          else                                    ledcWrite(Param.PWM_L[0], Param.current_duty[0]);
                                                                          Param.current_duty[0]++; 
                                                                          
                                                                          } 
                          if (Param.current_duty[0]==Param.des_duty[0]) { 
                                                                        timerStop(timer[0]); 
                                                                        
                                                                        Param.information["pwm_0"]=Param.current_duty[0]; 
                                                                        Param.process[0]=FINISH;
                                                                       
                                                                        }
                        }
  else                {  
                        if(Param.current_duty[0] >Param.des_duty[0]) {
                                                                      if(Param.current_direction[0]==FORWARD) ledcWrite(Param.PWM_R[0], Param.current_duty[0]);
                                                                      else                                    ledcWrite(Param.PWM_L[0], Param.current_duty[0]);
                                                                      Param.current_duty[0]--; 
                                                                      }
                        if(Param.current_duty[0]==Param.des_duty[0]) {don_fix=true; Param.information["pwm_0"]=Param.current_duty[0];}
                        if(don_fix){
                                                  if( Param.current_direction[0]==FORWARD  )    {digitalWrite(Param.LS_L[0],LOW); } //ets_printf("F%dL",Param.LS_L[0]);
                                                  else                                          {digitalWrite(Param.LS_R[0],LOW); } //ets_printf("B%dL",Param.LS_R[0]);
                                                  timerStop(timer[0]);
                                                  
                                                  don_fix=false;
                                                  Param.process[0]=FINISH;
                                                 
                                                  }
                        }
  portEXIT_CRITICAL_ISR(&lock0);              
}

void ARDUINO_ISR_ATTR timerCB1(){
portENTER_CRITICAL_ISR(&lock1);
  if(Param.MSG[1]==RAMP_UP){
                          if(      Param.current_direction[1]==FORWARD  )  { if(!digitalRead(Param.LS_L[1])) {digitalWrite(Param.LS_L[1],HIGH);  }}///ets_printf("F%dH",Param.LS_L[0]);
                          else if( Param.current_direction[1]==BACKWARD  ) { if(!digitalRead(Param.LS_R[1])) {digitalWrite(Param.LS_R[1],HIGH);}}  ///ets_printf("B%dH",Param.LS_R[0]);

                          if (Param.current_duty[1] <Param.des_duty[1]) {
                                                                          if(Param.current_direction[1]==FORWARD) ledcWrite(Param.PWM_R[1], Param.current_duty[1]);
                                                                          else                                    ledcWrite(Param.PWM_L[1], Param.current_duty[1]);
                                                                          Param.current_duty[1]++; 
                                                                          
                                                                          } 
                          if (Param.current_duty[1]==Param.des_duty[1]) { 
                                                                        timerStop(timer[1]); 
                                                                        
                                                                        Param.information["pwm_1"]=Param.current_duty[1]; 
                                                                        Param.process[1]=FINISH;
                                                                       
                                                                        }
                        }
  else                {  
                        if(Param.current_duty[1] >Param.des_duty[1]) {
                                                                      if(Param.current_direction[1]==FORWARD) ledcWrite(Param.PWM_R[1], Param.current_duty[1]);
                                                                      else                                    ledcWrite(Param.PWM_L[1], Param.current_duty[1]);
                                                                      Param.current_duty[1]--; 
                                                                      }
                        if(Param.current_duty[1]==Param.des_duty[1]) {don_fix1=true; Param.information["pwm_1"]=Param.current_duty[1];}
                        if(don_fix1){
                                                  if( Param.current_direction[1]==FORWARD  )    {digitalWrite(Param.LS_L[1],LOW); } //ets_printf("F%dL",Param.LS_L[0]);
                                                  else                                          {digitalWrite(Param.LS_R[1],LOW); } //ets_printf("B%dL",Param.LS_R[0]);
                                                  timerStop(timer[1]);
                                                  
                                                  don_fix1=false;
                                                  Param.process[1]=FINISH;
                                                 
                                                  }
                        }
  portEXIT_CRITICAL_ISR(&lock1);              
}

void ARDUINO_ISR_ATTR timerCB2(){
portENTER_CRITICAL_ISR(&lock2);
  if(Param.MSG[2]==RAMP_UP){
                          if(      Param.current_direction[2]==FORWARD  )  { if(!digitalRead(33)) {digitalWrite(33,HIGH);  }}///ets_printf("F%dH",Param.LS_L[0]);
                          else if( Param.current_direction[2]==BACKWARD  ) { if(!digitalRead(32)) {digitalWrite(32,HIGH);  }}  ///ets_printf("B%dH",Param.LS_R[0]);

                          if (Param.current_duty[2] <Param.des_duty[2]) {
                                                                          if(Param.current_direction[2]==FORWARD) {ledcWrite(Param.PWM_R[2], Param.current_duty[2]);  }
                                                                          else                                    {ledcWrite(Param.PWM_L[2], Param.current_duty[2]);  }
                                                                          Param.current_duty[2]++; 
                                                                          
                                                                          } 
                          if (Param.current_duty[2]==Param.des_duty[2]) { 
                                                                        timerStop(timer[2]); 
                                                                        
                                                                        Param.information["pwm_2"]=Param.current_duty[2]; 
                                                                        Param.process[2]=FINISH;
                                                                       
                                                                        }
                        }
  else                {  
                        if(Param.current_duty[2] >Param.des_duty[2]) {
                                                                      if(Param.current_direction[2]==FORWARD) {ledcWrite(Param.PWM_R[2], Param.current_duty[2]);  }
                                                                      else                                    {ledcWrite(Param.PWM_L[2], Param.current_duty[2]);  }
                                                                      Param.current_duty[2]--; 
                                                                      }
                        if(Param.current_duty[2]==Param.des_duty[2]) {don_fix2=true; Param.information["pwm_2"]=Param.current_duty[2];}
                        if(don_fix2){
                                                  if( Param.current_direction[2]==FORWARD  )    {digitalWrite(33,LOW);   } //ets_printf("F%dL",Param.LS_L[0]);
                                                  else                                          {digitalWrite(32,LOW); } //ets_printf("B%dL",Param.LS_R[0]);
                                                  timerStop(timer[2]);
                                                  
                                                  don_fix2=false;
                                                  Param.process[2]=FINISH;
                                                 
                                                  }
                        }
  portEXIT_CRITICAL_ISR(&lock2);              
}

typedef void (*irqCallback)  (void);
               irqCallback timerCBArray[] ={  timerCB0 ,timerCB1,timerCB2};

uint8_t  Motor::motorNumber=0;
uint8_t Motor::channel=0;

//.                setup(17,         13,        14,        27         );
void Motor::setup(uint8_t hl,uint8_t hr,uint8_t ll,uint8_t lr,uint8_t n, uint32_t t) 
{
    HS_L=hl;
    HS_R=hr;
    LS_L=ll;
    LS_R=lr;
    transiant=t;
    dt=transiant/255;
    String s="channal_";
    ledcSetup(channel, PWM_Freq, PWM_Res);  Param.information[s+String(channel)]=channel ; ets_printf("pin %d HS_L connect channel %d\n",HS_L,channel);
    Param.this_pwm_ch[channel]=true; 
    ledcAttachPin(HS_L,channel++);
    ledcSetup(channel, PWM_Freq, PWM_Res);  Param.information[s+String(channel)]=channel ; ets_printf("pin %d HS_R connect channel %d\n",HS_R,channel);
    Param.this_pwm_ch[channel]=true; ledcAttachPin(HS_R,channel++);
    pinMode(LS_L,OUTPUT);digitalWrite(LS_L,LOW);
    pinMode(LS_R,OUTPUT);digitalWrite(LS_R,LOW);
    
    Param.information["dt"]=dt;
    //N=motorNumber;
    N=n;
    timer[N]=timerBegin(N, 80, true);  
    timerAttachInterrupt(timer[N], timerCBArray[N], true);
     timerAlarmWrite(timer[N], 40000, true);
     timerAlarmEnable(timer[N]);
     timerStop(timer[N]);
    Param.process[N]=FINISH;
    Param.new_job[N]=false;
    ets_printf("Param.process[%d] %d, Param.new_job[%d] %d",N,Param.process[N],N,Param.new_job[N]);
    ets_printf("pin(%d)%d  output:%d pwm(%d)%d   ",LS_L, Param.LS_L[N],digitalRead(Param.LS_L[N]), HS_R,Param.HS_R[N]    );
    ets_printf("pin(%d)%d  output:%d pwm(%d)%d\n ",LS_R, Param.LS_R[N],digitalRead(Param.LS_R[N]), HS_L,Param.HS_L[N]);
    
    motorNumber++;
    Param.information["Fan Num"]=motorNumber;
    String jsn; serializeJson(Param.information, jsn); //PRINTLN(jsn);
}
Motor:: Motor(){
     timerMux 			= portMUX_INITIALIZER_UNLOCKED;
    
     emergency_button=false;
     th_turnOn =NULL;
     th_turnOff=NULL;
     Param.new_job[motorNumber]=false;
}
Motor::~Motor(){}
void Motor::set_transiant(uint8_t t){
    
transiant=t;
dt=transiant/100;
Param.information["dt"]=dt;
 

}
void Motor::changeDirection(void ){

 

  
}
void Motor::setDirection(int8_t dir ){

 
 
PRINTLN("direction set");
}
void Motor::loop(void){
//PRINTF("%d %d %d \n",N,Param.new_job[N],Param.pre_com[N]);

if(Param.new_job[N]){  PRINTF("new job %d \n",N);
                    if(Param.current_direction[N]!=Param.des_direction[N]){ PRINTLN("change direction"); // need 1 direction, 2 RAMP? 3 des_duty
                                                                              Param.MSG[N]=RAMP_DOWN; //turn the motor off 1
                                                                              Param.save_des_duty[N]= Param.des_duty[N];
                                                                              Param.des_duty[N]=0; //2
                                                                              //3 Param.current_direction no change
                                                                              Param.new_job[N]=false;
                                                                              Param.pre_com[N]=true; 
                                                                              if(Param.process[N]==FINISH)  {  
                                                                                PRINTF("timer %d Start reduce to 0",N); 
                                                                                 Param.process[N]=false;
                                                                                  timerStart(timer[N]);    }                                                                            
                                                                            }
                    else                                                    {
                                                                            Param.new_job[N]=false;  
                                                                            PRINTLN("go normal");  
                                                                            if(Param.process[N]==FINISH)  { 
                                                                              PRINTLN("timerStart for new job"); 
                                                                              Param.process[N]=false; 
                                                                              timerStart(timer[N]);    } 
                                                                              }                                                       
                  }

if(Param.pre_com[N])  {  PRINTF("Pre com %d Param.process %d \n",N,Param.process[N]);
                          Delay(2000);
                          if(Param.process[N]==FINISH){
                                                      // PRINTLN("pre-com");
                                                        Param.pre_com[N]=false; 
                                                        Param.new_job[N]=true;
                                                        Param.MSG[N]=RAMP_UP; //1
                                                        Param.des_duty[N]=Param.save_des_duty[N] ; //2
                                                        Param.current_direction[N]=Param.des_direction[N]; //3
                                                      }
                     }                             
}


void setup_0(void){

String s="channal_";
    ledcSetup(Param.PWM_L[0], PWM_Freq, PWM_Res); Param.information[s+String(Param.PWM_L[0])]=Param.PWM_L[0] ; Param.this_pwm_ch[0]=true;     ledcAttachPin(Param.HS_L[0],Param.PWM_L[0]); 
    ledcSetup(Param.PWM_R[0], PWM_Freq, PWM_Res); Param.information[s+String(Param.PWM_R[0])]=Param.PWM_R[0] ; Param.this_pwm_ch[1]=true;     ledcAttachPin(Param.HS_R[0],Param.PWM_R[0]);
    pinMode(Param.LS_L[0],OUTPUT);digitalWrite(Param.LS_L[0],LOW);
    pinMode(Param.LS_R[0],OUTPUT);digitalWrite(Param.LS_R[0],LOW);
    timer[0]=timerBegin(0, 80, true);  
    timerAttachInterrupt(timer[0], timerCBArray[0], true);
         timerAlarmWrite(timer[0], 40000, true);
        timerAlarmEnable(timer[0]);
               timerStop(timer[0]);
    Param.process[0]=FINISH;
    Param.new_job[0]=false;
    ets_printf("Param.process[%d] %d, Param.new_job[%d] %d",0,Param.process[0],0,Param.new_job[0]);
    ets_printf("pin %d  output:%d pwm %d   ", Param.LS_L[0],digitalRead(Param.LS_L[0]),Param.HS_R[0] );
    ets_printf("pin %d  output:%d pwm %d\n ", Param.LS_R[0],digitalRead(Param.LS_R[0]), Param.HS_L[0]);

  
}
void setup_1(void){

String s="channal_";
    ledcSetup(Param.PWM_L[1], PWM_Freq, PWM_Res); Param.information[s+String(Param.PWM_L[1])]=Param.PWM_L[1] ; Param.this_pwm_ch[2]=true;     ledcAttachPin(Param.HS_L[1],Param.PWM_L[1]); 
    ledcSetup(Param.PWM_R[1], PWM_Freq, PWM_Res); Param.information[s+String(Param.PWM_R[1])]=Param.PWM_R[1] ; Param.this_pwm_ch[3]=true;     ledcAttachPin(Param.HS_R[1],Param.PWM_R[1]);
    pinMode(Param.LS_L[1],OUTPUT);digitalWrite(Param.LS_L[1],LOW);
    pinMode(Param.LS_R[1],OUTPUT);digitalWrite(Param.LS_R[1],LOW);
    timer[1]=timerBegin(3, 80, true);  
    timerAttachInterrupt(timer[1], timerCBArray[1], true);
         timerAlarmWrite(timer[1], 40000, true);
        timerAlarmEnable(timer[1]);
               timerStop(timer[1]);
    Param.process[1]=FINISH;
    Param.new_job[1]=false;
    ets_printf("Param.process[%d] %d, Param.new_job[%d] %d",1,Param.process[1],1,Param.new_job[1]);
    ets_printf("pin %d  output:%d pwm %d   ", Param.LS_L[1],digitalRead(Param.LS_L[1]),Param.HS_R[1] );
    ets_printf("pin %d  output:%d pwm %d\n ", Param.LS_R[1],digitalRead(Param.LS_R[1]), Param.HS_L[1]);

  
}
void setup_2(void){
 
String s="channal_";
    ledcSetup(Param.PWM_L[2], PWM_Freq, PWM_Res); Param.information[s+String(Param.PWM_L[2])]=Param.PWM_L[2] ; Param.this_pwm_ch[2]=true;     ledcAttachPin(19,Param.PWM_L[2]);  
    ledcSetup(Param.PWM_R[2], PWM_Freq, PWM_Res); Param.information[s+String(Param.PWM_R[2])]=Param.PWM_R[2] ; Param.this_pwm_ch[2]=true;     ledcAttachPin(18,Param.PWM_R[2]);  
    pinMode(32,OUTPUT);digitalWrite(32,LOW);
    pinMode(33,OUTPUT);digitalWrite(33,LOW);
    
    timer[2]=timerBegin(2, 80, true);  
    
    timerAttachInterrupt(timer[2], timerCBArray[2], true);
         timerAlarmWrite(timer[2], 40000, true);
        timerAlarmEnable(timer[2]);
               timerStop(timer[2]);
    Param.process[2]=FINISH;
    Param.new_job[2]=false;
    ets_printf("Param.process[%d] %d, Param.new_job[%d] %d",2,Param.process[2],2,Param.new_job[2]);
    ets_printf("pin %d  output:%d pwm %d   ", Param.LS_L[2],digitalRead(Param.LS_L[2]),Param.HS_R[2] );
    ets_printf("pin %d  output:%d pwm %d\n ", Param.LS_R[2],digitalRead(Param.LS_R[2]), Param.HS_L[2]);

  
}


void loop_0(void){


if(Param.new_job[0]){  PRINTF("new job %d \n",0);
                    if(Param.current_direction[0]!=Param.des_direction[0]){ PRINTLN("change direction"); // need 1 direction, 2 RAMP? 3 des_duty
                                                                              Param.MSG[0]=RAMP_DOWN; //turn the motor off 1
                                                                              Param.save_des_duty[0]= Param.des_duty[0];
                                                                              Param.des_duty[0]=0; //2
                                                                              //3 Param.current_direction no change
                                                                              Param.new_job[0]=false;
                                                                              Param.pre_com[0]=true; 
                                                                              if(Param.process[0]==FINISH)  {  
                                                                                PRINTF("timer %d Start reduce to 0",0); 
                                                                                 Param.process[0]=false;
                                                                                  timerStart(timer[0]);    }                                                                            
                                                                            }
                    else                                                    {
                                                                            Param.new_job[0]=false;  
                                                                            PRINTLN("go normal");  
                                                                            if(Param.process[0]==FINISH)  { 
                                                                              PRINTLN("timerStart for new job"); 
                                                                              Param.process[0]=false; 
                                                                              timerStart(timer[0]);    } 
                                                                              }                                                       
                  }

if(Param.pre_com[0])  {  PRINTF("Pre com %d Param.process %d \n",0,Param.process[0]);
                          Delay(2000);
                          if(Param.process[0]==FINISH){
                                                      // PRINTLN("pre-com");
                                                        Param.pre_com[0]=false; 
                                                        Param.new_job[0]=true;
                                                        Param.MSG[0]=RAMP_UP; //1
                                                        Param.des_duty[0]=Param.save_des_duty[0] ; //2
                                                        Param.current_direction[0]=Param.des_direction[0]; //3
                                                      }
                     }                             
}
void loop_1(void){


if(Param.new_job[1]){  PRINTF("new job %d \n",1);
                    if(Param.current_direction[1]!=Param.des_direction[1]){ PRINTLN("change direction"); // need 1 direction, 2 RAMP? 3 des_duty
                                                                              Param.MSG[1]=RAMP_DOWN; //turn the motor off 1
                                                                              Param.save_des_duty[1]= Param.des_duty[1];
                                                                              Param.des_duty[1]=0; //2
                                                                              //3 Param.current_direction no change
                                                                              Param.new_job[1]=false;
                                                                              Param.pre_com[1]=true; 
                                                                              if(Param.process[1]==FINISH)  {  
                                                                                PRINTF("timer %d Start reduce to 0",0); 
                                                                                 Param.process[1]=false;
                                                                                  timerStart(timer[1]);    }                                                                            
                                                                            }
                    else                                                    {
                                                                            Param.new_job[1]=false;  
                                                                            PRINTLN("go normal");  
                                                                            if(Param.process[1]==FINISH)  { 
                                                                              PRINTLN("timerStart for new job"); 
                                                                              Param.process[1]=false; 
                                                                              timerStart(timer[1]);    } 
                                                                              }                                                       
                  }

if(Param.pre_com[1])  {  PRINTF("Pre com %d Param.process %d \n",1,Param.process[1]);
                          Delay(2000);
                          if(Param.process[1]==FINISH){
                                                      // PRINTLN("pre-com");
                                                        Param.pre_com[1]=false; 
                                                        Param.new_job[1]=true;
                                                        Param.MSG[1]=RAMP_UP; //1
                                                        Param.des_duty[1]=Param.save_des_duty[1] ; //2
                                                        Param.current_direction[1]=Param.des_direction[1]; //3
                                                      }
                     }                             
}
void loop_2(void){


if(Param.new_job[2]){  PRINTF("new job %d \n",2);
                    if(Param.current_direction[2]!=Param.des_direction[2]){ PRINTLN("change direction"); // need 1 direction, 2 RAMP? 3 des_duty
                                                                              Param.MSG[2]=RAMP_DOWN; //turn the motor off 1
                                                                              Param.save_des_duty[2]= Param.des_duty[2];
                                                                              Param.des_duty[2]=0; //2
                                                                              //3 Param.current_direction no change
                                                                              Param.new_job[2]=false;
                                                                              Param.pre_com[2]=true; 
                                                                              if(Param.process[2]==FINISH)  {  
                                                                                PRINTF("timer %d Start reduce to ZERO\n",2); 
                                                                                 Param.process[2]=false;
                                                                                  timerStart(timer[2]);    }                                                                            
                                                                            }
                    else                                                    {
                                                                            Param.new_job[2]=false;  
                                                                            PRINTLN("go normal");  
                                                                            if(Param.process[2]==FINISH)  { 
                                                                              PRINTLN("timerStart2 for new job"); 
                                                                              Param.process[2]=false; 
                                                                              timerStart(timer[2]);    } 
                                                                              }                                                       
                  }

if(Param.pre_com[2])  {  PRINTF("Pre com %d Param.process %d \n",2,Param.process[2]);
                          Delay(2000);
                          if(Param.process[2]==FINISH){
                                                      // PRINTLN("pre-com");
                                                        Param.pre_com[2]=false; 
                                                        Param.new_job[2]=true;
                                                        Param.MSG[2]=RAMP_UP; //1
                                                        Param.des_duty[2]=Param.save_des_duty[2] ; //2
                                                        Param.current_direction[2]=Param.des_direction[2]; //3
                                                      }
                     }                             
}

void ARDUINO_ISR_ATTR timerCB2P(){
portENTER_CRITICAL_ISR(&lock2);
  if(Param.MSG[2]==RAMP_UP){
                          if(      Param.current_direction[2]==FORWARD  )  { if(!digitalRead(Param.LS_L[2])) {digitalWrite(Param.LS_L[2],HIGH); ets_printf("F%dH\n",Param.LS_L[2]); }}///ets_printf("F%dH",Param.LS_L[0]);
                          else if( Param.current_direction[2]==BACKWARD  ) { if(!digitalRead(Param.LS_R[2])) {digitalWrite(Param.LS_R[2],HIGH); ets_printf("B%dH\n",Param.LS_R[2]); }}  ///ets_printf("B%dH",Param.LS_R[0]);

                          if (Param.current_duty[2] <Param.des_duty[2]) {
                                                                          if(Param.current_direction[2]==FORWARD) {ledcWrite(Param.PWM_R[2], Param.current_duty[2]); ets_printf("+%dF%d\n",Param.HS_R[2],Param.current_duty[2]);}
                                                                          else                                    {ledcWrite(Param.PWM_L[2], Param.current_duty[2]); ets_printf("+%dB%d\n",Param.HS_L[2],Param.current_duty[2]);}
                                                                          Param.current_duty[2]++; 
                                                                          
                                                                          } 
                          if (Param.current_duty[2]==Param.des_duty[2]) { 
                                                                        timerStop(timer[2]); 
                                                                        
                                                                        Param.information["pwm_2"]=Param.current_duty[2]; 
                                                                        Param.process[2]=FINISH;
                                                                       
                                                                        }
                        }
  else                {  
                        if(Param.current_duty[2] >Param.des_duty[2]) {
                                                                      if(Param.current_direction[2]==FORWARD) {ledcWrite(Param.PWM_R[2], Param.current_duty[2]); ets_printf("-%dF%d\n",Param.HS_R[2],Param.current_duty[2]);}
                                                                      else                                    {ledcWrite(Param.PWM_L[2], Param.current_duty[2]); ets_printf("-%dB%d\n",Param.HS_L[2],Param.current_duty[2]);}
                                                                      Param.current_duty[2]--; 
                                                                      }
                        if(Param.current_duty[2]==Param.des_duty[2]) {don_fix2=true; Param.information["pwm_2"]=Param.current_duty[2];}
                        if(don_fix2){
                                                  if( Param.current_direction[2]==FORWARD  )    {digitalWrite(Param.LS_L[2],LOW); ets_printf("F%dL\n",Param.LS_L[2]); } //ets_printf("F%dL",Param.LS_L[0]);
                                                  else                                          {digitalWrite(Param.LS_R[2],LOW); ets_printf("B%dL\n",Param.LS_R[2]);} //ets_printf("B%dL",Param.LS_R[0]);
                                                  timerStop(timer[2]);
                                                  
                                                  don_fix2=false;
                                                  Param.process[2]=FINISH;
                                                 
                                                  }
                        }
  portEXIT_CRITICAL_ISR(&lock2);              
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STEP_PWM_Res   4
portMUX_TYPE  locksT = portMUX_INITIALIZER_UNLOCKED;
stepper::stepper(){}
stepper::~stepper(){}

void stepper::setup(uint8_t _pin,uint8_t chan,uint32_t fre) 
{
pin=_pin;
channal=chan;
frequency=fre;
PRINTF("set pin  %d fre %d channel %d\n",pin,frequency,channal);
    pinMode(pin,GPIO_MODE_INPUT_OUTPUT); //
  ledcSetup(channal, frequency, STEP_PWM_Res); 
 ledcAttachPin(pin,channal);
 running=false;
}



void stepper::set_speed(uint32_t s){
 portENTER_CRITICAL_ISR(&locksT);
 frequency=s;
 //Param.information["speed_"+String(channal)]=frequency;
 //PRINTF("frequency(%d) %d \n",channal,frequency);
  ledcSetup(channal, frequency, STEP_PWM_Res); 
  portEXIT_CRITICAL_ISR(&locksT); 
  Delay(5);
}


void  stepper::stop(void){
 // PRINTF("stop c %d on pin %d\n",channal,pin);
portENTER_CRITICAL_ISR(&locksT);
 
 //Param.information["speed_"+String(channal)]=0;
  ledcWrite(channal,0);
  ledcDetachPin(pin);
  DISABLE_STEPPER;
  running=false;
 portEXIT_CRITICAL_ISR(&locksT); 
  Delay(5); //PRINTLN("Done");
}

void  stepper::start(void){
portENTER_CRITICAL_ISR(&locksT);
   // DISABLE_STEPPER;
  //  Param.information["speed_"+String(channal)]=frequency;
    ledcAttachPin(pin,channal);
    ledcSetup(channal, frequency, STEP_PWM_Res); 
    ledcWrite(channal,8);
    // PRINTF("start f=%d on pin %d\n",frequency,pin);
    ENABLE_STEPPER;
    running=true;
 portEXIT_CRITICAL_ISR(&locksT); 
 Delay(5); //PRINTLN("done");
}

 bool stepper::isRunning(){return running;} 