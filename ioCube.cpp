#include "ioCube.h"
#include "fonts.h"
#include <Wire.h>

// #include "FunctionalInterrupt.h"
//ESP32_C3 ioLED8X8
//LED io_10
//IIC SCL io_4
//IIC SDA io_5
//uart1 TXD io_19
//uart1 RXD io_18
void do_rxHandle(void *param);
class ioBus{
public:
    ioBus(){
        Serial1.begin(57600, SERIAL_8N1, 18, 19);
        Wire.begin(5,4);
    };
    ~ioBus(){

    };
    bool rxHandeling = false;
    byte rxBuffer[136];
    byte rxBufferLen = 0;
    int rxlaststamp =0;
    byte rx1293last =0;
    byte memberCnt = 0;
    void rx1293(){
      byte buffer[3] = {0,0,0};
      Wire.beginTransmission(0x28);
      Wire.write(0x10);
      delay(1);
      byte error = Wire.endTransmission(true);
      delay(1);
      byte bytesReceived = Wire.requestFrom(0x28, 3);
      if((bool)bytesReceived){   //If received more than zero bytes
        byte btnid = 0;
        Wire.readBytes(buffer, bytesReceived);
        for(uint8_t i=0;i<3;i++) if(buffer[i] < 0x80 && buffer[i] >0x40) btnid += (1 <<i);
        if (btnid != rx1293last) {
          if (btnid >0) {
            // Serial.printf("btn:[%02X] down\n",btnid);
            // byte msg_from, byte msg_cmd,byte data[],byte datalen
            byte data[5] = {btnid,1,0,0,0};
            tx(cmdInputState,00,0xff,0xfe,data,5);
            if (msgCB) msgCB(0xFE,cmdInputState,data,5);
          }  
          else {
            // Serial.printf("btn:[%02X] up\n",rx1293last);
            byte data[5] = {rx1293last,0,0,0,0};
            tx(cmdInputState,00,0xff,0xfe,data,5);
            if (msgCB) msgCB(0xFE,cmdInputState,data,5);
          }
          rx1293last = btnid;
        }
      };
    }
    void rxhandle(){
      // delay(10);
      // int now = millis();
      // if (now - rxlaststamp > 50) rxBufferLen = 0;
      // rxlaststamp = millis();
      rxBufferLen += Serial1.read(rxBuffer+rxBufferLen,136-rxBufferLen);
      if (rxBufferLen >2 && (rxBuffer[1]+3 <= rxBufferLen)) {
        byte checksum =0;
        for (int i=0;i<rxBuffer[1]+2;i++) checksum += rxBuffer[i];
        if (checksum != rxBuffer[rxBuffer[1]+2]){
          // Serial.printf("check sum error "); Serial.printf("rx len[%u]:[",rxBufferLen);
          // for (int i=0;i<rxBuffer[1]+3;i++) Serial.printf("%u,",rxBuffer[i]);
          // Serial.printf("]%u,\n",checksum);

        } else {
          if (msgCB) msgCB(rxBuffer[3],rxBuffer[4],rxBuffer+5,rxBuffer[1]-3);
        };
        rxBufferLen =0;
      };
      // rxHandeling = false;
    }
    ioBus& setMode(byte mode){
      switch (mode) {
      case MODE_HOST: case MODE_SCRIPT: case MODE_UPLOAD:
        byte tmp[1]; tmp[0] = mode;
        tx(cmdSetMode,0,0xFF,0xFE,tmp,1);
      break;
      
      default:
        break;
      };
      return *this;    
    };
    ioBus& tx(cmd_header header,byte data[],byte len){
      header.cmd_length = len+3;
      byte chksum = header.addr + header.cmd_length + header.target + header.sorceid + header.cmd;
      for (int i=0;i<len;i++) chksum+= data[i];

      // Serial.printf("header:%02X,%02X,%02X,%02X,%02X  ",header.addr,header.cmd_length,header.target,header.sorceid,header.cmd);
      // Serial.printf("data:");
      // for(int i=0;i<len;i++) Serial.printf("%02X,",data[i]);
      // Serial.println();

      Serial1.write((uint8_t*) (&header),sizeof(cmd_header));
      Serial1.write(data,len);
      Serial1.write(chksum);
      return *this;
    };
    ioBus& tx(byte cmd,byte addr,byte target,byte sorceid,byte data[],byte len){

      cmd_header header = {addr,0,target,sorceid,cmd};
      return tx(header,data,len);
    };
    void registerMsgCB(OnMsgCB cb){
      msgCB = cb;
    };
  protected:
    OnMsgCB msgCB;
};

