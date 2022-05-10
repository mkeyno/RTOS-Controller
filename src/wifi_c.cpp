#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "AsyncTCP.h"
#include <ArduinoOTA.h> 
#include <HTTPUpdate.h>
#include <ESPmDNS.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "cnst.h"
 
#include "wifi_c.h"
#include "can.h"
#include "util.h"
#include "htm.h"
#include "tacho.h"
 
#include "param.h"
extern PARAM Param;
#include "motor.h"
extern stepper D_left,D_right;

#define Qot(x) #x
#define P_SET(s,v) PRINT("Set "); PRINT(Qot(s)); PRINTS(" to:",v);
#define SAVE(s,v,t) Param.save_##t(#s,v) ;     Param.this_##s=v;         P_SET(s,v) ;   
#define P_JASON(s,v) Param.this_##s=v;   Param.information[#s ]=Param.this_##s ;




#define  host   "oxin" 

#define  ssid   "TSMC" //Your new Wi-Fi" //
#define  password   "TsmcTsmc" //200720511050"//"TsmcTsmc" //;
#define  preSetUser   "admin" 
#define  preSetPass   "admin" 


#include "motor.h"
extern Motor Fan[3];

void motor_control(void *pvParameters){
   //  uint8_t i=(uint8_t *) malloc(pvParameters);
     //uint8_t i =static_cast<uint8_t*>(malloc(sizeof(uint8_t)));
     uint8_t i =static_cast<uint8_t*>(pvParameters)[0];
    // int i =static_cast< int>(pvParameters);
    uint8_t num=i/10;
    uint8_t on_off=i%10;
    //Fan[num].setDirection(dir);
    PRINTF("Turn to motor num=%d and set to %s %d\n",num,on_off?"On":"Off");
    if(on_off==1) Fan[num].turnOn();
    else          Fan[num].turnOff();
    PRINTLN("end of task");
   // vTaskDelete(Fan[num].th_turnOn);
}




AsyncWebServer server(80);
AsyncWebSocket WS("/ws");


IPAddress apIP(192, 168, 4, 1);
 

File fsUploadFile;

IPAddress local_IP(192, 168, 1, 178);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

void  webSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *payload, size_t length);

void start_wifi()
{
  Param.client_active =false ;
  uint8_t conect_try=Param.this_last_connect;
  USERS[0]="admin"; 
  PASS[0]="admin";
  if(Param.this_wifi_mode==0){ // acees mode

      PRINTLN("acess point");
      WiFi.mode(WIFI_AP);
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      String temp_name="AP_" +String(ESP_getChipId(), HEX)+PROGRAM_VERSION;
      WiFi.softAP(temp_name.c_str(),"12345678");
      Param.MyServer=temp_name; Param.information["Server" ]=Param.MyServer;
      Param.MyIP=WiFi.softAPIP().toString();Param.information["IP" ]=Param.MyIP;
      Param.MyMAC=WiFi.softAPmacAddress();Param.information["MAC" ]=Param.MyMAC;

      PRINTS("AP IP address: ",Param.MyIP);
      PRINTS("My MAC: ",Param.MyMAC);
      PRINTS("My Server: ",Param.MyServer);

  }
  else {                   // station mode

              WiFi.mode(WIFI_STA);
            // if(STATIC)   
              WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);  
              WiFi.begin(Param.this_wifi_ssid.c_str(), Param.this_wifi_pass.c_str());           
              unsigned long startTime = millis();
              while (WiFi.status() != WL_CONNECTED && millis() - startTime < 7000) {Serial.write('>');delay(500);} 
                  if(WiFi.status() != WL_CONNECTED) {
                                                    conect_try++;     
                                                  if(conect_try>2){  SAVE(wifi_mode,0,d);	 conect_try=0;ESP.restart();}                                   
                                                  PRINTS("new num = ",conect_try);
                                                  SAVE(last_connect,conect_try,d);                                                                                            
                                                  delay(500);
                                                  ESP.restart();
                                                  }            
              Param.MyIP=WiFi.localIP().toString();Param.information["IP" ]=Param.MyIP;
              Param.MyMAC=WiFi.macAddress();Param.information["MAC" ]=Param.MyMAC;
              Param.MyServer=Param.this_wifi_ssid;Param.information["Server" ]=Param.MyServer;
              PRINTS("AP IP address: ",Param.MyIP);
              PRINTS("My MAC: ",Param.MyMAC);
              PRINTS("My Server: ",Param.MyServer);
                if (!MDNS.begin("home")) { //http://home.local
                  PRINTLN("Error setting up MDNS responder!");
                  while (1) {
                    delay(1000);
                  }
                }
                PRINTLN("mDNS responder started");

}
  server_handler();
  WS.onEvent(webSocketEvent);
  server.addHandler(&WS);
  server.begin();

 }


