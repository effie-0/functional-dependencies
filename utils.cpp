//
// Created by 赵尚明 on 2018/5/19.
//

#include "utils.h"

bool isUnique(int attributeIndex, const std::vector<line> &r) {
    int i = 0, rSize = r.size();
    value v = r[0][attributeIndex];
    if (v.isStr) {
        std::map<std::string, int> m; // map from value to line index
        std::map<std::string, int>::iterator it;
        for (i = 0;i < rSize;i++) {
            v = r[i][attributeIndex];
            it = m.find(v.str);
            if (it != m.end()) {
                if (!(r[i] == r[it->second])) return false;
            } else {
                m[v.str] = i;
            }
        }
    } else {
        std::map<int, int> m;
        std::map<int, int>::iterator it;
        for (i = 0;i < rSize;i++) {
            v = r[i][attributeIndex];
            it = m.find(v.num);
            if (it != m.end()) {
                if (!(r[i] == r[it->second])) return false;
            } else {
                m[v.num] = i;
            }
        }
    }
    return true;
}

std::set<uint32_t> mainLoop(const std::vector<line> &r) {
    std::set<uint32_t> result;
    int i = 0;
    for (i = 0;i < 15;i++) {
        if (isUnique(i, r)) {
            uint32_t tempA = (uint32_t)1 << (31 - i);
            uint32_t tempR = (uint32_t)65535 ^ ((uint32_t)1 << (15 - i));
            result.insert(tempA | tempR); // if A is unique, add A -> (R / {A}) to result
        }
    }
    for (i = 0;i < 15;i++) {
        std::set<uint32_t> k = findLHSs(i, r);
        std::set<uint32_t>::iterator it;
        for (it = k.begin();it != k.end();it++) {
            result.insert(*it);
        }
    }
    return result;
}

int computePartitionSize(uint32_t attributes, const std::vector<line> &r) {
    std::set<line> distinctLines;
    uint32_t test;
    std::vector<bool> attributeVec(15);
    attributes = attributes >> 16;
    for (test = (uint32_t)1; test <= 32768; test = test << 1) {
        if ((test ^ attributes) != attributes) {
            // found an attribute
            attributeVec[14 - (int)log2((double)test)] = 1;
        }
    }
    int lineCount = r.size(), i, j = 0;
    for (i = 0;i < lineCount;i++) {
        line tempLine;
        for (j = 0;j < 15;j++) {
            if (attributeVec[j] == 1) {
                tempLine[j] = r[i][j];
            }
        }
        distinctLines.insert(tempLine);
    }
    return int(distinctLines.size());
}

