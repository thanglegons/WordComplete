//
// Created by PC on 11/25/2020.
//

#ifndef WORDCOMPLETE_HUFFMANTREENODE_H
#define WORDCOMPLETE_HUFFMANTREENODE_H

#include <iostream>
#define MAX_SIZE 300

class HuffmanTreeNode {
public:
    int data;
    int freq;

    std::shared_ptr <HuffmanTreeNode> left;
    std::shared_ptr <HuffmanTreeNode> right;

    HuffmanTreeNode(int character,
                    int frequency)
    {
        data = character;
        freq = frequency;
        left = right = nullptr;
    }
};

class Compare {
public:
    bool operator()(std::shared_ptr<HuffmanTreeNode> a,
                    std::shared_ptr<HuffmanTreeNode> b)
    {
        return a->freq > b->freq;
    }
};

#endif //WORDCOMPLETE_HUFFMANTREENODE_H
