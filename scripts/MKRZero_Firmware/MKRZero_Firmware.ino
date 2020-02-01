#include <Servo.h>

int thrusters[8] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}; //value in microseconds between 1100 and 1900 to control thrusters 1-8
Servo T1, T2, T3, T4, T5, T6, T7, T8; //servo objects on pins 3, 5, 6, 9, 10, 11 respectively (328P can not handle more than 6 PWM, production plan is for an ATMSAME51 or ATSAMD21)


const int packetLength = 17;         //Packet structure = See Below
byte packet[packetLength];
boolean packetComplete = false;     // wether the string is complete

//Incomming packet structure: |thruster 1 LSB|thruster 1 MSB|thruster 2 LSB|thruster 2 MSB|
//thruster 3 LSB|thruster 3 MSB|thruster 4 LSB|thruster 4 MSB|thruster 5 LSB|thruster 5 MSB|thruster 6 LSB|
//thruster 6 MSB|thruster 7 LSB|thruster 7 MSB|thruster 8 LSB|thruster 8 MSB|

//For watchdog timer
unsigned long now = 0;
unsigned long lastPacket = 0;  //Two 16 bit ints, steering then thrptle
boolean wdt_isTripped = false; //so  the timer is not tripped continuously

//For sending packet
//unsigned long lastSend = 0;
//unsigned long sendPeriod = 20000; //in microsoconds (1/Hz*1000000)
//const int outGoingPacketLength = 36; 
//Outgoing packet structure: |ch1 LSB|ch1 MSB|ch2 LSB|ch2 MSB|ch3 LSB|ch3 MSB|ch4 LSB|ch4 MSB|ch5 LSB|ch5 MSB|  
// |encoder_left LSB|encoder_left|encoder_left|encoder_left MSB|  
// |encoder_right LSB|encoder_right|encoder_right|encoder_right MSB|  
// |lat LSB|lat|lat|lat MSB| 
// |lon LSB|lon|lon|lon MSB| 
// |speed LSB|speed MSB|angle LSB|angle MSB|altitude LSB|altitude MSB|
// |fix|fix quality|num satelites|
// |PEC|

//Function for converting input (-1000 to 1000) to microseconds (1000 to 2000)
inline int calculateHardwareValues(int input) {
  input = constrain(input, -1000, 1000);
  return map(input, -1000, 1000, -255, 255); //0-255 speend and +/- for direction
}

// Go from 16 bit to 32 bit two's complement
inline int twosComp(int input) {
  if (input & 0b1000000000000000) {
    return (input | 0b11111111111111111000000000000000);
  } else {
    return (input);
  }
}

void setup() {
  
  Serial.begin(115200);

  //Setup Servo pins
  T1.attach(2);
  T2.attach(3);
  T3.attach(4);
  T4.attach(5);
  T5.attach(6);
  T6.attach(7);
  T7.attach(8);
  T8.attach(9);

  //Set thrusters
  T1.writeMicroseconds(thrusters[0]);
  T2.writeMicroseconds(thrusters[1]);
  T3.writeMicroseconds(thrusters[2]);
  T4.writeMicroseconds(thrusters[3]);
  T5.writeMicroseconds(thrusters[4]);
  T6.writeMicroseconds(thrusters[5]);
  T7.writeMicroseconds(thrusters[6]);
  T8.writeMicroseconds(thrusters[7]);

  delay(1000); //delay to make sure thrusters get setup properly
//  Serial.println("Thruster controller initialized");

  lastPacket = millis(); //start watchdog timer for the first packet
  // Other pin setup
  pinMode(LED_BUILTIN, OUTPUT);

  while (!Serial); //Wait to connect to computer

  lastPacket = millis(); //start watchdog timer for the first packet

}

