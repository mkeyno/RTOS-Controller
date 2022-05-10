#include "util.h"
#include "cnst.h"
#include "can.h"

#include "param.h"

extern PARAM Param;

#define UserData "/data/user.dat"


 utility::utility()
{
      USERS[0]="admin"; 
      PASS[0]="admin";
}
 utility::~utility()
{
}


void utility:: restor_user_pass(void){
   
  File f = FileFS.open(UserData, "r");
 if(!f) 
     {
       f = FileFS.open(UserData, "w+"); f.close();
        PRINTLN(F("\t\t[Open User Pass Failed]"));
        return;
    } 
    int numLine=0;// it was 1 before 
 String line="";
   while(f.available())
   { 
 char c=f.read();              
  if(c=='\n' || c=='\r' )          
                      {  if(line.length() >4)
                                            {
                                              byte commaIndex = line.indexOf(',');
                                              byte secondCommaIndex = line.indexOf(',', commaIndex+1);
                                              USERS[numLine]= line.substring(0, commaIndex); USERS[numLine].trim();
                                              PASS[numLine] = line.substring(  commaIndex+1, secondCommaIndex); PASS[numLine].trim();
                                              String S=line.substring(secondCommaIndex+1);
                                              if(S[0]=='A') IsAdmin[numLine]=1; else IsAdmin[numLine]=0; 
                                               PRINTLN(USERS[numLine] +":"+PASS[numLine]+":"+String(IsAdmin[numLine]));              
                                              numLine++;  
                                            }        
                      }               
  else              line+=c;  
 } 
   f.close(); 


}



void utility::listDir(const char * dirname, uint8_t levels){
    PRINTF("Listing directory: %s\r\n", dirname);

    File root = FileFS.open(dirname);
    if(!root){        PRINTLN("- failed to open directory");        return;    }
    if(!root.isDirectory()){        PRINTLN(" - not a directory");        return;    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            PRINT("  DIR : ");
            PRINTLN(file.name());
            if(levels){
                listDir(file.path(), levels -1);
            }
        } else {
            PRINT("  FILE: ");
            PRINT(file.name());
            PRINT("\tSIZE: ");
            PRINTLN(file.size());
        }
        file = root.openNextFile();
    }
}

