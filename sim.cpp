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
  if( _simRestart != NULL)
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


bool SIM::initGprs(const char *apn)
{
  //First AT_SAPBR
  bool ok = false;
  ok = at_cmd_P(AT_SAPBR_GPRS, OK_REPLY);

  //Second AT_SAPBR
  char *p = _buf;
  //to overwite the previous data as cmd_cat will concate after null
  *p = '\0';
  p = str_my_cat_P(p, AT_SAPBR_APN);
  *p++ = '\"';
  *p = '\0';
  p = str_my_cat_P(p, apn);
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
bool SIM::httpSet(const __FlashStringHelper* url)
{

}
bool SIM::httpSet(const char *url_P)
{
  bool ok = false;
  ok = at_cmd_P(AT_HTTPINIT, OK_REPLY);
  if (!ok) {
    return false;
  }

  ok = at_cmd_P(AT_HTTPPARA_CID, OK_REPLY);
  if (!ok) {
    return false;
  }

  char *p = _buf;
  *p = '\0';
  p = str_my_cat_P(p, AT_HTTPPARA_URL);
  *p++ = '\"';
  *p = '\0';
  p = str_my_cat_P(p, url_P);
  *p++ = '\"';
  *p++ = '\r';
  *p = '\0';
  //  Serial.println(_buf);
  //Send URL
  ok = at_cmd(_buf, OK_REPLY);
  if (ok) {
    return true;
  }
  else {
    return false;
  }
}

bool SIM::httpPostSetPacketType(const char *content)
{
  char *p = _buf;
  *p = '\0';
  p = str_my_cat_P(p, AT_HTTPPARA_CONTENT);
  p = cat_char(p, '\"');
  p = str_my_cat_P(p, content);
  p = cat_char(p, '\"');
  p = cat_char(p, '\r');
  //  Serial.println(_buf);
  bool ok = at_cmd(_buf, OK_REPLY);
  return ok;

}
bool SIM::httpPostSetPayload(const char *payload)
{
  bool ok = false;
  char temp[5];
  char *p = _buf;
  *p = '\0';
  p = str_my_cat_P(p, AT_HTTPDATA);
  //  *p = '\0';

  int dataLen = strlen(payload);
  itoa (dataLen, temp, 10);
  p = cmd_cat(p, temp);
  p = cat_char(p, ',');

  itoa (_http_timeout, temp, 10);
  p = cmd_cat(p, temp);
  p = cat_char(p, '\r');

  ok = at_cmd(_buf, DOWNLOAD_REPLY);
  Serial.print(F("Download ok: ")); Serial.println(ok);
  if (!ok) {
    return false;
  }

  ok = at_cmd(payload, OK_REPLY);
  return ok;

}

// bool SIM::htttSetPayloadParam(int payloadSize,int timeOut)
// {
//   bool ok = false;
//   char *p = _buf;
//   *p = '\0';
//   p = str_my_cat_P(p, AT_HTTPDATA);

//   char temp[6];
//   itoa (payloadSize, temp, 10);
//   p = cmd_cat(p, temp);
//   p = cat_char(p, ',');

//   itoa (timeOut, temp, 10);
//   p = cmd_cat(p, temp);
//   p = cat_char(p, '\r');

//   ok = at_cmd(_buf, DOWNLOAD_REPLY);
//   Serial.print(F("Download ok: ")); Serial.println(ok);
//   _payloadLen = payloadSize;
//   _payloadSentLen = 0; 
// }

// bool SIM::httpWritePayload(const char *payload,bool endFlag)
// {
//   int len = strlen(payload);
//   serial -> write(payload,len-2);
//   // ok = at_cmd(payload, OK_REPLY);
//   _payloadSentLen +=   + len;
//   int remLen = _payloadLen - _payloadSentLen;
//   Serial.print(F("Remaining: "));Serial.println(remLen);
//   if(endFlag)
//   {
//   	if(_payloadSentLen < _payloadLen)
//   	{
//   		Serial.println(F("Printing null.."));
//   		char null_char = '\n';
//   		for(int i = 0; i< remLen; i++ )
//   		{
//   			Serial.print(".");
//   			serial -> write((uint8_t)null_char);
//   		}
//   	}
//   }
// }

char *SIM::httpStartTransmit(char req_type)
{
  bool ok = false;
  char *p = _sub_buf;
  *p = '\0';
  p = str_my_cat_P(p, AT_HTTPACTION);
  p = cat_char(p, req_type);
  p = cat_char(p, '\r');

  ok = at_cmd(_sub_buf, OK_REPLY);
  Serial.print(F("POST Session start ok: ")); Serial.println(ok);

  char *ptr;
  long startMillis = millis();
  do
  {
    ptr = _read_sim();
    Serial.println(ptr);
    if (strlen(ptr))
    {
      return ptr;
    }
  } while (millis() - startMillis < 10000);
  Serial.println(F("<POST TIMEOUT>"));
  return NULL;
}

bool SIM::httpClose()
{
  long start = millis();
  Serial.print("Start: "); Serial.println(start);  
//  char *ptr = send_cmd_P(AT_HTTPTERM);
  serial -> print(AT_HTTPTERM);
//  Serial.println(ptr);
  Serial.print("Close time: "); Serial.println(millis() - start);
}

bool SIM::httpPOST(const char *URL_P, const char *packet, const char * packetType, int httpCode)
{
  bool ok = false;
  ok = httpSet(URL_P);
  ok = httpPostSetPacketType(packetType);
  ok = httpPostSetPayload(packet);
  char *ptr = httpStartTransmit('1');
  char *retPtr = sub_string(ptr, ',', ',');
  Serial.println(retPtr);
  int retCode = atoi(retPtr);
  httpClose();
  if(retCode==httpCode)
  {
    Serial.println(F("HTTP POST Success!"));
    return true;
  }
  else
  {
    return false;
  }
}
/************************String helper functionality*************/

/***
   This function searches PROGMEM string in a user ram string.
   and return true if found else false
*/
bool SIM::cmd_cmp_P(char *ram_str, const char *prog_str)
{
  char *cmp_ptr = strstr_P(ram_str, prog_str);//second pointer in program space
  if (cmp_ptr != NULL)
  {
    //    Serial.print(F("Reply: "));
    Serial.println(cmp_ptr);
    return true;
  }
  else
  {
    Serial.println(F("Found Null"));
    return false;
  }
}
/*
   The function finds portion of string in between first and last char
*/
char *SIM::sub_string(const char *s, char first, char last)
{
  char *first_ptr = strchr(s, first);
  //    Serial.println(first_ptr);
  char *last_ptr = strchr(first_ptr + 1, last);
  //    Serial.println(last_ptr);
  if (last_ptr > first_ptr)
  {
    //    Serial.println(F("Greater"));
    uint8_t len = (uint8_t)(last_ptr - first_ptr);
    //  Serial.println("dot");

    char *ptr = strncpy(_sub_buf, ++first_ptr, --len);
    *(ptr + len) = '\0'; //Null terminating the string for safety
    //    Serial.println(F("sub: "));Serial.println(ptr);
    return ptr;
  }
  else
  {
    //    Serial.println(F("Not Greater"));
    return NULL;
  }
}
/*
   Concate ram string and return the pointer after concate.
*/
char *SIM::cmd_cat(char *dest, const char *src)
{
  char *ptr = dest;
  int8_t len  = strlen(src);
  strncat(ptr, src, len);
  ptr = ptr + len;
  *ptr = '\0';        //terminating as extra safety
  bool test = *ptr == NULL;
  //  Serial.print("Last char: ");Serial.print(test);
  //  Serial.println();
  return ptr;
}
/*
   concate progmem string and return the pointer of null position of string.
*/
char *SIM::str_my_cat_P(char *dest, const char *src)
{
  char *ptr = dest;
  int8_t len  = strlen_P(src);
  strncat_P(ptr, src, len);
  //  Serial.println(ptr);
  ptr = ptr + len;
  *ptr = '\0';
  return ptr;
}

char *SIM::cat_char(char *dest, char c)
{
  //  char *ptr = dest;
  *dest++ = c;
  *dest = '\0';
  return dest;
}

bool SIM::validate_ip(const char *ip)
{
  Serial.println(ip);

  char del = '\"';
  char *p1 = strchr((char*)ip, del);
  char *p2 = NULL;
  del = '.';
  char buf[5];
  for (int8_t i = 0; i < 4; i++)
  {
    if (i == 3) del = '\"';
    p1++;
    p2 = strchr(p1, del);
    strncpy(buf, p1, p2 - p1);

    Serial.println(buf);
    int num = atoi(buf);
    p1 = p2;
  }
}
bool SIM::validate_ip_fast(const char *ip)
{
  //   Serial.print(F("Validate id len: "));Serial.println(strlen(ip));
  if (strlen(ip) > 0)
  {
    //    Serial.println(ip);
    char *p = sub_string(ip, '\"', '\"');
    //    Serial.println(p);
    int8_t ip_len = strlen(p);
    //    Serial.print("Len: "); Serial.println(ip_len);
    Serial.println(p);
    if (ip_len < 8)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    //    Serial.println(F("Not an ip"));
    return false;
  }
}
