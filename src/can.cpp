#include <can.h>
#include "cnst.h"
#include "param.h"
#include "wifi_c.h"
#include <AsyncWebSocket.h>
extern AsyncWebSocket WS;

#include "motor.h"
extern stepper D_left,D_right;

extern PARAM Param;


#define Qot(x) #x
#define P_SET(s,v) PRINT("Set "); PRINT(#s); PRINTS(" to:",v);
#define P_GET(s,v) PRINT("This "); PRINT(#s); PRINTS(" value:",v);
#define SAVE(s,v,t) Param.save_##t(#s,v) ;     Param.this_##s=v;         P_SET(s,v)    ;         

#define READ_d(s)   Param.this_##s = Param.read_d(Qot(s)) ;   
#define READ_s(s)   Param.this_##s = Param.read_s(Qot(s)) ;  
#define READ_f(s)   Param.this_##s = Param.read_f(Qot(s)) ;   

#define P_JASON(s,v) Param.this_##s=v;   Param.information[#s ]=Param.this_##s ;


// Constructor
#define ZEYVIER           5100 
#define MAIN_PUMP_ID      20030
#define NOZZLE_LEFT_ID    20031
#define NOZZLE_RIGHT_ID   20032
#define DOSSING_LEFT_ID   20033
#define DOSSING_RIGHT_ID  20034
#define SEQUENCE_START    1000000

#define VALIDITY1   34
#define VALIDITY2   149

portMUX_TYPE  locksweb = portMUX_INITIALIZER_UNLOCKED;

union {
        float f;
        uint8_t d[4];
    } f_con;


SM_CAN::SM_CAN (){ 
  
 
}
SM_CAN::~SM_CAN () { // Destructor
  twai_stop();
  twai_driver_uninstall();
  Delay(200);
}

int SM_CAN::begin (bool ontask){
  //Initialize configuration structures using macro initializers
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_4, GPIO_NUM_5, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config  = TWAI_TIMING_CONFIG_250KBITS();
  twai_filter_config_t f_config  = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  //twai_filter_config_t f_config = {.acceptance_code = (0x100 << 21),.acceptance_mask = ~(0x7FF << 21),.single_filter = true};
  //Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) PRINTLN("Driver installed");
  else  {                                                             PRINTLN("Failed to install driver");      return 0;  }
  //Start TWAI driver
  if (twai_start() == ESP_OK) {P_JASON(CAN_start,1); PRINTLN("Driver started");}
  else  {                      P_JASON(CAN_start,0); PRINTLN("Failed to start driver");    return 0;  }
  
  PRINTLN("CAN started");


  flipper = true; 
  TWAI_send_failed = 0; 
  
  return 1;
}

void SM_CAN::resetTWAI(void){
  twai_stop();
  twai_driver_uninstall();
  Delay(200);
  PRINTLN("\t\t Resetting CANBUS module.");
  SW_RESET_BOOL = true;
  //periph_module_reset(PERIPH_CAN_MODULE);
  begin();
  PRINTLN("CAN reset");
  TWAI_send_failed = 0;
  P_JASON(CAN_start,0);  
}
// Add a value to the array