void handleProcess(AsyncWebServerRequest *request){
  
 String command =request->arg(0);
 PRINTLN(command);//request->arg(0));
 /////////////////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>if(!ACCESS) return request->send(404); 
 
      if(command=="CRF") Createfile(request->arg(1),request->arg(2),request) ; //http://192.168.4.1/process?code=RCF&patch=patch&file=file   [-CONFIG-]
 else if(command=="RRF") removefile(request->arg(1),request->arg(2),request) ; //http://192.168.4.1/process?code=RRF&patch=patch&file=file [-CONFIG-]
 else if(command=="RNF") RenameFile(request->arg(1),request->arg(2),request->arg(3),request);//http://192.168.4.1/process?code=RRN&patch=patch& [-CONFIG-]
 else if(command=="DIR") show_list_file(request) ;//http://192.168.4.1/process?code=RDH  CONFIG
 else if(command=="SFC") show_file_content(request->arg(1),request->arg(2),request) ;

//else if(command=="SER")  updateSerial(request->arg(1),request);
 else if(command=="SSF") updateScaleFactor(request->arg(1),request);
 else if(command=="SCI") setCADid(request->arg(1),request);
 else if(command=="TYP") setType(request->arg(1),request);
 else if(command=="RMT") setRemoteAddress(request->arg(1),request);
 else if(command=="RES") {request->send(200, "text/plain", ""); delay(500); ESP.restart();} //[-Standalone-]
 else if(command=="UPD") CheckForUpdate(request);
 else if(command=="WIM") SetWifiMode(request->arg(1),request);
 else if(command=="UWS") updateWifiStatus(request);
 else if(command=="SWP") set_Wifi_pass(request->arg(1),request->arg(2),request);
 else if(command=="INF") all_information(request);

 else if(command=="MPU") motorControl(request->arg(1),request);
 else if(command=="DOS") dosing(request->arg(1),request->arg(2),request);
 else if(command=="SLN") seletron(request->arg(1),request->arg(2),request);
  
 else if(command=="RUN") run_step(request->arg(1),request);

 else if(command=="NET") ScanNetwork(request); //[-Standalone-]
 else if(command=="RST") {request->send(200, "text/plain", ""); delay(500); ESP.restart();} //[-Standalone-]
 else if(command=="USL") show_user_list(request,"/data/user.dat","ListUserListBox","parse_line_and_put_inform");//http://192.168.4.1/process?code=RUL [-Standalone-]
 else if(command=="AUS") AddUser(request,request->arg(1),request->arg(2),request->arg(3));//http://192.168.4.1/process?code=RAU&user=user&pass=pass&scop=scop [-Standalone-]
 else if(command=="RUS") RemoveUser(request,request->arg(1)[0]-'0');//http://192.168.4.1/process?code=RRU&index=index  [-Standalone-]

  }

