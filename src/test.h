#pragma once
#include <iostream>

namespace test {

// https://en.wikipedia.org/wiki/ANSI_escape_code
static const std::string RED = "\033[31m";
static const std::string GREEN = "\033[32m";
static const std::string WHITE = "\033[27m";
static const std::string BRIGHT_WHITE = "\033[97m";
static const std::string RESET = "\033[0m";

void suite(const std::string &, std::function<void()>);
void describe(const std::string &, std::function<void()>);
void ok(bool, const std::string &);
void run_tests();
template <typename T> void ignore(T value){};
template <typename T> void ignoreRef(T &value){};
template <typename T> void ignoreRValue(T &&value){};

template <typename T> void equal(T a, T b, const std::string &message) {
  if (a != b) {
    std::cout << RED << "    𝘅 " << message << RESET << "\n\n";
    std::cout << GREEN << "expected: " << b << RESET << "\n";
    std::cout << RED << "received: " << a << RESET << "\n\n";
    throw std::string(message);
  } else {
    std::cout << GREEN << "    ✔ " << RESET << WHITE << message << RESET
              << "\n";
  }
}

// This macro shows off something that will not compile
#define COMPILER_ERROR(...)

} // namespace test
