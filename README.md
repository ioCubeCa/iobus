# iobus
It's a simple for controlling motor, LED 8*8, and RGB module.

## How to Use
### Arduino
1.Find out ioCube device has been connected to yourPC.
2.Open this arduino project
3.First, check the environment (Board and Port)
  * Tools>Board>esp32>ESP32C3 Dev Module
  ![image](https://github.com/ioCubeCa/iobus/blob/main/iobusselectboard.png)
  * Port>/dev/XX.usbseriaXX  or in termial command: ls /dev/tty*
   ![image](https://github.com/ioCubeCa/iocube_master_C3/blob/main/Portimg.png)
   ![image](https://github.com/ioCubeCa/iocube_master_C3/blob/main/terminalimg.png)
4. Upload your program to ioCube device.

### Platformio IDE via VSCode
Open up VSCode and check your iocube device port, then executing the project. The file named 'platformio.ini' already setting up.
![image](https://github.com/ioCubeCa/iobus/blob/main/platformioimg.png)



