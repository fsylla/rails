
/* ____________________________________________________________________________
 *
 * file:        train.h
 * created:     Wed Feb 22 22:55:24 2017
 * by:          frank
 *
 * description: trains are moving in nodemaps
 * ____________________________________________________________________________
 */


#ifndef TRAIN_H
#define TRAIN_H


#include <stdint.h>


class Train
{
public:
    Train(uint16_t nid, uint16_t nhead, uint16_t ntail);
    ~Train();
    void        dump();
    uint16_t    getId();
    uint16_t    getDest();
    uint16_t    getHead();
    uint16_t    getTail();
    void        setDest(uint16_t ndest);
    void        setHead(uint16_t nhead);
    void        setTail(uint16_t ntail);

private:
    uint16_t    id;
    uint16_t    dest;
    uint16_t    head;
    uint16_t    tail;
};


#endif

