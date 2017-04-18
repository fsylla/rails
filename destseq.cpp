
/* ____________________________________________________________________________
 *
 * file:	destseq.cpp
 * created:	Mon Apr 17 18:41:41 2017
 * by:		frank
 *
 * description: destination sequence configuration
 * ____________________________________________________________________________
 */


#include "destseq.h"


uint16_t        destNxt[DESTSEQ_SIZE];

void            initDestSeq()
{
    for (int i = 0; i < DESTSEQ_SIZE; ++i) {
        destNxt[i]      = 0;
    }

    destNxt[15]         = 114;
    destNxt[53]         =  97;
    destNxt[54]         = 123;
    destNxt[57]         =  97;
    destNxt[58]         =  81;
    destNxt[64]         =  87;
    destNxt[66]         = 115;
    destNxt[67]         = 116;
    destNxt[68]         = 117;
    destNxt[70]         =  15;
    destNxt[81]         =  58;
    destNxt[83]         =  57;
    destNxt[87]         =  64;
    destNxt[94]         = 122;
    destNxt[95]         =  94;
    destNxt[97]         = 110;
    destNxt[110]        =  83;
    destNxt[114]        =  66;
    destNxt[115]        =  67;
    destNxt[116]        =  68;
    destNxt[120]        = 123;
    destNxt[122]        = 125;
    destNxt[123]        =  53;
    destNxt[125]        =  54;
}

