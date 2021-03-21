#ifndef SIM_H
#define SIM_H
#include <Arduino.h>
#include "sim_commands.h"

#define PRIMARY_BUF_SIZE 128
#define SECONDARY_BUF_SIZE 32

//class SIM; // forward decl
typedef void (*func_t)(void);
#define POST '1'
#define GET  '0'

class SIM
{
  public:
    SIM(Stream *port);
    void begin(int timeout);
    void setSerialTimeout(int timeout = 50);
    void setSimReadTimeout(int timeout = 2000);
    void attachRestart(func_t restartFun);

    /********Sim Basic Functionality*************/
    bool isOk();
    bool isSimReady();
    bool isRegistered();
    bool setBaud(unsigned long baud);
    
    bool initGprs(const __FlashStringHelper *apn);
    bool initGprs(const char *apn);
    bool startGPRS();
    bool isGprsConnected(int8_t maxCount = 3);
    bool closeGPRS();
    int8_t getNetworkLevel();
    bool setGPRS(const char *apn);
    bool connectGprs(const __FlashStringHelper *apn);
    bool connectGprs(const char *apn);
    /***********Attach function******************/

    /***********HTTP Methods*********************/

    bool httpSet(const __FlashStringHelper *url);//Take addess from Flash
    bool httpSet(const char *url); //Take address from ram

    bool httpPostSetPacketType(const __FlashStringHelper *content);
    bool httpPostSetPacketType(const char *content);
    
    bool httpPostSetPayload(const char *payload);


    char *httpStartTransmit(char req_type = '1'); //1 = http post | 0 = get
    bool httpClose();

    bool httpPOST(const char *URL, const char *packet, const char * packetType, int httpCode = 200);
    bool httpPOST(const __FlashStringHelper *URL, const char *packet, const __FlashStringHelper * packetType, int httpCode = 200);

/************************New Post API*************************************/
    void postSetParam(const __FlashStringHelper *url, const __FlashStringHelper *conType);
    void postSetParam(char *url, const __FlashStringHelper *conType);
    int postWaitAck();
    void httpStart(char req_type);
    bool httpPOST(const char *packet);
    
    //basic commands methods
    char *send_cmd(const char *cmd);
    char *send_cmd_P(const char *cmd);
    bool at_cmd_P(const char *in_P, const char *out_P);
    bool at_cmd(const char *in, const char *out_P);


    //Serial basic methods
    int _timed_read();
    char *_read_string(char *str);
    char *_read_sim();

    //String manipulation methods
    char *str_cat(char *dest, const __FlashStringHelper *src);
    char *str_cat(char *dest, const char *src);
    char *str_cat_P(char *dest, const char *src);
    char *char_cat(char *dest, char c);
    char *sub_string(const char *s, char first, char last);   
    bool cmd_cmp_P(char *ram_str, const char *prog_str);
    bool validate_ip(const char *ip);
    bool validate_ip_fast(const char *ip);
  private:

    int _serialReadTimeout = 50;
    int _simReadTimeout = 2000;

    int8_t _max_try = 5;
    Stream *serial;
    char _buf[PRIMARY_BUF_SIZE];
    char _sub_buf[SECONDARY_BUF_SIZE];
    long _start_millis = 0;
    int _http_timeout = 5000;
    //    const char *_apnPtr;
    func_t _simRestart = NULL;

    __FlashStringHelper *_postUrl_P = NULL;
    char *_postUrl = NULL;

    __FlashStringHelper *_postContentType;

    


    void _buffer_clear();

};
#endif
