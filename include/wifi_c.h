#pragma once
#ifndef _WIFI_CONF_H
#define _WIFI_CONF_H

#include <Arduino.h>
//#include <AsyncWebSocket.h>
class AsyncWebServer;
class AsyncWebServerRequest;
class AsyncWebSocket;
class AsyncWebSocketClient;
 
void parse_webSocket(AsyncWebSocketClient * client, String income,uint32_t num);
//void  webSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *payload, size_t length);
static String USERS[5],PASS[5],LoginUser="UnKnown";
static byte  IsAdmin[5];
static bool shouldReboote;   

static bool ACCESS,ADMIN;

void  start_wifi(void);
void server_handler(void);
String dynamicPage(const String& var);


void updateSerial(String ser, AsyncWebServerRequest *request);
void updateScaleFactor(String ser, AsyncWebServerRequest *request);
void setCADid(String ser, AsyncWebServerRequest *request);
void SetWifiMode(String ser, AsyncWebServerRequest *request);
void setType(String ser, AsyncWebServerRequest *request);


void motorControl(String des_val,AsyncWebServerRequest *request);
void dosing(String side, String value,AsyncWebServerRequest *request);
void seletron(String num,String state,AsyncWebServerRequest *request);
void setPulses(String p,AsyncWebServerRequest *request);
void run_step(String s,AsyncWebServerRequest *request);


void handleProcess(AsyncWebServerRequest *request);
void show_user_list(AsyncWebServerRequest *request,String path,String _ID,String parseFunction);
void AddUser(AsyncWebServerRequest *request,String user,String pass,String scop);
void RemoveUser(AsyncWebServerRequest *request,byte num);
void Createfile(String path, String filename,AsyncWebServerRequest *request);
void removefile(String path, String filename,AsyncWebServerRequest *request);
void RenameFile(String patch,String oldfilename,String newfilename,AsyncWebServerRequest *request);
void setRemoteAddress(String ser, AsyncWebServerRequest *request);
void updateWifiStatus(AsyncWebServerRequest *request);
void set_Wifi_pass(String net,String pass,AsyncWebServerRequest *request);
void CheckForUpdate(AsyncWebServerRequest *request);
void show_list_file(AsyncWebServerRequest *request);
void show_file_content(String patch, String filename,AsyncWebServerRequest *request);
void ScanNetwork(AsyncWebServerRequest *request);
void handleConfig(AsyncWebServerRequest *request);
bool check_Auth(AsyncWebServerRequest *request);
void all_information(AsyncWebServerRequest *request);
String getContentType(String filename, AsyncWebServerRequest *request);
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
bool handleFileRead(String path, AsyncWebServerRequest *request);
void server_handler(void);




#endif