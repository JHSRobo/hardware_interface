#ifndef ARDUINO_COM_H
#define ARDUINO_COM_H

#include <unistd.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <bitset>

#include "serial/serial.h"

typedef union
{
    float number;
    uint8_t bytes[4];
} FLOATUNION_t;


class HardwareCom
{
  //Serial object that manages USB serial connection
  serial::Serial connection;
  const int outgoingPacketLength = 17;
  uint8_t outgoingPacket[17];   //Packet to the arduino (length = 16 + PEC)
  //Outgoing packet structure: |PEC|thruster 1 LSB|thruster 1 MSB|thruster 2 LSB|thruster 2 MSB|
  //thruster 3 LSB|thruster 3 MSB|thruster 4 LSB|thruster 4 MSB|thruster 5 LSB|thruster 5 MSB|thruster 6 LSB|
  //thruster 6 MSB|thruster 7 LSB|thruster 7 MSB|thruster 8 LSB|thruster 8 MSB|

//  const int incomingPacketLength = 36;
//  uint8_t incomingPacket[36]; //Packet from the zero (length = 12 + PEC)


public:
  HardwareCom(std::string port, int baud);            //Constructor
  bool setController(int t1, int t2, int t3, int t4, int t5, int t6, int t7, int t8); //Sends input commands to arduino zero
  bool readController();                            //Returns data read from the arduino zero
  int t1, t2, t3, t4, t5, t6, t7, t8;
};

#endif
