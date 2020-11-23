#ifndef WORDCOMPLETE_NODE_H
#define WORDCOMPLETE_NODE_H

#include <iostream>
#include <unordered_map>
#include <set>
#include <utility>
#include <queue>
#include <algorithm>
#include <memory>

class Candidate {
    std::string suffix;
    int32_t count;
public:
    Candidate(std::string suffix, int32_t count) {
        this->suffix = std::move(suffix);
        this->count = count;
    }

    bool operator<(const Candidate& rhs) const {
        return rhs.count < count;
    }

    const std::string &get_suffix() const {
        return suffix;
    }

    int32_t get_count() const {
        return count;
    }
};


class Candidates {
    const int LIMIT = 5;
    std::unordered_map<std::string, int32_t> suffix_to_count;
    std::priority_queue<Candidate> candidate_queue;
public:
    void add_candidate(const std::string& suffix, int32_t count) {
        if (suffix_to_count.find(suffix) != suffix_to_count.end()) {
            suffix_to_count[suffix] = count;
        } else {
            suffix_to_count[suffix] = count;
            candidate_queue.push(Candidate(suffix, count));
            update_queue();
        }
    }

    void update_queue() {
        while (candidate_queue.size() > LIMIT) {
            Candidate min_candidate = candidate_queue.top();
            const std::string& suffix = min_candidate.get_suffix();
            int count = min_candidate.get_count();
            candidate_queue.pop();
            if (suffix_to_count[suffix] != count) {
                candidate_queue.push(Candidate(suffix, suffix_to_count[suffix]));
                continue;
            } else {
                suffix_to_count.erase(suffix);
            }
        }
    }

    std::vector<Candidate> get_suggestion() {
        std::vector<Candidate> suggestion;
        for (const auto& it : suffix_to_count) {
            suggestion.emplace_back(it.first, it.second);
        }
        sort(suggestion.begin(), suggestion.end());
        return suggestion;
    }
};

class TrieNode {
    std::shared_ptr<TrieNode> parent;
    std::unordered_map<uint32_t, std::shared_ptr<TrieNode>> children;
    int32_t count = 0;
    uint32_t label;
    Candidates candidates;
public:
    TrieNode(std::shared_ptr<TrieNode> parent, uint32_t label) {
        this->parent = std::move(parent);
        this->label = label;
    }

    std::shared_ptr<TrieNode> get_child(uint32_t nxt_c) {
        if (children.find(nxt_c) == children.end()) return nullptr;
        return children[nxt_c];
    }

    std::shared_ptr<TrieNode> get_parent() {
        return parent;
    }

    uint32_t get_label() const {
        return label;
    }

    void set_child(uint32_t nxt_c, std::shared_ptr<TrieNode> nxt_n) {
        children[nxt_c] = std::move(nxt_n);
    }

    void change_count(int32_t delta) {
        count += delta;
    }

    int32_t get_count() const {
        return count;
    }

    void add_candidate(const std::string& suffix, uint32_t _count) {
        candidates.add_candidate(suffix, _count);
    }

    std::vector<Candidate> get_suggestion() {
        return candidates.get_suggestion();
    }
};

#endif //WORDCOMPLETE_NODE_H
