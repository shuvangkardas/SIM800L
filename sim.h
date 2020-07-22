#ifndef SIM_H
#define SIM_H
#include <Arduino.h>
#include "sim_commands.h"

#define PRIMARY_BUF_SIZE 128
#define SECONDARY_BUF_SIZE 32

//class SIM; // forward decl
typedef void (*func_t)(void);

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
    bool isRegistered();
    bool setBaud(unsigned long baud);
    bool initGprs(const char *apn);
    bool startGPRS();
    bool isGprsConnected(int8_t maxCount = 3);
    bool closeGPRS();
    int8_t getNetworkLevel();
    bool setGPRS(const char *apn);
    bool connectGprs(const char *apn);
    /***********Attach function******************/

    /***********HTTP Methods*********************/
    // const __FlashStringHelper* ADDRESS
    bool httpSet(const char *url_P);
    bool httpSet(const __FlashStringHelper* url);

    bool httpPostSetPacketType(const char *content);
    bool httpPostSetPayload(const char *payload);

    // bool htttSetPayloadParam(int payloadSize, int timeOut = 10000);
    // bool httpWritePayload(const char *payload,bool endFlag = false);
   
    char *httpStartTransmit(char req_type = '1'); //1 = http post | 0 = get
    bool httpClose();

    bool httpPOST(const char *URL_P, const char *packet, const char * packetType, int httpCode = 200);

    //basic commands methods
    char *send_cmd(const char *cmd);
    char *send_cmd_P(const char *cmd);
    bool at_cmd_P(const char *in_P, const char *out_P);
    bool at_cmd(const char *in, const char *out_P);

    //    typedef char * (*send_func_t)(const char * );
    //    bool cmd_check(send_func_t send_ptr,const char *in, const char *out);

    //Serial basic methods
    int _timed_read();
    char *_read_string(char *str);
    char *_read_sim();

    //String manipulation methods
    char *sub_string(const char *s, char first, char last);
    char *str_my_cat(char *dest, const char *src);
    char *str_my_cat_P(char *dest, const char *src);
    char *char_cat(char *dest, char c);
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



    //    int _timed_read();
    //    char *_read_string(char *str);
    void _buffer_clear();

};
#endif
