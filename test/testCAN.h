#ifndef TEST_CAN_H
#define TEST_CAN_H

#include "driver/periph_ctrl.h"
#include <esp_task_wdt.h> //for watchdog
#include "driver/gpio.h"
#include "driver/twai.h"
#include "Preferences.h" 

#define ZEYVIER           5100 
#define MAIN_PUMP_ID      20030
#define NOZZLE_LEFT_ID    20031
#define NOZZLE_RIGHT_ID   20032
#define DOSSING_LEFT_ID   20033
#define DOSSING_RIGHT_ID  20034
#define SEQUENCE_START    1000000

byte MSG1[]={0x31, 0x250, 0x33, 0x34, 0x0, 0x0, 0x0, 0x0 };
byte MSG2[]={0x31, 0x250, 0x33, 0x34, 0x255, 0x255, 0x255, 0x255 };
uint64_t last_time_can;
twai_message_t messageIn,messageOut;
bool CAN_init(void);
bool CAN_period(uint64_t period);
bool CAN_HeartBeat(twai_message_t message);
uint8_t CAN_resive(twai_message_t message);
bool CAN_send(uint64_t period);
bool CAN_AKL(twai_message_t message);


#endif // CIRCULAR_BUFFER_H