void show_user_list(AsyncWebServerRequest *request,String path,String _ID,String parseFunction){ 
                     //"/data/user.dat","ListUserListBox","parse_line_and_put_inform"
  String records="<select name=\"Combobox2\" size=\"5\" id=\""+_ID+"\"  onChange=\""+parseFunction+"();\"   >";
  File f = FileFS.open(path, "a+");   ///a+ if not exist then create it 
  if (!f) return request->send(200, "text/plain", records); 
  byte line_num_count=0;
  String line="";
   while(f.available())
   { 
 char c=f.read();              
  if(c=='\n' || c=='\r' )          
                      {  if(line.length() >2)
                                            {
                                             // Serial.print("line="); Serial.print(line);     Serial.print("[number]"); Serial.println(line_num_count);  
                                              line_num_count++; 
                                               records+="<option value=\"" + String(line_num_count)+"\">" +line +"</option>";
                                              line="";  
                                            }        
                      }               
  else              line+=c;  
 } 
   f.close(); 
  
 records+="</select>";
  request->send(200, "text/plain", records); 
}
void AddUser(AsyncWebServerRequest *request,String user,String pass,String scop){
  request->send(404);
  String line=user+ "," + pass + "," + scop;
  //Serial.println(line); 
   File f = FileFS.open("/data/user.dat", "a+");  
  if (!f) return ; 
  f.println(line);
  f.close();
}
void RemoveUser(AsyncWebServerRequest *request,byte num){
  request->send(404);
  String temp="";
  File f = FileFS.open("/data/user.dat", "r");
  byte line_num_count=0;
    String line="";
  while(f.available())
  {//Lets read line by line from the file      
  char c=f.read();              
  if(c=='\n' || c=='\r' )          
                      {if(line.length() >2)
                                            {
                                           //Serial.print("line="); Serial.print(line);     Serial.print("[number]"); Serial.println(line_num_count);    
                                             if(num!=line_num_count)temp=temp+line+"\n\r";              
                                              line_num_count++;
                                             line="";     
                                            }  
                       }
 else              line+=c;                         
} 
  
  f.close();
  f = FileFS.open("/data/user.dat", "w+");
  f.println(temp);
   f.close();
}
void set_Wifi_pass(String net,String pass,AsyncWebServerRequest *request){
request->send(200, "text/plain", "");
SAVE(wifi_ssid,net,s);
SAVE(wifi_pass,pass,s);
 
}
void Createfile(String path, String filename,AsyncWebServerRequest *request){
  PRINTLN(path+filename);
  if(FileFS.exists(path+filename)) { PRINTLN( "FILE EXISTS"); return request->send(200, "text/plain", "FILE EXISTS"); }
  File file = FileFS.open(path+filename, "w");
    if(file) {file.close(); PRINTLN("CREATE "+path+filename); return request->send(200, "text/plain", "CREATE File"); }
  else                  {  PRINTLN("CREATE FAILED");     return request->send(200, "text/plain", "CREATE FAILED");}
}
void removefile(String path, String filename,AsyncWebServerRequest *request){
  PRINTLN(path+filename);
  if(!FileFS.exists(path+filename))    request->send(404, "text/plain", "FileNotFound");  
      FileFS.remove(path+filename);    request->send(200, "text/plain", "removed");
}
void RenameFile(String patch,String oldfilename,String newfilename,AsyncWebServerRequest *request){
 String fold=patch+oldfilename;
 String fnew=patch+newfilename;
if (FileFS.exists(fold))
          {
            FileFS.rename(fold,fnew);
            request->send(200, "text/plain", "Renamed");
          }
  
}

