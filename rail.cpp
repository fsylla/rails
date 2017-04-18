
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

    l           = 0;
    r           = 0;

    for (uint8_t p = 0; p < RAIL_SIZE_POINTS; ++p) {
        a[p]    = 0;
        x[p]    = 0;
        y[p]    = 0;
    }
}


Rail::~Rail()
{
}


int             Rail::initArc(double nx1, double ny1, double nx2, double ny2, double nr)
{
    x[1]        = nx1;
    y[1]        = ny1;
    x[2]        = nx2;
    y[2]        = ny2;
    r           = nr;

    double xm   = (x[1] + x[2]) / 2;
    double ym   = (y[1] + y[2]) / 2;

    double al;
    double g;                           // distance (P2 - P1) / 2
    double gg   = (x[2] - xm) * (x[2] - xm) + (y[2] - ym) * (y[2] - ym);
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

        x[0]    = xm - h * (y[2] - ym) / g;
        y[0]    = ym + h * (x[2] - xm) / g;

        a[1]    = unify(atan2(y[1] - y[0], x[1] - x[0]));
        a[2]    = unify(atan2(y[2] - y[0], x[2] - x[0]));
        a[0]    = a[2] - a[1];
        al      = fabs(a[0]);
        
        if (al > M_PI) {
            al  = 2 * M_PI - al;
        }

        l       = fabs(r) * al;
    }

    return(rc);
}


int             Rail::initLine(double nx1, double ny1, double nx2, double ny2)
{
    x[1]        = nx1;
    y[1]        = ny1;
    x[2]        = nx2;
    y[2]        = ny2;

    l           = sqrt((x[2] - x[1]) * (x[2] - x[1]) + (y[2] - y[1]) * (y[2] - y[1]));

    return(0);
}


void            Rail::clear()
{
    for (uint8_t p = 3; p < RAIL_SIZE_POINTS; ++p) {
        a[p]    = 0;
        x[p]    = 0;
        y[p]    = 0;
    }
}


void            Rail::dump(uint16_t id)
{
    printf("Rail %5d n1 = %6d n2 = %6d  l = %6.2f  r = %6.2f\n", id, n1, n2, l, r);

    for (uint8_t p = 0; p < RAIL_SIZE_POINTS; ++p) {
        printf("           a%d = %6.2f x%d = %6.2f y%d = %6.2f\n", p, a[p], p, x[p], p, y[p]);
    }
}



int             Rail::fill(uint16_t n, double pos1, double pos2)
{
    double      as;
    int         rc      = 1;            // assume error

    if (pos1 >= 0 && pos1 <= l && pos2 >= 0 && pos2 <= l) {
        if (r == 0) {
            if (n == n1) {
                x[3]    = x[1] + pos1 * (x[2] - x[1]) / l;
                y[3]    = y[1] + pos1 * (y[2] - y[1]) / l;
                x[4]    = x[1] + pos2 * (x[2] - x[1]) / l;
                y[4]    = y[1] + pos2 * (y[2] - y[1]) / l;

                rc      = 0;
            }

            if (n == n2) {
                x[3]    = x[2] + pos1 * (x[1] - x[2]) / l;
                y[3]    = y[2] + pos1 * (y[1] - y[2]) / l;
                x[4]    = x[2] + pos2 * (x[1] - x[2]) / l;
                y[4]    = y[2] + pos2 * (y[1] - y[2]) / l;

                rc      = 0;
            }
        } else {
            if (a[0] > M_PI) {
                as      = a[0] - 2 * M_PI;
            } else {
                if (a[0] < - M_PI) {
                    as  = a[0] + 2 * M_PI;
                } else {
                    as  = a[0];
                }
            }

            if (n == n1) {
                a[3]    = a[1] + pos1 * as / l;
                a[4]    = a[1] + pos2 * as / l;
                
                rc      = 0;
            }

            if (n == n2) {
                a[3]    = a[2] - pos1 * as / l;
                a[4]    = a[2] - pos2 * as / l;
                
                rc      = 0;
            }

            if (r > 0) {
                if (a[3] > a[4]) {
                    as          = a[3];
                    a[3]        = a[4];
                    a[4]        = as;
                }
            } else {
                if (a[4] > a[3]) {
                    as          = a[3];
                    a[3]        = a[4];
                    a[4]        = as;
                }
            }
        }
    } else {
        printf("Rail::fill: pos1 = %6.2f pos2 = %6.2f l = %6.2f out of range\n", pos1, pos2, l);
    }

    return(rc);
}


uint16_t        Rail::getN1()
{
    return(n1);
}


uint16_t        Rail::getN2()
{
    return(n2);
}


double          Rail::getA(uint8_t p)
{
    double val  = 0.0;

    if (p < RAIL_SIZE_POINTS) {
        val     = a[p];
    }

    return(val);
}


double          Rail::getL()
{
    return(l);
}


double          Rail::getR()
{
    return(r);
}


double          Rail::getX(uint8_t p)
{
    double val  = 0.0;

    if (p < RAIL_SIZE_POINTS) {
        val     = x[p];
    }

    return(val);
}


double          Rail::getY(uint8_t p)
{
    double val  = 0.0;

    if (p < RAIL_SIZE_POINTS) {
        val     = y[p];
    }

    return(val);
}


double          Rail::unify(double a)
{
    double val  = 0.0;

    if (a < 0) {
        val     = 2 * M_PI + a;
    } else {
        val     = a;
    }

    return(val);
}