std::set<uint32_t> findLHSs(int attributeIndex, const std::vector<line> &r) {
    std::set<Node> seeds, newSeeds;
    std::set<Node>::iterator nit;
    int i = 0;
    uint32_t tempComb = 0, test = 0, tempSub = 0, tempSup = 0;
    std::map<uint32_t, int> visitedNodes;
    std::map<uint32_t, int>::iterator vnit, vnend;
    std::vector<uint32_t>::iterator vit, vend;
    std::map<uint32_t, int>::iterator it;
    std::set<uint32_t> minDeps;
    std::set<uint32_t> maxNonDeps;
    for (i = 0;i < 15;i++) {
        if (i == attributeIndex) continue;
        seeds.insert(Node((uint32_t)1 << (31 - i), false, 0));  // use left 16 bits to store combination
    }
    while (!seeds.empty()) {
        Node node = *(seeds.begin()), newNode;
        std::stack<Node> trace;
        while (true) {
            if (node.isVisited) {
                if (node.category == 3) {
                    // minimal dependency candidate
                    tempComb = node.attributes >> 16;
                    test = (uint32_t)1;
                    node.category = 2;
                    while (test <= tempComb) {
                        tempSub = tempComb ^ test;
                        if (tempSub != tempComb) {
                            // find a sub combination
                            it = visitedNodes.find(tempSub << 16);
                            if (it == visitedNodes.end() || it->second != 4) {
                                node.category = 3;
                            }
                        }
                        test = test << 1;
                    }
                    if (node.category == 2) {
                        // a new minimal dependency
                        minDeps.insert(node.attributes | ((uint32_t)1 << (15 - attributeIndex)));
                    }
                } else if (node.category == 6) {
                    // maximal non-dependency candidate
                    tempComb = node.attributes >> 16;
                    test = (uint32_t)1;
                    node.category = 5;
                    while (test <= 32768) {
                        tempSup = tempComb | test;
                        if (tempSup != tempComb) {
                            // find a super combination
                            it = visitedNodes.find(tempSup << 16);
                            if (it != visitedNodes.end() && it->second != 1) {
                                node.category = 6;
                            }
                        }
                        test = test << 1;
                    }
                    if (node.category == 5) {
                        // a new max non-dependency
                        maxNonDeps.insert(node.attributes | ((uint32_t)1 << (15 - attributeIndex)));
                    }
                }
            } else {
                // not visited
                vnend = visitedNodes.end();
                for (vnit = visitedNodes.begin(); vnit != vnend; vnit++) {
                    if ((vnit->first | node.attributes) == node.attributes && (vnit->second == 1 || vnit->second == 2 || vnit->second == 3)) {
                        // a subset
                        node.category = 1;
                        break;
                    } else if ((vnit->first | node.attributes) == vnit->first && (vnit->first | node.attributes) != node.attributes) {
                        // a superset
                        node.category = 4;
                    }
                }
            } // not visited
            bool flag = pickNextNode(node, trace, newNode, minDeps, maxNonDeps, visitedNodes);
            visitedNodes[node.attributes] = node.category;
            if (!flag) break;
        }
        seeds.erase(*seeds.begin());
        newSeeds.clear();
        generateNextSeeds(minDeps, maxNonDeps, newSeeds);
        for (nit = newSeeds.begin(); nit != newSeeds.end(); nit++) {
            seeds.insert(*nit);
        }
    }
}

bool pickNextNode(Node &node, std::stack<Node> &trace, Node &nextNode, std::set<uint32_t> &minDeps, std::set<uint32_t> &maxNonDeps, std::map<uint32_t, int> &visitedNodes) {
    uint32_t tempComb, test, tempSub, tempSup;
    std::map<uint32_t, std::vector<uint32_t>>::iterator supit;
    std::set<uint32_t> nodes;
    if (node.category == 3) {
        // candidate minimal dep
        tempComb = node.attributes >> 16;
        test = (uint32_t)1;
        while (test <= tempComb) {
            tempSub = tempComb ^ test;
            if (tempSub != tempComb) {
                // find a subset
                if (visitedNodes.find(tempSub << 16) == visitedNodes.end()) {
                    nodes.insert(tempSub << 16);
                }
                // TO DO: haven't pruned
            }
            test = test << 1;
        }
        if (nodes.size() == 0) {
            minDeps.insert(node);
        } else {
            srand((unsigned)time(NULL));
            nextNode.attributes = nodes[rand() % (nodes.size())];
            trace.push(node);
            return true;
        }
    } else if (node.category == 6) {
        tempComb = node.attributes >> 16;
        test = (uint32_t)1;
        while (test <= 32768) {
            tempSup = tempComb | test;
            if (tempSup != tempComb) {
                // find a super combination
                if (visitedNodes.find(tempSup << 16) == visitedNodes.end()) {
                    nodes.insert(tempSup << 16);
                }
            }
            test = test << 1;
        }
        if (nodes.size() == 0) {
            maxNonDeps.insert(node);
        } else {
            nextNode.attributes = nodes[rand() % (nodes.size())];
            trace.push(node);
            return true;
        }
    } else {
        if (trace.empty()) return false;
        nextNode = trace.top();
        trace.pop();
        return true;
    }
}