void updateSerial(String ser, AsyncWebServerRequest *request){ 
request->send(200, "text/plain", ser); 
//PRINTS("got serial raw number=",ser);
uint16_t n=ser.toInt();
//PRINTS("got serial number=",n);

SAVE(device_serial,n,d);
Param.information["device_serial"]=Param.this_device_serial;
}
void updateScaleFactor(String ser, AsyncWebServerRequest *request){
request->send(200, "text/plain", "DONE"); 
float n=ser.toFloat();
//PRINTS("got scale factor=",n);
SAVE(scale_factor,n,f);
Param.information["scale_factor"]=Param.this_scale_factor;

}
void setCADid(String ser, AsyncWebServerRequest *request){
uint32_t n=ser.toInt();
//This_scale_factor=n;
//write_nvs("scaleFactor",n);
request->send(200, "text/plain", ""); 
}
void SetWifiMode(String ser, AsyncWebServerRequest *request){
request->send(200, "text/plain", ""); 
uint32_t n=ser.toInt();

SAVE(wifi_mode,n,d);
}
void setType(String ser, AsyncWebServerRequest *request){
request->send(200, "text/plain", ""); 
uint32_t n=ser.toInt();

SAVE(device_type,n%2,d);
Param.this_CAN_ID=PARAM::IDS[n%2];
SAVE(CAN_ID,Param.this_CAN_ID,d);
}
void setRemoteAddress(String ser, AsyncWebServerRequest *request){
//Device_info[Master_IP]=ser;
//write_string("RemoteAddress",ser);
request->send(200, "text/plain", ""); 
}
void CheckForUpdate(AsyncWebServerRequest *request){

//Check_For_Update=true;
 request->send(200, "text/plain", ""); 
}

void all_information(AsyncWebServerRequest *request){
AsyncResponseStream *response = request->beginResponseStream("application/json");

//serializeJsonPretty(Param.information,Serial);
 
 //

//AsyncJsonResponse * response = new AsyncJsonResponse();
//response->addHeader("Content-Type", "application/json");
 
String jsn;
serializeJson(Param.information, jsn);
PRINTLN(jsn);
request->send(200, "text/plain", jsn);

}
void updateWifiStatus(AsyncWebServerRequest *request){


String ssd=Param.this_wifi_ssid;
String m="";
if(Param.this_wifi_mode==1) m="Station";
else                        m="AP";
String sss=Param.MyIP+"%"+ssd+"%"+m; //+CAN.get_wifi_mode().c_str()?"Station":"AP"
 
//PRINTF("%s %s %d",MyIP,CAN.get_wifi_ssid(),CAN.get_wifi_mode());
//Serial.println(sss);
request->send(200, "text/plain", sss); 
}



void show_list_file(AsyncWebServerRequest *request){ ///replace all item of list box
   String records="<select name=\"Combobox1\" size=\"10\" id=\"listfile\"  onChange=\"select_file_path();\"  style=\"position:absolute;left:2px;width:200px;\" >";
   byte i=0;
   File root = FileFS.open("/");
   File file = root.openNextFile();
    while (file) {    
              String fileName = file.name();
              size_t fileSize = file.size();
              records+="<option value=\"" + String(i)+"\">" +fileName +"</option>";
              i++;
              file = root.openNextFile();
            }
  records+="</select>";
 request->send(200, "text/plain", records);   
}
void show_file_content(String patch, String filename,AsyncWebServerRequest *request){ 
request->send(FileFS,patch+filename, String());
}
void ScanNetwork(AsyncWebServerRequest *request){  //this id of listbox defined here
  String records="<select name=\"Combobox2\" size=\"10\" id=\"_netList\"  onChange=\"selectssid_pass();\"   >";
    records+="<option value=\"100\"> Network Name  ,    Network Strength     </option>";
   int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_FAILED)   WiFi.scanNetworks(true);
  else if (n > 0) 
     {
      for (int i = 0; i < n; ++i)records+="<option value=\"" + String(n)+"\">" +WiFi.SSID(i) +" , " + WiFi.RSSI(i)+"</option>";      
    }
  records+="</select>";
  WiFi.scanDelete();
    request->send(200, "text/html", records); 
}  

void seletron(String num,String state,AsyncWebServerRequest *request){
request->send(200, "text/plain", "sure");  
uint32_t side=num.toInt();
uint32_t s=state.toInt();
if(s>0) { if(Param.num_nozzel<2) Param.num_nozzel++;}
else if(Param.num_nozzel>0) Param.num_nozzel--;
PRINTF("%d,%d, num=%d \n",side,s,Param.num_nozzel);
if(side>0) {SELETRON_LEFT(s); }
else      { SELETRON_RIGHT(s);}

}
void motorControl(String des_val,AsyncWebServerRequest *request){
request->send(200, "text/plain", "sure"); 
uint32_t n=des_val.toInt();
//n=map(n,0,255,0,1000);
Param.information["main_pump"]=n;
//PRINTS("speed=",n);
 //ledcWrite(0, n);
}

