

# Scope of work

This  repository consist firmware using RTOS API and ESP32 MCU  for setting  the two sensor data coming from   4 set of flow sensors ,  INA219 current sensor   and calibration of 2 DRV8825 stepper driver

All information about sensor and parameters setting are available in wifi interface as well as CAN message

## Software algorithms 
Firmware use pulse counter (PCNT) to reading the 4 flow sensor (razing edge), then time duration converted to frequency and multiplying with scale factor, then every 200 ms each side of sensor date transmitted via CAN message, and if Wi-Fi is enable, then both data send to webpage as JSON message 
use INA219 lib for reading the current sensor, 

## Wi-Fi features 
Wi-Fi features can be enable and disable both by web interface and CAN message 
Wi-Fi can be enable in two of Access Point & Connection Mode 
In AP mode the SSID is 
"AP_" + ESP_ChipId(HEX)+PROGRAM_VERSION;

And defaults password is “12345678”
In AP mode IP address of webpage is 192.168.4.1
In Connection Mode IP address of webpage is 192.168.1.178 or http://home.local/
In both Wi-Fi modes defaults username and password is “admin”
https is NOT enable for webpage interface
Three links available for webpage interface as follow
1.	main_IP_address/
2.	main_IP_address/config
3.	main_IP_address/update
