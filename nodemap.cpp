
/* ____________________________________________________________________________
 *
 * file:        nodemap.cpp
 * created:     Thu Jan 19 18:13:57 2017
 * by:          frank
 *
 * description: nodemaps are maps of linked nodes for navigation from one
 *              node to another
 * ____________________________________________________________________________
 */


#include <math.h>
#include <stdio.h>

#include "nodemap.h"


NodeMap::NodeMap()
{
    nodeIdMax   = 0;
    railIdMax   = 0;
    trainIdMax  = 0;
}


NodeMap::~NodeMap()
{
}


std::map<uint16_t,Node>         *NodeMap::getNodes()
{
    return(&nodes);
}


std::map<uint16_t,Rail>         *NodeMap::getRails()
{
    return(&rails);
}


std::map<uint16_t,Train>        *NodeMap::getTrains()
{
    return(&trains);
}


void    NodeMap::hopsDump()
{
    for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        printf("%3d:", it->first);

        for (std::map<uint16_t,Node>::iterator jt = nodes.begin(); jt != nodes.end(); ++jt) {
            printf(" %3d", it->second.getHops(jt->first));
        }

        printf("\n");
    }
}


void    NodeMap::hopsEval()
{
    for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        for (std::map<uint16_t,Node>::iterator jt = nodes.begin(); jt != nodes.end(); ++jt) {
            it->second.setHops(jt->first, 0);
        }
    }

    for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        for (int i = 0; i < it->second.getLinkSize(); ++i) {
            it->second.setHops(it->second.getLink(i), 1);
        }
    }

    for (int h = 2; h < nodes.size(); ++h) {
        for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
            printf("evaluating hops for h = %3d id = %3d\r", h, it->first);

            for (std::map<uint16_t,Node>::iterator jt = nodes.begin(); jt != nodes.end(); ++jt) {
                if (it->second.getHops(jt->first) > 0 && it->second.getHops(jt->first) < h) {
                    for (std::map<uint16_t,Node>::iterator kt = nodes.begin(); kt != nodes.end(); ++kt) {
                        if (kt->first != it->first &&
                            it->second.getHops(kt->first) == 0 &&
                            (it->second.getHops(jt->first) + jt->second.getHops(kt->first) == h)) {
                            it->second.setHops(kt->first, h);
                        }
                    }
                }
            }
        }
    }

    printf("\n");
}


int     NodeMap::hopsLoad(const char* path)
{
    char        line[80];
    FILE*       pFile   = fopen(path, "r");
    uint16_t    h, i, j, l;
    int         rc      = 1;

    if (pFile) {
        while (fgets(line, 80, pFile)) {
            l = sscanf(line, "%hu;%hu;%hu", &i, &j, &h);

            nodes[i].setHops(j, h);
        }

        fclose(pFile);
        rc = 0;
    } else {
        printf("NodeMap::hopsLoad: Unable to load %s\n", path);
    }

    return(rc);
}


int     NodeMap::hopsSave(const char* path)
{
    FILE*       pFile   = fopen(path, "w");
    int         rc      = 1;

    if (pFile) {
        for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
            for (std::map<uint16_t,Node>::iterator jt = nodes.begin(); jt != nodes.end(); ++jt) {
                fprintf(pFile, "%5d;%5d;%5d\n", it->first, jt->first, it->second.getHops(jt->first));
            }
        }

        fclose(pFile);
        rc = 0;
    }

    return(rc);
}


uint16_t        NodeMap::nodeAdd(float x, float y)
{
    ++nodeIdMax;

    nodes[nodeIdMax]    = Node();
    nodes[nodeIdMax].setPos(x, y);

    return(nodeIdMax);
}


uint16_t        NodeMap::nodeGetToken(uint16_t id)
{
    uint16_t token = 0;

    if (nodes.count(id)) {
        token = nodes[id].getToken();
    }

    return(token);
}


float           NodeMap::nodeGetX(uint16_t id)
{
    float x = 0;

    if (nodes.count(id)) {
        x = nodes[id].getX();
    }

    return(x);
}


float           NodeMap::nodeGetY(uint16_t id)
{
    float y = 0;

    if (nodes.count(id)) {
        y = nodes[id].getY();
    }

    return(y);
}


