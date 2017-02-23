
/* ____________________________________________________________________________
 *
 * file:        nodemap.h
 * created:     Thu Jan 19 17:58:47 2017
 * by:          frank
 *
 * description: nodemaps are maps of connected nodes for navigation from one
 *              node to another
 * ____________________________________________________________________________
 */


#ifndef NODEMAP_H
#define NODEMAP_H

#include <stdint.h>
#include <map>

#include "node.h"
#include "train.h"


class NodeMap
{
public:
    NodeMap(uint16_t nsize);
    ~NodeMap();
    int         connect(uint16_t id1, uint16_t id2, uint8_t duplex);
    void        dump();
    void        dumpHops();
    void        evalHops();
    uint16_t    getSize();
    uint16_t    getToken(uint16_t id);
    float       getX(uint16_t id);
    float       getY(uint16_t id);
    int         load(const char* path);
    int         loadHops(const char* path);
    int         route(uint16_t id1, uint16_t id2);
    int         save(const char* path);
    int         saveHops(const char* path);
    int         trainAdd(Train* train);
    int         trainRun(Train* train);

private:
    std::map<uint16_t,Node> nodes;

};

#endif

