#include "sim.h"

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

/*********************Str_CAT function overloading*****************/
char *SIM::str_cat(char *dest, const __FlashStringHelper *src)
{
//  Serial.println(F("Flash Function: "));
  char *ptr = dest;
  const char *src1 = (const char*)src;

  int8_t len  = strlen_P(src1);
  strncat_P(ptr, src1, len);
  ptr = ptr + len;
  *ptr = '\0';
  //  Serial.println(dest);
  return ptr;
}

char *SIM::str_cat(char *dest, const char *src)
{
//  Serial.println(F("RAM Function: "));
  char *ptr = dest;
  int8_t len  = strlen(src);
  strncat(ptr, src, len);
  ptr = ptr + len;
  *ptr = '\0';
  //  Serial.println(dest);
  return ptr;
}

/*
   concate progmem string and return the pointer of null position of string.
*/
char *SIM::str_cat_P(char *dest, const char *src)
{
  char *ptr = dest;
  int8_t len  = strlen_P(src);
  strncat_P(ptr, src, len);
  //  Serial.println(ptr);
  ptr = ptr + len;
  *ptr = '\0';
  return ptr;
}
/*
   Concate ram string and return the pointer after concate.
*/
//char *SIM::str_cat(char *dest, const char *src)
//{
//  char *ptr = dest;
//  int8_t len  = strlen(src);
//  strncat(ptr, src, len);
//  ptr = ptr + len;
//  *ptr = '\0';        //terminating as extra safety
//  // bool test = *ptr == NULL;
//  //  Serial.print("Last char: ");Serial.print(test);
//  //  Serial.println();
//  return ptr;
//}

/*

*/
char *SIM::char_cat(char *dest, char c)
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
