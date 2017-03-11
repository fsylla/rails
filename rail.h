
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


#include <stdint.h>


class Rail
{
public:
    Rail();
    Rail(uint16_t nn1, uint16_t nn2);
    ~Rail();
    int         initArc(double nx1, double ny1, double nx2, double ny2, double nr);
    int         initLine(double nx1, double ny1, double nx2, double ny2);
    void        dump(uint16_t id);
    uint16_t    getN1();
    uint16_t    getN2();
    double      getA1();
    double      getA2();
    double      getL();
    double      getR();
    double      getX1();
    double      getX2();
    double      getXc();
    double      getY1();
    double      getY2();
    double      getYc();

private:
    uint16_t    n1;
    uint16_t    n2;
    double      a1;
    double      a2;
    double      l;
    double      r;
    double      x1;
    double      x2;
    double      xc;
    double      y1;
    double      y2;
    double      yc;
};


#endif

