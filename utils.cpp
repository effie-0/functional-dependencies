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
                            if (it != visitedNodes.end() && it->second != 4) {
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
                tempComb = node.attributes >> 16;
                test = (uint32_t)1;
                while (test <= tempComb) {
                    tempSub = tempComb ^ test;
                    if (tempSub != tempComb) {
                        it = visitedNodes.find(tempSub << 16);
                        if (it != visitedNodes.end() && (it->second == 1 || it->second == 2 || it->second == 3)) {
                            node.category = 1;
                            break;
                        }
                    }
                    test = test << 1;
                }
                for (it = visitedNodes.find(node.attributes); it != visitedNodes.end(); it++) {
                    if (it->second == 4 || it->second == 5 || it->second == 6) {
                        node.category = 3;
                        break;
                    }
                }
                if (node.category == 0) {
                    // haven't found category, need to calculate partition
                    if (computePartitionSize(node.attributes, r) == computePartitionSize(node.attributes | ((uint32_t)1 << (31 - attributeIndex)), r)) {
                        // node.attributes -> A
                        node.category = 3;
                    } else {
                        node.category = 6;
                    }
                }
            } // not visited
            if (!pickNextNode(node, trace, newNode, minDeps, maxNonDeps)) {
                visitedNodes[node.attributes] = node.category;
                break;
            }
            visitedNodes[node.attributes] = node.category;
        }
        seeds.erase(seeds.begin());
        newSeeds.clear();
        generateNextSeeds(minDeps, maxNonDeps, newSeeds, visitedNodes);
        for (nit = newSeeds.begin(); nit != newSeeds.end(); nit++) {
            seeds.insert(*nit);
        }
    }
}

void generateNextSeeds(std::set<uint32_t> &minDeps, std::set<uint32_t> &maxNonDeps, std::set<Node> &Seeds, std::map<uint32_t, int> visitedNodes) {
    std::set<Node> seeds, newSeeds;
    std::set<uint32_t>::iterator nit;
    int i;

    for (nit = maxNonDeps.begin(); nit != maxNonDeps.end(); nit++) {
        uint32_t maxND = ~(*nit);
        maxND = maxND >> 16;
        if (seeds.empty()) {
            i = 16;
            while(maxND != 0) {
                if ((maxND & 1) == 1) {
                    auto iter = visitedNodes.find((uint32_t) 1 << (32 - i));
                    if (iter != visitedNodes.end()) {
                        seeds.insert(Node((uint32_t) 1 << (32 - i), true, iter->second));
                    }
                    else {
                        seeds.insert(Node((uint32_t) 1 << (32 - i), false, 0));
                    }
                }
                maxND = maxND >> 1;
                i--;
            }
        }
        else {
            i = 16;
            std::set<Node>::iterator iter;
            for (iter = seeds.begin(); iter != seeds.end(); iter++) {
                while(maxND != 0) {
                    if ((maxND & 1) == 1) {
                        uint32_t attr = (uint32_t) 1 << (32 - i) + iter->attributes;
                        auto it = visitedNodes.find(attr);
                        if (it != visitedNodes.end()) {
                            newSeeds.insert(Node(attr, true, it->second));
                        }
                        else {
                            newSeeds.insert(Node(attr, false, 0));
                        }
                    }
                    maxND = maxND >> 1;
                    i--;
                }
            }

            std::set<Node> minimizedNewDeps;
            minimize(minimizedNewDeps, newSeeds);
            seeds.clear();
            seeds = minimizedNewDeps;
            newSeeds.clear();
        }
    }

    std::set<Node>::iterator it;
    for (it = seeds.begin(); it != seeds.end(); it++) {
        if (minDeps.find(it->attributes) == minDeps.end()) {
            Seeds.insert(*it);
        }
    }
}

void minimize(std::set<Node> &mini, std::set<Node> &newSeeds) {
    std::map<Node, bool> value;
    std::set<Node>::iterator it1, it2;

    for(it1 = newSeeds.begin(); it1 != newSeeds.end(); it1++) {
        value[*it1] = true;
    }

    uint32_t temp = 0;
    for (it1 = newSeeds.begin(); it1 != (--newSeeds.end()); it1++) {
        if (!value[*it1])
            continue;
        for (it2 = (++it1); it2 != newSeeds.end(); it2++) {
            it1--;
            if(!value[*it2])
                continue;
            temp = it1->attributes & it2->attributes;
            if (temp == it1->attributes) {
                value[*it2] = false;
                continue;
            }
            else if (temp == it2->attributes) {
                value[*it1] = false;
            }
        }
    }

    auto it = value.begin();
    for (; it != value.end(); it++) {
        if (it->second) {
            mini.insert(it->first);
        }
    }
}
