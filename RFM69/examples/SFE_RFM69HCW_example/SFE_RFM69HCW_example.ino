#include <Ethernet.h>
#include <Dhcp.h>
#include <EthernetClient.h>
#include <Dns.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>

// RFM69HCW Example Sketch
// Send serial input characters from one RFM69 node to another
// Based on RFM69 library sample code by Felix Rusu
// http://LowPowerLab.com/contact
// Modified for RFM69HCW by Mike Grusin, 4/16

// This sketch will show you the basics of using an
// RFM69HCW radio module. SparkFun's part numbers are:
// 915MHz: https://www.sparkfun.com/products/12775
// 434MHz: https://www.sparkfun.com/products/12823

// See the hook-up guide for wiring instructions:
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

// Uses the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// SparkFun repository: https://github.com/sparkfun/RFM69HCW_Breakout

// Include the RFM69 and SPI libraries:

#include <RFM69.h>
#include <SPI.h>

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID     1   // Must be the same for all nodes (0 to 255)
#define MYNODEID      1   // My node ID (0 to 255)
#define TONODEID      68   // Destination node ID (0 to 254, 255 = broadcast)

// RFM69 frequency, uncomment the frequency of your module:

//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY     RF69_915MHZ

// AES encryption (or not):

#define ENCRYPT       false // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):

#define USEACK        false // Request ACKs or not

// Packet sent/received indicator LED (optional):

#define LED           9 // LED positive pin
#define CS1           8
#define CS2           10
//#define GND           8 // LED ground pin

// Create a library object for our RFM69HCW module:

RFM69 radio;

void setup()
{
  // Open a serial port so we can send keystrokes to the module:
  
  Serial.begin(9600);
  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");  

  // Set up the indicator LED (optional):
  pinMode(CS2, OUTPUT);
  pinMode(LED, OUTPUT);
  //pinMode(GND,OUTPUT);
  //digitalWrite(GND,LOW);
    
  // Initialize the RFM69HCW:

  digitalWrite(CS2, LOW);
  digitalWrite(LED, LOW);
  delay(4);
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.writeReg(0x03,0x0A);
  radio.writeReg(0x04,0x00);
  radio.setHighPower(); // Always use this for RFM69HCW


  // Turn on encryption if desired:
  
  if (ENCRYPT)
  radio.encrypt(ENCRYPTKEY);
}

void loop()
{
  // Set up a "buffer" for characters that we'll send:
  /*Serial.println("Bit rate LSB:");
  Serial.println(radio.readReg(0x04));*/
  
  static char sendbuffer[4];
  for(int i = 0; i<4; i++)
  {
    sendbuffer[i]= 'a';
  }
  static int sendlength = 4;

  // SENDING

  // In this section, we'll gather serial characters and
  // send them to the other node if we (1) get a carriage return,
  // or (2) the buffer is full (61 characters).
  
  // If there is any serial input, add it to the buffer:
  if(radio.sendWithRetry(TONODEID, sendbuffer, sendlength))
  {
    Serial.println("something");
    digitalWrite(LED,HIGH);
  }
  
  delay(1000);
  digitalWrite(LED,LOW);
  delay(1000);
}
