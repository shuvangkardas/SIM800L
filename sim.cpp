#include "sim.h"
#include <stdlib.h>
#include <string.h>

#define SERIAL_AVAILABLE_TIMEOUT 2000

SIM::SIM(Stream *port)
{

  serial = port;
}

void SIM::begin(int timeout)
{
  Serial.println(F("Sim begin"));
  if ( _simRestart != NULL)
  {
    _simRestart();
  }
  //  _max_time = timeout;
}
void SIM::attachRestart(func_t restartFun)
{
  _simRestart = restartFun;
}

void SIM::setSerialTimeout(int timeout)
{
  _serialReadTimeout = timeout;
}
void SIM::setSimReadTimeout(int timeout)
{
  _simReadTimeout = timeout;
}
/**************Implementing Basic Function*******************/

/**
  The function wait for a character to read

  @returns the character else -1.
**/
int SIM::_timed_read()
{
  int c;
  _start_millis = millis();
  long current_millis;
  do
  {
    c = serial -> read();
    //    Serial.println(c);
    if (c > 0 )
    {
      return c;
    }
    current_millis = millis();
  } while ((current_millis - _start_millis) < _serialReadTimeout);
  //  Serial.print('%');
  return -1;
}

/**
  The function read string from serial buffer.

  @param *str it takes a parameter pointer to store the string
  @returns the same pointer it takes to store the value
**/
char *SIM::_read_string(char *str)
{
  char *ptr = str;
  int c = _timed_read();
  //  Serial.print(c);
  while (c >= 0)
  {
    //    Serial.print('.');
    *ptr = (char)c;
    ptr++;
    c = _timed_read();
    //    Serial.print("^");
  }
  *ptr = '\0'; //string terminator as extra safety
  return str;
}

/***
  This just clear serial buffer
***/
void SIM::_buffer_clear()
{
  while (serial -> available())
  {
    serial -> read();
  }
}

/**
  This function read sim response

  @returns the pointer of _buf variable
 **/
char *SIM::_read_sim()
{
  _buffer_clear(); //Clear Serial buffer

  /*********This code is for extra safety***/
  int len = 0;
  long current_millis;
  do
  {
    len = serial -> available();
    //    Serial.print(F("Length: "));Serial.println(len);
    if (len > 0)
    {
      break;
    }
  } while (millis() - _start_millis < _simReadTimeout);
  //  Serial.print(F("Wait Ends|"));
  //  Serial.print(F("Len: ")); Serial.println(len);
  /***************Extra safety code ends***/
  char *ptr = _read_string(_buf);
  //  Serial.println(ptr);
  return ptr;
}

/*
   This function sends sim at command and return the response in _buf pointer
   @param *cmd takse the input at command
   @returns _buf pointer where it stores the sim response
*/
char *SIM::send_cmd(const char *cmd)
{
  Serial.println(cmd);
  serial -> print(cmd);
  char *ptr = NULL;
  ptr = _read_sim(); //_read_sim() stores all data in _buf variable.
  return ptr;
}

char *SIM::send_cmd_P(const char *cmd)
{
  strcpy_P(_buf, cmd);
  return send_cmd(_buf);
}


bool SIM::at_cmd_P(const char *in_P, const char *out_P)
{
  char *ptr = send_cmd_P(in_P);
  return cmd_cmp_P(ptr, out_P);
}

bool SIM::at_cmd(const char *in, const char *out_P)
{
  char *ptr = send_cmd(in);
  return cmd_cmp_P(ptr, out_P);
}


/************************Sim basic functionality****************/
bool SIM::isOk()
{
  bool sim_ok = false;
  int8_t try_count = 5;
  long start = millis();
  do
  {
    //    Serial.print(F("Try: ")); Serial.println(try_count);
    sim_ok = at_cmd_P(AT, OK_REPLY);
    //    sim_ok = at_cmd(send_cmd_P,AT,OK_REPLY);
    if (sim_ok) return true;
  } while (--try_count);
  return false;
}
bool SIM::setBaud(unsigned long baud)
{
  if (isOk())
  {
    char *ptr = send_cmd_P(AT_BAUD);
    //    Serial.println(ptr);
  }

}
bool SIM::isRegistered()
{
  //  char *ptr = send_cmd_P(AT_CREG);
  //  Serial.println(ptr);
  //  char *regPtr = strchr(ptr,',');
  //  Serial.println(regPtr);
  //  Serial.println(atoi(++regPtr));

  char *ptr;
  char *reg;
  long ms1 = millis();
  do
  {
    ptr = send_cmd_P(AT_CREG);
    Serial.println(ptr);
    reg = strchr(ptr, ',');
    if (atoi(++reg) == 1)
    {
      return true;
    }
  } while (millis() - ms1 < 10000);
  return false;
}

