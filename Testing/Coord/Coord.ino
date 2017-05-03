#include <Packet.h>
#include <Network.h>
//#include <Logger.h>
#include <Queue.h>

#define LED           9 // LED positive pin
#define CS1           8
#define CS2           10

Network net;
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Node ");
  Serial.println(" ready");

  pinMode(CS2, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(3, INPUT);

  digitalWrite(CS2, LOW);
  digitalWrite(LED, LOW);
  delay(4);

  net.setmyID(1);
  net.setCoord();
  net.initNetwork(1);
}

void loop() {
  long opcode = 100;
  // put your main code here, to run repeatedly:
  Serial.println("enter run");
  net.runNetwork();
  Serial.println("exit run");
//  delay(10);
}
