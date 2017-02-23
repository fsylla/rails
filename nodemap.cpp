
/* ____________________________________________________________________________
 *
 * file:        nodemap.cpp
 * created:     Thu Jan 19 18:13:57 2017
 * by:          frank
 *
 * description: nodemaps are maps of connected nodes for navigation from one
 *              node to another
 * ____________________________________________________________________________
 */


#include <stdio.h>

#include "nodemap.h"


NodeMap::NodeMap(uint16_t nsize)
{
    for (uint16_t i = 0; i < nsize; ++i) {
        nodes[i] = Node();
    }
}


NodeMap::~NodeMap()
{
}


int     NodeMap::connect(uint16_t id1, uint16_t id2, uint8_t duplex)
{
    int     rc  = 0;

    if (id1 >= nodes.size()) {
        printf("NodeMap::connect size error id1 = %d\n", id1);
        rc = 1;
    }

    if (rc == 0 && id2 >= nodes.size()) {
        printf("NodeMap::connect size error id2 = %d\n", id2);
        rc = 1;
    }

    if (rc == 0 && nodes[id1].getNc() < NODE_SIZE_CONNECTIONS) {
        if (duplex) {
            if (nodes[id2].getNc() < NODE_SIZE_CONNECTIONS) {
                nodes[id1].connect(id2);
                nodes[id2].connect(id1);
            } else {
                printf("NodeMap::connect failed to connect node %d to %d\n", id1, id2);
                rc = 2;
            }
        } else {
            nodes[id1].connect(id2);
        }
    } else {
        printf("NodeMap::connect failed to connect node %d to %d\n", id1, id2);
        rc = 2;
    }

    return(rc);
}


void        NodeMap::dump()
{
    printf("NodeMap size = %6ld\n", nodes.size());

    for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        nodes[it->first].dump(it->first);
    }
}


void        NodeMap::dumpHops()
{
    for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        printf("%3d:", it->first);

        for (std::map<uint16_t,Node>::iterator jt = nodes.begin(); jt != nodes.end(); ++jt) {
            printf(" %3d", nodes[it->first].getHops(jt->first));
        }

        printf("\n");
    }
}


void        NodeMap::evalHops()
{
    for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        for (std::map<uint16_t,Node>::iterator jt = nodes.begin(); jt != nodes.end(); ++jt) {
            nodes[it->first].setHops(jt->first, 0);
        }
    }

    for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
        for (int i = 0; i < nodes[it->first].getNc(); ++i) {
            nodes[it->first].setHops(nodes[it->first].getConnection(i), 1);
        }
    }

    for (int h = 2; h < nodes.size(); ++h) {
        for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
            printf("evaluating hops for h = %3d id = %3d\r", h, it->first);

            for (std::map<uint16_t,Node>::iterator jt = nodes.begin(); jt != nodes.end(); ++jt) {
                if (nodes[it->first].getHops(jt->first) > 0 && nodes[it->first].getHops(jt->first) < h) {
                    for (std::map<uint16_t,Node>::iterator kt = nodes.begin(); kt != nodes.end(); ++kt) {
                        if (kt->first != it->first &&
                                nodes[it->first].getHops(kt->first) == 0 &&
                                (nodes[it->first].getHops(jt->first) + nodes[jt->first].getHops(kt->first) == h)) {
                            nodes[it->first].setHops(kt->first, h);
                        }
                    }
                }
            }
        }
    }

    printf("\n");
}


uint16_t    NodeMap::getSize()
{
    return(nodes.size());
}


uint16_t    NodeMap::getToken(uint16_t id)
{
    return(nodes[id].getToken());
}


float       NodeMap::getX(uint16_t id)
{
    return(nodes[id].getX());
}


float       NodeMap::getY(uint16_t id)
{
    return(nodes[id].getY());
}


