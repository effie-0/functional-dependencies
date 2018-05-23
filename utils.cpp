//
// Created by 赵尚明 on 2018/5/19.
//

#include <iostream>
#include "utils.h"

bool isUnique(int attributeIndex, const std::vector<line> &r) {
    int i = 0, rSize = r.size();
    value v = line(r[0])[attributeIndex + 1];
    if (v.isStr) {
        std::map<std::string, int> m; // map from value to line index
        std::map<std::string, int>::iterator it;
        for (i = 0;i < rSize;i++) {
            v = line(r[i])[attributeIndex + 1];
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
            v = line(r[i])[attributeIndex + 1];
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
    std::vector<bool> uniqueAttributes;
    uint32_t tempA;
    int i = 0, j = 0;
    for (i = 0;i < 15;i++) {
        if (isUnique(i, r)) {
            tempA = (uint32_t)1 << (31 - i);
            for (j = 0; j < 15; j++) {
                if (j == i) continue;
                uint32_t tempR = (uint32_t)1 << (15 - j);
                result.insert(tempA | tempR); // if A is unique, add A -> R to result
            }
            uniqueAttributes.push_back(true);
        } else {
            uniqueAttributes.push_back(false);
        }
    }
    for (i = 0;i < 15;i++) {
        if (uniqueAttributes[i]) continue;
        tempA = (uint32_t)1 << (15 - i);
        std::set<uint32_t> k = findLHSs(i, r, uniqueAttributes);
        std::set<uint32_t>::iterator it, itend = k.end();
        for (it = k.begin();it != itend;it++) {
            result.insert((*it) | tempA);
        }
        // debug
        std::cout << "RHS " << (i + 1) << "/15\n";
    }
    return result;
}

int computePartitionSize(uint32_t attributes, const std::vector<line> &r) {
    std::set<line> distinctLines;
    uint32_t test;
    std::vector<bool> attributeVec(15);
    attributes = attributes >> 16;
    for (test = (uint32_t)2; test <= attributes; test = test << 1) {
        if ((test ^ attributes) != attributes) {
            // found an attribute
            attributeVec[15 - (int)log2((double)test)] = true;
        }
    }
    int lineCount = r.size(), i, j = 0;
    for (i = 0;i < lineCount;i++) {
        line tempLine;
        for (j = 0;j < 15;j++) {
            if (attributeVec[j]) {
                tempLine.set(j + 1, line(r[i])[j + 1]);
            }
        }
        distinctLines.insert(tempLine);
    }
    return int(distinctLines.size());
}

std::set<uint32_t> findLHSs(int attributeIndex, const std::vector<line> &r, const std::vector<bool> &uniqueAttributes) {
    // return LHS of mini deps
    std::set<Node> seeds, newSeeds;
    std::set<Node>::iterator nit, nend;
    int i = 0;
    uint32_t tempComb = 0, test = 0, tempSub = 0, tempSup = 0;
    std::map<uint32_t, int> visitedNodes;
    std::map<uint32_t, int>::iterator vnit, vnend;
    std::map<uint32_t, int>::iterator it;
    std::set<uint32_t> minDeps;
    std::set<uint32_t> maxNonDeps;
    for (i = 0; i < 15; i++) {
        if (uniqueAttributes[i] || i == attributeIndex) continue;
        seeds.insert(Node((uint32_t)1 << (31 - i), false, 0));
    }
    while (!seeds.empty()) {
        Node node = *(seeds.begin());
        std::stack<Node> trace;
        while (true) {
            if (node.isVisited) {
                if (node.category == 3) {
                    // minimal dependency candidate
                    tempComb = node.attributes >> 16;
                    test = (uint32_t)2;
                    node.category = 2;
                    while (test <= tempComb) {
                        tempSub = tempComb ^ test;
                        if (tempSub != tempComb) {
                            // find a sub combination
                            it = visitedNodes.find(tempSub << 16);
                            if (it == visitedNodes.end() || (it->second != 4 && it->second != 5 && it->second != 6)) {
                                node.category = 3;
                            }
                        }
                        test = test << 1;
                    }
                    if (node.category == 2) {
                        // a new minimal dependency
                        minDeps.insert(node.attributes);
                    }
                } else if (node.category == 6) {
                    // maximal non-dependency candidate
                    tempComb = node.attributes >> 16;
                    test = (uint32_t)2;
                    node.category = 5;
                    while (test <= 32768) {
                        if (!uniqueAttributes[15 - (int)log2((double)test)]) {
                            tempSup = tempComb | test;
                            if (tempSup != tempComb) {
                                // find a super combination
                                it = visitedNodes.find(tempSup << 16);
                                if (it == visitedNodes.end() || (it->second != 1 && it->second != 2 && it->second != 3)) {
                                    node.category = 6;
                                }
                            }
                        }
                        test = test << 1;
                    }
                    if (node.category == 5) {
                        // a new max non-dependency
                        maxNonDeps.insert(node.attributes);
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
                    } else if ((vnit->first | node.attributes) == vnit->first) {
                        // a superset
                        node.category = 4;
                    }
                }
                if (node.category == 0) {
                    // need partition
                    if (computePartitionSize(node.attributes, r) == computePartitionSize(node.attributes | ((uint32_t)1 << (31 - attributeIndex)), r)) {
                        // node.attribute -> A
                        node.category = 3;
                    } else {
                        node.category = 6;
                    }
                }
            } // not visited
            node.isVisited = true;
            Node newNode;
            bool flag = pickNextNode(node, trace, newNode, minDeps, maxNonDeps, visitedNodes, uniqueAttributes, attributeIndex);
            visitedNodes[node.attributes] = node.category;
            if (!flag) break;
            node = newNode;
        }
        seeds.erase(*seeds.begin());
        newSeeds.clear();
        generateNextSeeds(minDeps, maxNonDeps, newSeeds, visitedNodes, uniqueAttributes, attributeIndex);
        nend = newSeeds.end();
        for (nit = newSeeds.begin(); nit != nend; nit++) {
            seeds.insert(*nit);
        }
    }
    return minDeps;
}


bool pickNextNode(Node &node, std::stack<Node> &trace, Node &nextNode, std::set<uint32_t> &minDeps, std::set<uint32_t> &maxNonDeps, std::map<uint32_t, int> &visitedNodes, const std::vector<bool> &uniqueAttributes, int attributeIndex) {
    uint32_t tempComb, test, tempSub, tempSup;
    std::map<uint32_t, std::vector<uint32_t>>::iterator supit;
    std::set<uint32_t> nodes;
    std::set<uint32_t>::iterator nit, nend;
    std::vector<uint32_t> nodeVec;
    uint32_t attributeTest = (uint32_t)1 << (15 - attributeIndex);
    auto vnend = visitedNodes.end();
    if (node.category == 3) {
        // candidate minimal dep
        tempComb = node.attributes >> 16;
        test = (uint32_t)2;
        while (test <= tempComb) {
            tempSub = tempComb ^ test;
            if (tempSub != tempComb && visitedNodes.find(tempSub << 16) == vnend) {
                // find a subset
                nodes.insert(tempSub << 16);
                // TO DO: haven't pruned
            }
            test = test << 1;
        }
        nend = nodes.end();
        for (nit = nodes.begin(); nit != nend; nit++) {
            nodeVec.push_back(*nit);
        }
        if (nodes.empty()) {
            node.category = 2;
            minDeps.insert(node.attributes);
        } else {
            srand((unsigned)time(NULL));
            nextNode.attributes = nodeVec[rand() % (nodes.size())];
            trace.push(node);
            return true;
        }
    } else if (node.category == 6) {
        tempComb = node.attributes >> 16;
        test = (uint32_t)2;
        while (test <= 32768) {
            if (!uniqueAttributes[15 - (int)log2((double)test)] && test != attributeTest) {
                tempSup = tempComb | test;
                if (tempSup != tempComb && visitedNodes.find(tempSup << 16) == vnend) {
                    // find a super combination
                    nodes.insert(tempSup << 16);
                }
            }
            test = test << 1;
        }
        nend = nodes.end();
        for (nit = nodes.begin(); nit != nend; nit++) {
            nodeVec.push_back(*nit);
        }
        if (nodes.empty()) {
            node.category = 5;
            maxNonDeps.insert(node.attributes);
        } else {
            nextNode.attributes = nodeVec[rand() % (nodes.size())];
            trace.push(node);
            return true;
        }
    }
    if (trace.empty()) return false;
    nextNode = trace.top();
    trace.pop();
    return true;
}


void generateNextSeeds(std::set<uint32_t> &minDeps, std::set<uint32_t> &maxNonDeps, std::set<Node> &Seeds, std::map<uint32_t, int> visitedNodes, const std::vector<bool> &uniqueAttributes, int attributeIndex) {
    std::set<uint32_t> seeds, newSeeds;
    std::set<uint32_t>::iterator nit, nend = maxNonDeps.end();
    std::map<uint32_t, int>::iterator vit, vnend = visitedNodes.end();
    std::set<uint32_t>::iterator it;
    int i;

    for (nit = maxNonDeps.begin(); nit != nend; nit++) {
        uint32_t maxND = ~(*nit);
        maxND = maxND >> 16;
        if (seeds.empty()) {
            i = 16;
            while(maxND != 0) {
                if (i <= 15 && (!uniqueAttributes[i - 1]) && attributeIndex != i - 1) {
                    if ((maxND & 1) == 1) {
                        auto iter = visitedNodes.find((uint32_t)1 << (32 - i));
                        if (iter == vnend) {
                            seeds.insert((uint32_t)1 << (32 - i));
                        }
                    }
                }
                maxND = maxND >> 1;
                i--;
            }
        }
        else {
            std::set<uint32_t>::iterator iter;
            auto send = seeds.end();
            for (iter = seeds.begin(); iter != send; iter++) {
                i = 16;
                maxND = ~(*nit);
                maxND = maxND >> 16;
                while(maxND != 0) {
                    if (i <= 15 && (!uniqueAttributes[i - 1]) && attributeIndex != i - 1) {
                        if ((maxND & 1) == 1) {
                            uint32_t attr = (uint32_t) (1 << (32 - i)) | (*iter);
                            if (visitedNodes.find(attr) == vnend) {
                                newSeeds.insert(attr);
                            }
                        }
                    }
                    maxND = maxND >> 1;
                    i--;
                }
            }

            std::set<uint32_t> minimizedNewDeps;
            minimize(minimizedNewDeps, newSeeds);
            seeds.clear();
            seeds = minimizedNewDeps;
            newSeeds.clear();
        }
    }

    auto mnend = minDeps.end();
    for (it = minDeps.begin(); it != mnend; it++) {
        seeds.erase(*it);
    }
    auto send = seeds.end();
    for (it = seeds.begin(); it != send; it++) {
        Seeds.insert(Node(*it, false, 0));
    }
}

void minimize(std::set<uint32_t> &mini, std::set<uint32_t> &newSeeds) {
    std::map<uint32_t, bool> value;
    std::set<uint32_t>::iterator it1, it2;
    int seedNum = newSeeds.size();
    auto nsend = newSeeds.end(), nslbo = --newSeeds.end();
    if (seedNum == 0 || seedNum == 1) {
        mini = newSeeds;
        return;
    }

    for(it1 = newSeeds.begin(); it1 != nsend; it1++) {
        value[*it1] = true;
    }

    uint32_t temp = 0;
    for (it1 = newSeeds.begin(); it1 != nslbo; it1++) {
        if (!value[*it1])
            continue;
        for (it2 = it1++; it2 != nsend; it2++) {
            if (it2 == it1) it1--;
            if(!value[*it2])
                continue;
            temp = (*it1) & (*it2);
            if (temp == (*it1)) {
                value[*it2] = false;
                continue;
            }
            else if (temp == (*it2)) {
                value[*it1] = false;
            }
        }
    }

    auto it = value.begin(), vend = value.end();
    for (; it != vend; it++) {
        if (it->second) {
            mini.insert(it->first);
        }
    }
}
