//
// Created by PC on 11/25/2020.
//

#ifndef WORDCOMPLETE_HUFFMAN_H
#define WORDCOMPLETE_HUFFMAN_H


#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <memory>
#include "HuffmanTreeNode.h"
#include "vn_lang_tool.h"

#define DECODE_FILE "./resources/decode_table.txt"
#define RES_FILE "./resources/VNESEcorpus.txt"

const uint32_t UINT32_LIM = uint32_t(1) << uint32_t(28);

class Huffman {
public:
    std::shared_ptr<HuffmanTreeNode> root;
    std::map<int, int> freq;
    std::map<int, std::string> huffman_table;
    std::map<std::string, int> re_huffman_table;

    void BuildTable(const std::shared_ptr<HuffmanTreeNode>& node, int depth, char encoding[]) {
        if (node->left == nullptr && node->right == nullptr) {
            std::string decode;
            huffman_table[node->data] = "";
            for (int i = 0; i < depth; i++)
                decode += char('0' + encoding[i]);
            huffman_table[node->data] = decode;
            re_huffman_table[decode] = node->data;
        } else {
            if (node->left != nullptr) {
                encoding[depth] = 0;
                BuildTable(node->left, depth + 1, encoding);
            }
            if (node->right != nullptr) {
                encoding[depth] = 1;
                BuildTable(node->right, depth + 1, encoding);
            }
        }
    }

    Huffman() {
//        build_huffman_from_file(RES_FILE);
        if (!loadDecodeTableFromFile()) {
            std::cout << "Load from file failed. Proceed to rebuild from text\n";
            build_huffman_from_file(RES_FILE);
        } else
            std::cout << "Load table from file success\n";
    }

    void build_huffman_from_file(const std::string &data_path) {
        //Load text from file into "text"
        VnLangTool::init("", true);
        std::ifstream input_file;
        input_file.open(data_path);
        std::string line;
        std::string text;
        while (getline(input_file, line)) {
            // normalize each line
            std::string normalized_line = VnLangTool::normalize_lower_NFD_UTF(line);
            text += normalized_line;
            //Limit character read in
//            if (text.size() >= 1000000)
//                break;
        }

        std::vector<uint32_t> utf8_text = VnLangTool::to_UTF(text);
        //Huffman build
        freq.clear();
        huffman_table.clear();
        for (uint32_t code : utf8_text) {
            freq[code]++;
        }
        std::priority_queue<
                std::shared_ptr<HuffmanTreeNode>,
                std::vector<std::shared_ptr<HuffmanTreeNode> >,
                Compare> huffman_pq;

        std::map<int, int>::iterator it;
        for (it = freq.begin(); it != freq.end(); it++) {
            auto newNode = std::make_shared<HuffmanTreeNode>(it->first, it->second);
            huffman_pq.push(newNode);
        }
        while (huffman_pq.size() != 1) {
            auto left = huffman_pq.top();
            huffman_pq.pop();
            auto right = huffman_pq.top();
            huffman_pq.pop();
            auto node = std::make_shared<HuffmanTreeNode>('$',
                                                          left->freq + right->freq);
            node->left = left;
            node->right = right;
            huffman_pq.push(node);
        }
        root = huffman_pq.top();
        char arr[MAX_SIZE];
        BuildTable(root, 0, arr);
        input_file.close();
    }

    std::shared_ptr<HuffmanTreeNode> get_root() const {
        return root;
    }

    uint32_t* encode(const std::string &text) {
        //std::string encoded = "";
        const std::string &normalized_text = text;//VnLangTool::normalize_lower_NFD_UTF(text);
        std::vector<uint32_t> utf8_text = VnLangTool::to_UTF(normalized_text);

        std::vector<uint32_t> encoded;
        encoded.push_back(1);
        int tail = 0;

        for (uint32_t code : utf8_text) {
            if (huffman_table[code].empty())
                std::cout << "encode error " << code << "\n";
            for (char c : huffman_table[code]) {
                int bit = c - '0';
                encoded[tail] = encoded[tail] * 2 + bit;
                if (encoded[tail] >= UINT32_LIM) {
                    encoded.push_back(1);
                    tail++;
                }
            }
        }
        tail = encoded.size();
        auto da_encoded = new uint32_t[tail + 1];
        da_encoded[0] = tail;
        for (int i = 1; i <= tail; i++)
            da_encoded[i] = encoded[i - 1];
        return da_encoded;
    }

    static std::string int_to_string(uint32_t code) {
        std::string str = std::bitset<32>(code).to_string();
        std::string normalized_str;
        bool flag = false;
        for (char c: str) {
            if (flag) {
                normalized_str += c;
            }
            if (c != '0')
                flag = true;
        }
        return normalized_str;
    }

    std::string decode(uint32_t da_text[]) {
        if (da_text == nullptr) return "";
        std::vector<uint32_t> utf8_text;
        std::vector<uint32_t> text;
        std::string prefix;
        uint32_t size = da_text[0];
        for (int i = 1; i <= size; i++) {
            text.push_back(da_text[i]);
        }
        for (uint32_t code : text) {
            std::string binary_str = int_to_string(code);
            for (char c: binary_str) {
                prefix += c;
                if (re_huffman_table[prefix]) {
                    utf8_text.push_back(re_huffman_table[prefix]);
                    prefix = "";
                }
            }
        }
        if (!prefix.empty())
            std::cout << "decode error!\n";
        std::string decoded = VnLangTool::vector_to_string(utf8_text);
        return decoded;
    }

    void saveDecodeTable() {
        std::ofstream fo;
        fo.open(DECODE_FILE);
        fo << re_huffman_table.size() << "\n";
        std::map<std::string, int>::iterator it;
        for (it = re_huffman_table.begin(); it != re_huffman_table.end(); it++) {
            fo << it->first << " " << it->second << "\n";
        }
        fo << huffman_table.size() << "\n";
        std::map<int, std::string>::iterator it2;
        for (it2 = huffman_table.begin(); it2 != huffman_table.end(); it2++) {
            fo << it2->first << " " << it2->second << "\n";
        }
        fo.close();
    }

    int loadDecodeTableFromFile() {
        std::ifstream fi;
        fi.open(DECODE_FILE);
        int tableSize = 0;
        re_huffman_table.clear();
        huffman_table.clear();
        if (fi >> tableSize) {
            while (tableSize-- > 0) {
                std::string key;
                int value;
                fi >> key >> value;
                re_huffman_table[key] = value;
            }
            fi >> tableSize;
            while (tableSize-- > 0) {
                std::string key;
                int value;
                fi >> value >> key;
                huffman_table[value] = key;
            }
            fi.close();
            return 1;
        }
        fi.close();
        return 0;
    }
};

#endif //WORDCOMPLETE_HUFFMAN_H
