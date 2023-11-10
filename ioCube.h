#include <Arduino.h>
#include "cmds_define.h"

// #if !defined(MaxStrLen)
#define MaxStrLen 30
// #endif

// actual payload of the header file

// #define cmdP8x8img {00,	6+8+1,0xFF,00,05,0x20} //IICAddr	LENGTH	TARGETID	SOURCEID	COMMAND	DATA	CHECKSUM
typedef struct {
    byte addr;
    byte cmd_length;
    byte target;
    byte sorceid;
    byte cmd;
} cmd_header;

typedef void (*OnMsgCB)(byte msg_from, byte msg_cmd,byte data[],byte datalen);


class ioLED8X8{
public:
    ioLED8X8();
    ioLED8X8(byte target);
    ~ioLED8X8();
    ioLED8X8& print(String s);
    ioLED8X8& print(String s,int rotation);
    ioLED8X8& print(byte img[8]);
    ioLED8X8& print(byte img[8],byte rotation);
    void setImg(byte start);
    void setImg(byte start,byte rotation,byte imglen);
    void ShiftImg();
    void ShiftImg(byte length);
    const uint8_t cmdP8x8img[7] = {00,	6+8+1,0xFF,00,05,0x20};
protected:
    byte image[MaxStrLen*8+8];
    int imgbyte =0;
    int imgCurrent =0;
    byte target = 0xFF;
    void drawPixel(int x,int y,bool invert);
    void drawPixel(int x,int y);
    void drawChar(char c,int start);
    void dumpImg();
};

class ioRGB{
public:
    ioRGB();
    ioRGB(byte target);
    ~ioRGB();
    ioRGB& RGB(byte R,byte G,byte B);
    ioRGB& toggle(byte count);
    ioRGB& toggle();
    void on();
    void off();
protected:
    byte R =0xFE;
    byte G =0xFE;
    byte B =0xFE;
    byte target = 0xFF;
};

class ioBuzzer{
public:
    ioBuzzer();
    ~ioBuzzer();
    void play();
protected:
    byte target = 0xFF;
};

class ioMotor{
public:
    ioMotor();
    ~ioMotor();
    void run(byte dir,byte speed);
    void stop();
protected:
    void run(byte on,byte dir,byte speed);
    byte target = 0xFF;
};



void RegisterMsgCB(OnMsgCB cb);