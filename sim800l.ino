#include <SoftwareSerial.h>
#include <ShiftRegister74HC595.h>
#include "sim.h"


ShiftRegister74HC595 sr(1, 7, 5, 6);
#define SR_SIM_PIN 6
#define SR_SIM_ON() sr.set(SR_SIM_PIN, LOW)
#define SR_SIM_OFF() sr.set(SR_SIM_PIN, HIGH)

const char apn[] PROGMEM = "gpinternet";
const char URL[] PROGMEM = "http://dataxlogger.com/data/k5199h01";
const char CONTENT_TYPE[] PROGMEM = "application/json";

//char dataPacket[] = "{\"time\":\"2019-10-30T10:20:30Z\",\"v1\":20,\"I1\":15,\"i1\":27}";
char dataPacket[] = "{\"time\":\"10:20:30\",\"v1\":20,\"I1\":15,\"i1\":27}";

//char buf[128] = "";
//char *buf_ptr = buf;
const char ok[] = "OK";


SoftwareSerial simSerial(3, 4);

SIM sim(&simSerial);

char ip_addr[] = "+SAPBR: 1,1,\"100.120.204.132\"";
//char ip_addr[] = "+SAPBR: 1,1,\"0.0.0.0\"";
void setup()
{
  Serial.begin(115200);
  simSerial.begin(57600);
  
  sim.attachRestart(simRetart);
  sim.begin(1000);
  Serial.println(F("Setup done"));

  sim.setSerialTimeout(5);
  sim.setSimReadTimeout(1000);
  sim.isOk();
  sim.setBaud(115200);
  Serial.print(F("Sig: "));
  Serial.println(sim.getNetworkLevel());

  sim.initGprs(apn);
  sim.closeGPRS();
}

void loop()
{
  Serial.print(F("<----------------Loop Starts------------------>"));

  bool reg = sim.isRegistered();
  Serial.print(F("Reg: ")); Serial.println(reg);
  int8_t netLevel = sim.getNetworkLevel();
  Serial.print(F("Net: ")); Serial.println(netLevel);

  long startms = millis();
  Serial.print(F("Start millis: ")); Serial.println(startms);

  if (sim.connectGprs(apn))
  {
    sim.httpPOST(URL, dataPacket, CONTENT_TYPE);
    //    sim.httpSet(URL);
    //    sim.httpPostSetPacketType(CONTENT_TYPE);
    //    sim.httpPostSetPayload(dataPacket);
    //    sim.httpStartTransmit('1');
    Serial.print(F("------Time : ")); Serial.println(millis() - startms);
    //    sim.httpClose();
    //  Serial.println(sim._read_sim());

//    sim.closeGPRS();
    Serial.println(F("<-----------------Loop Ends------------------->"));
    delay(10000);
  }



}

void simRetart()
{
  Serial.println(F("Sim Restarts.."));
  SR_SIM_OFF();
  delay(100);
  SR_SIM_ON();
  delay(500);
}

//char *sub_string(const char *s,char first, char last)
//{
//  Serial.println(s);
//  char *first_ptr = strchr(s,first);
//  Serial.println(first_ptr);
//  char *last_ptr = strchr(first_ptr,last);
//  Serial.println(last_ptr);
//  uint8_t len = (uint8_t)(last_ptr - first_ptr);
//  Serial.print("Len : ");Serial.println(len);
//  strncpy(buf,++first_ptr,--len);
//  Serial.println(buf);
//}


//  simCmd(AT);
//    send_cmd("AT\r\n");


//  sim_cmd(buf, "AT\r");
//  sim_cmd(buf, "AT+CSQ\r");

















//
//
//int timed_serial_read()
//{
//  int c;
//  start_millis = millis();
//  do
//  {
//    c = simSerial.read();
//    if(c > 0 )
//    {
//      return c;
//    }
//  }while((millis() - start_millis) < 1000);
//  return -1;
//}
//char *readString(char *str)
//{
//  char *ptr = str;
//  int c = timed_serial_read();
//  while(c >=0 )
//  {
//    *ptr = (char)c;
//    ptr++;
//    c = timed_serial_read();
//  }
//  return str;
//}
//
//
//void sim_cmd(char *output, const char *cmd)
//{
//  buffer_clear();
//  char *ptr = output;
//
//
//  Serial.println(cmd);
//  simSerial.println(cmd);
//
//  int count = 0;
//  int timeOut = 1000;
//  do
//  {
//    delay(1);
//  } while (!simSerial.available() && --timeOut);
//
//  Serial.println(readString(ptr));
//}
//
//
//
//
//void sim_cmd()
//{
//  simSerial.println("AT\n\r");
//  while (simSerial.available());
//  String str = simSerial.readString();
//  Serial.println(str);
//}
//
//char *simCmd(const char *at)
//{
//  char buffer[30] = "";
//  strcpy_P(buffer, at);
//  Serial.println(buffer);
//  buffer_clear();
//  simSerial.write(buffer);
//  simSerial.write("\r\n");
//
//  while (simSerial.available());
//  String str = simSerial.readString();
//  Serial.println(str);
//}
//void buffer_clear(void)
//{
//  while (simSerial.available())
//  {
//    simSerial.read();
//  }
//  Serial.println(F("Buffer Cleared"));
//}
//void send_cmd(const char *cmd)
//{
//  buffer_clear();
//  simSerial.print(cmd);
//  while (simSerial.available());
//  String str = simSerial.readString();
//  Serial.print(str);
//}
//

