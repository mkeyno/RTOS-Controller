#ifndef _PARAM_h
#define _PARAM_h

#ifndef BUILTIN_LED
 #define BUILTIN_LED  2
#endif

#include <ArduinoJson.h>
#include "driver/twai.h"
#define Get(x) Param.get_##()

class PARAM
{
private:
 


public:
    PARAM(/* args */);
    ~PARAM();


    static     char *s_wifi_mode[2];//={"Acess Point","Connect to machine wifi"};
    static     char *Role[2];//={"MOWER","MULCHER"};
	static     char *en_dis[2];//={"Disable","Enable"};
    static uint32_t IDS[2];//={IMPLEMENT_MOWER_TELEM_CAN_ID,IMPLEMENT_MULCHER_CAN_ID};


     uint32_t this_wifi_mode,this_device_type,this_device_serial,this_wifi_enabale,this_last_connect,this_CAN_ID,this_enable_update;
     String this_wifi_ssid,this_wifi_pass,this_user_name,this_user_pass;
     float this_scale_factor;

     String MyIP,MyMAC,MyServer;
     uint32_t  MainWSclinet;
     bool  client_active ;

     uint32_t this_CAN_start,this_CAN_send, this_CAN_SSR;

    uint16_t period_tacho,PCNT_scope;
    float frequency[8],old_frequency[8],this_rpm_0,this_rpm_1;

    float this_current,this_pressure,old_pressure,this_reg_a,this_reg_b; 

    uint8_t CAN_data[8];
    bool this_pwm_ch[8]={0};


    bool MSG[3],current_direction[3],des_direction[3],new_job[3],pre_com[3];
    volatile bool process[3];
    volatile uint8_t current_duty[3] ;

    volatile unsigned long  duration[8],last_measurment[8];
      volatile uint8_t        notZero[8] ;

     uint8_t des_duty[3],save_des_duty[3] ;
     static  uint8_t HS_L[3],HS_R[3],LS_L[3],LS_R[3],PWM_R[3],PWM_L[3];
    
    uint8_t num_step_motor,num_nozzel;
    uint32_t steper_speed_left,steper_speed_right,this_max_f_L,this_max_f_R,currenst_f_l,currenst_f_r,target_f_l,target_f_r;
    float this_dos_left_cal,this_dos_right_cal,this_pre_cal;
    bool acceleration_left,acceleration_right,main_pump_active,done_halfing_l,done_halfing_r;
    unsigned long last_heart_bit;
    bool status=true;
    
    StaticJsonDocument<1024> information; //DeserializationError error = deserializeJson(doc, file)

//////////////////////////////////////////////////////////////////////
    uint32_t   read_d(const char *name);
    String     read_s(String name);
    float      read_f(const char *name);
    void       save_d(const char *name,uint32_t value);
    void       save_s(String name,String value);
    void       save_f(const char *name,float	 value)  ;
   
   void restor_param(void);
      
};

extern PARAM Param;



#endif