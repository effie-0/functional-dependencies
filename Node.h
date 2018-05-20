//
// Created by 赵尚明 on 2018/5/20.
//

#ifndef FD_NODE_H
#define FD_NODE_H

#include <stdint.h>

class Node {
public:
    uint32_t attributes;
    bool isVisited;
    /* unknown: 0, dependency: 1, minimal dependency: 2, candidate minimal dependency: 3, non-dependency: 4,
     * maximal non-dependency: 5, candidate maximal non-dependency: 6 */
    int category;

public:
    Node () {
        attributes = 0;
        isVisited = false;
        category = 0;
    }
    Node (uint32_t attributes, bool isVisited, int category) {
        this->attributes = attributes;
        this->isVisited = isVisited;
        this->category = category;
    }
    bool operator < (const Node &node) const {
        return (attributes < node.attributes);
    }
};



#endif //FD_NODE_H
