#include "sim.h"



bool SIM::httpSet(const __FlashStringHelper *url)
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
  p = str_cat_P(p, AT_HTTPPARA_URL);
  *p++ = '\"';
  *p = '\0';
  p = str_cat(p, url);
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
bool SIM::httpSet(const char *url)
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
  p = str_cat_P(p, AT_HTTPPARA_URL);
  *p++ = '\"';
  *p = '\0';
  p = str_cat(p, url);
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
bool SIM::httpPostSetPacketType(const __FlashStringHelper *content)
{
  char *p = _buf;
  *p = '\0';
  p = str_cat_P(p, AT_HTTPPARA_CONTENT);
  p = char_cat(p, '\"');
  p = str_cat(p, content);
  p = char_cat(p, '\"');
  p = char_cat(p, '\r');
  //  Serial.println(_buf);
  bool ok = at_cmd(_buf, OK_REPLY);
  return ok;
}
bool SIM::httpPostSetPacketType(const char *content)
{
  char *p = _buf;
  *p = '\0';
  p = str_cat_P(p, AT_HTTPPARA_CONTENT);
  p = char_cat(p, '\"');
  p = str_cat(p, content);
  p = char_cat(p, '\"');
  p = char_cat(p, '\r');
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
  p = str_cat_P(p, AT_HTTPDATA);
  //  *p = '\0';

  int dataLen = strlen(payload);
  itoa (dataLen, temp, 10);
  p = str_cat(p, temp);
  p = char_cat(p, ',');

  itoa (_http_timeout, temp, 10);
  p = str_cat(p, temp);
  p = char_cat(p, '\r');

  ok = at_cmd(_buf, DOWNLOAD_REPLY);
  Serial.print(F("Download ok: ")); Serial.println(ok);
  if (!ok) {
    return false;
  }

  ok = at_cmd(payload, OK_REPLY);
  return ok;

}

void SIM::httpStart(char req_type);
{
  bool ok = false;
  char *p = _sub_buf;
  *p = '\0';
  p = str_cat_P(p, AT_HTTPACTION);
  p = char_cat(p, req_type);
  p = char_cat(p, '\r');
  ok = at_cmd(_sub_buf, OK_REPLY);
  Serial.print(F("HTTP Session Starts : ")); Serial.println(ok);
}

char *SIM::httpStartTransmit(char req_type)
{
  // bool ok = false;
  // char *p = _sub_buf;
  // *p = '\0';
  // p = str_cat_P(p, AT_HTTPACTION);
  // p = char_cat(p, req_type);
  // p = char_cat(p, '\r');

  // ok = at_cmd(_sub_buf, OK_REPLY);
  // Serial.print(F("POST Session start ok: ")); Serial.println(ok);
  httpStart(req_type);

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


bool SIM::httpPOST(const char *URL, const char *packet, const char * packetType, int httpCode)
{
  bool ok = false;
  ok = httpSet(URL);
  ok = httpPostSetPacketType(packetType);
  ok = httpPostSetPayload(packet);
  char *ptr = httpStartTransmit('1');
  char *retPtr = sub_string(ptr, ',', ',');
  Serial.println(retPtr);
  int retCode = atoi(retPtr);
  httpClose();
  if (retCode == httpCode)
  {
    Serial.println(F("HTTP POST Success!"));
    return true;
  }
  else
  {
    return false;
  }
}

bool SIM::httpPOST(const __FlashStringHelper *URL, const char *packet, const __FlashStringHelper * packetType, int httpCode)
{
  bool ok = false;
  ok = httpSet(URL);
  ok = httpPostSetPacketType(packetType);
  ok = httpPostSetPayload(packet);
  char *ptr = httpStartTransmit('1');
  char *retPtr = sub_string(ptr, ',', ',');
  Serial.println(retPtr);
  int retCode = atoi(retPtr);
  httpClose();
  if (retCode == httpCode)
  {
    Serial.println(F("HTTP POST Success!"));
    return true;
  }
  else
  {
    return false;
  }
}

/***********************Modifying POST API************************/

void SIM::postSetParam(const __FlashStringHelper *url, const __FlashStringHelper *conType)
{
	_postUrl = url;
    _postContentType =  conType;
}

bool SIM::postWaitAck()
{
  char *ptr;
  long startMillis = millis();
  do
  {
    ptr = _read_sim();
    Serial.println(ptr);
    if (strlen(ptr))
    {
      // return ptr;

      char *retPtr = sub_string(ptr, ',', ',');
  	  Serial.println(retPtr);
  	  int retCode; = atoi(retPtr);
  	  return retCode;
    }
  } while (millis() - startMillis < 10000);
  Serial.println(F("<POST TIMEOUT>"));
  return -1;
}

bool httpPOST(const char *packet)
{
  bool ok = false;
  ok = httpSet(_postUrl);
  ok = httpPostSetPacketType(_postContentType);
  ok = httpPostSetPayload(packet);
  httpStart(POST);
}