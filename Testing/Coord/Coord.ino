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

  net.setmyID(1);
  net.setCoord();
  net.initNetwork();
}

void loop() {
  long opcode = 100;
  // put your main code here, to run repeatedly:
  Serial.println("enter run");
  net.runNetwork();
  Serial.println("exit run");
//  delay(10);
}
