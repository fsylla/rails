
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


Train::Train(uint16_t nid, uint16_t nhead, uint16_t ntail)
{
    id   = nid;
    dest = nhead;
    head = nhead;
    tail = ntail;
}


Train::~Train()
{
}


void            Train::dump()
{
    printf("Train %3d dest = %3d head = %3d tail = %3d\n", id, dest, head, tail);
}


uint16_t        Train::getId()
{
    return(id);
}


uint16_t        Train::getDest()
{
    return(dest);
}


uint16_t        Train::getHead()
{
    return(head);
}


uint16_t        Train::getTail()
{
    return(tail);
}


void            Train::setDest(uint16_t ndest)
{
    dest = ndest;
}


void            Train::setHead(uint16_t nhead)
{
    head = nhead;
}


void            Train::setTail(uint16_t ntail)
{
    tail = ntail;
}

