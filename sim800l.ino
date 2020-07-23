#include <SoftwareSerial.h>
#include <ShiftRegister74HC595.h>
#include "sim.h"


ShiftRegister74HC595 sr(1, 7, 5, 6);
#define SR_SIM_PIN 6
#define SR_SIM_ON() sr.set(SR_SIM_PIN, LOW)
#define SR_SIM_OFF() sr.set(SR_SIM_PIN, HIGH)

const char apn[] PROGMEM = "gpinternet";
//const char URL[] PROGMEM = "http://dataxlogger.com/data/k5199h01";
//#define URL F("http://dataxlogger.com/data/k5199h01")
const char URL[] = "http://dataxlogger.com/data/k5199h01";

const char CONTENT_TYPE[] PROGMEM = "application/json";


char dataPacket[] = "{\"time\":\"10:20:30\",\"v1\":20,\"I1\":15,\"i1\":27}";


SoftwareSerial simSerial(3, 4);
SIM sim(&simSerial);

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
  /***************Test Area***********************************/


  /************************************************************/

  
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
    Serial.print(F("------Time : ")); Serial.println(millis() - startms);

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
  delay(3000);
}








