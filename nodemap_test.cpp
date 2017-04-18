
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
    std::map<uint16_t,Train>    *trains = nodeMap->getTrains();

    uint8_t                     move    = 1;
    uint16_t                    head;
    uint16_t                    tail;

    nodeMap->trainAdd(70, 113, 1);
    nodeMap->trainAdd(75, 114, 1);
    nodeMap->trainAdd(77, 115, 1);
    nodeMap->trainAdd(79, 116, 1);

    nodeMap->trainSetDest(1, 57);
    nodeMap->trainSetDest(2, 58);
    nodeMap->trainSetDest(3, 81);
    nodeMap->trainSetDest(4, 83);

    for (std::map<uint16_t,Train>::iterator it = trains->begin(); it != trains->end(); ++it) {
        it->second.dump(it->first);
        tx('e', it->first, it->second.getTail(), it->second.getHead());
    }

    printf("\n");

    while (move) {
        move = 0;

        for (std::map<uint16_t,Train>::iterator it = trains->begin(); it != trains->end(); ++it) {
            head = it->second.getHead();
            tail = it->second.getTail();

            if (head != it->second.getDest()) {
                if (nodeMap->trainRun(it->first) == ok) {
                    if (it->second.getHead() != head) {
                        tx('e', it->first, head, it->second.getHead());
                    }

                    if (it->second.getTail() != tail) {
                        tx('l', it->first, tail, it->second.getTail());
                    }

                    ++move;
                }
            }
        }
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

