#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <vector>

constexpr int ALPHABET_SIZE = 2;

enum class ALPHABET { A };

size_t hash(const std::vector<ALPHABET> &pattern, size_t eval_num, size_t prime) {
    size_t hash = 0;
    for (const ALPHABET &num : pattern) {
        size_t num_val = static_cast<size_t>(num);
        hash *= eval_num;
        hash += num_val;
        hash %= prime;
    }
    return hash;
}

bool is_match(const std::vector<ALPHABET> &text, const std::vector<ALPHABET> &pattern, size_t i) {
    for (size_t j = 0; j < pattern.size(); j++) {
        if (text[i + j] != pattern[j]) {
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

void safe_subtract(int64_t &a, int64_t &b, int64_t &prime) {
    assert(b >= 0);
    assert(a >= 0);
    assert(prime > 0);
    a = ((a % prime) - (b % prime) + prime) % prime;
}

void safe_add(int64_t &a, int64_t &b, int64_t &prime) {
    assert(b >= 0);
    assert(a >= 0);
    assert(prime > 0);
    a = ((a % prime) + (b % prime)) % prime;
}

void safe_multiply(int64_t &a, int64_t b, int64_t &prime) {
    assert(b >= 0);
    assert(a >= 0);
    assert(prime > 0);
    a = ((a % prime) * (b % prime)) % prime;
}

void safe_power(int64_t &num, int64_t pow, int64_t &prime) {
    assert(num >= 0);
    assert(pow >= 0);
    assert(prime > 0);
    int64_t result = 1;
    for (int64_t i = 0; i < pow; i++) {
        safe_multiply(result, num, prime);
    }
    num = result;
}

size_t karp_rabin(const std::vector<ALPHABET> &text, const std::vector<ALPHABET> &pattern,
                  bool use_naive, bool use_check, bool use_logn) {
    if (use_naive) {
        return num_matches(text, pattern);
    }
    size_t num_iterations = 1;
    int64_t prime = 1000000007;
    if (use_logn) {
        num_iterations = std::log2(text.size()) + 1;
    }
    std::set<size_t> global_matches;

    for (size_t i = 0; i < num_iterations; i++) {
        static constexpr char kLetters[] = "ACGT";
        size_t eval_num = 100 + i;
        std::set<size_t> matches;
        size_t pattern_hash = hash(pattern, eval_num, prime);
        std::vector<int64_t> text_hashes = std::vector<int64_t>(text.size() - pattern.size() + 1);
        std::vector<ALPHABET> text_subset = std::vector<ALPHABET>(pattern.size());
        for (size_t i = 0; i < pattern.size(); i++) {
            text_subset[i] = text[i];
        }
        text_hashes[0] = hash(text_subset, eval_num, prime);
        if (text_hashes[0] == pattern_hash) {
            matches.insert(0);
        }

        int64_t eval_num_pow = eval_num;
        safe_power(eval_num_pow, pattern.size() - 1, prime);
        for (size_t j = 1; j + pattern.size() <= text.size(); j++) {
            int64_t to_subtract = eval_num_pow;
            int64_t to_multiply = static_cast<int64_t>(eval_num);
            safe_multiply(to_subtract, static_cast<int64_t>(text[j - 1]), prime);
            int64_t to_add = static_cast<int64_t>(text[j + pattern.size() - 1]);
            int64_t prev_hash = text_hashes[j - 1];
            safe_subtract(prev_hash, to_subtract, prime);
            safe_multiply(prev_hash, to_multiply, prime);
            safe_add(prev_hash, to_add, prime);
            text_hashes[j] = prev_hash;
            // for (size_t k = 0; k < pattern.size(); k++) {
            //     text_subset[k] = text[k + j];
            // }
            // std::cout << "text subset";
            // for (auto elem : text_subset) {
            //     std::cout << kLetters[static_cast<size_t>(elem)] << " ";
            // }
            // std::cout << std::endl;
            // if (hash(text_subset, eval_num, prime) != text_hashes[j]) {
            //     std::cout << "Expected hash: " << hash(text_subset, eval_num, prime) <<
            //     std::endl; std::cout << "Actual hash: " << text_hashes[j] << std::endl;
            // }
            // assert(hash(text_subset, eval_num, prime) == text_hashes[j]);
            if (text_hashes[j] == pattern_hash) {
                matches.insert(j);
            }
        }
        if (i == 0) {
            global_matches = matches;
        } else {
            std::set<size_t> intersection;
            std::set_intersection(global_matches.begin(), global_matches.end(), matches.begin(),
                                  matches.end(), std::inserter(intersection, intersection.begin()));
            global_matches = intersection;
        }
    }

    if (use_check) {
        std::set<size_t> incorrect_matches;
        for (size_t match : global_matches) {
            if (!is_match(text, pattern, match)) {
                incorrect_matches.insert(match);
            }
        }
        for (size_t match : incorrect_matches) {
            global_matches.erase(match);
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
    file << "category,text_length,pattern_length,seconds,matches, misses" << std::endl;
    std::vector<std::string> categories = {"naive", "no-checks", "checks", "logn", "logn + checks"};
    std::vector<bool> use_naive = {true, false, false, false, false};
    std::vector<bool> use_checks = {false, false, true, false, true};
    std::vector<bool> use_logn = {false, false, false, true, true};

    for (size_t text_length = 200; text_length <= 10000; text_length *= 1.2) {

        for (size_t pattern_length = 1; pattern_length <= 200; pattern_length *= 14) {
            if (pattern_length < 1) {
                pattern_length = 1;
            }
            std::vector<ALPHABET> text;
            for (size_t i = 0; i < text_length; i++) {
                text.push_back(static_cast<ALPHABET>(base_dis(gen)));
            }

            std::vector<ALPHABET> pattern;
            for (size_t i = 0; i < pattern_length; i++) {
                pattern.push_back(static_cast<ALPHABET>(base_dis(gen)));
            }
            // print out text and pattern
            // std::cout << "Text: ";
            // for (const ALPHABET &num : text) {
            //     std::cout << kLetters[static_cast<size_t>(num)] << ' ';
            // }
            // std::cout << "Pattern: ";
            // for (const ALPHABET &num : pattern) {
            //     std::cout << kLetters[static_cast<size_t>(num)] << ' ';
            // }
            int total_matches = 0;
            for (size_t i = 0; i < categories.size(); i++) {
                auto start = std::chrono::steady_clock::now();
                int matches = karp_rabin(text, pattern, use_naive[i], use_checks[i], use_logn[i]);
                if (i == 0) {
                    total_matches = matches;
                } else {
                    if (matches != total_matches) {
                        std::cout << "Error: matches != total_matches for category "
                                  << categories[i] << std::endl;
                        std::cout << "Expected: " << total_matches << " but got: " << matches
                                  << std::endl;
                    }
                }
                // std::cout << categories[i] << " " << "Matches: " << matches << std::endl;
                auto end = std::chrono::steady_clock::now();
                double seconds = std::chrono::duration<double>(end - start).count();
                file << categories[i] << "," << text_length << "," << pattern_length << ","
                     << seconds << "," << matches << "," << matches - total_matches << std::endl;
            }
        }
    }

    // things to change:
    // - alphabet size
    // - log log vs linear linear
    // - all one letter to show badness of naive

    return 0;
}