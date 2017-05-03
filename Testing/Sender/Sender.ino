#include <Packet.h>
#include <Network.h>
//#include <Logger.h>
#include <Queue.h>


Network net;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Node ");
  Serial.println(" ready");

  delay(4);

  net.setmyID(2);
  net.initNetwork();
  //net.setNextHop(1);
  //net.setCoord();
}

void loop() {
  static long opcode = 0x33333333L;
  // put your main code here, to run repeatedly:
  net.sendOverNetwork(opcode);
  opcode++;
  delay(4000);
  net.runNetwork();
}
