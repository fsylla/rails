
/* ____________________________________________________________________________
 *
 * file:        matrix.c
 * created:     Sun Feb  7 18:32:08 2016
 * by:          frank
 *
 * description: calculate the determinant of a 3x3 matrix
 * ____________________________________________________________________________
 */


#include "matrix.h"


GLfloat determinant(GLfloat m[3][3])
{
    GLfloat d;

    d = m[0][0] * m[1][1] * m[2][2] +
        m[0][1] * m[1][2] * m[2][0] +
        m[0][2] * m[1][0] * m[2][1] -
        m[0][2] * m[1][1] * m[2][0] -
        m[0][0] * m[1][2] * m[2][1] -
        m[0][1] * m[1][0] * m[2][2];

    return(d);
}