void utility::DIR(void){
  PRINTLN("C:\\>dir");
  uint32_t total=0;
    File root = FileFS.open("/");
    File file = root.openNextFile();
    while (file) {    
              String fileName = file.name();
              size_t fileSize = file.size();
                    total+=fileSize;
              PRINTF("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
              file = root.openNextFile();
            }
    PRINT("\t\t\tTotal:");PRINTLN(total);
  
}

void utility::createDir(const char * path){
    PRINTF("Creating Dir: %s\n", path);
    if(FileFS.mkdir(path))        PRINTLN("Dir created");
     else                         PRINTLN("mkdir failed");    
}
void utility::removeDir(const char * path){
    PRINTF("Removing Dir: %s\n", path);
    if(FileFS.rmdir(path))        PRINTLN("Dir removed");
    else                          PRINTLN("rmdir failed");
    
}
void utility::readFile(const char * path){
    PRINTF("Reading file: %s\n", path);

    File file = FileFS.open(path);
    if(!file){
        PRINTLN("Failed to open file for reading");
        return;
    }

    PRINTLN("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}
void utility::writeFile( const char * path, const char * message){
    PRINTF("Writing file: %s\n", path);

    File file = FileFS.open(path, FILE_WRITE);
    if(!file){
        PRINTLN("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        PRINTLN("File written");
    } else {
        PRINTLN("Write failed");
    }
    file.close();
}
void utility::appendFile(const char * path, const char * message){
    PRINTF("Appending to file: %s\n", path);

    File file = FileFS.open(path, FILE_APPEND);
    if(!file){
        PRINTLN("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        PRINTLN("Message appended");
    } else {
        PRINTLN("Append failed");
    }
    file.close();
}
void utility::renameFile(const char * path1, const char * path2){
    PRINTF("Renaming file %s to %s\n", path1, path2);
    if (FileFS.rename(path1, path2)) {
        PRINTLN("File renamed");
    } else {
        PRINTLN("Rename failed");
    }
}
void utility::deleteFile(const char * path){
    PRINTF("Deleting file: %s\n", path);
    if(FileFS.remove(path)){
        PRINTLN("File deleted");
    } else {
        PRINTLN("Delete failed");
    }
}


String utility::read_line(String _path,byte line_num){
File f = FileFS.open(_path, "r");
if (!f) return "" ; 
byte counter=0; 
String line="";
while(f.available())//1AAA@THAT54545646|1$15:
{ 
 line = f.readStringUntil('\n'); 
 line.trim();
                         
 if(line.length() >2 && counter==line_num) break;
 counter++;
}
 f.close();  
 return line;
}


String utility::formatBytes(size_t bytes){
       if (bytes < 1024)                 return String(bytes)+"B"; 
  else if(bytes < (1024 * 1024))         return String(bytes/1024.0)+"KB";
  else if(bytes < (1024 * 1024 * 1024))  return String(bytes/1024.0/1024.0)+"MB";
  else                                   return String(bytes/1024.0/1024.0/1024.0)+"GB";
}

void utility::verbose_print_reset_reason()
{
  
 /*
 typedef enum {
    ESP_RST_UNKNOWN = 0,    //!< Reset reason can not be determined
    ESP_RST_POWERON,        //!< Reset due to power-on event
    ESP_RST_EXT,            //!< Reset by external pin (not applicable for ESP8266)
    ESP_RST_SW,             //!< Software reset via esp_restart
    ESP_RST_PANIC,          //!< Software reset due to exception/panic
    ESP_RST_INT_WDT,        //!< Reset (software or hardware) due to interrupt watchdog
    ESP_RST_TASK_WDT,       //!< Reset due to task watchdog
    ESP_RST_WDT,            //!< Reset due to other watchdogs
    ESP_RST_DEEPSLEEP,      //!< Reset after exiting deep sleep mode
    ESP_RST_BROWNOUT,       //!< Brownout reset (software or hardware)
    ESP_RST_SDIO,           //!< Reset over SDIO
    ESP_RST_FAST_SW,        //!< Fast reboot
} esp_reset_reason_t;
 
 */ 
  esp_reset_reason_t reason = esp_reset_reason();
  
  switch ( reason)
  {
    case 0 : Serial.println (" Reset reason can not be determined");break;           
    case 1 : Serial.println ("Reset due to power-on event");break;          
    case 3 : Serial.println ("Reset by external pin");break;               
    case 4 : Serial.println ("Software reset via esp_restart");break;             
    case 5 : Serial.println ("Software reset due to exception-panic");break;        
    case 6 : Serial.println ("Reset (software or hardware) due to interrupt watchdog");break;             
    case 7 : Serial.println ("Reset due to task watchdog");break;        
    case 8 : Serial.println ("Reset due to other watchdogs");break;       
    case 9 : Serial.println ("Reset after exiting deep sleep mode");break;       
    case 10 : Serial.println ("Brownout reset (software or hardware)");break;        
    case 11 : Serial.println ("Reset over SDIO");break;       
    case 12 : Serial.println ("Fast reboot");break;          
    default : Serial.println ("NO_MEAN");
  }
}

void utility::verbose_print_reset_core_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1  : Serial.println ("Vbat power on reset");break;
    case 3  : Serial.println ("Software reset digital core");break;
    case 4  : Serial.println ("Legacy watch dog reset digital core");break;
    case 5  : Serial.println ("Deep Sleep reset digital core");break;
    case 6  : Serial.println ("Reset by SLC module, reset digital core");break;
    case 7  : Serial.println ("Timer Group0 Watch dog reset digital core");break;
    case 8  : Serial.println ("Timer Group1 Watch dog reset digital core");break;
    case 9  : Serial.println ("RTC Watch dog Reset digital core");break;
    case 10 : Serial.println ("Instrusion tested to reset CPU");break;
    case 11 : Serial.println ("Time Group reset CPU");break;
    case 12 : Serial.println ("Software reset CPU");break;
    case 13 : Serial.println ("RTC Watch dog Reset CPU");break;
    case 14 : Serial.println ("for APP CPU, reseted by PRO CPU");break;
    case 15 : Serial.println ("Reset when the vdd voltage is not stable");break;
    case 16 : Serial.println ("RTC Watch dog reset digital core and rtc module");break;
    default : Serial.println ("NO_MEAN");
  }
}

void utility::reset_reason(){

  verbose_print_reset_reason();
  Serial.print("CPU0 reset reason: ");  verbose_print_reset_core_reason(rtc_get_reset_reason(0));
  Serial.print("CPU1 reset reason: ");  verbose_print_reset_core_reason(rtc_get_reset_reason(1)); 
  
  }

void utility::shut_down(){


Param.status=CEASE;

}


void utility::UpdateContent(String path, String income){
          File f = FileFS.open(path, "w");   
  if (!f) {
  
    PRINTLN("[F-w-"+path+"]");// now write two lines in key/value style with  end-of-line characters  
    return;
      }   
  else {   f.println(income); f.close();  }        
}


void utility::frimware_update(void){
//CAN.SW_RESET_BOOL=false;
PRINTLN("frimware_update");
}

enum { CMD_BUF_LEN = 255};  // Number of characters in command buffer


void setup_cli(void){

     xTaskCreatePinnedToCore(doCLI,
                          "Do CLI",
                          1024,
                          NULL,
                          2,
                          NULL,
                          1);
}
// Serial terminal task
void doCLI(void *parameters) {

  
  char c;
  char cmd_buf[CMD_BUF_LEN];
  uint8_t idx = 0;
 

  // Clear whole buffer
  memset(cmd_buf, 0, CMD_BUF_LEN);

  // Loop forever
  while (1) {

    // Look for any error messages that need to be printed
    //if (xQueueReceive(msg_queue, (void *)&rcv_msg, 0) == pdTRUE) {      Serial.println(rcv_msg.body);    }

    // Read characters from serial
    if (Serial.available() > 0) {
      c = Serial.read();

      // Store received character to buffer if not over buffer limit
      if (idx < CMD_BUF_LEN - 1) {
        cmd_buf[idx] = c;
        idx++;
      }

      // Print newline and check input on 'enter'
      if ((c == '\n') || (c == '\r')) {
        
        // Print newline to terminal
        //Serial.print("\r\n");

        // Print average value if command given is "avg"
        cmd_buf[idx - 1] = '\0';
       // if (strcmp(cmd_buf, command) == 0) { parsing         }

        // Reset receive buffer and index counter
        memset(cmd_buf, 0, CMD_BUF_LEN);
        idx = 0;

      // Otherwise, echo character back to serial terminal
      } else {
        ///Serial.print(c);
      }
    }

    // Don't hog the CPU. Yield to other tasks for a while
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}










