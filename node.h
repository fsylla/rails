
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


#define NODE_SIZE_CONNECTIONS       3


#include <map>
#include <stdint.h>


class Node
{
public:
    Node();
    ~Node();
    int         connect(uint16_t id);
    void        dump(uint16_t id);
    uint16_t    getConnection(uint8_t c);
    uint16_t    getHops(uint16_t id);
    uint8_t     getNc();
    uint16_t    getToken();
    float       getX();
    float       getY();
    void        setHops(uint16_t nid, uint16_t nhops);
    void        setPos(float nx, float ny);
    void        setToken(uint16_t ntoken);

private:
    uint16_t    connection[NODE_SIZE_CONNECTIONS];
    uint8_t     nc;
    uint16_t    token;
    float       x;
    float       y;

    std::map<uint16_t, uint16_t>    hops;
};


#endif