void dosing(String side, String value,AsyncWebServerRequest *request){
request->send(200, "text/plain", "sure"); 
uint8_t s=side.toInt();
uint32_t v=value.toInt();
//PRINTS("speed=",v);
if(v<11){
        PRINTLN("stop");
      //  if(s==1){ D_left.stop();} //D_left.set_speed(v); 
      //  else    { D_right.stop();}// D_right.set_speed(v);
      }
v=map(v,0,100,3000,400);

//if(s==1){ D_left.set_speed(v); D_left.start();}
//else    { D_right.set_speed(v); D_right.start();}
 
}

 

void run_step(String s,AsyncWebServerRequest *request){

request->send(200, "text/plain", "sure"); 
uint8_t status=s.toInt();
//PRINTS("status=",status);

 if(status==1) D_right.start();
 else          D_right.stop();

}


void motorDirection(String num, AsyncWebServerRequest *request){
request->send(200, "text/plain", "sure"); 
uint32_t n=num.toInt();
 Fan[n].changeDirection();

}

void ConfigureOTA(void) {
  // ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("OTA_SMT");//+preSetUser).c_str());
  ArduinoOTA.setPassword(password);//preSetPass.c_str());  
 // ArduinoOTA.onStart([]() {FS_Name.end(); /* Disable client connections */WS.enable(false);  WS.textAll("OTA Update Started");WS.closeAll(); PRINTLN(F("StartOTA \n"));  });
 // ArduinoOTA.onEnd([]()   {FS_Name.end(); PRINTLN("\nEnd OTA\n");  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { PRINTF("OTA Progress: %u%%\n", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error) {
                                            PRINTF("Error[%u]: ", error);
                                                 if (error == OTA_AUTH_ERROR)    PRINTLN(F ("Auth Failed"));
                                            else if (error == OTA_BEGIN_ERROR)   PRINTLN(F ("Begin Failed"));
                                            else if (error == OTA_CONNECT_ERROR) PRINTLN(F ("Connect Failed"));
                                            else if (error == OTA_RECEIVE_ERROR) PRINTLN(F ("Receive Failed"));
                                            else if (error == OTA_END_ERROR)     PRINTLN(F ("End Failed"));
                                          });
  PRINTLN(F("\nOTA Ready"));
  ArduinoOTA.begin();
}

void handleConfig(AsyncWebServerRequest *request){
  if(!request->authenticate(preSetUser,preSetPass)) return request->requestAuthentication(); 
  ACCESS=ADMIN=true;
  request->send_P(200, "text/html", config_html); //,dynamicPage
}

String dynamicPage(const String& var){
  //Serial.println(var);
       if(var == "MyIP")    return Param.MyIP;  
  else if(var == "Mode")    return "Mode";
  else if(var == "NetWork") return "NetWork";
  return String();
}

bool check_Auth(AsyncWebServerRequest *request){ //if(!check_Auth(request)) request->send(404);  return request->requestAuthentication(); 
 // PRINTLN(F("check_Auth"));
  if(ACCESS || ADMIN) return true;
 for(byte i=0;i<5;i++) if(request->authenticate(USERS[i].c_str(), PASS[i].c_str())){ 
                                      LoginUser=USERS[i];
                                      //TelegramPass=PASS[i]; 
                                      PRINTLN(F("find user"));
                                      ACCESS=true; 
                                      if(IsAdmin[i])ADMIN=true; 
                                      return true;
                                      }
 
  return false; 
}
String getContentType(String filename, AsyncWebServerRequest *request) {
       if (request->hasArg("download")) return "application/octet-stream";
     
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}