int SM_CAN::sendRPM(uint8_t side, double rpm) {
String can_s="";
														  can_telemetry_frame_out.imp_serial_number  = Param.this_device_serial;    //0,1                          
														  can_telemetry_frame_out.telemetry_parameter_number = side;          //2
														  can_telemetry_frame_out.instruction_number = 0;///INSTRUCTION_def;//3
														  can_telemetry_frame_out.parameter_quantity = (float)rpm;  //4,5,6,7
							               memcpy(messageOutRPM.data,   can_telemetry_frame_out.msg_data, 8);
                             //memcpy(Param.CAN_data,       can_telemetry_frame_out.msg_data, 8);
                                    messageOutRPM.identifier = Param.this_CAN_ID;
 /// if (Param.this_CAN_ID > 2048)    
   messageOutRPM.extd = 1;
 /// else              				        messageOutRPM.extd = 0;  
									                  messageOutRPM.data_length_code = 8;
                                           can_s=String(messageOutRPM.identifier)+"," +
                                                 String(messageOutRPM.data[0])+"," +
                                                 String(messageOutRPM.data[1])+"," +
                                                 String(messageOutRPM.data[2])+"," +
                                                 String(messageOutRPM.data[3])+"," +
                                                 String(messageOutRPM.data[4])+"," +
                                                 String(messageOutRPM.data[5])+"," +
                                                 String(messageOutRPM.data[6])+"," +
                                                 String(messageOutRPM.data[7])
                                                 
                                                 ;
                                    Param.information["CAN_OUT" ]=can_s ; 
									
        if (twai_transmit(&messageOutRPM, pdMS_TO_TICKS(1)) == ESP_OK) {  P_JASON(CAN_send,1); PRINTLN(can_s); return 1;}//Queue message for transmission
				else  { 
               TWAI_send_failed++;  if( TWAI_send_failed >= 10){ resetTWAI(); }  
               P_JASON(CAN_send,0);
               //PRINTS("Failed ",can_s); 
               return 0;
               }
   
    
  
}

void SM_CAN::parse_CAN(twai_message_t message)
{

    //PRINTLN("new can");
    Param.status=RUNNING;
    Param.last_heart_bit=millis();
    Param.information["RUNNING"]=true;

    uint16_t imp_serial_number        = word(message.data[1], message.data[0]);         //serial number of this specific implement
    uint8_t telemetry_parameter_number=message.data[2]; //telem FROM controller
    uint8_t instruction_number        =message.data[3];         //instruction number TO controller memcpy(&f, &b, sizeof(f));

    uint8_t bytes[4]={ message.data[4], message.data[5], message.data[6], message.data[7] };
    float   floatV; memcpy(&floatV, bytes, sizeof bytes); //floatV=constrain(floatV, -100, 100);
    uint32_t intV=(uint32_t)floatV;
    uint32_t byteV; memcpy(&byteV , bytes, sizeof bytes);
    uint32_t id = message.identifier;
    uint8_t command=message.data[3];
    
    switch (id){

        case MAIN_PUMP_ID:      PRINTF("MAIN_PUMP_ID d(%d)\n",intV);     intV=map(intV,0,1000,0,255); if(intV>0) Param.main_pump_active=true; else Param.main_pump_active=false; ledcWrite(0, intV);  Param.information["main_pump"]=intV; break;
        case NOZZLE_LEFT_ID:    PRINTF("NOZZLE_LEFT_ID d(%d)\n",intV);   SELETRON_LEFT(intV) if(intV>0) Param.num_nozzel++; if(intV==0 & Param.num_nozzel>0)  Param.num_nozzel--; break;
        case NOZZLE_RIGHT_ID:   PRINTF("NOZZLE_RIGHT_ID d(%d)\n",intV);  SELETRON_RIGHT(intV) if(intV>0) Param.num_nozzel++; if(intV==0 & Param.num_nozzel>0)  Param.num_nozzel--; break;
        case DOSSING_LEFT_ID:   PRINTF("DOSSING_LEFT_ID d(%d)\n",intV);  if(intV<=1)  {Param.acceleration_left=false; D_left.stop();   Param.num_step_motor--;}  else { Param.num_step_motor++; 
            
                Param.done_halfing_l=true;
                Param.target_f_l=1.5*intV*Param.this_dos_left_cal/10;
                if(Param.target_f_l>Param.this_max_f_L) Param.target_f_l=Param.this_max_f_L;
            if (Param.target_f_l>450) {
                                      Param.acceleration_left=true;
                                      Param.currenst_f_l=500;
                                      }
            else          {D_left.set_speed(Param.target_f_l); Delay(5); D_left.start();}
           
            }  break;
        case DOSSING_RIGHT_ID:  PRINTF("DOSSING_RIGHT_ID d(%d)\n",intV); if(intV<=1)  {Param.acceleration_right=false; D_right.stop();  Param.num_step_motor--;}  else { Param.num_step_motor++; 

                 Param.done_halfing_r=true;
                 Param.target_f_r=1.5*intV*Param.this_dos_right_cal/10;
                if(Param.target_f_r>Param.this_max_f_R) Param.target_f_r=Param.this_max_f_R;
            if (Param.target_f_r>450) {
                                      Param.acceleration_right=true;
                                      Param.currenst_f_r=500;
                                      }
            else          {D_right.set_speed(Param.target_f_r); Delay(5); D_right.start();}
        
        
        } break;  
        case ZEYVIER: break;
    }
    Param.information["total motor:"]=Param.num_step_motor;
    
     /*
     switch (command)
					  {                       
						 case SET_DEVICE_ROLE: 	  SAVE(device_type,value%=2,d);												break;//1    8 210 4 0 (1&0) 0 0 0 0        
						 case SET_SERIAL_NUMBER:  SAVE(device_serial,value,d);													break; //2    8  210 4 0 2 1 2 3 4
						 case SET_SCALE_FACTOR:   SAVE(scale_factor,(float)value,f);      	break; //4   8  210 4 0 3 4 0 0 0
				     case SET_WIFI: 	   	if( value^Param.this_wifi_enabale)    { SAVE(wifi_enabale,value,d); ESP.restart();} 	break; //5 value=0 8  210 4 0 5 (0&1) 0 0 0
						 case SET_WIFI_MODE: 	if( value^Param.this_wifi_mode)    	  { SAVE(wifi_mode   ,value,d); ESP.restart();} 	break;  //6
             case SET_SERVER_UPDATE: 	SAVE(	enable_update   ,value,d); break;  //7
             case RESTART:            ESP.restart(); 	break;//8
						// default: 	PRINTLN("Not Recognise command");													break;

					  } 
  */

}

