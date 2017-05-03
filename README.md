Logger
======
This code is heavily based off the Alog library by Northern Widget. When
completed, this implementation will include radio telemetry using the RFM69 chip
for RF communication. This library is currently a work in progress and should
not be expected to work on its own. Expect a working copy in early May.
The new libraries in this repo are the Network.cpp, Network.h, Queue.cpp,
Queue.h, and Packet.h files. In total about 650 lines of code were added.

How to use the Network:

In order to use the network in the Logger Library, the user need only call 2
functions: sendOverNetwork(float data) with a floating point of the data that
they want to send to the coord, and then runNetwork(). runNetwork() is the
heart of the network library. It takes care of keeping up each nodes connection
to the coordinator, and handling any packets it receives or needs to send. A 
more detailed description of the algorithm can be found below, but these two
functions are all the end user NEEDS to get the network rolling.

A more detailed description:

The network algorithm works like so in pseudocode:

runNetwork()
{
	//condition 1 we are the coord listen for data
	if(IamCoord)
	{
		listen for incoming data and upload it to server
	}
	//condition 2 we have a connection to the coord, send data
	else if(IhaveConnectionToCoord)
	{
		send all of my data towards coord
		listen for data to forward to coord
	}
	//condition 3 we do not have connection to the coord
	else
	{
		try to connect to coordinator
	}
}
A few notable points in this algorithm. Originally, we were trying to forward
the data from the furthest leaf nodes to the router nodes first, then forward it
to the coordinator, but there is a nightmare of timing issues and other
complexities. Then we decided to do the opposite:send the data from the closest
nodes to the coordinator first. Therefore in condition 2, all the nodes that
aren't coord will be sending their data over and over again until it finally
reaches the coordinator. Then they will start listening for other nodes that
are using them as a next hop to get to the coord, and forward that data. This
eliminates a lot of the timing issues, and the need for each node to know the
other nodes depending on it.


Contributors to the network enabled ALog: Luke Cesarz, Jeff Worm, David Nickel,
Ying Yang, Bobby Schulz

Data logger library for the Arduino-based ALog (http://northernwidget.com/?page_id=8)
