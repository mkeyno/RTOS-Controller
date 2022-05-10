#ifndef _MOTOR_H
#define _MOTOR_H

#include "Arduino.h"
#include <driver/timer.h>


typedef struct highside{
uint8_t HS_L,HS_R;

}highside_t;

class Motor
{
private:
    uint8_t HS_L,HS_R,LS_L,LS_R;
    uint32_t transiant,dt;
    uint8_t N;
    
    highside_t hs;
    bool emergency_button;

    static uint8_t channel;
    static uint8_t motorNumber;
    portMUX_TYPE timerMux ;
    

public:
 
      Motor(/* args */);
    ~Motor();
    
    void setup(uint8_t hl,uint8_t hr,uint8_t ll,uint8_t lr,uint8_t n, uint32_t t=4000);
    void set_transiant(uint8_t t);
    bool ramp_up(uint8_t direction);
    bool ramp_down(uint8_t direction);
    void test(void );
    void turnOn(void );
    void turnOff(void );
    void setDirection(int8_t dir );
    void changeDirection(void );
    void loop(void);
    TaskHandle_t *th_turnOn, *th_turnOff;

};

void loop_0(void);
void setup_0(void);

void loop_1(void);
void setup_1(void);

void loop_2(void);
void setup_2(void);

class stepper
{
private:
    /* data */
    
    uint8_t pin,channal;
    hw_timer_t *timer=NULL;
   char task_name[12];
   uint16_t speed;
   uint32_t frequency;
   bool running;
public:


    stepper();
    ~stepper();
    
    void setup(uint8_t _pin,uint8_t chan,uint32_t fre) ;

    void start(void);
    void stop(void);
    void set_speed(uint32_t s);
    void period(void);
    void periodT(void);
    void setupT(uint8_t);
    bool isRunning();
};



#endif