void SM_CAN::onReceive(twai_message_t message){
 // PRINTLN("new can");
  uint32_t id=message.identifier;
  if(id==MAIN_PUMP_ID || id==NOZZLE_LEFT_ID ||id==NOZZLE_RIGHT_ID ||id==DOSSING_LEFT_ID ||id==DOSSING_RIGHT_ID || id==ZEYVIER)   parse_CAN(message);  
}

void SM_CAN::check_CAN(){
  
  if (twai_receive(&messageIn, pdMS_TO_TICKS(1)) == ESP_OK) { onReceive(messageIn); }
   
}

uint16_t SM_CAN::can_fail_num(){

  return TWAI_send_failed;
}

void SM_CAN::set_enablae_update(uint32_t	 value){
//if(enable_update)
SW_RESET_BOOL=value;
}

uint32_t SM_CAN::remote_server_update(void){
  PRINTLN("remote_server_update");
  return 1;
}

void SM_CAN::send_web(void){
if (period_web <  millis()-last_time_web) {// every 400 ms
         
  String jsn;
 portENTER_CRITICAL_ISR(&locksweb);   
serializeJson(Param.information, jsn);  
portEXIT_CRITICAL_ISR(&locksweb);             
  if(Param.client_active>0)     {
                                    WS.text(Param.MainWSclinet,jsn); 

                                    WS.clearQ(Param.MainWSclinet);} 

 last_time_web = millis();
}

}

