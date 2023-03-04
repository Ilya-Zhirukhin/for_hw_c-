#include <iostream>
#include <vector>
#include <algorithm> // for copy_if and sort
#include <numeric>   // for for_each
//#include <catch2/catch_test_macros.hpp>


//TEST_CASE("hw01", "[test]")
//{
//REQUIRE(hw01_test(std::vector < int > {1, 2, 3, 4}) == std::vector < int > {15, 9, 5});
//REQUIRE(hw01_test(std::vector < int > {3, 5, 7, 11, 13}) == std::vector < int > {15, 26, 22, 10, 6});
//REQUIRE(hw01_test(std::vector < int > {10, 11, 12, 13, 14, 15}) == std::vector < int > {30, 26, 22, 10, 6});
//}
std::vector<int> hw01_test(std::vector<int> nums) {



    // 1. Filter out even numbers
    auto odds = std::vector<int>(nums.size());
    auto last_odd = std::copy_if(nums.begin(), nums.end(), odds.begin(), [](int n) { return n % 2 != 0; });
    odds.resize(std::distance(odds.begin(), last_odd));
    // 2. Filter out numbers divisible by 3 or 11
    auto filtered = std::vector<int>(nums.size());
    auto last_filt = std::copy_if(odds.begin(), odds.end(), filtered.begin(), [](int n) { return n % 3 != 0 && n % 11 != 0; });
    filtered.resize(std::distance(filtered.begin(), last_filt));
    // 3. Add 5 to the beginning of the vector
    filtered.insert(filtered.begin(), 5);

    // 4. Sort in descending order
    std::sort(filtered.begin(), filtered.end(), std::greater<>());

    // 5. Double numbers in the range [5, 15]
    std::for_each(filtered.begin(), filtered.end(), [](int& n) {
        if (n >= 5 && n <= 15) {
            n *= 2;
        }
    });

    return filtered;
}

int main(){
    auto res = hw01_test(std::vector<int>{1, 2, 3, 4, 5, 6, 7});
    std::for_each(res.begin(), res.end(), [](int x){std::cout<< x << ' ';});
    return 0;
}

//TEST_CASE("hw01", "[test]")
//{
//REQUIRE(hw01_test(std::vector < int > {1, 2, 3, 4}) == std::vector < int > {15, 9, 5});
//REQUIRE(hw01_test(std::vector < int > {3, 5, 7, 11, 13}) == std::vector < int > {15, 26, 22, 10, 6});
//REQUIRE(hw01_test(std::vector < int > {10, 11, 12, 13, 14, 15}) == std::vector < int > {30, 26, 22, 10, 6});
//}
