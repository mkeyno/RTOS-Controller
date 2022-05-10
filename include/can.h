#ifndef _CAN_H
#define _CAN_H


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
#define Delay(ms) vTaskDelay(ms / portTICK_PERIOD_MS);

#include "driver/periph_ctrl.h"
#include <esp_task_wdt.h> //for watchdog
#include "driver/gpio.h"
#include "driver/twai.h"
#include "Preferences.h"

 



#define SET_DEVICE_ROLE   1
#define SET_SERIAL_NUMBER   2
#define SET_SCALE_FACTOR   3
#define SET_WIFI            5
#define SET_WIFI_MODE       6
#define SET_SERVER_UPDATE 7
#define RESTART 8
#define MOWER  0
#define MULCHER  1



#define CAN_SECURITY  1234

//Implement Controller CAN message format
typedef  union {
  struct {
    uint16_t imp_serial_number; //serial number of this specific implement
    uint8_t telemetry_parameter_number; //telem FROM controller
    uint8_t instruction_number; //instruction number TO controller
    float parameter_quantity;
  };
  uint8_t msg_data[8];
} frame_out_t;


//Defoliator Head speed CAN message format
typedef union {
  struct {
    uint16_t left_defoliator_head_speed; //left motor speed 0-1000
    uint16_t right_defoliator_head_speed; //right motor speed 0-1000
    uint8_t msg_byte4;
    uint8_t msg_byte5;
    uint8_t msg_byte6;
    uint8_t msg_byte7;
  };
  uint8_t msg_data[8];
} defoliator_frame_t;//xavier_defoliator_head_speed_msg;



 
class SM_CAN {
  private:
     
	    frame_out_t 	can_telemetry_frame_out;
	twai_message_t		messageOutRPM,messageIn;											
	  
     
    
    bool        flipper; 
    uint16_t    TWAI_send_failed; 
    
	  uint8_t fanCounter;
	 
  
	
  public:
  bool SW_RESET_BOOL = false;  
  unsigned long  last_time_web,last_time_can,period_web=400,period_can=250;
    SM_CAN();
    ~SM_CAN(); // Destructor to clean up when class instance killed
  int begin (bool task=false);
  void     resetTWAI(void);
	bool stream(size_t filesize,uint8_t *p);
 	int sendRPM(uint8_t side, double rpm) ;
	void parse_CAN(twai_message_t message);
  void parse_1CAN(twai_message_t message);
  void onReceive(twai_message_t message);
  void check_CAN(void);
  void set_enablae_update(uint32_t	 value);
  uint32_t remote_server_update(void);

  uint16_t can_fail_num(void);
  void send_web(void);
  void senStatus(void);
  int send_pre( float rpm);
  bool sendImpFormat(uint32_t const _identifier,uint16_t _imp_serial_number, uint8_t _telemetry_parameter_number, uint8_t _instruction_number,  float _parameter_quantity);
};

extern SM_CAN CAN;
#endif