void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index) 
  { 
    PRINTF("handleFileUpload Name: %s\n", filename.c_str());
    if (!filename.startsWith("/")) filename = "/" + filename;
    fsUploadFile = FileFS.open(filename, "w");
  }
  if (fsUploadFile)
  {
    if (fsUploadFile.write(data, len) != len) PRINTLN(F("Write error during upload"));
  }
  
  if (final) 
  { 
    if (fsUploadFile)   fsUploadFile.close();     
    PRINTF("handleFileUpload Size: %u\n", len);
  }
}
bool handleFileRead(String path, AsyncWebServerRequest *request) {
 PRINT(F("handleFileRead: ")); PRINTLN(path);
  if (path.endsWith("/"))   path += "index.htm";
  String contentType = getContentType(path, request);  //return "text/html";
  String pathWithGz = path + ".gz";   //pathWithGz=Sindex.html.gz
  if (FileFS.exists(pathWithGz) || FileFS.exists(path))
  {
  //  if (SPIFFS.exists(pathWithGz))    path += ".gz";  //"application/javascript"
    AsyncWebServerResponse *response = request->beginResponse(FileFS, path, contentType);
   // if (path.endsWith(".gz"))      response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    return true;
  }
  PRINTF("Cannot find %s\n", path.c_str()); 
  return false;
} 

void server_handler(void){

server.on("/exit",             [](AsyncWebServerRequest *request){ ACCESS=ADMIN=false; request->send(401, "text/plain", "Thanks\n Exit"); /*request->requestAuthentication(); */  });
server.on("/process", HTTP_GET,[](AsyncWebServerRequest *request){ /*if(Access) */handleProcess(request); });  
server.on("/config",           [](AsyncWebServerRequest *request){ handleConfig(request);  });
server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
              if(!check_Auth(request))   request->requestAuthentication(); 
              request->send(200, "text/html", "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form><div id='prg'>progress: 0%</div>");
              });
server.on("/update", HTTP_POST, [&](AsyncWebServerRequest *request) {
                // the request handler is triggered after the upload has finished... 
                // create the response, add header, and send response
                AsyncWebServerResponse *response = request->beginResponse((Update.hasError())?500:200, "text/plain", (Update.hasError())?"FAIL":"OK");
                response->addHeader("Connection", "close");
                response->addHeader("Access-Control-Allow-Origin", "*");
                request->send(response);
               // restartRequired = true;
            }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
                //Upload handler chunks in data
                if (!index) {
                
                   // content_len = request->contentLength();

                        int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
                        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) { // Start with max available size

                            Update.printError(Serial);   
                        }
                
                }

                // Write chunked data to the free sketch space
                if (Update.write(data, len) != len) {
                    Update.printError(Serial); 
                }
                    
                if (final) { // if the final flag is set then this is the last frame of data
                    if (Update.end(true)) { //true to set the size to the current progress
                     // ESP.restart();
                    }
                ///    
                }
            }); 
server.on("/upload", HTTP_POST,[](AsyncWebServerRequest *request){ request->send(200, "text/plain", ""); }, handleFileUpload);  
server.onFileUpload([](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
if (!index)  PRINTF("UploadStart: %s\n", filename.c_str());
PRINTF("%s", (const char*)data);
if (final)  PRINTF("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
});
server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
if (!index)  PRINTF("BodyStart: %u\n", total);
PRINTF("%s", (const char*)data);
if (index + len == total)  PRINTF("BodyEnd: %u\n", total);
}); 
//server.serveStatic("/", SPIFFS, "/Sindex.html.gz","max-age=86400").setFilter(check_Auth);//.setFilter(FilterStandalone);
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){   
                              if(!check_Auth(request))  request->requestAuthentication();
                               else
                               {    
                                 {///if(STANDALONE) {
                                      if(!handleFileRead("/Sindex.html", request)) request->send(404, "text/plain", "Sindex.html FileNotFound");
                                      }                                                                     
                               }   
                                             
                                });
//server.serveStatic("/jquery.3.4.js", FileFS, "/jquery.3.4.js.gz"); 
server.serveStatic("/images/logo.png", FileFS, "/images/logo.png" ); 

