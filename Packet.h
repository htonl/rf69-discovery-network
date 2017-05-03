/*
 * =====================================================================================
 *
 *       Filename:  Packet.h
 *
 *    Description:  Packet class used for network-logger
 *
 *        Version:  1.0
 *        Created:  03/19/2017 08:22:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dr. Fritz Mehner (mn), mehner@fh-swf.de
 *        Company:  FH Südwestfalen, Iserlohn
 *
 * =====================================================================================
 */
#ifndef PACKET_H
#define PACKET_H
#define MAXDATASIZE 60
class Packet {
public:
    Packet(){}

    //getters
    inline uint8_t getopCode() { return opCode; }
    inline uint8_t getsAddr() { return sAddr; }
    inline uint8_t getdAddr() { return dAddr; }
    inline uint8_t getdSize() { return dSize; }
    inline uint8_t getData(int index) { return data[index]; }
    inline uint16_t getRSSI() { return RSSI; }

    //setters
    inline void setopCode(uint8_t code) { this->opCode = code; }
    inline void setsAddr(uint8_t sAddr) { this->sAddr = sAddr; }
    inline void setdAddr(uint8_t dAddr) { this->dAddr = dAddr; }
    inline void setdSize(uint8_t dSize) { this->dSize = dSize; }
    inline void setdata(uint8_t data, int index) { this->data[index] = data; }
    inline void setRSSI(uint16_t RSSI) { this->RSSI = RSSI; }

private:
    //parts of the packet structure
    uint8_t opCode; //Operation we are performing (data transmission, I have coord, etc.)
    uint8_t sAddr; //Source address
    uint8_t dAddr; //Destination address
    uint8_t dSize; //Size of info in data
    uint8_t data[MAXDATASIZE]; //The data (optional)
    uint16_t RSSI; //the signal strength
};

#endif
