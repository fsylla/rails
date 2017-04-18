
/* ____________________________________________________________________________
 *
 * file:	rail.h
 * created:	Fri Feb 12 19:21:11 2016
 * by:		frank
 *
 * description:	rails are connections between two nodes n1 and n2
 *              drawn as straight lines or arcs
 *              a rail with radius 0 is assumed to be a straight line
 * ____________________________________________________________________________
 */


#ifndef RAIL_H
#define RAIL_H

#define RAIL_SIZE_POINTS                5


#include <stdint.h>


class Rail
{
public:
    Rail();
    Rail(uint16_t nn1, uint16_t nn2);
    ~Rail();
    int         initArc(double nx1, double ny1, double nx2, double ny2, double nr);
    int         initLine(double nx1, double ny1, double nx2, double ny2);
    void        clear();
    void        dump(uint16_t id);
    int         fill(uint16_t n, double pos1, double pos2);
    uint16_t    getN1();
    uint16_t    getN2();
    double      getA(uint8_t p);
    double      getL();
    double      getR();
    double      getX(uint8_t p);
    double      getY(uint8_t p);
    double      unify(double a);

private:
    uint16_t    n1;
    uint16_t    n2;
    double      a[RAIL_SIZE_POINTS];
    double      l;
    double      r;
    double      x[RAIL_SIZE_POINTS];
    double      y[RAIL_SIZE_POINTS];
};


#endif

