
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


#include <stdio.h>

#include "nodemap.h"


NodeMap::NodeMap()
{
    nodeIdMax   = 0;
    railIdMax   = 0;
}


NodeMap::~NodeMap()
{
}


std::map<uint16_t,Node> *NodeMap::getNodes()
{
    return(&nodes);
}


std::map<uint16_t,Rail> *NodeMap::getRails()
{
    return(&rails);
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
            l = sscanf(line, "%hu;%hu;%hu;%lf", &id, &n1, &n2, &r);

            rails[id] = Rail(n1, n2);

            if (r == 0) {
                rails[id].initLine(nodes[n1].getX(), nodes[n1].getY(), nodes[n2].getX(), nodes[n2].getY());
            } else {
                rails[id].initArc(nodes[n1].getX(), nodes[n1].getY(), nodes[n2].getX(), nodes[n2].getY(), r);
            }

            if (id > railIdMax) {
                railIdMax   = id;
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
    uint8_t     lOpt;
    uint16_t    hops    = nodes[head].getHops(dest);
    int         rc      = 0;

    if (hops > 0) {
        for (int l = 0; l < nodes[head].getLinkSize(); ++l) {
            headNxt = nodes[head].getLink(l);

            if (nodes[headNxt].getHops(dest) < hops) {
                hops = nodes[headNxt].getHops(dest);
                lOpt = l;
            }
        }

        if (nodes[head].getLinkSize() < 3 || lOpt == 0) {
            headNxt = nodes[head].getLink(lOpt);
        } else {
            headNxt = nodes[head].getLink(0);

            if (headNxt == tail) {
                headNxt = nodes[head].getLink(lOpt);
            } else {
                printf("NodeMap::trainRun: headNxt redirected from %d to %d\n", nodes[head].getLink(lOpt), headNxt);
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

