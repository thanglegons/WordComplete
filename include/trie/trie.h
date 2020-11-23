#ifndef WORDCOMPLETE_TRIE_H
#define WORDCOMPLETE_TRIE_H

#include <iostream>
#include <vector>
#include <trie/node.h>
#include <vn_lang_tool.h>

class Trie {
    std::shared_ptr<TrieNode> root;

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
public:
    Trie() {
        root = std::make_shared<TrieNode>(nullptr, 0);
    }

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
};

#endif //WORDCOMPLETE_TRIE_H
