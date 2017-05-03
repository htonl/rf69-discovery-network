/*
 * =====================================================================================
 *
 *       Filename:  Queue.h
 *
 *    Description:  The queue class for the network-logger taken from
 *                  cplusplus.com
 *
 *        Version:  1.0
 *        Created:  03/19/2017 08:18:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dr. Fritz Mehner (mn), mehner@fh-swf.de
 *        Company:  FH Südwestfalen, Iserlohn
 *
 * =====================================================================================
 */

#ifndef QUEUE_H
#define QUEUE_H
#define MAXQUEUESIZE 100 //TBD maybe even remove
//#include <cstddef>

class Queue{
public:
    Queue();
    bool isEmpty();
    bool enqueue(long data);
    long dequeue();

private:
    struct Node {
        long date;
        Node* next;
    };

    Node* frontPtr;
    Node* backPtr;
    int count;
};

#endif // QUEUE_H
