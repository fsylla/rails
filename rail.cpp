
/* ____________________________________________________________________________
 *
 * file:	rail.cpp
 * created:	Fri Feb 12 19:21:11 2016
 * by:		frank
 *
 * description:	
 * ____________________________________________________________________________
 */


#include <math.h>
#include <stdio.h>

#include "rail.h"


/*
 *  Default constructor required for std::map<uint16_t,Rail> in class Nodemap
 *  Should never be used.
 */

Rail::Rail()
{
    Rail(0, 0);
}


Rail::Rail(uint16_t nn1, uint16_t nn2)
{
    n1          = nn1;
    n2          = nn2;

    a1          = 0;
    a2          = 0;
    l           = 0;
    r           = 0;
    x1          = 0;
    x2          = 0;
    xc          = 0;
    y1          = 0;
    y2          = 0;
    yc          = 0;
}


Rail::~Rail()
{
}


int             Rail::initArc(double nx1, double ny1, double nx2, double ny2, double nr)
{
    x1          = nx1;
    y1          = ny1;
    x2          = nx2;
    y2          = ny2;
    r           = nr;

    double xm   = (x1 + x2) / 2;
    double ym   = (y1 + y2) / 2;

    double g;                           // distance (P2 - P1) / 2
    double gg   = (x2 - xm) * (x2 - xm) + (y2 - ym) * (y2 - ym);
    double h;                           // distance (P2 - PM)
    double rr   = r * r;

    int rc      = 0;

    if (rr < gg) {
        printf("Rail::initArc: radius %f too small\n", r);
        rc      = 1;
    } else {
        if (r > 0) {
            h   = sqrt(rr - gg);
        } else {
            h   = - sqrt(rr - gg);
        }

        g       = sqrt(gg);

        xc      = xm - h * (y2 - ym) / g;
        yc      = ym + h * (x2 - xm) / g;

        a1      = atan2(y1 - yc, x1 - xc);
        a2      = atan2(y2 - yc, x2 - xc);
    }

    return(rc);
}


int             Rail::initLine(double nx1, double ny1, double nx2, double ny2)
{
    x1          = nx1;
    y1          = ny1;
    x2          = nx2;
    y2          = ny2;

    l           = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

    return(0);
}


void            Rail::dump(uint16_t id)
{
    printf("Rail %5d n1 = %5d n2 = %5d r = %f\n", id, n1, n2, r);
}


uint16_t        Rail::getN1()
{
    return(n1);
}


uint16_t        Rail::getN2()
{
    return(n2);
}


double          Rail::getA1()
{
    return(a1);
}


double          Rail::getA2()
{
    return(a2);
}


double          Rail::getL()
{
    return(l);
}


double          Rail::getR()
{
    return(r);
}


double          Rail::getX1()
{
    return(x1);
}


double          Rail::getX2()
{
    return(x2);
}


double          Rail::getXc()
{
    return(xc);
}


double          Rail::getY1()
{
    return(y1);
}


double          Rail::getY2()
{
    return(y2);
}


double          Rail::getYc()
{
    return(yc);
}