ioBus bus;
void do_rxHandle(void *param){
  // ioBus* abus = (ioBus*)param;
  // abus->rxhandle();
  for (;;){
    bus.rxhandle();
    bus.rx1293();
    delay(100);
  }
  // vTaskDelete( NULL );
};
byte error = xTaskCreate(do_rxHandle, "rxhandling", 9120,NULL, 1, NULL);
// byte err = xTaskCreatePinnedToCore(do_rxHandle,"rxhandling",8192,NULL,2,NULL,0);
ioLED8X8::ioLED8X8() {
}

ioLED8X8::ioLED8X8(byte atarget){
  target = atarget;
}

ioLED8X8::~ioLED8X8() {
}

void ioLED8X8::drawPixel(int x, int y, bool invert){
  // if (x < 0 || x >= MaxStrLen*7 || y < 0 || y >= 8) return; 
  // y = 7-y; 
  // for Font12
  if (x < 0 || x >= MaxStrLen*20 || y < 1 || y >= 9) return; 
   y = y-1;
  if (invert) image[(x *8 + y ) / 8] |= 0x80 >> (y % 8);
  else image[(x * 8 + y ) / 8] &= ~(0x80 >> (y % 8));
}

void ioLED8X8::drawPixel(int x, int y) {
  return this->drawPixel(x,y,true);
}

void ioLED8X8::ShiftImg(){
  ShiftImg(8);
}

void ioLED8X8::ShiftImg(byte length) {
    imgCurrent = ++imgCurrent % (imgbyte+length);
    if (target != 0xFF) length = 8;
    else length = (length >> 3) << 3;
    setImg(imgCurrent,4,length);
}

void ioLED8X8::drawChar(char c,int start){
    int i, j;
    unsigned int char_offset = (c - ' ') * Font12.Height * (Font12.Width / 8 + (Font12.Width % 8 ? 1 : 0));
    const unsigned char* ptr = &Font12.table[char_offset];
    // Serial.printf("%c,%u,%u\n",c,char_offset,start);
    for (j = 0; j < Font12.Height; j++) {
        for (i = 0; i < Font12.Width; i++) {
            // if (pgm_read_byte(ptr) & (0x80 >> (i % 8))) this->drawPixel(start + (Font12.Width-i-1), j);
            if (pgm_read_byte(ptr) & (0x80 >> (i % 8))) this->drawPixel(start + i, j);
            if (i % 8 == 7) ptr++;
        }
        if (Font12.Width % 8 != 0) ptr++;
    }
}


ioLED8X8 & ioLED8X8::print(String s) {
  return print(s,4);
}

ioLED8X8 & ioLED8X8::print(String s, int rotation) {
  for (int i=0;i<MaxStrLen*8;i++) image[i] = 0;
  for (int i=0;i<s.length();i++) drawChar(s[i],(Font12.Width-1)*i);
  imgbyte = (Font12.Width -1)*s.length();
  setImg(imgbyte,rotation,imgbyte);
  delay(20);
  return *this;
}

ioLED8X8 & ioLED8X8::print(byte img[8]) {
  // Serial.printf("membercnt:%u\n",bus.memberCnt);
  return print(img,4);
}