int     NodeMap::nodeLink(uint16_t n1, uint16_t n2, uint8_t duplex)
{
    int     rc  = 0;

    if (nodes.count(n1) < 1) {
        printf("NodeMap::nodeLink error n1 = %d\n", n1);
        rc = 1;
    }

    if (nodes.count(n2) < 1) {
        printf("NodeMap::nodeLink error n2 = %d\n", n2);
        rc = 1;
    }

    if (rc == 0 && nodes[n1].getLinkSize() < NODE_LINKS_MAX) {
        if (duplex) {
            if (nodes[n2].getLinkSize() < NODE_LINKS_MAX) {
                nodes[n1].link(n2);
                nodes[n2].link(n1);
            } else {
                printf("NodeMap::nodeLink failed to link node %d to %d\n", n1, n2);
                rc = 2;
            }
        } else {
            nodes[n1].link(n2);
        }
    } else {
        printf("NodeMap::nodeLink failed to link node %d to %d\n", n1, n2);
        rc = 2;
    }

    return(rc);
}


void        NodeMap::nodesDump()
{
    printf("Nodes size = %6ld\n", nodes.size());

    for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        it->second.dump(it->first);
    }
}


int     NodeMap::nodesLoad(const char* path)
{
    char        line[80];
    FILE*       pFile   = fopen(path, "r");
    uint16_t    id;
    float       x, y;
    uint16_t    c[NODE_LINKS_MAX];
    int         l;
    int         rc      = 1;

    if (pFile) {
        while (fgets(line, 80, pFile)) {
            l = sscanf(line, "%hu;%f;%f;%hu;%hu;%hu", &id, &x, &y, &c[0], &c[1], &c[2]);

            nodes[id] = Node();
            nodes[id].setPos(x, y);

            for (int i = 0; i < l - 3; ++i) {
                nodes[id].link(c[i]);
            }

            if (id > nodeIdMax) {
                nodeIdMax   = id;
            }
        }

        fclose(pFile);
        rc = 0;
    } else {
        printf("NodeMap::nodesLoad: Unable to load %s\n", path);
    }

    return(rc);
}


int     NodeMap::nodesSave(const char* path)
{
    FILE*       pFile   = fopen(path, "w");
    int         rc      = 1;

    if (pFile) {
        for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
            fprintf(pFile, "%5d;%8.3f;%8.3f", it->first, it->second.getX(), it->second.getY());

            for (int c = 0; c < it->second.getLinkSize(); ++c) {
                fprintf(pFile, ";%5d", it->second.getLink(c));
            }

            fprintf(pFile, "\n");
        }

        fclose(pFile);
        rc = 0;
    }

    return(rc);
}


uint16_t        NodeMap::railAdd(uint16_t n1, uint16_t n2, float r)
{
    ++railIdMax;

    rails[railIdMax]    = Rail(n1, n2);

    if (r == 0) {
        rails[railIdMax].initLine(nodes[n1].getX(), nodes[n1].getY(), nodes[n2].getX(), nodes[n2].getY());
    } else {
        rails[railIdMax].initArc(nodes[n1].getX(), nodes[n1].getY(), nodes[n2].getX(), nodes[n2].getY(), r);
    }

    return(railIdMax);
}


void    NodeMap::railsDump()
{
    printf("Rails size = %6ld\n", rails.size());

    for (std::map<uint16_t,Rail>::iterator it = rails.begin(); it != rails.end(); ++it) {
        it->second.dump(it->first);
    }
}


int     NodeMap::railsLoad(const char* path)
{
    char        line[80];
    FILE*       pFile   = fopen(path, "r");
    uint16_t    id, l, n1, n2;
    double      r, x1, x2, y1, y2;

    int         rc      = 1;

    if (pFile) {
        while (fgets(line, 80, pFile)) {
            sscanf(line, "%hu;%hu;%hu;%lf", &id, &n1, &n2, &r);

            rails[id] = Rail(n1, n2);

            if (r == 0) {
                rails[id].initLine(nodes[n1].getX(), nodes[n1].getY(), nodes[n2].getX(), nodes[n2].getY());
            } else {
                rails[id].initArc(nodes[n1].getX(), nodes[n1].getY(), nodes[n2].getX(), nodes[n2].getY(), r);
            }

            if (id > railIdMax) {
                railIdMax   = id;
            }

            for (l = 0; l < nodes[n1].getLinkSize(); ++l) {
                if (nodes[n1].getLink(l) == n2) {
                    nodes[n1].setRail(l, id);
                    // printf("NodeMap::railsLoad: node %d rail %d\n", n1, id);
                    break;
                }
            }
    
            for (l = 0; l < nodes[n2].getLinkSize(); ++l) {
                if (nodes[n2].getLink(l) == n1) {
                    nodes[n2].setRail(l, id);
                    // printf("NodeMap::railsLoad: node %d rail %d\n", n2, id);
                    break;
                }
            }
        }

        fclose(pFile);
        rc = 0;
    } else {
        printf("NodeMap::railsLoad: Unable to load %s\n", path);
    }

    return(rc);
}


