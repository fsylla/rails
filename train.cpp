
/* ____________________________________________________________________________
 *
 * file:        train.cpp
 * created:     Wed Feb 22 22:55:24 2017
 * by:          frank
 *
 * description: trains are moving in nodemaps
 * ____________________________________________________________________________
 */


#include <stdio.h>

#include "train.h"


/*
 *  Default constructor required for std::map<uint16_t,Rail> in class Nodemap
 *  Should never be used.
 */

Train::Train()
{
    Train(0, 0, 0);
}


Train::Train(uint16_t nhead, uint16_t ntail, double nlength)
{
    dest        = nhead;
    head        = nhead;
    headPos     = 0.0;
    headRail    = 0;
    length      = nlength;
    speed       = 0.0;
    speedMax    = 0.1;
    tail        = ntail;
    tailPos     = 0.0;
    tailRail    = 0;
}


Train::~Train()
{
}


void            Train::dump(uint16_t id)
{
    printf("Train %5d length = %8.2f\n", id, length);
    printf("              dest = %5d   speed = %8.2f\n", dest, speed);
    printf("              head = %5d headPos = %8.2f headRail = %5d\n", head, headPos, headRail);
    printf("              tail = %5d tailPos = %8.2f tailRail = %5d\n", tail, tailPos, tailRail);
    printf("\n");
}


uint16_t        Train::getDest()
{
    return(dest);
}


uint16_t        Train::getHead()
{
    return(head);
}


double          Train::getHeadPos()
{
    return(headPos);
}


uint16_t        Train::getHeadRail()
{
    return(headRail);
}


double          Train::getLength()
{
    return(length);
}


double          Train::getSpeed()
{
    if (head != dest) {
        if (speed < speedMax) {
            speed   = speed + 0.002 * (speedMax - speed);
        }
    } else {
        if (headPos > -20 * speedMax && speed > 0.001) {
            speed   = -headPos / 20;
        }
    }

    return(speed);
}


double          Train::getSpeedMax()
{
    return(speedMax);
}


uint16_t        Train::getTail()
{
    return(tail);
}


double          Train::getTailPos()
{
    return(tailPos);
}


uint16_t        Train::getTailRail()
{
    return(tailRail);
}


void            Train::setDest(uint16_t ndest)
{
    dest        = ndest;
}


void            Train::setHead(uint16_t nhead)
{
    head        = nhead;
}


void            Train::setHeadPos(double nheadPos)
{
    headPos     = nheadPos;
}


void            Train::setHeadRail(uint16_t nheadRail)
{
    headRail    = nheadRail;
}


void            Train::setLength(double nlength)
{
    length      = nlength;
}


void            Train::setSpeed(double nspeed)
{
    speed       = nspeed;
}


void            Train::setSpeedMax(double nspeedMax)
{
    speedMax    = nspeedMax;
}


void            Train::setTail(uint16_t ntail)
{
    tail        = ntail;
}


void            Train::setTailPos(double ntailPos)
{
    tailPos     = ntailPos;
}


void            Train::setTailRail(uint16_t ntailRail)
{
    tailRail   = ntailRail;
}

