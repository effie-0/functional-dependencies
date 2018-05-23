//
// Created by 赵尚明 on 2018/5/19.
//

#ifndef FD_UTILS_H
#define FD_UTILS_H

#include <vector>
#include <stdint.h>
#include <set>
#include <map>
#include <stack>
#include <math.h>
#include <time.h>
#include "base.h"
#include "Node.h"

extern std::set<uint32_t> mainLoop(const std::vector<line> &r);  // first 16 bits represent lhs, last 16 bits represent rhs
extern std::set<uint32_t> findLHSs(int attributeIndex, const std::vector<line> &r, const std::vector<bool> &uniqueAttributes);
extern bool isUnique(int attributeIndex, const std::vector<line> &r);
bool pickNextNode(Node &node, std::stack<Node> &trace, Node &nextNode, std::set<uint32_t> &minDeps, std::set<uint32_t> &maxNonDeps, std::map<uint32_t, int> &visitedNodes, const std::vector<bool> &uniqueAttributes, int attributeIndex);
extern void generateNextSeeds(std::set<uint32_t> &minDeps, std::set<uint32_t> &maxNonDeps, std::set<Node> &newSeeds, std::map<uint32_t, int> visitedNodes, const std::vector<bool> &uniqueAttributes, int attributeIndex);
extern int computePartitionSize(uint32_t attributes, const std::vector<line> &r);
extern void minimize(std::set<uint32_t> &mini, std::set<uint32_t> &newSeeds);

#endif //FD_UTILS_H
