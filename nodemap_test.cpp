
/* ____________________________________________________________________________
 *
 * file:        nodemap_test.cpp
 * created:     Thu Jan 19 18:34:58 2017
 * by:          frank
 *
 * description: travel around a nodemap with a couple of trains
 * ____________________________________________________________________________
 */


#include <getopt.h>
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

    trains[0]           = new Train(1, 70, 71);
    trains[1]           = new Train(2, 75, 76);
    trains[2]           = new Train(3, 77, 78);
    trains[3]           = new Train(4, 79, 80);

    trains[0]->setDest(57);
    trains[1]->setDest(58);
    trains[2]->setDest(81);
    trains[3]->setDest(83);

    for (int i = 0; i < SIZE_TRAINS; ++i) {
        nodeMap->trainAdd(trains[i]);
        trains[i]->dump();
        tx('e', trains[i]->getId(), trains[i]->getTail(), trains[i]->getHead());
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
    int         opt = getopt(argc, argv, "e");
    uint16_t    j;
    NodeMap*    nodeMap;

    printf("initializing nodemap\n");

    nodeMap = new NodeMap();
    nodeMap->nodesLoad("nodes.txt");
    nodeMap->nodesDump();

    if (opt == 'e') {
        nodeMap->hopsEval();
        nodeMap->hopsSave("hops.txt");
    } else {
        nodeMap->hopsLoad("hops.txt");
    }

    nodeMap->hopsDump();
    nodeMap->railsLoad("rails.txt");
    nodeMap->railsDump();

    travel(nodeMap);

    return(0);
}

