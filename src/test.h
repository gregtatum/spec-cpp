#pragma once
#include <iostream>

namespace test {

// https://en.wikipedia.org/wiki/ANSI_escape_code
static const std::string RED = "\033[31m";
static const std::string GREEN = "\033[32m";
static const std::string WHITE = "\033[27m";
static const std::string BRIGHT_WHITE = "\033[97m";
static const std::string RESET = "\033[0m";

void suite(const std::string&, std::function<void ()>);
void describe(const std::string&, std::function<void ()>);
void assert(bool, const std::string&);
void run_tests();

template<typename T>
void assertEq(T a, T b, const std::string& message) {
  if (a != b) {
    std::cout << RED << "    𝘅 " << message << RESET << "\n\n";
    std::cout << GREEN << "expected: " << b << RESET << "\n";
    std::cout << RED << "received: " << a << RESET << "\n\n";
    throw std::string(message);
  } else {
    std::cout << GREEN << "    ✔ "<< RESET << WHITE << message << RESET << "\n";
  }
}

} // test
