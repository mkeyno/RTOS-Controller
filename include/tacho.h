#ifndef _TACHO_H
#define _TACHO_H

#include "driver/pcnt.h"

class frequency
{
private:
    /* data */
public:
    frequency(/* args */);
    ~frequency();




static volatile unsigned long  duration[8],last_measurment[8];
static volatile uint8_t       notZero[8] ;
uint8_t unit_number;
unsigned long  last_time,period=100;
void setup(uint8_t pin ,uint8_t uni,int16_t max_c,uint8_t chanel,uint8_t mode);
void measure(void);



};


static volatile double pulseTime_0, lastTime_0,pulse_wide_0;
static volatile uint8_t rpm1 = 0;
static volatile double pulseTime_1, lastTime_1,pulse_wide_1;
static volatile uint8_t rpm0=0;
//static uint64_t tacho_last_measurment;
//static bool flipper=false;

class tachometer
{
private:
     uint8_t pin0, pin1;
     uint64_t last_measurment_interval,last_measurment;
     bool flipper;
public:
    tachometer( );
    ~tachometer();
     void initTacho(const uint8_t ,const uint8_t);
     void measure(void);

};

void flow_sensors_measure(void);


#endif