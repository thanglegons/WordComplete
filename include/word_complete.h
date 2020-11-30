#ifndef WORDCOMPLETE_WORD_COMPLETE_H
#define WORDCOMPLETE_WORD_COMPLETE_H

#include <iostream>
#include <trie/trie.h>
#include <trie/trie_loader.h>
#include <huffman/Huffman.h>

class WordComplete {
    const std::string SAVE_LOAD_PATH = "./resources/trie";
    std::shared_ptr<Trie> trie;
    const int N_GRAM = 4;
    static const int LIMIT_CANDIDATES = 5;
    bool is_compressed = false;
    TrieLoader trieLoader;
private:
    static std::vector<std::string> merge_candidates(const std::vector<Candidate>& candidates) {
        std::vector<std::string> merged_candidates;
        std::unordered_set<std::string> duplicated;
        for (const Candidate& candidate : candidates) {
            const std::string& suffix = candidate.get_suffix();
            if (suffix.empty() || suffix == " ") continue;
            if (duplicated.count(suffix) == 0) {
                merged_candidates.push_back(suffix);
                duplicated.insert(suffix);
            }
            if (merged_candidates.size() == LIMIT_CANDIDATES) break;
        }
        return merged_candidates;
    }
public:
    WordComplete() {
        VnLangTool::init("", true);
    }

    void load_raw_data(const std::string& data_path, int limit = -1) {
        if (is_compressed) {
            std::cout << "Can not loaded raw data when compressed";
            return;
        }
        trie = std::make_shared<Trie>();
        std::ifstream input_file;
        input_file.open(data_path);
        std::string line;
        int counter = 0;
        while (getline(input_file, line)) {
            std::string normalized_line = VnLangTool::normalize_lower_NFD_UTF(line);
            std::vector<std::string> ngrams = VnLangTool::generate_ngram(normalized_line, N_GRAM);
            for (const std::string& ngram : ngrams) {
                trie->insert(ngram, 1);
            }
            counter++;
            if (counter % 1000 == 0)
                std::cout << counter << "\n";
            if (limit != -1 && counter == limit) break;
        }
        input_file.close();
    }

    bool load_compressed_data() {
        std::cout << "Start loading compressed data\n";
        trie = trieLoader.load_trie(SAVE_LOAD_PATH);
        if (trie == nullptr) {
            std::cout << "Failed to load compressed data\n";
            return false;
        }
        is_compressed = true;
        std::cout << "Loaded compressed data\n";
        return true;
    }

    void compress_data() {
        if (is_compressed) {
            std::cout << "Already compressed";
            return;
        }
        try {
            this->trie->compress_tree();
            is_compressed = true;
        } catch (...) {
            std::cout << "Compress error!";
        }
    }

    std::vector<std::string> get_suggestion(const std::string& text) {
        std::string normed_text = VnLangTool::normalize_lower_NFD_UTF(text);
        std::vector<Candidate> suggestions;
        std::vector<std::string> tokens = VnLangTool::split_str(normed_text, " ");
        reverse(tokens.begin(), tokens.end());
        std::string sub_text;
        for (int i = 0; i < std::min((int) tokens.size(), N_GRAM); i++) {
            sub_text.insert(0, tokens[i]);
            std::vector<Candidate> sub_suggestion = this->trie->get_suggestion(sub_text);
            suggestions.insert(suggestions.begin(), sub_suggestion.begin(), sub_suggestion.end());
            sub_text.insert(0, " ");
        }
        return merge_candidates(suggestions);
    }

    void save_compressed_data() {
        if (!is_compressed) {
            std::cout << "Please compress before saving";
            return;
        }
        std::cout << "Start saving\n";
        trieLoader.save_trie(SAVE_LOAD_PATH, trie);
    }

    pybind11::list get_suggestion_python(const std::string& text) {
        std::vector<std::string> suggestion = get_suggestion(text);
        pybind11::list plist;
        for (auto & i : suggestion) {
            plist.append(i);
        }
        return plist;
    }
};

#endif //WORDCOMPLETE_WORD_COMPLETE_H
