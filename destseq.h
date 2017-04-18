
/* ____________________________________________________________________________
 *
 * file:	destseq.h
 * created:	Mon Apr 17 18:30:31 2017
 * by:		frank
 *
 * description: destination sequence configuration
 * ____________________________________________________________________________
 */


#ifndef DESTSEQ_H
#define DESTSEQ_H


#include <stdint.h>


#define         DESTSEQ_SIZE            256


extern uint16_t destNxt[DESTSEQ_SIZE];

void            initDestSeq();


#endif

