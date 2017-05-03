/*
 * =====================================================================================
 *
 *       Filename:  Network.cpp
 *
 *    Description:  Protocol used for radio communication using the ALogger
 *
 *        Version:  1.0
 *        Created:  02/01/2017 11:26:22 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Luke Cesarz, Jeff Worm, David Nickel, Robert Schulz, Ying
 *                  Yang
 *     University:  University of Minnesota TC
 *
 * =====================================================================================
 *
## LICENSE: GNU GPL v3

Network.h is part of Logger, an Arduino library written by Andrew D. Wickert
and Chad T. Sandell.

Copyright (C) 2011-2015, Andrew D. Wickert<br>
Copyright (C) 2016, Andrew D. Wickert and Chad T. Sandell

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//TODO: RSSI format

#include <Network.h>
#include <Queue.h>
#include <Packet.h>
#define DEBUG
//Default Constructor
Network::Network():
    driver(CHIPSELECTPIN,INTERRUPTPIN),
    radio(driver) {}

void Network::initNetwork()
{
    //TODO: set myID from EEPROM
    //this->myID = EEPROM.read(idaddress);
    this->radio.setRetries(SENDRETRIES);
    this->radio.setTimeout(SENDTIMEOUT);
    //this->networkID = networkID;
    //radio.initialize(RF69_915MHZ, this->myID, this->networkID);
    this->radio.init();
    this->radio.setThisAddress(this->myID);
    Serial.println(this->radio.thisAddress());
    //radio.writeReg(0x03, 0x0A);
    //radio.writeReg(0x04, 0x00);
    //radio.setHighPower();
    //TODO: change bitrate
    //TODO: check usb connection and (un)set amCoord then (do nothing)broadcast i am coord
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Fills a packet class object with data from a packet that
 * that has been received. Returns NOMESSAGE if there was
 * nothing to be received, or returns SUCCESS
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
retVal Network::readPacket(Packet* p)
{
    uint8_t len = this->driver.maxMessageLength();
    uint8_t buf[len];
    uint8_t to, from;
    if (this->radio.recvfromAckTimeout(buf, &len, RECEIVETIMEOUT, &from, &to)) {
	      delay(15);
        p->setdSize(len - 1);
        p->setsAddr(from);
        p->setdAddr(to);
        p->setopCode(buf[0]);
        p->setRSSI(this->driver.rssiRead());
        for (int i = 0; i < p->getdSize(); i++) {
            p->setdata(buf[i + 1], i);
        }
        for (int i = p->getdSize(); i < MAXDATASIZE; i++) {
            p->setdata(0, i);
        }
        return SUCCESS;
    }
    return NOMESSAGE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Sends the packet to the node identified by p->getdAddr()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
retVal Network::sendPacket(Packet* p)
{
    uint8_t data[p->getdSize() + 1];
    data[0] = p->getopCode();
    for (int i = 0; i < p->getdSize(); i++) {
        data[i + 1] = p->getData(i);
    }
    if (this->radio.sendtoWait(data, p->getdSize() + 1, p->getdAddr())) {
        return SUCCESS;
    }
    else {
        return NOACK;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Fills the packet pointed to by p with the info passed in
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Network::createPacket(uint8_t opCode, uint8_t sAddr, uint8_t dAddr, uint8_t dSize, uint8_t data[], Packet* p)
{
    p->setopCode(opCode);
    p->setsAddr(sAddr);
    p->setdAddr(dAddr);
    p->setdSize(dSize);
    for (int i = 0; i < dSize; i++)
        p->setdata(data[i], i);
    for (int i = dSize; i < MAXDATASIZE; i++)
        p->setdata(0, i);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Sends a broadcast packet to the network looking for
 * the coordinator node
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Network::lookForCoord()
{
    //Serial.println("LFC");
    Packet *p = new Packet();
    createPacket(ASKFORCOORD, this->myID, BROADCASTADDRESS, 0, NULL, p);
    sendPacket(p);
    delete p;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Sends a broadcast packet to the network saying that
 * we have a connection to coord
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Network::foundCoord()
{
    Packet *p = new Packet();
    createPacket(IHAVECOORD, this->myID, BROADCASTADDRESS, 0, NULL, p);
    sendPacket(p);
    delete p;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Sends a broadcast packet to the network saying that
 * we have dropped connection to coord and sets appropriate
 * variables for the loss of connection
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void Network::droppedCoord(){
    this->haveCoord = false;
    this->currentRSSI = MINRSSI;
    Packet *p = new Packet();
    createPacket(DROPPEDCOORD, this->myID, BROADCASTADDRESS, 0, NULL, p);
    sendPacket(p);
    delete p;
}

void Network::runNetwork()
{
    //wake up radio
    this->noPacketReceived = 0;
    Packet *p = new Packet();
    if(this->amCoord){
		    //listen
        //TODO figure out these timeouts, there are infinite ways to do them,
        //but I think this way will work: have a counter on the number of times
        //a packet was not received and once it exceeds COORDLISTENTIMEOUT then
        //stop listening we also tune the sleeps accordingly.
        while(noPacketReceived < COORDLISTENTIMEOUT){
            if(readPacket(p) != NOMESSAGE){
                receivedPacket(p);
            }
            else{
                ++noPacketReceived;
            }
            delay(10);
            //TODO sleep for some amount of time. Can change the sleep time depending
            //on whether or not we received a packet.
        }
	}
	else if(this->haveCoord){
		    //send yourdata
        uint8_t data[MAXDATASIZE];
        uint8_t index;
        long d;
        while(!dataQueue.isEmpty()){
            data[0] = this->myID;
            for(index = 1 ; index<(MAXDATASIZE-3) && !dataQueue.isEmpty();){ //if index == MAXDATASIZE-4 then one more data will fit
                d = dataQueue.dequeue();
                data[index++] = d >> 24;
                data[index++] = d >> 16;
                data[index++] = d >> 8;
                data[index++] = d;
            }
            uint8_t failedSend = 0;
            createPacket(DTRANSMISSION, this->myID, this->nextHop, index, data, p);
            if(sendPacket(p) == NOACK){
              //handle what happens on timeout (Never get ACK'd)
              droppedCoord();
              delete p;
              //radio.sleep();
              return;
            }
        }
		    // //listen forward
        // bool dropped = false;
        // while(readPacket(p) == SUCCESS){
        //     if(receivedPacket(p) == DROPPED){
        //       dropped = true;
        //       break;
        //     }
        // }
        // if(dropped){
        //   droppedCoord();
        // }
	}
	else{
        //TODO: While not reconnected?
		    //ask for coord
        lookForCoord(); //NOTE: from Jeff, I think we should send this every so many (5 maybe? TDB) noPacketReceived
        delay(20);
		    //listen and choose best next hop
        while(readPacket(p) ==  SUCCESS){ //read packet blocks until a message is received or the timeout occurs
            receivedPacket(p);
        }
        //next best hop is selected in receivedPacket()
        //after we have listened long enough and we have
        //reconnected we can say we have coord again and
        //move on to the next round of runNetwork()
        if(this->reconnected){
          this->reconnected = false;
          this->haveCoord = true;
          foundCoord();
        }
	}
  delete p;
}

long Network::receivedPacket(Packet* p)
{

    uint8_t opCode = p->getopCode();
    uint8_t failedSend;
    if(this->amCoord){
      switch(opCode){
        case DTRANSMISSION:
          //TODO: upload to server;
      	  // for(int i = 0; i < p->getdSize(); i++){
          //       	Serial.println(p->getData(i));
      	  // }
          #ifdef DEBUG
          Serial.print("Received data from: ");
          Serial.println(p->getsAddr());
          Serial.print("Data belongs to: ");
          Serial.println(p->getData(0));
          Serial.print("LSB of first data: ");
          Serial.println(p->getData(5));
          #endif
          return SUCCESS;
        case ASKFORCOORD:
          //tell the asking node that you are coord
          #ifdef DEBUG
          Serial.print("Asked for access from: ");
          Serial.println(p->getsAddr());
          #endif
          failedSend = 0;
          createPacket(IAMCOORD, this->myID, p->getsAddr(), 0, NULL, p);
          if(sendPacket(p) == NOACK){
            #ifdef DEBUG
            Serial.println("Failed to tell amCoord");
            #endif
            return FAIL; //if we couldn't get them the message then our RSSI is probibly to weak
          }
          #ifdef DEBUG
          Serial.println("succeded to tell amCoord");
          #endif
          return SUCCESS;
        default:
          return IGNORED;
      }
    }
    else if(this->haveCoord){
      switch(opCode){
        case DTRANSMISSION:
          //send data up to the next hop
          #ifdef DEBUG
          Serial.print("Received data from: ");
          Serial.println(p->getsAddr());
          Serial.print("Sending to next hop: ");
          Serial.println(this->nextHop);
          #endif
          failedSend = 0;
          p->setdAddr(this->nextHop);
          if(sendPacket(p) == NOACK){
            #ifdef DEBUG
            Serial.println("Failed to send, dropped coord");
            #endif
            return DROPPED;
          }
          #ifdef DEBUG
          Serial.println("Succeded in retransmiting data");
          #endif
          return SUCCESS;
        case DROPPEDCOORD:
          //if your next hop dropped coord, then so did you
          if(p->getsAddr() == this->nextHop){
            #ifdef DEBUG
            Serial.println("Next hop dropped coord so I did as well");
            #endif
            return DROPPED;
          }
          return IGNORED;
        case ASKFORCOORD:
          //tell the asking node that you have coord access
          #ifdef DEBUG
          Serial.print("Asked for access from: ");
          Serial.println(p->getsAddr());
          #endif
          failedSend = 0;
          createPacket(IHAVECOORD, this->myID, p->getsAddr(), 0, NULL, p);
          if(sendPacket(p) == NOACK){
            #ifdef DEBUG
            Serial.println("Failed to tell haveCoord");
            #endif
            return FAIL; //if we couldn't get them the message then our RSSI is probibly to weak
          }
          #ifdef DEBUG
          Serial.println("Succeded to tell haveCoord");
          #endif
          return SUCCESS;
        default:
          return IGNORED;
      }
    }
    else{
      switch(opCode){
        case IAMCOORD:
	        //TODO: may want to differentiate these later by prioritizing connecting directly to the coord
          #ifdef DEBUG
          Serial.print("New connection found: ");
          Serial.println(p->getsAddr());
          Serial.print("RSSI: ");
          Serial.println(p->getRSSI());
          #endif
          if(p->getRSSI() >= MINRSSI && p->getRSSI() >= currentRSSI){
            #ifdef DEBUG
            Serial.println("Choose new next hop");
            #endif
            this->reconnected = true;
            this->currentRSSI = p->getRSSI();
            this->nextHop = p->getsAddr();
            return SUCCESS;
          }
          return IGNORED;
        case IHAVECOORD:
          #ifdef DEBUG
          Serial.print("New connection found: ");
          Serial.println(p->getsAddr());
          Serial.print("RSSI: ");
          Serial.println(p->getRSSI());
          #endif
          if(p->getRSSI() >= MINRSSI && p->getRSSI() >= currentRSSI){
            #ifdef DEBUG
            Serial.println("Choose new next hop");
            #endif
            this->reconnected = true;
            this->currentRSSI = p->getRSSI();
            this->nextHop = p->getsAddr();
            return SUCCESS;
          }
          return IGNORED;
        case DTRANSMISSION:
          //let the node that just sent us data know that we don't have coord access anymore
          failedSend = 0;
          createPacket(DROPPEDCOORD, this->myID, p->getsAddr(), 0, NULL, p);
          if(sendPacket(p) == NOACK){
            return FAIL; //if we couldn't get them the message then our RSSI is probibly to weak
          }
          return SUCCESS;
        default:
          return IGNORED;
      }
    }
}
