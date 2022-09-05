
#include "testCAN.h"


bool CAN_init(void){

twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_4, GPIO_NUM_5, TWAI_MODE_NORMAL);
twai_timing_config_t t_config  = TWAI_TIMING_CONFIG_250KBITS();
twai_filter_config_t f_config  = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  //twai_filter_config_t f_config = {.acceptance_code = (0x100 << 21),.acceptance_mask = ~(0x7FF << 21),.single_filter = true};
  //Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) return true;
  else       return false;   

}

bool CAN_period(uint64_t period){ 
  byte count=0;  
    if(period <  millis()-last_time_can) {

if (twai_receive(&messageIn, pdMS_TO_TICKS(1)) == ESP_OK){
     uint32_t id=messageIn.identifier;
  if(id==MAIN_PUMP_ID || id==NOZZLE_LEFT_ID ||id==NOZZLE_RIGHT_ID ||id==DOSSING_LEFT_ID ||id==DOSSING_RIGHT_ID || id==ZEYVIER)  
   count++;

}

last_time_can = millis();
}

if(count>10) return true;
else return false;

}


bool CAN_HeartBeat(twai_message_t message){





}
uint8_t CAN_resive(twai_message_t message){

message.identifier=ZEYVIER;


}
bool CAN_send(uint64_t period){

 byte count=0;  
 if(period <  millis()-last_time_can) {

 memcpy(MSG1,  messageOut.data, 8);
messageOut.identifier=ZEYVIER;
if (twai_transmit(&messageOut, pdMS_TO_TICKS(1)) == ESP_OK) count++;
last_time_can = millis();
}

if(count>10)    return true;
else            return false;


}
bool CAN_AKL(twai_message_t message){






}