int     NodeMap::railsSave(const char* path)
{
    FILE*       pFile   = fopen(path, "w");
    uint16_t    id      = 1;            // rail id
    uint16_t    n2;                     // rail node 2
    int         duplex;
    int         rc      = 1;

    if (pFile) {
        for (std::map<uint16_t,Rail>::iterator it = rails.begin(); it != rails.end(); ++it) {
            fprintf(pFile, "%5d;%5d;%5d;%6.2f\n", it->first, it->second.getN1(), it->second.getN2(), it->second.getR());
        }

        fclose(pFile);
        rc = 0;
    }

    return(rc);
}


uint16_t        NodeMap::trainAdd(uint16_t n1, uint16_t n2, float l)
{
    uint16_t    id      = 0;
    uint16_t    rid     = 0;
    double      rl;

    if (nodes.count(n1)) {
        if (nodes.count(n2)) {
            id          = trainIdMax + 1;
        } else {
            printf("NodeMap::trainAdd: invalid node %d\n", n2);
        }
    } else {
        printf("NodeMap::trainAdd: invalid node %d\n", n1);
    }
        
    if (id) {
        if (nodes[n2].getHops(n1) == 1) {
            for (int l = 0; l < nodes[n2].getLinkSize(); ++l) {
                if (nodes[n2].getLink(l) == n1) {
                    rid = nodes[n2].getRail(l);
                    rl  = rails[rid].getL();
                    printf("NodeMap::trainAdd: rid = %d l = %6.2f\n", rid, rl);
                    break;
                }
            }
        } else {
            printf("NodeMap::trainAdd: invalid nodes %d %d\n", n1, n2);
            id  = 0;
        }
    }

    if (rid) {
        if (l <= rl) {
            trains[id]  = Train(n1, n2, l);
            trainIdMax  = id;

            trains[id].setHeadRail(rid);
            trains[id].setTailRail(rid);
            trains[id].setTailPos(rl - l);

            nodes[n1].setToken(id);
            nodes[n2].setToken(id);

            rails[rid].fill(n1, 0, l);
        } else {
            printf("NodeMap::trainAdd: train length %6.2f exceeds rail length %6.2f\n", l, rl);
            id  = 0;
        }
    }
            
    return(id);
}


