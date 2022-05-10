# Objective

This documents comprised all information regards to M&M feedback data and setting parameters through Wi-Fi & CAN message 

# Scope of work

This documents consist of all information for setting up and calibration the two sensor data coming from a pair of proximity sensors    

## Introduction 
	Firmware version 2.4 uploaded at   repository with name of RTOS-controler
Code information for modifying come as below 
•	Software algorithms 
Firmware use pulse counter (PCNT) to reading the 4 flow sensor (razing edge), then time duration converted to frequency and multiplying with scale factor, then every 350 ms each side of sensor date transmitted via CAN message, and if Wi-Fi is enable, then both data send to webpage as JSON message 
use INA219 lib for reading the current sensor, 

•	Wi-Fi features 
Wi-Fi features can be enable and disable both by web interface and CAN message 
Wi-Fi can be enable in two of Access Point & Connection Mode 
In AP mode the SSID is 
"OXIN_M&M_" + ESP_ChipId(HEX)+PROGRAM_VERSION;

And defaults password is “12345678”
In AP mode IP address of webpage is 192.168.4.1
In Connection Mode IP address of webpage is 192.168.1.178 or http://oxin.local/
In both Wi-Fi modes defaults username and password is “admin”
https is NOT enable for webpage interface
Three links available for webpage interface as follow
1.	main_IP_address/
2.	main_IP_address/config
3.	main_IP_address/update
