#include "Arduino.h"
#include "cnst.h"
#include "param.h"
#include "Preferences.h" 

Preferences Flash;
#define SMP "SMP"
//#define Qot(x) #x
//#define P_SET(s,v) PRINT("Set "); PRINT(Qot(s)); PRINTS(" to:",v);
#define P_GET(s,v) PRINT("This "); PRINT(#s); PRINTS(" value:",v);
//#define SAVE(s,v,t) PARAM::save_##t(Qot(s),v) ;     Param.this_##s=v;            

#define READd(s)        this_##s = read_d(#s) ;    P_GET(s,this_##s);
#define READs(s)        this_##s = read_s(#s) ;    P_GET(s,this_##s);
#define READf(s)        this_##s = read_f(#s) ;    P_GET(s,this_##s);       
#define PUT_JASON(s)    information[#s ]=this_##s ;

 

#define IMPLEMENT_MULCHER_CAN_ID          20100
#define IMPLEMENT_MOWER_TELEM_CAN_ID      20100

PARAM::PARAM(/* args */)
{
 ///period_tacho=1000;
 //PCNT_scope=1000;  in tacho frequency

}

PARAM::~PARAM()
{
}

 uint8_t PARAM::HS_L[]={17,23,18};
 uint8_t PARAM::LS_R[]={27,26,32};

 uint8_t PARAM::HS_R[]={13,16,19};
 uint8_t PARAM::LS_L[]={14,25,33}; 
 
 
 uint8_t PARAM::PWM_R[]={0,2,4};
 uint8_t PARAM::PWM_L[]={1,3,5};

char* PARAM::s_wifi_mode[] = {"Acess Point","Connect to machine wifi"}; 
char* PARAM::Role[]        ={"MOWER","MULCHER"};
char* PARAM::en_dis[]      ={"Disable","Enable"};
uint32_t  PARAM::IDS[]     ={IMPLEMENT_MOWER_TELEM_CAN_ID,IMPLEMENT_MULCHER_CAN_ID};

uint32_t  PARAM::read_d(const char *name){

  Flash.begin(SMP, false); 
  uint32_t n = Flash.getUInt(name, 0);
  Flash.end();
 // PRINTF("This %s value =%d",name,n);
  return n;
  }   
void      PARAM::save_d(const char *name,uint32_t value){

  Flash.begin(SMP, false); 
  Flash.putUInt(name, value);
  Flash.end();
 // PRINTF("Set %s value to %d",name,value); 
  }
String    PARAM::read_s(String name){
	
  Flash.begin(SMP, false); 
  String ss= Flash.getString(name.c_str(), "NON");  
  Flash.end();
  return ss ;
	
} 
void      PARAM::save_s(String name,String value){
	
	Flash.begin(SMP, false); 
    Flash.putString(name.c_str(), value.c_str());
    Flash.end();
	 
}
float     PARAM::read_f(const char *name){
  Flash.begin(SMP, false); 
  float n = Flash.getFloat(name, 1);
  Flash.end();
  return n;
	
}
void      PARAM::save_f(const char *name,float	 value){
	if(value<=0) value=1;
	Flash.begin(SMP, false); 
    Flash.putFloat(name, value);
    Flash.end();
}


void  PARAM::restor_param(void){

 
    PRINTLN("Restor_param");
    READd(wifi_mode); PUT_JASON(wifi_mode);
    READd(device_type);PUT_JASON(device_type);
    READd(device_serial);PUT_JASON(device_serial);
    READd(wifi_enabale);PUT_JASON(wifi_enabale);
    READd(last_connect);PUT_JASON(last_connect);
    //READd(CAN_ID);PUT_JASON(CAN_ID);
    READf(dos_left_cal);PUT_JASON(dos_left_cal);
    READf(dos_right_cal);PUT_JASON(dos_right_cal);
    READd(max_f_L);PUT_JASON(max_f_L);
    READd(max_f_R);PUT_JASON(max_f_R);

    READd(CAN_SSR);PUT_JASON(CAN_SSR);
    READd(enable_update);PUT_JASON(enable_update);
    READf(pre_cal);PUT_JASON(pre_cal);
    
    READs(wifi_ssid);PUT_JASON(wifi_ssid);
    READs(wifi_pass);PUT_JASON(wifi_pass);
    READs(user_name);PUT_JASON(user_name);
    READs(user_pass);PUT_JASON(user_pass);
    
    this_CAN_send=0;PUT_JASON(CAN_send);
    this_CAN_start=0;PUT_JASON(CAN_start);

    serializeJsonPretty(information, Serial);

}