nodemap_state_t         NodeMap::trainRun(uint16_t id)
{
    uint16_t    dest;
    uint16_t    head;
    uint16_t    headNxt;
    uint16_t    headRail;
    uint16_t    tail;
    uint16_t    tailExt;
    uint16_t    tailNxt;
    uint16_t    tailRail;
    uint16_t    token;
    uint16_t    hops    = 0;
    uint16_t    rid     = 0;
    uint8_t     lOpt;
    
    double      headPos = 0.0;
    double      tailPos = 0.0;
    double      rl;
    double      speed;

    nodemap_state_t     rc      = error;

    if (trains.count(id)) {
        dest            = trains[id].getDest();
        head            = trains[id].getHead();
        headPos         = trains[id].getHeadPos();
        headRail        = trains[id].getHeadRail();
        tail            = trains[id].getTail();
        tailPos         = trains[id].getTailPos();
        tailRail        = trains[id].getTailRail();
        speed           = trains[id].getSpeed();
        hops            = nodes[head].getHops(dest);
    } else {
        printf("NodeMap::trainRun: invalid id %d\n", id);
    }

    if (hops > 0) {
        headPos = headPos + speed;

        if (headPos < 0) {
            trains[id].setHeadPos(headPos);
            rl  = rails[headRail].getL();
            
            if (rails[headRail].fill(head, rl, - headPos)) {
                printf("NodeMap::trainRun: fill error A\n");
                trains[id].dump(id);
            }

            rc  = ok;
        } else {
            for (int l = 0; l < nodes[head].getLinkSize(); ++l) {
                headNxt = nodes[head].getLink(l);
    
                if (nodes[headNxt].getHops(dest) < hops) {
                    hops = nodes[headNxt].getHops(dest);
                    lOpt = l;
                }
            }
   
            headNxt     = nodes[head].getLink(lOpt);
            rid         = nodes[head].getRail(lOpt);
            rl          = rails[rid].getL();
            token       = nodes[headNxt].getToken();

            if (token) {
                if (id == token) {
                    trains[id].setHead(tail);
                    trains[id].setHeadPos(- tailPos);
                    trains[id].setHeadRail(tailRail);
                    trains[id].setTail(head);
                    trains[id].setTailPos(speed - headPos);
                    trains[id].setTailRail(headRail);

                    printf("NodeMap::trainRun: id = %d swapped\n", id);
                    rc  = blocked;
                } else {
                    printf("NodeMap::trainRun: id = %d node %d blocked by token %d\n", id, headNxt, nodes[headNxt].getToken());
                    rc  = blocked;
                }
            } else {
                if (headPos < rl) {
                    if (rails[headRail].fill(head, 0, rails[headRail].getL())) {
                        printf("NodeMap::trainRun: fill error B\n");
                        trains[id].dump(id);
                    }

                    trains[id].setHead(headNxt);
                    trains[id].setHeadPos(headPos -rl);
                    trains[id].setHeadRail(rid);

                    if (rails[rid].fill(head, 0, headPos)) {
                        printf("NodeMap::trainRun: fill error C\n");
                        trains[id].dump(id);
                    }

                    nodes[headNxt].setToken(id);

                    rc  = ok;
                } else {
                    printf("NodeMap::trainRun: headPos %6.2f > rl %6.2f\n", headPos, rl);
                    printf("                   head = %d rid = %d lOpt = %d\n", head, rid, lOpt);

                    trains[id].dump(id);
                }
            }
        }
    } else {                            // head == dest
        headPos         = headPos + speed;

        if (headPos < 0) {
            trains[id].setHeadPos(headPos);
            rl          = rails[headRail].getL();
            if (rails[headRail].fill(head, rl, - headPos)) {
                        printf("NodeMap::trainRun: fill error D\n");
                        trains[id].dump(id);
            }
            
            rc  = ok;
        } else {
            rc  = done;
        }
    }

    if (rc == ok) {
        tailPos = tailPos + speed;
        rl      = rails[tailRail].getL();

        if (tailPos > rl) {
            // clear previous section
            nodes[tail].setToken(0);

            rails[tailRail].clear();

            // init next section
            hops        = nodes[tail].getHops(head);

            for (int l = 0; l < nodes[tail].getLinkSize(); ++l) {
                tailNxt = nodes[tail].getLink(l);
    
                if (nodes[tailNxt].getHops(head) < hops) {
                    hops = nodes[tailNxt].getHops(head);
                    lOpt = l;
                }
            }
    
            tailNxt     = nodes[tail].getLink(lOpt);
            hops        = nodes[tailNxt].getHops(head);

            for (int l = 0; l < nodes[tailNxt].getLinkSize(); ++l) {
                tailExt = nodes[tailNxt].getLink(l);
    
                if (nodes[tailExt].getHops(head) < hops) {
                    hops = nodes[tailExt].getHops(head);
                    lOpt = l;
                }
            }
    
            rid     = nodes[tailNxt].getRail(lOpt);

            nodes[tailNxt].setToken(id);

            trains[id].setTail(tailNxt);
            trains[id].setTailPos(tailPos - rl);
            trains[id].setTailRail(rid);
        } else {
            trains[id].setTailPos(tailPos);
        }

        // ensure to use updated values

        head            = trains[id].getHead();
        headPos         = trains[id].getHeadPos();
        headRail        = trains[id].getHeadRail();
        tail            = trains[id].getTail();
        tailPos         = trains[id].getTailPos();
        tailRail        = trains[id].getTailRail();
        rl              = rails[tailRail].getL();

        if (tailRail == headRail) {
            if (rails[tailRail].fill(tail, tailPos, rl + headPos)) {
                        printf("NodeMap::trainRun: fill error E\n");
                        trains[id].dump(id);
            }
        } else {
            if (rails[tailRail].fill(tail, tailPos, rl)) {
                        printf("NodeMap::trainRun: fill error F\n");
                        trains[id].dump(id);
            }
        }
    }

    return(rc);
}


int     NodeMap::trainSetDest(uint16_t id, uint16_t ndest)
{
    int         rc      = 1;            // assume error

    if (trains.count(id)) {
        if (nodes.count(ndest)) {
            trains[id].setDest(ndest);
            rc          = 0;
        } else {
            printf("NodeMap::trainSetDest: invalid destination %d\n", ndest);
        }
    } else {
        printf("NodeMap::trainSetDest: invalid id %d\n", id);
    }
        
    return(rc);
}


void        NodeMap::trainsDump()
{
    for (std::map<uint16_t,Train>::iterator it = trains.begin(); it != trains.end(); ++it) {
        it->second.dump(it->first);
    }
}