ioLED8X8 & ioLED8X8::print(byte img[8], byte rotation) {
  cmd_header header = {0,0,target,0xFE,cmdSetAttribute};
  byte data[11];
  data[0] = OI_LED8x8;
  memcpy(data+1,img,8);
  data[9] = MODE_HOST;
  data[10] = rotation;
  bus.tx(header,data,11);
  delay(10);
  return *this;
}

void ioLED8X8::setImg(byte start) {
  setImg(start,4,8);
}

void ioLED8X8::setImg(byte start,byte rotation,byte imglen) {
  imgCurrent = start;
  cmd_header header = {0,0,target,0xFF,cmdSetAttribute};
  if (imglen ==8) header.sorceid = 0xFE;
  byte data[imglen+3];
  data[0] = OI_LED8x8;
  // memcpy(data+1,image+start,imglen);
  byte shift = imglen;
  for(int i=0;i<imglen;i++)
    if ((start-shift+i > -1) && (start-shift+i < imgbyte)) data[i+1] = image[start-shift+i];
    else data[i+1] = 0;
  
  data[imglen+1] = MODE_HOST;
  data[imglen+2] = rotation;
  bus.tx(header,data,imglen+3);

}

void ioLED8X8::dumpImg() {
  for (int i=0;i<8;i++){
    for (int j=0;j<MaxStrLen*5;j++){
      if (image[j] & 1<<i) Serial.printf("o");
      else Serial.printf(" ");
    };
    Serial.println();

  }
}

void RegisterMsgCB(OnMsgCB cb){
  bus.registerMsgCB(cb);
}

ioRGB::ioRGB() {

}


ioRGB::ioRGB(byte atarget) {
  target = atarget;
}

ioRGB& ioRGB::RGB(byte aR, byte aG, byte aB) {
// PN_LED_R,PN_LED_G,PN_LED_B,PN_LED_R_TARGET,PN_LED_G_TARGET,PN_LED_B_TARGET,PN_LED_TOGGLE_COUNT,PN_LED_INTERVAL
  R = aR>>1; G = aG>>1; B = aB>>1;
  bus.setMode(MODE_HOST);
  delay(100);
  cmd_header header = {0,0,target,0xfe,cmdSetAttribute};
  byte data[7] = {OI_LED_RGB,R,G,B,R,G,B};
  // cmd_header header = {0,0,target,0xfe,cmdObjectSet}; byte data[3] = {OI_LED_RGB,0,R}; bus.tx(header,data,3); delay(100); data[1] = 1; data[2] = G; bus.tx(header,data,3); delay(100); data[1] = 2; data[2] = B;
  bus.tx(header,data,7);
  delay(10);
  return *this;
};

ioRGB& ioRGB::toggle(byte count){
  cmd_header header = {0,0,target,0xfe,cmdSetAttribute};
  byte data[8] = {OI_LED_RGB,R,G,B,R,G,B,count};
  bus.tx(header,data,8);
  delay(10);
  return *this;
}

ioRGB& ioRGB::toggle() {
  return toggle(7);
}

void ioRGB::on() {
  RGB(255,255,255);
}

void ioRGB::off() {
  RGB(0,0,0);
}

ioBuzzer::ioBuzzer() {

}

ioBuzzer::~ioBuzzer() {
}

void ioBuzzer::play(){ 
   cmd_header header = {0,0,target,0xfe,cmdObjectSet};
  byte data[4] = {0x31,0,1,0};
  bus.tx(header,data,4);
  delay(10); 
}

ioMotor::ioMotor(){
}

ioMotor::~ioMotor() {
}

void ioMotor::run(byte dir, byte speed) {
  run(1,dir,speed);
}

void ioMotor::stop() {
  run(0,0,0);
}

void ioMotor::run(byte on, byte dir, byte speed) {
  if (speed > 100) speed = 100;
  if (dir >1) dir = 1;
  cmd_header header = {0,0,target,0xfe,cmdSetAttribute};
  byte data[4] = {OI_MOTOR,on,dir,speed};
  bus.tx(header,data,4);
  delay(10);
}
