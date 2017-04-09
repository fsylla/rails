
/* ____________________________________________________________________________
 *
 * file:        nodemap_server.cpp
 * created:     Thu Jan 19 18:34:58 2017
 * by:          frank
 *
 * description: wait for client to connect and send commands to socket
 * ____________________________________________________________________________
 */


#include <stdio.h>
#include <unistd.h>

#include "nodemap.h"
#include "train.h"
#include "socketio.h"


#define SIZE_TRAINS         4


void tx(SocketIO* socketIO, char cmd, int train, uint16_t n1, uint16_t n2)
{
    char    buffer[256];
    int     n   = sprintf(buffer, "%c %d %d %d\n", cmd, train, n1, n2);

    if (n < 256) {
        socketIO->tx(buffer, n);
        printf("tx: cmd = %c train = %d n1 = %3d n2 = %3d\n", cmd, train, n1, n2);
    } else {
        printf("tx: %d exceeds buffer size\n", n);
    }
}


void travel(NodeMap* nodeMap, SocketIO* socketIO)
{
    uint8_t     move    = 1;
    uint16_t    head, tail;

    Train*      trains[SIZE_TRAINS];

    trains[0]           = new Train(1, 70, 113);
    trains[1]           = new Train(2, 75, 114);
    trains[2]           = new Train(3, 77, 115);
    trains[3]           = new Train(4, 79, 116);

    trains[0]->setDest(57);
    trains[1]->setDest(58);
    trains[2]->setDest(81);
    trains[3]->setDest(83);

    for (int i = 0; i < SIZE_TRAINS; ++i) {
        nodeMap->trainAdd(trains[i]);
        trains[i]->dump();
        tx(socketIO, 'e', trains[i]->getId(), trains[i]->getTail(), trains[i]->getHead());
    }

    sleep(1);

    while (move) {
        move = 0;

        for (int i = 0; i < SIZE_TRAINS; ++i) {
            head = trains[i]->getHead();
            tail = trains[i]->getTail();

            if (head != trains[i]->getDest()) {
                if (nodeMap->trainRun(trains[i])) {
                    tx(socketIO, 'e', i + 1, head, trains[i]->getHead());
                    tx(socketIO, 'l', i + 1, tail, trains[i]->getTail());
                    ++move;
                }
            }
        }

        sleep(1);
    }
}


int main(int argc, char **argv)
{
    NodeMap*    nodeMap;
    SocketIO*   socketIO;

    printf("initializing nodeMap\n");

    nodeMap = new NodeMap();
    nodeMap->nodesLoad("nodes.txt");
    nodeMap->hopsLoad("hops.txt");

    printf("initializing socketIO\n");

    socketIO    = new SocketIO(9999);
    socketIO->initServer();

    travel(nodeMap, socketIO);

    printf("terminating socketIO\n");
    socketIO->~SocketIO();

    return(0);
}