int8_t SIM::getNetworkLevel()
{
  //  Serial.println(AT_CSQ);
  int8_t try_count = 5;
  int net  = 0;
  do
  {
    char *ptr = send_cmd_P(AT_CSQ);
    //    Serial.println(ptr);
    char *sub_ptr = sub_string(ptr, ':', ',');
    //    Serial.println(sub_ptr);
    net  = atoi(sub_ptr);
    if (net > 0) {
      return net;
    }
  } while (--try_count);

  return 0;
}

bool SIM::initGprs(const __FlashStringHelper *apn)
{
  //First AT_SAPBR
  bool ok = false;
  ok = at_cmd_P(AT_SAPBR_GPRS, OK_REPLY);

  //Second AT_SAPBR
  char *p = _buf;
  //to overwite the previous data as str_cat will concate after null
  *p = '\0';
  p = str_cat_P(p, AT_SAPBR_APN);
  *p++ = '\"';
  *p = '\0';
  p = str_cat(p, apn);
  *p++ = '\"';
  *p++ = '\r';
  *p = '\0';

  ok = at_cmd(_buf, OK_REPLY);
}
bool SIM::initGprs(const char *apn)
{
  //First AT_SAPBR
  bool ok = false;
  ok = at_cmd_P(AT_SAPBR_GPRS, OK_REPLY);

  //Second AT_SAPBR
  char *p = _buf;
  //to overwite the previous data as str_cat will concate after null
  *p = '\0';
  p = str_cat_P(p, AT_SAPBR_APN);
  *p++ = '\"';
  *p = '\0';
  p = str_cat(p, apn);
  *p++ = '\"';
  *p++ = '\r';
  *p = '\0';

  ok = at_cmd(_buf, OK_REPLY);
}

bool SIM::startGPRS()
{
  //There might be issue with this functions.
  bool ok = at_cmd_P(AT_SAPBR_OPEN_GPRS, OK_REPLY);
  return ok;
}

bool SIM::isGprsConnected(int8_t maxCount)
{
  int8_t tryCount = maxCount;
  char *p;
  bool ip = false;
  do
  {
    Serial.print(F("Try ")); Serial.println(tryCount);
    p = send_cmd_P(AT_SAPBR_QRY_IP);
    Serial.println(p);
    ip = validate_ip_fast(p);
    Serial.print(F("IP: ")); Serial.println(ip);
    if (ip)
    {
      //      Serial.print(F("IP: "));Serial.println(ip);
      return true;
    }
  } while (--tryCount);
  return false;
}
bool SIM::connectGprs(const __FlashStringHelper *apn)
{
  bool ok = isGprsConnected(1);
  if (ok) {
    return true;
  }

  int8_t tryCount = 5;
  do
  {
    Serial.println(F("->GPRS Connecting..."));
    ok = initGprs(apn);
    ok = startGPRS();
    ok = isGprsConnected();
    if (ok)
    {
      return true;
    }
  } while (--tryCount);
  return false;
}
bool SIM::connectGprs(const char *apn)
{
  bool ok = isGprsConnected(1);
  if (ok) {
    return true;
  }

  int8_t tryCount = 5;
  do
  {
    Serial.println(F("->GPRS Connecting..."));
    ok = initGprs(apn);
    ok = startGPRS();
    ok = isGprsConnected();
    if (ok)
    {
      return true;
    }
  } while (--tryCount);
  return false;
}
bool SIM::closeGPRS()
{
  bool ok = at_cmd_P(AT_SAPBR_CLOSE_GPRS, OK_REPLY);
  return ok;
}

/******************************HTTP*****************************/

/************************String helper functionality*************/


