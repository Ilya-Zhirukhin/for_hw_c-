#include <vector>
#include <algorithm>
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include "catch.hpp"
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

std::vector<char> hw02_test(std::vector<int> ascii) {
    std::vector<char> chars(ascii.size());
    std::transform(ascii.begin(), ascii.end(), chars.begin(), [](int code) {
        return static_cast<char>(code);
    });
    std::sort(chars.begin(), chars.end(), [](char a, char b) {
        return a > b;
    });
    return chars;
}

//int main() {
//    std::vector<int> ascii_codes = {65, 66, 67, 97, 98, 99};
//    std::vector<char> result = hw02_test(ascii_codes);
//    for (char c : result) {
//        std::cout << c << " ";
//    }
//    std::cout << std::endl;
//    return 0;
//}
//

// Tests

TEST_CASE("hw02_test") {
SECTION("empty input") {
std::vector<int> input = {};
std::vector<char> expected = {};
REQUIRE(hw02_test(input) == expected);
}
SECTION("one char") {
std::vector<int> input = {65};
std::vector<char> expected = {'A'};
REQUIRE(hw02_test(input) == expected);
}
SECTION("two chars in descending order") {
std::vector<int> input = {66, 65};
std::vector<char> expected = {'B', 'A'};
REQUIRE(hw02_test(input) == expected);
}
SECTION("three chars in descending order") {
std::vector<int> input = {67, 66, 65};
std::vector<char> expected = {'C', 'B', 'A'};
REQUIRE(hw02_test(input) == expected);
}
SECTION("two equal chars") {
std::vector<int> input = {65, 65};
std::vector<char> expected = {'A', 'A'};
REQUIRE(hw02_test(input) == expected);
}
}
TEST_CASE("hw02", "[test]") {
REQUIRE(hw02_test(std::vector<int>{65, 66, 67}) == std::vector<char>{'C', 'B', 'A'});
}


struct Blob {
    char hello_world[12]; // символьный массив фиксированного размера
    char task2complete[14];
};

void hw02_complex_test() {
    Blob blob;
    std::ifstream file("hw02_data.bin", std::ios::binary | std::ios::in);
    if (file.is_open()) {
        // читаем данные в структуру Blob
        file.read(reinterpret_cast<char*>(&blob), sizeof(blob));
        // выводим данные на консоль

        //std::cout << "hello_world = " << blob.hello_world << std::endl;
        for (const char& item : blob.hello_world) {
            std::cout << item << "";
        }
        std::cout << std::endl;


        //std::cout << "task2complete = " << blob.task2complete << std::endl;
        for (const char& item : blob.task2complete) {
            std::cout << item << "";
        }
        std::cout << std::endl;

    } else {
        std::cerr << "Failed to open file!" << std::endl;
    }
}


int main() {
    hw02_complex_test();
}

//
//В данной работе был предоставлен код программы на C++,
// который считывает данные из бинарного файла и выводит их на консоль.
//
//Структура Blob содержит два символьных массива фиксированного размера:
// hello_world размером 12 символов и task2complete размером 14 символов.
//
//Функция hw02_complex_test() отвечает за чтение данных из файла и вывод на консоль.
// Функция открывает бинарный файл hw02_data.bin и проверяет, был ли файл успешно открыт.
// Затем она читает данные из файла в структуру Blob, используя функцию read(). После успешного чтения данных,
// функция выводит их на консоль.
//
//В функции main() вызывается функция hw02_complex_test() для выполнения задачи.
//
//Программа выводит на консоль текст из двух символьных массивов hello_world и task2complete.

