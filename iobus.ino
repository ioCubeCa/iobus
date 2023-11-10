// #define MaxStrLen 10
#include <Wire.h>
#include <Arduino.h>
#include "ioCube.h" 
ioLED8X8 led8x8;
ioRGB rgb;
// ioBuzzer buzzer;
ioMotor motor;
bool run=false;
byte shift_len = 16;
void setup(){
  Serial.begin(115200);
  RegisterMsgCB([](byte msg_from, byte msg_cmd,byte data[],byte datalen){
    if (msg_cmd == cmdInputState && data[1]==0){ 
      // btn click up,data==> [id],[0->up,1->down]
      // data[0]=16  --> distance
      // data[0]=1 square  (EC_INPUT_S)
      // data[0]=2 circle  (EC_INPUT_O)
      // data[0]=4 cross   (EC_INPUT_X)
      // data[0]=3 square && circle  (EC_INPUT_SO)
      // data[0]=5 square && cross  (EC_INPUT_SX)
      // data[0]=6 circle && cross  (EC_INPUT_OX)
      // data[0]=7 3 btn (EC_INPUT_SOX)

      run = false;
      rgb.RGB(128,0,0).toggle();
      byte img[8] = {0,96,98,2,2,98,96,0};  //smile -90 degree
      // 0  -> 0 0 0 0 0 0 0 0
      //100 -> 0 1 1 0 0 1 0 0
      //98  -> 0 1 1 0 0 0 1 0
      //2   -> 0 0 0 0 0 0 1 0
      //2   -> 0 0 0 0 0 0 1 0
      //98  -> 0 1 1 0 0 0 1 0
      //100 -> 0 1 1 0 0 1 0 0
      // 0  -> 0 0 0 0 0 0 0 0
      
      delay(500);
      switch (data[0]) {
        case EC_INPUT_S:motor.run(1,80); led8x8.print("Coffreedom");run=true; break;
        case EC_INPUT_O:motor.run(0,90); rgb.RGB(0,0,255).toggle(); break;
        case EC_INPUT_X:motor.stop(); rgb.RGB(255,0,0).toggle(6);led8x8.print(img,1); break;
        case EC_INPUT_SO:led8x8.print("ZZzzzz"); break;
        case EC_INPUT_SX:led8x8.print("5"); break;
        case EC_INPUT_OX:led8x8.print("6"); break;
        case EC_INPUT_SOX:led8x8.print("7"); break;      
        case 16: //distance 
          Serial.printf("distance:%u\n",data[1]);
        
          break; 
      default: break;
      }
    }
  });
  delay(3000); //Take some time to open up the Serial Monitor
  byte img[8] = {0,100,98,2,2,98,100,0};
  led8x8.print(img,1);
  delay(1000);
  rgb.RGB(255,0,0).toggle(10);
  delay(1000);
  // motor.run(1,99);
  // buzzer.play();
  #define bleStateLed 10
  pinMode(bleStateLed,OUTPUT);
  digitalWrite(bleStateLed,HIGH);
  delay(1000);
  digitalWrite(bleStateLed,LOW);
}
int cnt=0;
uint8_t recvBuffer[3] = {0,0,0};
void loop(){
  delay(300);
  if (run) led8x8.ShiftImg(shift_len);
}