server.onNotFound([](AsyncWebServerRequest *request) {
PRINTF("Not found: %s\r\n", request->url().c_str());
AsyncWebServerResponse *response = request->beginResponse(200);
          response->addHeader("Connection", "close");
          response->addHeader("Access-Control-Allow-Origin", "*");
if (!handleFileRead(request->url(), request))     request->send(404, "text/plain", "FileNotFound");
}); 
}


void parse_webSocket(AsyncWebSocketClient * client, String income,uint32_t num){
     //PRINTLN(income);
     byte eqI1 = income.indexOf('&');
 String cammand = income.substring(0, 3); 
 uint32_t value   = (income.substring(4)).toInt();  
 float fv=(income.substring(4)).toFloat();
PRINTF("cammand=%s value=%d\n",cammand,value);
//Serial.print(cammand);Serial.print("<->");Serial.println(value);
 
      if(cammand=="SGT")  { // [-Standalone-]
 
     String respond= "Web>"+cammand+" send back This";  //RST 
    PRINTLN(respond);                                     
    client->text(respond); 
 } 
 else if(cammand=="VST")  client->text("web> VST  send back this"); 
 //'''''''''''''''''''''''''''''''''''''''''   page setting     ''''''''''''''''''''''''''''''''''''''''''''''''''''''''
// else if(cammand=="UDM")  { UPDATING=true; check_update(num); UPDATING=false; } 
 else if(cammand=="TTS")  {client->text("web> TTS send back this"); }
 
 else if(cammand=="SER")  { SAVE(device_serial,value,d); Param.information["device_serial" ]=Param.this_device_serial ; }                          
  else if(cammand=="RUN")  {
                            
                            if(income[3]=='0'){  PRINTS("left ",Param.acceleration_left);
                                              if(value<=5)  {D_left.stop(); Param.acceleration_left=false;  Param.num_step_motor--;}  
                                              else {  
                                                   Param.target_f_l=value*Param.this_dos_left_cal/10; 
                                                if(Param.target_f_l>Param.this_max_f_L) Param.target_f_l=Param.this_max_f_L;
                                                PRINTS("target F(L)=",Param.target_f_l);
                                                if(Param.target_f_l>450) {
                                                                            Param.acceleration_left=true;
                                                                            Param.currenst_f_l=500;                                                                            
                                                                            }
                                                  else                       {D_left.set_speed(Param.target_f_l); Delay(5); D_left.start();}
                                                 Param.num_step_motor++;
                                                 }  
                                              }

                             else           {PRINTS("right ",Param.acceleration_right);
                                              
                                              if(value<=5)  {D_right.stop();  Param.acceleration_right=false; Param.num_step_motor--;}  
                                              else { 
                                                     Param.target_f_r=value*Param.this_dos_right_cal/10;
                                                  if(Param.target_f_r>Param.this_max_f_R) Param.target_f_r=Param.this_max_f_R;
                                                  PRINTS("target F(R)=",Param.target_f_r);
                                                  if(Param.target_f_r>450) {
                                                                        Param.acceleration_right=true;
                                                                        Param.currenst_f_r=500;
                                                                        }
                                                  else          {D_right.set_speed(Param.target_f_r); Delay(5); D_right.start();}

                                                 Param.num_step_motor++;
                                                 } 
                                             }                 
                            }  

   

  else if(cammand=="CAL")  {
                              if(income[3]=='0') {SAVE(dos_left_cal,fv,f);	}   
                              else               {SAVE(dos_right_cal,fv,f); }     


                            }                                                    
  else if(cammand=="MPU")  {
                              Param.information["main_pump"]=value;
                            //  ledcAttachPin(MAIN_PUMP_PWM,0);
                              ledcWrite(0, value);
                              if(value<5){                                
                               // ledcDetachPin(MAIN_PUMP_PWM);
                               ledcWrite(0, 0);
                              }

                            }  

  else if(cammand=="SFQ")  {
                               if(income[3]=='0'){  
                                              D_left.set_speed(value);  D_left.start();
                                              }

                             else           {
                                              D_right.set_speed(value);  D_right.start();
                                             }   

                            } 
    else if(cammand=="SMF")  {
                             if(income[3]=='0'){  
                                              SAVE(max_f_L,value,d);
                                              }

                             else           {
                                             SAVE(max_f_R,value,d);
                                             }   
                              
                            }                            

  else if(cammand=="CLP")  {
                             
                             //PRINTS("calibration=",fv) ;  
                             SAVE(pre_cal,fv,f);
                              
                            }  
}


