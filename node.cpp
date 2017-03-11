
/* ____________________________________________________________________________
 *
 * file:        node.cpp
 * created:     Wed Feb 22 22:35:55 2017
 * by:          frank
 *
 * description: nodes are points with static coordinates and links to
 *              other nodes.
 * ____________________________________________________________________________
 */


#include <stdio.h>

#include "node.h"


Node::Node()
{
    token       = 0;
    x           = 0.0;
    y           = 0.0;
}


Node::~Node()
{
    hops.~map();
    links.~map();
    rails.~map();
}


void        Node::dump(uint16_t id)
{
    printf("Node %5d token = %5d x = %8.3f y = %8.3f\n", id, token, x, y);

    for (std::map<uint16_t,uint16_t>::iterator it = links.begin(); it != links.end(); ++it) {

        printf("  -> %5d\n", it->second);
    }

    printf("\n");
}


uint16_t    Node::getHops(uint16_t id)
{
    uint16_t    h       = 0;

    if (hops.count(id)) {
        h               = hops[id];
    }

    return(h);
}


uint16_t     Node::getLink(uint16_t l)
{
    uint16_t    link    = 0;

    if (links.count(l)) {
        link            = links[l];
    }

    return(link);
}


uint16_t    Node::getLinkSize()
{
    return(links.size());
}


uint16_t     Node::getRail(uint16_t l)
{
    uint16_t    rail    = 0;

    if (rails.count(l)) {
        rail            = rails[l];
    }

    return(l);
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


void        Node::link(uint16_t id)
{
    links[links.size()] = id;
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

