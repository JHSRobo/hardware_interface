#include "usb_hw_interface/arduino_com.h"

/**
* @breif constrians value between min and max inclusive. Value is returned by reference.
* @param[in,out] value input to be constrianed
* @param[in] min The minimum value that "value" should be able to be
* @param[in] max The maximum value that "value" should be able to be
*/
template <class T>
void constrain(T &value, T min, T max){
    if(value > max){
        value = max;
    } else if(value < min){
        value = min;
    }
}

/**
* @breif returns a number mapped proportioanlly from one range of numbers to another
* @param[in] input Value to be mapped
* @param[in] inMax The maximum value for the range of the input
* @param[in] inMin The minimum value for the range of the input
* @param[in] outMin The minimum value for the range of the output
* @param[in] outMax The maximum value for the range of the output
* @return The input trnslated proportionally from range in to range out
*/
template <class T>
T map(T input, T inMin, T inMax, T outMin, T outMax){
    T output = (input - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    return output;
}


HardwareCom::HardwareCom(std::string port, int baud): connection(port, baud, serial::Timeout::simpleTimeout(10),
                                                                 serial::eightbits, serial::parity_even, serial::stopbits_one)
//Serial set to SER_8E1 (8 bit, even parity, 1 stop bit)
{
  //open serial port
  if(connection.isOpen()){
    std::cout << "Port " << port << " opened successfully!\n";
  } else {
    std::cerr << "Port " << port << " failed to open successfully!\n";
    exit(1);
  }
}

bool HardwareCom::setController(int t1, int t2, int t3, int t4, int t5, int t6, int t7, int t8){
//  std::cout << "rmotor_cmd: " << rmotor_cmd << "\n";
//  std::cout << "lmotor_cmd: " << lmotor_cmd << "\n";

   constrain(t1, -1000, 1000);
   constrain(t2, -1000, 1000);
   constrain(t3, -1000, 1000);
   constrain(t4, -1000, 1000);
   constrain(t5, -1000, 1000);
   constrain(t6, -1000, 1000);
   constrain(t7, -1000, 1000);
   constrain(t8, -1000, 1000);

  //Fill outGoingPacket with four bytes with proper packet structure
   outgoingPacket[15] = static_cast<uint8_t>(t1 >> 8);                 //t1 MSB
   outgoingPacket[14] = static_cast<uint8_t>(t1 & 0b0000000011111111); //t1 LSB
   outgoingPacket[13] = static_cast<uint8_t>(t2 >> 8);                 //t2 MSB
   outgoingPacket[12] = static_cast<uint8_t>(t2 & 0b0000000011111111); //t2 LSB
   outgoingPacket[11] = static_cast<uint8_t>(t3 >> 8);                 //t3 MSB
   outgoingPacket[10] = static_cast<uint8_t>(t3 & 0b0000000011111111); //t3 LSB
   outgoingPacket[9] = static_cast<uint8_t>(t4 >> 8);                 //t4 MSB
   outgoingPacket[8] = static_cast<uint8_t>(t4 & 0b0000000011111111); //t4 LSB
   outgoingPacket[7] = static_cast<uint8_t>(t5 >> 8);                 //t5 MSB
   outgoingPacket[6] = static_cast<uint8_t>(t5 & 0b0000000011111111); //t5 LSB
   outgoingPacket[5] = static_cast<uint8_t>(t6 >> 8);                 //t6 MSB
   outgoingPacket[4] = static_cast<uint8_t>(t6 & 0b0000000011111111); //t6 LSB
   outgoingPacket[3] = static_cast<uint8_t>(t7 >> 8);                 //t7 MSB
   outgoingPacket[2] = static_cast<uint8_t>(t7 & 0b0000000011111111); //t7 LSB
   outgoingPacket[1] = static_cast<uint8_t>(t8 >> 8);                 //t8 MSB
   outgoingPacket[0] = static_cast<uint8_t>(t8 & 0b0000000011111111); //t8 LSB

   //PEC by XORing all values
   outgoingPacket[outgoingPacketLength-1] = outgoingPacket[0];
   for(int i=1; i<outgoingPacketLength-1;i++){outgoingPacket[outgoingPacketLength-1]^=outgoingPacket[i];}

//  std::cout << "packet down: " <<  std::bitset<8>(outgoingPacket[4]) << std::bitset<8>(outgoingPacket[3]) << std::bitset<8>(outgoingPacket[2]) << std::bitset<8>(outgoingPacket[1]) << std::bitset<8>(outgoingPacket[0]) << "\n";

  size_t bytesSent = connection.write(outgoingPacket, outgoingPacketLength); //Sending 17 bytes

  if(bytesSent != outgoingPacketLength){
    std::cerr << "Number of bytes (" << bytesSent << ") sent not equal to seventeen!\n";
    return false;
  }
//  std::cout << "packet sent\n";
  return true;
}

bool HardwareCom::readController(){
//    if(connection.available()>=incomingPacketLength){
//        connection.read(incomingPacket, incomingPacketLength);
//        connection.flushInput();
//
//        //Print out the entire packet
////        std::cout << "Packet received:   ";
////        for(int x=0; x<incomingPacketLength; x++){
////            std::cout << std::bitset<8>(incomingPacket[x]);
////        }
////        std::cout << "\n";
//
//        uint8_t pec = incomingPacket[0];
//        for(int i=1; i<incomingPacketLength-1;i++){pec^=incomingPacket[i];}
//        if(pec == static_cast<uint8_t>(incomingPacket[incomingPacketLength-1])){
////            std::cout << "PEC Correct\n";
//            //Fill in channel array with distance sensor values
//            for(int i =0; i<8; i+=2){channels[i/2] = static_cast<int>(incomingPacket[i] | (incomingPacket[i+1]<<8));}
//
//            return true;
//        } //check PEC byte
//        return false;
//    }
    return false;
}

