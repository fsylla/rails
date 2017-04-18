
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

#include "destseq.h"
#include "nodemap.h"
#include "train.h"
#include "socketio.h"


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
    std::map<uint16_t,Train>    *trains = nodeMap->getTrains();

    uint8_t                     move    = 1;
    uint16_t                    dest;
    uint16_t                    head;
    uint16_t                    tail;
    nodemap_state_t             state;

    nodeMap->trainAdd(95, 124, 3);
    nodeMap->trainAdd(81, 82, 4);
    nodeMap->trainAdd(64, 62, 4);
    nodeMap->trainAdd(70, 113, 3);

    nodeMap->trainSetDest(1, 94);
    nodeMap->trainSetDest(2, 58);
    nodeMap->trainSetDest(3, 87);
    nodeMap->trainSetDest(4, 15);
    
    while (move) {
        move    = 0;

        for (std::map<uint16_t,Train>::iterator it = trains->begin(); it != trains->end(); ++it) {
            head        = it->second.getHead();
            tail        = it->second.getTail();
            state       = nodeMap->trainRun(it->first);

            switch (state) {
                case ok:
                    if (it->second.getHead() != head) {
                        tx(socketIO, 'e', it->first, head, it->second.getHead());
                    }
    
                    if (it->second.getTail() != tail) {
                        tx(socketIO, 'l', it->first, tail, it->second.getTail());
                    }
    
                    ++move;
                    break;

                case done:
                    dest        = destNxt[it->second.getDest()];

                    if (dest) {
                        nodeMap->trainSetDest(it->first, dest);
                        ++move;
                    }

                    break;

            }
        }

        usleep(20000);                  // 20 ms cycle time
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
    nodeMap->railsLoad("rails.txt");

    printf("initializing socketIO\n");

    socketIO    = new SocketIO(9999);
    socketIO->initServer();

    initDestSeq();

    travel(nodeMap, socketIO);

    printf("terminating socketIO\n");
    socketIO->~SocketIO();

    return(0);
}

