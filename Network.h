/*
 * =====================================================================================
 *
 *       Filename:  Network.h
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
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <Packet.h>
#include <Queue.h>

#ifndef Network_h
#define Network_h
//radio globals
//#define FREQUENCY     RF69_915MHZ
#define INTERRUPTPIN 2
#define CHIPSELECTPIN 10
    //RetVal
    enum retVal { SUCCESS, 		//Generic Success
        		  FAIL, 		//Generic Failure
        		  NOMESSAGE, 	//No message received to read in
        		  NOACK, 		//An ACK was requested and not received
        		  IGNORED, 		//A message was received that we don't need to do anything for
        		  DROPPED }; 	//Dropped coord accses
//Opcodes
#define DTRANSMISSION 0x01 //dataOwner, data0, data1, ... , dataN use dSize to determin N
#define DROPPEDCOORD 0x02 //no data
#define ASKFORCOORD 0x03 //no data
#define IHAVECOORD 0x04 //no data
#define IAMCOORD 0x05 //no data

/*Defining the packet*/
#define MAXDATASIZE 59
#define BROADCASTADDRESS 255
#define COORDLISTENTIMEOUT 10
#define SENDTIMEOUT 500
#define SENDRETRIES 5
#define RECEIVETIMEOUT 2500
#define MINRSSI -128

//These functions are just placeholders currently, as this code is currently
//in development.
//
class Network {
public:
    //default construct
    Network();

    //Network Functions:
    //inits the network (main constructor)
    void initNetwork();

    //handles the every wake network events
    void runNetwork();

    //The data to the network class for forwarding to the coordinator
    bool sendOverNetwork(long data){ return dataQueue.enqueue(data); };

	//Set Coord (For testing purposes)
	void setCoord() {this->amCoord = true; };

	//Set nextHop (For testing)
	void setNextHop(int hop) { this->nextHop = hop; this->haveCoord = true;};

	//Set myID (For testing)
	void setmyID(int id) { this->myID = id;};


private:
    //fill a packet class object with data that was received
    retVal readPacket(Packet* p);

    //send a packet
    retVal sendPacket(Packet* p);

    //fill a packet class object with data to send
    void createPacket(uint8_t opCode, uint8_t sAddr, uint8_t dAddr, uint8_t dSize, uint8_t data[], Packet* p);

    //returns true if the chip running this fnc is coord
    bool checkIfCoord(){ return this->amCoord; };

    //finds a path to coord
    void lookForCoord();

    //inform network that we have coord connection
    void foundCoord();

    //inform network that we dropped coord connection and update associated variables
    void droppedCoord();

    //decide what to do with a received packet
    long receivedPacket(Packet* p);

    //void setPath(uint8_t p[])

    RH_RF69 driver; //the radio object, Chip select 5, interrupt pin 13
    RHReliableDatagram radio;
    uint8_t myID; //TODO:set this from EEPROM in initialization
    //bool useAck = true; //do we want acks
    //bool 		encrypt = false;   //TODO:this will be implemented last
    //char 		*encryptKey;       //TODO:this will be implemented last
    //uint8_t networkID = 0;
    uint8_t nextHop;
    int8_t currentRSSI = MINRSSI;
    bool haveCoord = false;
    bool amCoord = false;
    bool reconnected = false;
    int noPacketReceived;
    Queue dataQueue;
};

#endif
