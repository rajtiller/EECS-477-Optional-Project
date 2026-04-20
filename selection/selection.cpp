#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

// things to expirement with: better mediant, batch size, input type (size and range),
// finding median vs lowest vs random, does mediant of medians work for finding 20th percentile?
// varying batch size as we get lower

int median_of_medians(std::vector<int> &nums, size_t batch_size) {
    std::vector<int> medians;
    if (nums.size() < batch_size) {
        std::sort(nums.begin(), nums.end());
        return nums[nums.size() / 2];
    }
    for (size_t i = 0; i < nums.size(); i += batch_size) {
        std::vector<int> batch(nums.begin() + i,
                               nums.begin() + std::min(i + batch_size, nums.size()));
        std::sort(batch.begin(), batch.end());
        medians.push_back(batch[batch.size() / 2]);
    }
    return median_of_medians(medians, batch_size);
}

std::pair<std::vector<int>, std::vector<int>> partition_set(std::vector<int> &nums, int x) {
    // returns left and right set
    std::pair<std::vector<int>, std::vector<int>> partition;
    for (size_t i = 0; i < nums.size(); i++) {
        if (nums[i] < x) {
            partition.first.push_back(nums[i]);
        } else if (nums[i] > x) {
            partition.second.push_back(nums[i]);
        }
    }
    return partition;
}

int find_nth_element(std::vector<int> nums, size_t n, bool use_better_median, size_t batch_size) {
    int median_estimate = use_better_median ? median_of_medians(nums, batch_size) : nums[0];
    auto partition = partition_set(nums, median_estimate);
    int mediant_estimate_count = nums.size() - partition.first.size() - partition.second.size();
    assert(mediant_estimate_count >= 1);
    if (n < partition.first.size()) {
        return find_nth_element(partition.first, n, use_better_median, batch_size);
    } else if (n < partition.first.size() + mediant_estimate_count) {
        return median_estimate;
    } else {
        return find_nth_element(
            partition.second, n - partition.first.size() - mediant_estimate_count,
            use_better_median, batch_size); // -1 because we already counted the median_estimate
    }
}

double find_median_good(std::vector<int> &nums, bool use_sorting, bool use_better_median,
                        size_t batch_size) {
    if (use_sorting) {
        std::sort(nums.begin(), nums.end());
        if (nums.size() % 2 == 1) {
            return static_cast<double>(nums[nums.size() / 2]);
        }
        return (static_cast<double>(nums[nums.size() / 2 - 1]) +
                static_cast<double>(nums[nums.size() / 2])) /
               2.0;
    }
    if (nums.size() % 2 == 1) {
        return static_cast<double>(find_nth_element(
            nums, static_cast<size_t>((nums.size() - 1) / 2), use_better_median, batch_size));
    }
    double lo = static_cast<double>(
        find_nth_element(nums, (nums.size() - 2) / 2, use_better_median, batch_size));
    double hi =
        static_cast<double>(find_nth_element(nums, nums.size() / 2, use_better_median, batch_size));
    return (lo + hi) / 2.0;
}

int main() {
    // if (argc <= 2) {
    //     std::cerr << "Usage: " << argv[0] << " <size><median>" << std::endl;
    //     return 1;
    // }
    std::vector<int> nums = {};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0,
                                        10000); // so there are basically zero duplicates
    // +1 above so there is one median
    std::ofstream file("selection_running_times.csv");
    file << "category,size,seconds" << std::endl;
    std::vector<std::string> catergories = {"sorting",
                                            "quickselect",
                                            "quickselect + better median (5)",
                                            "quickselect + better median (7)",
                                            "quickselect + better median (9)",
                                            "quickselect + better median (101)"};
    std::vector<bool> use_sorting = {true, false, false, false, false, false};
    std::vector<bool> use_better_median = {false, false, true, true, true, true};
    std::vector<size_t> batch_size = {0, 0, 5, 7, 9, 101};
    for (size_t i = 0; i < catergories.size(); i++) {
        for (size_t SIZE = 10; SIZE <= 10000000; SIZE *= 2) {
            nums.clear();
            for (size_t j = 0; j < SIZE; j++) {
                nums.push_back(dis(gen));
            }
            auto start = std::chrono::steady_clock::now();
            double guessed_median =
                find_median_good(nums, use_sorting[i], use_better_median[i], batch_size[i]);
            auto end = std::chrono::steady_clock::now();
            double seconds = std::chrono::duration<double>(end - start).count();
            (void)guessed_median;
            file << catergories[i] << "," << SIZE << "," << seconds << std::endl;
        }
    }
    return 0;
}