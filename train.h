
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
    Train();
    Train(uint16_t nhead, uint16_t ntail, double nlength);
    ~Train();
    void        dump(uint16_t id);

    uint16_t    getDest();
    uint16_t    getHead();
    double      getHeadPos();
    uint16_t    getHeadRail();
    double      getLength();
    double      getSpeed();
    double      getSpeedMax();
    uint16_t    getTail();
    double      getTailPos();
    uint16_t    getTailRail();
    void        setDest(uint16_t ndest);
    void        setHead(uint16_t nhead);
    void        setHeadPos(double nheadPos);
    void        setHeadRail(uint16_t nheadRail);
    void        setLength(double nlength);
    void        setSpeed(double nspeed);
    void        setSpeedMax(double nspeedMax);
    void        setTail(uint16_t ntail);
    void        setTailPos(double ntailPos);
    void        setTailRail(uint16_t ntailRail);

private:
    uint16_t    dest;
    uint16_t    head;
    double      headPos;
    uint16_t    headRail;
    double      length;
    uint16_t    tail;
    double      tailPos;
    uint16_t    tailRail;
    double      speed;
    double      speedMax;
};


#endif