void SM_CAN::senStatus(void){if (period_can <  millis()-last_time_can) {

uint8_t n=(fanCounter++)%3;
float rpm;
if(Param.current_direction[n]>0)  rpm= 100*Param.current_duty[n]/255;
else                              rpm=-100*Param.current_duty[n]/255;
String can_s="";
														  can_telemetry_frame_out.imp_serial_number  = Param.this_device_serial;    //0,1                          
														  can_telemetry_frame_out.telemetry_parameter_number = n;          //2
														  can_telemetry_frame_out.instruction_number = 0;///INSTRUCTION_def;//3
														  can_telemetry_frame_out.parameter_quantity = (float)rpm;  //4,5,6,7
							               memcpy(messageOutRPM.data,   can_telemetry_frame_out.msg_data, 8);
                             //memcpy(Param.CAN_data,       can_telemetry_frame_out.msg_data, 8);
                                    messageOutRPM.identifier =20030;
                                     messageOutRPM.extd = 1;
 // if (Param.this_CAN_ID > 2048)     messageOutRPM.extd = 1;
 // else              				        messageOutRPM.extd = 0;  
									                  messageOutRPM.data_length_code = 8;
                                           can_s=String(messageOutRPM.identifier)+"," +
                                                 String(messageOutRPM.data[0])+"," +
                                                 String(messageOutRPM.data[1])+"," +
                                                 String(messageOutRPM.data[2])+"," +
                                                 String(messageOutRPM.data[3])+"," +
                                                 String(messageOutRPM.data[4])+"," +
                                                 String(messageOutRPM.data[5])+"," +
                                                 String(messageOutRPM.data[6])+"," +
                                                 String(messageOutRPM.data[7])+"," +
                                                 rpm
                                                 ;
                                    Param.information["CAN_OUT" ]=can_s ; 
									
        if (twai_transmit(&messageOutRPM, pdMS_TO_TICKS(1)) == ESP_OK) {  P_JASON(CAN_send,1);  }// PRINTLN(can_s); Queue message for transmission
				else  { 
               TWAI_send_failed++;  if( TWAI_send_failed >= 10){ resetTWAI(); }  
               P_JASON(CAN_send,0);
               //PRINTS("Failed ",can_s); 
               
               }





 last_time_can = millis();
}}

bool SM_CAN::sendImpFormat(uint32_t _identifier,uint16_t _imp_serial_number, uint8_t _telemetry_parameter_number, uint8_t _instruction_number,  float _parameter_quantity){

  twai_message_t out;
  frame_out_t msg;
  msg.imp_serial_number                 = _imp_serial_number;                          
  msg.telemetry_parameter_number        = _telemetry_parameter_number;          
  msg.instruction_number                = _instruction_number; 
  msg.parameter_quantity                = _parameter_quantity;  
  memcpy(out.data,   msg.msg_data, 8);
 
                                    out.identifier =_identifier;
                                     out.extd = 1;
  //if (Param.this_CAN_ID > 20)     out.extd = 1;
 // else              				        out.extd = 0;  
									                  out.data_length_code = 8;

 String can_s="";
        can_s=String(out.identifier)+"," +
              String(out.data[0])+"," +
              String(out.data[1])+"," +
              String(out.data[2])+"," +
              String(out.data[3])+"," +
              String(out.data[4])+"," +
              String(out.data[5])+"," +
              String(out.data[6])+"," +
              String(out.data[7])
                      ;
Param.information["CAN_OUT" ]=can_s ; 
//Serial.print(out.identifier,HEX);Serial.print("-");Serial.print(out.data[0],HEX);Serial.print(out.data[1],HEX);Serial.print(out.data[2],HEX);Serial.print(out.data[3],HEX);Serial.print(out.data[4],HEX);Serial.print(out.data[5],HEX);Serial.print(out.data[6],HEX);Serial.println(out.data[7],HEX);

if (twai_transmit(&out, pdMS_TO_TICKS(1)) == ESP_OK) {  P_JASON(CAN_send,1); return true;}//Queue message for transmission
else  { 
        TWAI_send_failed++;  if( TWAI_send_failed >= 10){ resetTWAI(); }  
        P_JASON(CAN_send,0);
        //PRINTS("Failed ",can_s); 
        return false;
        }
}

