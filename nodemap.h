
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
#include "rail.h"
#include "train.h"


class NodeMap
{
public:
    NodeMap();
    ~NodeMap();

    std::map<uint16_t,Node> *getNodes();
    std::map<uint16_t,Rail> *getRails();
    
    void        hopsDump();
    void        hopsEval();
    int         hopsLoad(const char* path);
    int         hopsSave(const char* path);
    
    uint16_t    nodeAdd(float x, float y);
    uint16_t    nodeGetToken(uint16_t id);
    float       nodeGetX(uint16_t id);
    float       nodeGetY(uint16_t id);
    int         nodeLink(uint16_t n1, uint16_t n2, uint8_t duplex);
    
    void        nodesDump();
    int         nodesLoad(const char* path);
    int         nodesSave(const char* path);

    uint16_t    railAdd(uint16_t n1, uint16_t n2, float r);
    void        railsDump();
    int         railsLoad(const char* path);
    int         railsSave(const char* path);
    
    int         trainAdd(Train* train);
    int         trainRun(Train* train);

private:
    uint16_t                    nodeIdMax;
    std::map<uint16_t,Node>     nodes;
    uint16_t                    railIdMax;
    std::map<uint16_t,Rail>     rails;

};

#endif

