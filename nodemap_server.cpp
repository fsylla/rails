
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

    for (int i = 0; i < SIZE_TRAINS; ++i) {
        trains[i] = new Train(i + 1, i + 11, i + 1);
        trains[i]->setDest(i + 6);
        nodeMap->trainAdd(trains[i]);
        tx(socketIO, 'e', i + 1, i + 1, i + 11);
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

    nodeMap = new NodeMap(0);
    nodeMap->load("nodes.txt");
    nodeMap->loadHops("hops.txt");

    printf("initializing socketIO\n");

    socketIO    = new SocketIO(9999);
    socketIO->initServer();

    travel(nodeMap, socketIO);

    socketIO->~SocketIO();

    return(0);
}

