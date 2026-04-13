#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <fstream>
#include <set>
#include <cassert>

enum class ALPHABET {
    A,
    C,
    G,
    T
};

constexpr int ALPHABET_SIZE = 4;


size_t hash(const std::vector<ALPHABET> &pattern, size_t eval_num, size_t prime) {
    size_t hash = 0;
    for (const ALPHABET &num:pattern) {
        size_t num_val = static_cast<size_t>(num);
        hash *= eval_num;
        hash += num_val;
        hash %= prime;
    }
    return hash;
}

bool is_match(const std::vector<ALPHABET> &text, const std::vector<ALPHABET> &pattern, size_t i) {
    for (size_t j = 0; j < pattern.size(); j++) {
        if (text[i+j] != pattern[j]) {
            return false;
        }
    }
    return true;
}

size_t num_matches(const std::vector<ALPHABET> &text, const std::vector<ALPHABET> &pattern) {
    size_t num_matches = 0;
    for (size_t i = 0; i < text.size() - pattern.size() + 1; i++) {
        if (is_match(text, pattern, i)) {
            num_matches++;
        }
    }

    return num_matches;
}

size_t karp_rabin(const std::vector<ALPHABET> &text, const std::vector<ALPHABET> &pattern,  bool use_naive, bool use_check, bool use_logn) {
    if (use_naive) {
        return num_matches(text, pattern);
    }
    size_t num_iterations = 1;
    size_t prime = 1000000007;
    if (use_logn) {
        num_iterations = std::log2(text.size()) + 1;
    }
    std::set<size_t> global_matches;
   
    for (size_t i = 0; i < num_iterations; i++) {
        size_t eval_num = 100 + i;
        std::set<size_t> matches;
    size_t pattern_hash = hash(pattern, eval_num, prime);
    std::vector<size_t> text_hashes = std::vector<size_t>(text.size() - pattern.size() + 1);
    std::vector<ALPHABET> text_subset = std::vector<ALPHABET>(pattern.size());
    for (size_t i = 0; i < pattern.size(); i++) {
        text_subset[i] = text[i];
    }
    text_hashes[0] = hash(text_subset, eval_num, prime);
    if (text_hashes[0] == pattern_hash) {
        matches.insert(0);
    }
    size_t eval_num_pow = static_cast<size_t>(std::pow(eval_num, pattern.size() - 1)) % prime;
    for (size_t i = 1; i < text.size() - pattern.size() + 1; i++) {
        text_hashes[i] = (text_hashes[i - 1] - (static_cast<size_t>(text[i - 1]) * eval_num_pow) % prime) * eval_num + static_cast<size_t>(text[i + pattern.size() - 1]);
        text_hashes[i] %= prime;
        for (size_t j = 0; j < pattern.size(); j++) {
            text_subset[j] = text[i + j];
        }
        if (text_hashes[i] == pattern_hash) {
            matches.insert(i);
        }
    }
    if (i == 0) {
        global_matches = matches;
    } else {
        std::set<size_t> intersection;
        std::set_intersection(global_matches.begin(), global_matches.end(), matches.begin(), matches.end(),std::inserter(intersection, intersection.begin()));
        global_matches = intersection;
    }
}
    return global_matches.size();
}



int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> base_dis(0, ALPHABET_SIZE - 1);


    static constexpr char kLetters[] = "ACGT";
    std::vector<ALPHABET> pattern = {ALPHABET::A};
    
    std::ofstream file("pattern_matching_running_times.csv");
    std::vector<std::string> catergories = {"naive", "no-checks","checks","logn","logn + checks"};
    std::vector<bool> use_naive = {true, false, false, false, false};
    std::vector<bool> use_checks = {false, false, true, false, true};
    std::vector<bool> use_logn = {false, false, false, true, true};
     
        for (size_t text_length = 10; text_length <= 10000000; text_length *= 2) {
            
            size_t pattern_length = log2(text_length)/4;
            std::vector<ALPHABET> text;
            for (size_t i = 0; i < text_length; i++) {
                text.push_back(static_cast<ALPHABET>(base_dis(gen)));
            }

            std::vector<ALPHABET> pattern;
            for (size_t i = 0; i < pattern_length; i++) {
                pattern.push_back(static_cast<ALPHABET>(base_dis(gen)));
            }
            // print out text and pattern
            std::cout << "Text: ";
            for (const ALPHABET &num : text) {
                std::cout << kLetters[static_cast<size_t>(num)] << ' ';
            }
            std::cout << "Pattern: ";
            for (const ALPHABET &num : pattern) {
                std::cout << kLetters[static_cast<size_t>(num)] << ' ';}
            for (size_t i = 0; i < catergories.size(); i++) {
                auto start = std::chrono::steady_clock::now();
                size_t matches = karp_rabin(text, pattern, use_naive[i], use_checks[i], use_logn[i]);
                std::cout << catergories[i] << " " << "Matches: " << matches << std::endl;
                auto end = std::chrono::steady_clock::now();
                double seconds = std::chrono::duration<double>(end - start).count();
                file << text_length << "," << seconds << "," << matches << std::endl;
        }
    }

    return 0;
}