#include <iostream>
#include <fstream>
#include <trie/node.h>
#include <trie/trie.h>
#include <autocomplete.h>
#include <queue>

using namespace std;

int main() {

    AutoComplete autoComplete;
    autoComplete.load_data("./resources/VNESEcorpus.txt");
    std::vector<Candidate> suggestion = autoComplete.get_suggestion("của các nhà hảo tâm gửi giúp trước sự chứng kiến c");
    for (const Candidate& sug : suggestion) {
        cout << sug.get_suffix() << " " << sug.get_count() << "\n";
    }

//    vector<string> ngrams = VnLangTool::generate_ngram("hom nay toi di hoc nhu lon", 2);
//    for (string ngram : ngrams) {
//        cout << ngram << "\n";
//    }
//    ifstream input_file;
//    input_file.open("./resources/VNESEcorpus.txt");
//    std::string line;
//    int counter = 0;
//    VnLangTool::init("", true);
//    while (getline(input_file, line)) {
//        std::cout << VnLangTool::normalize_lower_NFD_UTF(line) << "\n";
//        counter++;
//        if (counter == 10) break;
//    }
//    priority_queue<Candidate> candidates;
//    candidates.push(Candidate("abc", 125));
//    candidates.push(Candidate("abcd", 130));
//    while (!candidates.empty()) {
//        Candidate top = candidates.top();
//        candidates.pop();
//        cout << top.get_suffix() << " " << top.get_count() << "\n";
//    }
//    Trie trie;
//    trie.insert("phí phần mềm", 5);
//    trie.insert("phí phần mềm", 10);
//    TrieNode* root = trie.get_root();
//    std::vector<Candidate> suggestion = root->get_suggestion();
//    for (const Candidate& sug : suggestion) {
//        cout << sug.get_suffix() << " " << sug.get_count() << "\n";
//    }
}
