#ifndef WORDCOMPLETE_AUTOCOMPLETE_H
#define WORDCOMPLETE_AUTOCOMPLETE_H

#include <iostream>
#include <trie/trie.h>

class AutoComplete {
    std::shared_ptr<Trie> trie;
    const int N_GRAM = 3;
    const int LIMIT_CANDIDATES = 5;
public:
    AutoComplete() {
        trie = std::make_shared<Trie>();
    }

    void load_data(const std::string& data_path) {
        VnLangTool::init("", true);
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
            if (counter == 10000) break;
        }
    }

    std::vector<Candidate> get_sub_suggestion(const std::string& sub_string) {
        std::shared_ptr<TrieNode> node = trie->get_root();
        std::vector<uint32_t> codepoints = VnLangTool::to_UTF(sub_string);
        for (uint32_t codepoint : codepoints) {
            node = node->get_child(codepoint);
            if (node == nullptr) return {};
        }
        return node->get_suggestion();
    }

    std::vector<Candidate> merge_candidates(const std::vector<Candidate>& candidates) {
        std::vector<Candidate> merged_candidates;
        std::unordered_set<std::string> duplicated;
        for (const Candidate& candidate : candidates) {
            if (duplicated.count(candidate.get_suffix()) == 0) {
                merged_candidates.push_back(candidate);
                duplicated.insert(candidate.get_suffix());
            }
        }
        return merged_candidates;
    }

    std::vector<Candidate> get_suggestion(const std::string& text) {
        std::vector<Candidate> suggestions;
        std::vector<std::string> tokens = VnLangTool::split_str(text, " ");
        reverse(tokens.begin(), tokens.end());
        std::string sub_text;
        for (int i = 0; i < std::min((int) tokens.size(), N_GRAM); i++) {
            sub_text.insert(0, tokens[i]);
            std::vector<Candidate> sub_suggestion = get_sub_suggestion(sub_text);
            suggestions.insert(suggestions.begin(), sub_suggestion.begin(), sub_suggestion.end());
            sub_text.insert(0, " ");
        }
        return merge_candidates(suggestions);
    }
};

#endif //WORDCOMPLETE_AUTOCOMPLETE_H
