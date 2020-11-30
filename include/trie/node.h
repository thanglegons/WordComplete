#ifndef WORDCOMPLETE_NODE_H
#define WORDCOMPLETE_NODE_H

#include <iostream>
#include <unordered_map>
#include <set>
#include <utility>
#include <queue>
#include <algorithm>
#include <memory>
#include <huffman/Huffman.h>
#include <cassert>

class Candidate {
    std::shared_ptr<std::string> suffix;
    uint32_t* compressed_suffix = nullptr;
    int32_t count = 0;
public:
    Candidate() = default;

    virtual ~Candidate() {
        delete compressed_suffix;
    }

    Candidate(std::shared_ptr<std::string> suffix, int32_t count) {
        this->suffix = std::move(suffix);
        this->count = count;
    }

    Candidate(uint32_t *_compressed_suffix, int32_t count) {
        this->compressed_suffix = _compressed_suffix;
        this->count = count;
    }

    bool operator<(const Candidate& rhs) const {
        return rhs.count < count;
    }

    const std::string &get_suffix() const {
        return *suffix;
    }

    const std::shared_ptr<std::string> &get_raw_suffix() const {
        return suffix;
    }

    int32_t get_count() const {
        return count;
    }

    uint32_t *get_compressed_suffix() const {
        return compressed_suffix;
    }
};


class Candidates {
    static const int LIMIT = 5;
    std::unordered_map<std::string, int32_t> *suffix_to_count{};
    std::priority_queue<Candidate> *candidate_queue{};
    std::vector<Candidate*> compressed_candidates;
public:

    explicit Candidates(bool is_compressed) {
        if (!is_compressed) {
            suffix_to_count = new std::unordered_map<std::string, int32_t>();
            candidate_queue = new std::priority_queue<Candidate>();
        }
    }

    ~Candidates() {
        delete suffix_to_count;
        delete candidate_queue;
        for (auto& candidate : compressed_candidates) delete candidate;
        compressed_candidates.clear();
    }

    void set_compressed_candidates(const std::vector<Candidate*> &_compressed_candidates) {
        this->compressed_candidates = _compressed_candidates;
    }

    void add_candidate(const std::string& suffix, int32_t count) {
        if (suffix_to_count->find(suffix) != suffix_to_count->end()) {
            suffix_to_count->operator[](suffix) = count;
        } else {
            suffix_to_count->operator[](suffix) = count;
            candidate_queue->push(Candidate(std::make_shared<std::string>(suffix), count));
            update_queue();
        }
    }

    void update_queue() {
        while (candidate_queue->size() > LIMIT) {
            Candidate min_candidate = candidate_queue->top();
            const std::shared_ptr<std::string>& suffix = min_candidate.get_raw_suffix();
            int count = min_candidate.get_count();
            candidate_queue->pop();
            if (suffix_to_count->operator[](*suffix) != count) {
                candidate_queue->push(Candidate(suffix, suffix_to_count->operator[](*suffix)));
                continue;
            } else {
                suffix_to_count->erase(*suffix);
            }
        }
    }

    std::vector<Candidate> get_suggestion(const std::shared_ptr<Huffman>& huffman = nullptr) {
        if (huffman == nullptr) {
            std::vector<Candidate> suggestion;
            for (const auto& it : *suffix_to_count) {
                suggestion.emplace_back(std::make_shared<std::string>(it.first), it.second);
            }
            sort(suggestion.begin(), suggestion.end());
            return suggestion;
        } else {
            std::vector<Candidate> suggestion;
            for (const auto & compressed_candidate : compressed_candidates) {
                uint32_t* compressed_suffix = compressed_candidate->get_compressed_suffix();
                if (compressed_suffix == nullptr) break;
                std::string decoded_suffix = huffman->decode(compressed_suffix);
                int32_t count = compressed_candidate->get_count();
                suggestion.emplace_back(std::make_shared<std::string>(decoded_suffix), count);
            }
            return suggestion;
        }
    }

    void compress_candidates(const std::shared_ptr<Huffman>& huffman) {
        std::vector<Candidate> candidates = get_suggestion();
        delete suffix_to_count;
        delete candidate_queue;
        for (auto & candidate : candidates) {
            uint32_t* compress_suffix = huffman->encode(candidate.get_suffix());
            compressed_candidates.push_back(new Candidate(compress_suffix, candidate.get_count()));
        }
    }

    const std::vector<Candidate *> &get_compressed_candidates() const {
        return compressed_candidates;
    }
};

class TrieNode {
    std::shared_ptr<TrieNode> parent;
    std::unordered_map<uint32_t, std::shared_ptr<TrieNode>> children;
    int32_t count = 0;
    uint32_t label;
    std::shared_ptr<Candidates> candidates;

public:
    TrieNode(std::shared_ptr<TrieNode> parent, uint32_t label) {
        this->parent = std::move(parent);
        this->label = label;
        candidates = std::make_shared<Candidates>(false);
    }

    TrieNode(uint32_t label, int32_t count, const std::vector<Candidate*>& compressed_candidates) {
        this->label = label;
        this->count = count;
        candidates = std::make_shared<Candidates>(true);
        this->candidates->set_compressed_candidates(compressed_candidates);
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
        candidates->add_candidate(suffix, _count);
    }

    std::vector<Candidate> get_suggestion(const std::shared_ptr<Huffman>& huffman) {
        return candidates->get_suggestion(huffman);
    }

    void compress_data(const std::shared_ptr<Huffman>& huffman) {
        candidates->compress_candidates(huffman);
    }

    const std::unordered_map<uint32_t, std::shared_ptr<TrieNode>> &get_children() const {
        return children;
    }

    const std::shared_ptr<Candidates> &get_candidates() const {
        return candidates;
    }

    void set_parent(const std::shared_ptr<TrieNode> &_parent) {
        TrieNode::parent = _parent;
    }
};

#endif //WORDCOMPLETE_NODE_H
