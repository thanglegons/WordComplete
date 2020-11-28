#ifndef WORDCOMPLETE_TRIE_H
#define WORDCOMPLETE_TRIE_H

#include <iostream>
#include <utility>
#include <vector>
#include <trie/node.h>
#include <vn_lang_tool.h>

class Trie {
    std::shared_ptr<TrieNode> root = nullptr;
    std::shared_ptr<Huffman> huffman = nullptr;

    static void update_candidate(const std::shared_ptr<TrieNode>& node) {
        std::shared_ptr<TrieNode> temp = node;
        std::vector<uint32_t> suffix_codepoints;
        int count = temp->get_count();
        while (temp != nullptr) {
            if (!suffix_codepoints.empty()) {
                temp->add_candidate(VnLangTool::vector_to_string(suffix_codepoints), count);
            }
            suffix_codepoints.insert(suffix_codepoints.begin(), temp->get_label());
            temp = temp->get_parent();
        }
    }

    void recursive_compress(const std::shared_ptr<TrieNode>& node) {
        node->compress_data(this->huffman);
        for (auto& child : node->get_children()) {
            std::shared_ptr<TrieNode> child_node = child.second;
            recursive_compress(child_node);
        }
    }
public:
    Trie() {
        root = std::make_shared<TrieNode>(nullptr, 0);
    }

    Trie(std::shared_ptr<TrieNode> root, std::shared_ptr<Huffman> huffman) : root(std::move(root)),
                                                                                           huffman(std::move(huffman)) {}

    std::shared_ptr<TrieNode> get_root() {
        return root;
    }

    void insert(const std::string& text, int32_t count) {
        std::vector<uint32_t> codepoints = VnLangTool::to_UTF(text);
        std::shared_ptr<TrieNode> temp = root;
        for (uint32_t codepoint : codepoints) {
            std::shared_ptr<TrieNode> next_node = temp->get_child(codepoint);
            if (next_node == nullptr) {
                next_node = std::make_shared<TrieNode>(temp, codepoint);
                temp->set_child(codepoint, next_node);
            }
            next_node->change_count(count);
            temp = next_node;
        }
        update_candidate(temp);
    }

    void compress_tree() {
        this->huffman = std::make_shared<Huffman>();
        recursive_compress(this->root);
    }

    std::vector<Candidate> get_suggestion(const std::string& sub_string) {
        std::shared_ptr<TrieNode> node = this->root;
        std::vector<uint32_t> codepoints = VnLangTool::to_UTF(sub_string);
        for (uint32_t codepoint : codepoints) {
            node = node->get_child(codepoint);
            if (node == nullptr) return {};
        }
        return node->get_suggestion(this->huffman);
    }
};

#endif //WORDCOMPLETE_TRIE_H
