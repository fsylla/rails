
/* ____________________________________________________________________________
 *
 * file:        node.cpp
 * created:     Wed Feb 22 22:35:55 2017
 * by:          frank
 *
 * description: nodes are points with static coordinates and connections to
 *              other nodes.
 * ____________________________________________________________________________
 */


#include <stdio.h>

#include "node.h"


Node::Node()
{
    nc          = 0;
    token       = 0;
    x           = 0.0;
    y           = 0.0;

    for (int i = 0; i < NODE_SIZE_CONNECTIONS; ++i) {
        connection[i] = 0xffff;
    }
}


Node::~Node()
{
}


int     Node::connect(uint16_t id)
{
    int rc      = 1;

    if (nc < NODE_SIZE_CONNECTIONS) {
        connection[nc++] = id;
        hops[id] = 1;
        rc = 0;
    }

    return(rc);
}


void        Node::dump(uint16_t id)
{
    printf("Node %3d token = %3d\n", id, token);

    for (int i = 0; i < nc; ++i) {
        printf("  -> %3d\n", connection[i]);
    }

    printf("\n");
}


uint16_t    Node::getConnection(uint8_t c)
{
    uint16_t    id  = 0xffff;

    if (c < nc) {
        id = connection[c];
    }

    return(id);
}


uint16_t    Node::getHops(uint16_t id)
{
    return(hops[id]);
}


uint8_t     Node::getNc()
{
    return(nc);
}


uint16_t    Node::getToken()
{
    return(token);
}


float       Node::getX()
{
    return(x);
}


float       Node::getY()
{
    return(y);
}


void        Node::setHops(uint16_t nid, uint16_t nhops)
{
    hops[nid]   = nhops;
}


void        Node::setPos(float nx, float ny)
{
    x           = nx;
    y           = ny;
}


void        Node::setToken(uint16_t ntoken)
{
    token       = ntoken;
}