int     NodeMap::load(const char* path)
{
    char        line[80];
    FILE*       pFile   = fopen(path, "r");
    uint16_t    id;
    float       x, y;
    uint16_t    c[NODE_SIZE_CONNECTIONS];
    int         l;
    int         rc      = 1;

    if (pFile) {
        while (fgets(line, 80, pFile)) {
            l = sscanf(line, "%hu;%f;%f;%hu;%hu;%hu", &id, &x, &y, &c[0], &c[1], &c[2]);

            nodes[id] = Node();
            nodes[id].setPos(x, y);

            for (int i = 0; i < l - 3; ++i) {
                nodes[id].connect(c[i]);
            }
        }

        fclose(pFile);
        rc = 0;
    } else {
        printf("NodeMap::load: Unable to load %s\n", path);
    }

    return(rc);
}


int     NodeMap::loadHops(const char* path)
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
        printf("NodeMap::loadHops: Unable to load %s\n", path);
    }

    return(rc);
}


int         NodeMap::route(uint16_t id1, uint16_t id2)
{
    uint8_t     cOpt;
    uint16_t    d       = nodes[id1].getHops(id2);
    uint16_t    idx     = id1;
    uint16_t    idy;
    int         rc      = 1;

    if (d > 0) {
        printf("distance from %d to %d = %d\n", id1, id2, d);
        printf("              %d", id1);

        while (d > 1) {
            for (int c = 0; c < nodes[idx].getNc(); ++c) {
                idy = nodes[idx].getConnection(c);

                if (nodes[idy].getHops(id2) < d) {
                    d = nodes[idy].getHops(id2);
                    cOpt = c;
                }
            }

            idx = nodes[idx].getConnection(cOpt);
            printf(" - %d", idx);
        }

        printf(" - %d\n", id2);
        rc = 0;
    } else {
        printf("route: ERROR\n");
    }

    return(rc);
}


int     NodeMap::save(const char* path)
{
    FILE*       pFile   = fopen(path, "w");
    int         rc      = 1;

    if (pFile) {
        for (std::map<uint16_t,Node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
            fprintf(pFile, "%5d;%8.3f;%8.3f", it->first, it->second.getX(), it->second.getY());

            for (int c = 0; c < it->second.getNc(); ++c) {
                fprintf(pFile, ";%5d", it->second.getConnection(c));
            }

            fprintf(pFile, "\n");
        }

        fclose(pFile);
        rc = 0;
    }

    return(rc);
}


int     NodeMap::saveHops(const char* path)
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


int     NodeMap::trainAdd(Train* train)
{
    uint16_t    id      = train->getId();
    uint16_t    head    = train->getHead();
    uint16_t    tail    = train->getTail();
    int         rc      = 0;

    nodes[head].setToken(id);
    nodes[tail].setToken(id);
    return(rc);
}


int    NodeMap::trainRun(Train* train)
{
    uint16_t    id      = train->getId();
    uint16_t    dest    = train->getDest();
    uint16_t    head    = train->getHead();
    uint16_t    headNxt;
    uint16_t    tail    = train->getTail();
    uint16_t    token;
    uint8_t     cOpt;
    uint16_t    hops    = nodes[head].getHops(dest);
    int         rc      = 0;

    if (hops > 0) {
        for (int c = 0; c < nodes[head].getNc(); ++c) {
            headNxt = nodes[head].getConnection(c);

            if (nodes[headNxt].getHops(dest) < hops) {
                hops = nodes[headNxt].getHops(dest);
                cOpt = c;
            }
        }

        if (nodes[head].getNc() < 3 || cOpt == 0) {
            headNxt = nodes[head].getConnection(cOpt);
        } else {
            headNxt = nodes[head].getConnection(0);

            if (headNxt == tail) {
                headNxt = nodes[head].getConnection(cOpt);
            } else {
                printf("NodeMap::trainRun: headNxt redirected from %d to %d\n", nodes[head].getConnection(cOpt), headNxt);
            }
        }

        token       = nodes[headNxt].getToken();

        if (token) {
            if (id == token) {
                train->setTail(head);
                train->setHead(tail);
                rc  = 1;
            } else {
                printf("NodeMap::trainRun: Node %d blocked by token %d\n", headNxt, nodes[headNxt].getToken());
            }
        } else {
            nodes[headNxt].setToken(id);
            nodes[tail].setToken(0);
            train->setTail(head);
            train->setHead(headNxt);
            rc      = 1;                // TBD !!!
        }
    } else {
        printf("NodeMap::trainRun: ERROR\n");
    }

    return(rc);
}