void loop() {
 // Serial.println("CH1: " + String(ch1_duty_cycle) + " CH2: " + String(ch2_duty_cycle) + " CH3: " + String(ch3_duty_cycle) + " CH4: " + String(ch4_duty_cycle) + " CH5: " + String(ch5_duty_cycle));

  now = millis();//get current time to  ensure connection to main contorller

  if (wdt_isTripped || now - lastPacket > 500) { //If the contorller hasn't recived a new packet in half a second (short circuit limits calcs)

    // update thrusters to new values
    T1.writeMicroseconds(1500);
    T2.writeMicroseconds(1500);
    T3.writeMicroseconds(1500);
    T4.writeMicroseconds(1500);
    T5.writeMicroseconds(1500);
    T6.writeMicroseconds(1500);
    T7.writeMicroseconds(1500);
    T8.writeMicroseconds(1500);

    if (wdt_isTripped == false) {
      Serial.println("Drive controller watchdog tripped!");
    }
    wdt_isTripped = true;
  }

  // When a new packet arrives indicated by a newline '\n' char:
  if (packetComplete) {      //If packet is valid
    
  //Incomming packet structure: |thruster 1 LSB|thruster 1 MSB|thruster 2 LSB|thruster 2 MSB|
  //thruster 3 LSB|thruster 3 MSB|thruster 4 LSB|thruster 4 MSB|thruster 5 LSB|thruster 5 MSB|thruster 6 LSB|
  //thruster 6 MSB|thruster 7 LSB|thruster 7 MSB|thruster 8 LSB|thruster 8 MSB|

  for(int i=0;i<packetLength-1;i+=2){
    thrusters[i/2] = constrain(map(twosComp((packet[i+1] << 8) | packet[i]), -1000, 1000, 1000, 2000), 1000, 2000);
  }

    //      Serial.println(packet, BIN);      //DEBUG
    //
    //      Serial.println(left_wheel_cmd, BIN); //DEBUG
    //      Serial.println(left_wheel_cmd);      //DEBUG
    //      Serial.println(right_wheel_cmd, BIN); //DEBUG
    //      Serial.println(right_wheel_cmd);      //DEBUG

    lastPacket = millis(); //Pet the watchdog timer
    wdt_isTripped = false;

    // clear the packet:
    for (int i = 0; i < packetLength; i++) {
      packet[i] = 0;
    }
    packetComplete = false;
  }

  // update thrusters to new values
  T1.writeMicroseconds(thrusters[0]);
  T2.writeMicroseconds(thrusters[1]);
  T3.writeMicroseconds(thrusters[2]);
  T4.writeMicroseconds(thrusters[3]);
  T5.writeMicroseconds(thrusters[4]);
  T6.writeMicroseconds(thrusters[5]);
  T7.writeMicroseconds(thrusters[6]);
  T8.writeMicroseconds(thrusters[7]);
 
//  //Send packet to computer
//  if (micros() - lastSend > sendPeriod) {
//    //Outgoing packet structure: |ch1 LSB|ch1 MSB|ch2 LSB|ch2 MSB|ch3 LSB|ch3 MSB|ch4 LSB|ch4 MSB|ch5 LSB|ch5 MSB|  
//    // |encoder_left LSB|encoder_left|encoder_left|encoder_left MSB|  
//    // |encoder_right LSB|encoder_right|encoder_right|encoder_right MSB|  
//    // |lat LSB|lat|lat|lat MSB| 
//    // |lon LSB|lon|lon|lon MSB| 
//    // |speed LSB|speed MSB|angle LSB|angle MSB|altitude LSB|altitude MSB|
//    // |fix|fix quality|num satelites|
//    // |PEC|
//    
//    byte PEC = outGoingPacket[0];
//    for(int i=1; i<outGoingPacketLength-1;i++){PEC ^= outGoingPacket[i];}
//    outGoingPacket[outGoingPacketLength-1] = PEC;
//
//    Serial.write(outGoingPacket, outGoingPacketLength);
//  }

  if (Serial.available() > 0) {
    serialEvent();
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void serialEvent() {

  byte pecVal = 0; //for Packet Error Checking

  for (int i = 0; i < packetLength; i++) {
    packet[i] = static_cast<byte>(Serial.read()); //Never ever use readBytes()
    if (i < packetLength-1) {
      pecVal = pecVal ^ packet[i]; //XOR with incomming byte
    }
    delayMicroseconds(1000); //Wait to finish adding the incomming byte to the buffer before reading it
  }

  //if packet is good based on PEC byte
  if (pecVal == packet[packetLength-1])
  {
    packetComplete = true;
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    while (Serial.available()) {
      Serial.read(); //Clear the Serial input buffer
    }
    // clear the packet:
    for (int i = 0; i < packetLength; i++) {
      packet[i] = 0;
    }
  }
}
