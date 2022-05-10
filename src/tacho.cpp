


#include "param.h"
#include "can.h"

extern PARAM Param;
extern SM_CAN CAN;

#include "wifi_c.h"
#include <AsyncWebSocket.h>
extern AsyncWebSocket WS;
 

#define Qot(s) #s
#define PUTJASON(s,v) Param.this_rpm_##s=v;   Param.information[Qot(rpm_##s)]=Param.this_rpm_##s ;

#define CHECK(f) f?"Done":"Finish"
#include  "tacho.h"
 
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; 
volatile   bool flip=false;
 

volatile unsigned long  frequency::last_measurment[] = {0};
volatile unsigned long  frequency::duration[] = {0};
volatile uint8_t        frequency::notZero[] = {0};

volatile unsigned long old;

void IRAM_ATTR Intr_handler(void *arg){
portENTER_CRITICAL_ISR(&timerMux);  
digitalWrite(BUILTIN_LED,flip); 
    flip = !flip; 
int unit = (int)arg;
frequency::notZero[unit]=1;
 old=frequency::last_measurment[unit];
     frequency::last_measurment[unit]=micros();
     frequency::duration[unit]=frequency::last_measurment[unit]-old;

portEXIT_CRITICAL_ISR(&timerMux); 
}




frequency::frequency(/* args */)
{
    Param.period_tacho=800;
    Param.PCNT_scope=5;
}

frequency::~frequency()
{
}

void frequency::setup(uint8_t pin ,uint8_t uni,int16_t max_c,uint8_t chanel,uint8_t mode)
{

    unit_number=uni;  
pinMode(pin,INPUT);
pcnt_unit_t unit   =static_cast<pcnt_unit_t>(uni%8);
pcnt_channel_t channel=static_cast<pcnt_channel_t>(chanel%2);
pcnt_count_mode_t pos_do;   
pcnt_count_mode_t neg_do;
if (mode==RISING)
    {
        pos_do=PCNT_COUNT_INC;
        neg_do=PCNT_COUNT_DIS;
    }
else
    {
        pos_do=PCNT_COUNT_DIS;
        neg_do=PCNT_COUNT_INC ;
    }
                                                  // Rotina de inicializacao do pulse count

  pcnt_config_t config = { };                                        // Instancia pulse config
  config.pulse_gpio_num = pin;                       // Port de entrada dos pulsos
  config.ctrl_gpio_num = -1;                       // Controle da contagem
  config.unit = unit;                                     // Unidade de contagem
  config.channel = channel;                               // Canal de contagem
  config.counter_h_lim = max_c;                             // Limite maximo de contagem
  config.pos_mode = pos_do;                                  // Conta na subida do pulso
  config.neg_mode = neg_do;                                  // Conta na descida do pulso
  config.lctrl_mode = PCNT_MODE_KEEP;                             // Nao usado
  config.hctrl_mode = PCNT_MODE_KEEP;                                // Se HIGH conta incrementando
  
  pcnt_unit_config(&config);                                         // Inicializa PCNT
  pcnt_counter_pause(unit);                                    // Inicializa o contador PCNT
  pcnt_counter_clear(unit);                                    // Zera o contador PCNT
  pcnt_event_enable(unit, PCNT_EVT_H_LIM); 
  pcnt_isr_service_install(0);
  pcnt_isr_handler_add(unit, Intr_handler, (void *)unit);
   pcnt_intr_enable(unit);                                      // Habilita interrup de pcnt
  pcnt_counter_resume(unit);                                   // inicia a contagem
}

uint32_t flowids[4]={20031,20032,20033,20034};
uint8_t flowtele[4]={1,2,4,5};
void frequency::measure(){if(millis()-last_time>Param.period_tacho){

        if(notZero[unit_number]){ // if  counter reach to 1000 tick, let me know for how long  
                                    float f=(60000000*Param.PCNT_scope/236.0)/(duration[unit_number]+1);
                                    Param.frequency[unit_number]=f;//*Param.this_scale_factor;
                                    notZero[unit_number]=0;
                                    }
        else Param.frequency[unit_number]=0;  
         
          Param.information["flow_"+String(unit_number)]=Param.frequency[unit_number];
                 
// PRINTF("%d:%f\n",unit_number,Param.frequency[unit_number]);
       if(Param.old_frequency[unit_number]!=Param.frequency[unit_number])         CAN.sendImpFormat(flowids[unit_number],666,flowtele[unit_number],0,Param.frequency[unit_number]);

       Param.old_frequency[unit_number]=Param.frequency[unit_number];

       last_time = millis();
 }}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 void ICACHE_RAM_ATTR isr_pulse_1(){
   rpm1++;
  lastTime_1 = pulseTime_1;
  pulseTime_1 = micros();
  pulse_wide_1 = (pulseTime_1 - lastTime_1); 

}

void ICACHE_RAM_ATTR isr_pulse_0(){
    
  rpm0++;
  lastTime_0 = pulseTime_0;
  pulseTime_0 = micros();
  pulse_wide_0 = (pulseTime_0 - lastTime_0);
}


tachometer::tachometer( )
{
  last_measurment_interval=200; 
  last_measurment = 0; 
  flipper=false;
}

tachometer::~tachometer()
{
}


void tachometer::initTacho(const uint8_t _pin0,const uint8_t _pin1) {

 // pinMode(PULSE_IN_0_PIN, INPUT);
 // pinMode(PULSE_IN_1_PIN, INPUT);
    pin0=_pin0;
    pin1=_pin1;
  pinMode(pin0, INPUT);  digitalWrite(pin0, HIGH);  attachInterrupt(digitalPinToInterrupt(pin0), isr_pulse_0, RISING);
  pinMode(pin1, INPUT);  digitalWrite(pin1, HIGH);  attachInterrupt(digitalPinToInterrupt(pin1), isr_pulse_1, RISING);
  
}

void tachometer::measure(){
  
   if (last_measurment_interval <  millis()-last_measurment) 
          {
            
            detachInterrupt(pin0);
            detachInterrupt(pin1);

            
            esp_task_wdt_reset();
             
           
            if(rpm0)  Param.this_rpm_0=60000000/pulse_wide_0/Param.this_scale_factor;          
            if(rpm1)  Param.this_rpm_1= 60000000/pulse_wide_1/Param.this_scale_factor;


 
             if (flipper)    CAN.sendRPM(flipper,Param.this_rpm_1);
             else            CAN.sendRPM(flipper,Param.this_rpm_0);   
             flipper = !flipper;



            last_measurment = millis();
            rpm0=rpm1=0;
            
            attachInterrupt(pin0, isr_pulse_0, RISING);
            attachInterrupt(pin1, isr_pulse_1, RISING); 
          
          }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long last_time_reading_sensor=0;
uint8_t toggeler=0;
uint16_t period_reading_sensor=350;

void flow_sensors_measure(void){

if(millis()-last_time_reading_sensor>period_reading_sensor){
        unsigned long rawFS;
        float flow_value;
        uint8_t n=toggeler%4;
        if(Param.notZero[n]>0)     rawFS=(1000000*Param.PCNT_scope)*Param.this_scale_factor/(Param.duration[n]+1); // if have not readed the have rpm 
        else                              rawFS=0;
        flow_value=Param.this_reg_a*rawFS+Param.this_reg_b;
        Param.information["Flow_"+String(n)]=flow_value;    
                              
        Param.notZero[n]=0; // readed
        CAN.sendRPM(n,flow_value);  
        toggeler++;    
        last_time_reading_sensor = millis();
 }


}