#ifndef _UTILL_H
#define _UTILL_H

#include <Arduino.h>
#include "cnst.h"

class utility
{
private:
   String USERS[5],PASS[5],LoginUser; 
   byte  IsAdmin[5];
public:
 utility();
  ~utility();
 void restor_user_pass(void);

void listDir(const char * dirname, uint8_t levels);
void DIR(void);
void createDir(const char * path);
void removeDir(const char * path);

void shut_down(void);

void readFile(const char * path);
void writeFile( const char * path, const char * message);
void appendFile(const char * path, const char * message);
void renameFile(const char * path1, const char * path2);
void deleteFile(const char * path);
String read_line(String _path,byte line_num);
String formatBytes(size_t bytes);
void verbose_print_reset_reason(void);
void verbose_print_reset_core_reason(RESET_REASON );
void reset_reason(void);
void UpdateContent(String path, String income);
void frimware_update(void);

};




extern utility UTIL;


void doCLI(void *parameters);
void setup_cli(void);

#endif