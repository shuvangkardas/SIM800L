#ifndef SIM_COMMANDS_H
#define SIM_COMMANDS_H
#include <Arduino.h>


const char AT[] PROGMEM = "AT\r";
const char OK_REPLY[] PROGMEM = "OK";
const char AT_CSQ[] PROGMEM = "AT+CSQ\r";
const char AT_CREG[] PROGMEM = "AT+CREG?\r";
const char AT_BAUD[] PROGMEM = "AT+IPR=57600;&W\r";

/*************GPRS Connection****************/
const char AT_SAPBR_GPRS[] PROGMEM = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r"; 
const char AT_SAPBR_APN[] PROGMEM = "AT+SAPBR=3,1,\"APN\",";
const char AT_SAPBR_OPEN_GPRS[] PROGMEM = "AT+SAPBR=1,1\r";
const char AT_SAPBR_QRY_IP[] PROGMEM = "AT+SAPBR=2,1\r\n";
const char AT_SAPBR_CLOSE_GPRS[] PROGMEM = "AT+SAPBR=0,1\r";

/*****************HTTP *************************/
const char AT_HTTPINIT[] PROGMEM = "AT+HTTPINIT\r";
const char AT_HTTPPARA_CID[] PROGMEM = "AT+HTTPPARA=\"CID\",1\r";
const char AT_HTTPPARA_URL[] PROGMEM = "AT+HTTPPARA=\"URL\",";
const char AT_HTTPPARA_CONTENT[] PROGMEM = "AT+HTTPPARA=\"CONTENT\",";
const char AT_HTTPDATA[] PROGMEM = "AT+HTTPDATA=";
const char AT_HTTPACTION[] PROGMEM = "AT+HTTPACTION=";
const char AT_HTTPTERM[] PROGMEM = "AT+HTTPTERM\r";

const char DOWNLOAD_REPLY[] PROGMEM = "DOWNLOAD";
const char HTTP_SUCCESS_CODE[] PROGMEM = "200";





 #endif