bool SM_CAN::stream(size_t filesize,uint8_t *p){
uint32_t identifer=SEQUENCE_START;


twai_message_t message;
 while(filesize<8) {
                    memcpy(message.data, (p+8), 8);
                    message.identifier=identifer;                 
                    message.data_length_code=8;           
                    if( twai_transmit(&messageOutRPM, pdMS_TO_TICKS(1)) == ESP_OK){
                                                                                     p+=8;
                                                                                    identifer++; 
                                                                                    filesize-=8;
                                                                                    }
                    }
 if(filesize>0)   {
                    memcpy(message.data, (p+filesize), filesize);
                    message.identifier=identifer;                 
                    message.data_length_code=filesize;           
                    if( twai_transmit(&messageOutRPM, pdMS_TO_TICKS(1)) == ESP_OK) return true;
                    }  
   return true;                                
}


int SM_CAN::send_pre( float rpm) {
 
														  can_telemetry_frame_out.imp_serial_number  = 0;    //0,1                          
														  can_telemetry_frame_out.telemetry_parameter_number = 3;          //2
														  can_telemetry_frame_out.instruction_number = 0;///INSTRUCTION_def;//3
														  can_telemetry_frame_out.parameter_quantity = (float)rpm;  //4,5,6,7
							               memcpy(messageOutRPM.data,   can_telemetry_frame_out.msg_data, 8);
                            
                                    messageOutRPM.identifier = 20030 ;
 // if (Param.this_CAN_ID > 20)       
  messageOutRPM.extd = 1;
 // else              				        messageOutRPM.extd = 0;  
									                  messageOutRPM.data_length_code = 8;
                                     
									
        if (twai_transmit(&messageOutRPM, pdMS_TO_TICKS(1)) == ESP_OK) {  P_JASON(CAN_send,1);  return 1;}//Queue message for transmission
				else  { 
               TWAI_send_failed++;  if( TWAI_send_failed >= 10){ resetTWAI(); }  
               P_JASON(CAN_send,0);
               //PRINTS("Failed ",can_s); 
               return 0;
               }
   
    
  
}

void SM_CAN::parse_1CAN(twai_message_t message){
    Param.status=RUNNING;
    Param.last_heart_bit=millis();
    Param.information["RUNNING"]=true;

    uint8_t validity1=message.data[1];
    uint8_t validity2=message.data[2];
//if(validity1==VALIDITY1 && validity2==VALIDITY2){
    uint8_t sel_L=message.data[7];
    uint8_t sel_R=message.data[6];
    uint8_t Doz_L=message.data[5];
    uint8_t Doz_R=message.data[4];
    uint8_t main_P=message.data[3];
    SELETRON_LEFT(sel_L);
    SELETRON_RIGHT(sel_R);

if(main_P>0) Param.main_pump_active=true; 
else         Param.main_pump_active=false; 
ledcWrite(0, main_P);  Param.information["main_pump"]=main_P;

    uint8_t total_sel=sel_L+sel_R;
    Param.target_f_l=1.5*Doz_L*Param.this_dos_left_cal/10; if(Param.target_f_l>Param.this_max_f_L) Param.target_f_l=Param.this_max_f_L;
    Param.target_f_r=1.5*Doz_R*Param.this_dos_right_cal/10;if(Param.target_f_r>Param.this_max_f_R) Param.target_f_r=Param.this_max_f_R;
    if(total_sel==1) {Param.target_f_l=Param.target_f_l/2;  Param.target_f_r=Param.target_f_r/2; } //dohalfing

   if (total_sel==0) {  // trun off 
                        Param.acceleration_left=false;  D_left.stop(); 
                        Param.acceleration_right=false; D_right.stop();
                      }
    else             {

                    if (Param.target_f_r>450) {Param.acceleration_right=true;  Param.currenst_f_r=450; }
                    if (Param.target_f_l>450) {Param.acceleration_left=true;   Param.currenst_f_l=450; }
                  }  // do normal
//}
}