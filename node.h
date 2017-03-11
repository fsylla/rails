
/* ____________________________________________________________________________
 *
 * file:        node.h
 * created:     Wed Feb 22 22:35:55 2017
 * by:          frank
 *
 * description: nodes are points with static coordinates and connections to
 *              other nodes.
 * ____________________________________________________________________________
 */


#ifndef NODE_H
#define NODE_H


#include <map>
#include <stdint.h>

#define NODE_LINKS_MAX                  4


class Node
{
public:
    Node();
    ~Node();
    void        dump(uint16_t id);
    uint16_t    getHops(uint16_t id);
    uint16_t    getLink(uint16_t l);
    uint16_t    getLinkSize();
    uint16_t    getRail(uint16_t l);
    uint16_t    getToken();
    float       getX();
    float       getY();
    void        link(uint16_t id);
    void        setHops(uint16_t id, uint16_t h);
    void        setPos(float nx, float ny);
    void        setRail(uint16_t l, uint16_t r);
    void        setToken(uint16_t ntoken);

private:
    std::map<uint16_t, uint16_t>    hops;
    std::map<uint16_t, uint16_t>    links;
    std::map<uint16_t, uint16_t>    rails;
    uint16_t                        token;
    float                           x;
    float                           y;
};


#endif