void  webSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *payload, size_t length) {
     if (type == WS_EVT_DISCONNECT) {
                    PRINTF("WS URL=%s ID=%u Disconnected: %u\n", server->url(), client->id());
                    Param.client_active=false;
                  //  client->close();
                  /*
                    for (uint8_t i=0; i<MAX_WS_CLIENT ; i++) {
                                        if (ws_client[i].id == client->id() ) {
                                                                                ws_client[i].id = 0;
                                                                                ws_client[i].state = CLIENT_NONE;
                                                                                PRINTF("freed[%d]\n", i);
                                                                                MainWSclinet--;
                                                                                break; // Exit for loop
                                                                            }
                                        }
                    */                    
                   // client->ping();
                }
else if (type == WS_EVT_CONNECT) { 
                PRINTF("\nClient %u added with ip=", client->id()); PRINTLN(client->remoteIP());  
                client->ping();
                Param.MainWSclinet=client->id();
                Param.client_active=true;
                //  PRINTF("MainWSclinet:%d \n",MainWSclinet);
                 // client->keepAlivePeriod(2);
/*
                for (uint8_t index = 0; index < MAX_WS_CLIENT; index++) {
                            if (ws_client[index].id == 0) {
                                                            ws_client[index].id = client->id();
                                                            ws_client[index].state = CLIENT_ACTIVE;
                                                            PRINTF("added #%u at index[%d]\n", client->id(), index);
                                                            MainWSclinet++;
                                                            client->printf("[[b;green;]Hello Client #%u, added you at index %d]", client->id(), index);
                                                            client->ping();
                                                            break; // Exit for loop
                                                        }
                                                  }
                      */

            
                                }
else if (type == WS_EVT_DATA)  {
          AwsFrameInfo * info = (AwsFrameInfo*)arg;
          String msg = "";
          if (info->final && info->index == 0 && info->len == length){//the whole message is in a single frame and we got all of it's data
                              if (info->opcode == WS_TEXT) { for (size_t i = 0; i < info->len; i++) msg += (char)payload[i]; }
                              else { // Binary
                                    char buff[3];
                                    for (size_t i = 0; i < info->len; i++) 
                                                                          {
                                                                            sprintf(buff, "%02x ", (uint8_t)payload[i]);
                                                                            msg += buff;
                                                                          }
                                    }
                             // PRINTF("[[WS]]%s\r\n",msg.c_str());///<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                            }
          else             {//message is comprised of multiple frames or the frame is split into multiple packets
                        if (info->index == 0)                         { // Message start
                        //if (info->num == 0) //PRINTF("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
                          //      PRINTF("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
                        }
                        // Continue message
                       //  PRINTF("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + length);
                        if (info->opcode == WS_TEXT) {for (size_t i = 0; i < info->len; i++)  msg += (char)payload[i];} // Text
                        else { char buff[3];          for (size_t i = 0; i < info->len; i++) { sprintf(buff, "%02x ", (uint8_t)payload[i]); msg += buff; }}// Binary
                       //  PRINTF("%s\r\n", msg.c_str());
                        if ((info->index + length) == info->len) { // Message end
                        
                     //   PRINTF("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
                        if (info->final) 
                        {
                    //      PRINTF("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
                        }
                        }
                      }
          
          parse_webSocket(client, msg,client->id()); 
          msg.~String();  
          }
}







