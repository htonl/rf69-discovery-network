/*
 * =====================================================================================
 *
 *       Filename:  Queue.cpp
 *
 *    Description:  The defeinition for the Queue class
 *
 *        Version:  1.0
 *        Created:  03/19/2017 08:19:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dr. Fritz Mehner (mn), mehner@fh-swf.de
 *        Company:  FH Südwestfalen, Iserlohn
 *
 * =====================================================================================
 */

#include <Queue.h>

Queue::Queue(){
    frontPtr = nullptr;
    backPtr = nullptr;
    count = 0;
}


bool Queue::isEmpty()
{
    return (count == 0);
}


bool Queue::enqueue(long data)
{
    if (count < MAXQUEUESIZE) {
        Node* newOne = new Node;
        newOne->date = data;
        newOne->next = nullptr;
        if (isEmpty()) {
            frontPtr = newOne;
        }
        else {
            backPtr->next = newOne;
        }
        backPtr = newOne;
        count++;
        return true;
    }
    return false;
}


long Queue::dequeue()
{
    if (isEmpty()) {
        return 0;
    }
    else {
        long data;
        Node* temp = frontPtr;
        if (frontPtr == backPtr) {
            frontPtr = nullptr;
            backPtr = nullptr;
        }
        else {
            frontPtr = frontPtr->next;
        }
        data = temp->date;
        delete temp;
        count--;
        return data;
    }
}
