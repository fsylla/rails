
/* ____________________________________________________________________________
 *
 * file:        nodemap_test.cpp
 * created:     Thu Jan 19 18:34:58 2017
 * by:          frank
 *
 * description: travel around a nodemap with a couple of trains
 * ____________________________________________________________________________
 */


#include <stdio.h>

#include "nodemap.h"
#include "train.h"


#define SIZE_TRAINS         4


void tx(char cmd, int train, uint16_t n1, uint16_t n2)
{
    printf("tx: cmd = %c train = %d n1 = %3d n2 = %3d\n", cmd, train, n1, n2);
}


void travel(NodeMap* nodeMap)
{
    uint8_t     move    = 1;
    uint16_t    head, tail;

    Train*      trains[SIZE_TRAINS];

    for (int i = 0; i < SIZE_TRAINS; ++i) {
        trains[i] = new Train(i + 1, i + 11, i + 1);
        trains[i]->setDest(i + 6);
        nodeMap->trainAdd(trains[i]);
//        trains[i]->dump(i + 1);
        tx('e', i + 1, i + 1, i + 11);
    }

    printf("\n");

    while (move) {
        move = 0;

        for (int i = 0; i < SIZE_TRAINS; ++i) {
            head = trains[i]->getHead();
            tail = trains[i]->getTail();

            if (head != trains[i]->getDest()) {
                if (nodeMap->trainRun(trains[i])) {
                    tx('e', i + 1, head, trains[i]->getHead());
                    tx('l', i + 1, tail, trains[i]->getTail());
                    ++move;
                }
            }
        }

        printf("\n");
    }
}


int main(int argc, char **argv)
{
    uint16_t    j;
    NodeMap*    nodeMap;

    printf("initializing nodemap\n");

    nodeMap = new NodeMap(0);
    nodeMap->load("nodes.txt");

    nodeMap->dump();
//    nodeMap->evalHops();
    nodeMap->loadHops("hops.txt");
    nodeMap->dumpHops();
//    nodeMap->saveHops("hops.txt");

    travel(nodeMap);

    return(0);
}

