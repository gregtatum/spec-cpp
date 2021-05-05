#pragma once
#include <iostream>
#include <string_view>

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
void info(const std::string &);
void run_tests();
template <typename T> void ignore(T value){};
template <typename T> void ignoreRef(T &value){};
template <typename T> void ignoreRValue(T &&value){};

template <typename A, typename B>
void equal(A a, B b, const std::string &message = std::string{"A and B are equal"}) {
  if constexpr (std::is_same<A, B>::value) {
    if (a != b) {
      std::cout << RED << "    𝘅 " << message << RESET << "\n\n";
      std::cout << GREEN << "expected: " << b << RESET << "\n";
      std::cout << RED << "received: " << a << RESET << "\n\n";
      throw std::string(message);
    } else {
      std::cout << GREEN << "    ✔ " << RESET << WHITE << message << RESET << "\n";
    }
  } else {
    if constexpr (std::is_same<A, std::string_view>::value &&
                  (std::is_same<B, const char *>::value)) {
      std::string_view bStringView{b};
      if (a != bStringView) {
        std::cout << RED << "    𝘅 " << message << RESET << "\n\n";
        std::cout << GREEN << "expected: " << b << RESET << "\n";
        std::cout << RED << "received: " << a << RESET << "\n\n";
        throw std::string(message);
      } else {
        std::cout << GREEN << "    ✔ " << RESET << WHITE << message << RESET << "\n";
      }
    } else {
      static_assert(std::is_same<A, B>::value, "The two types are not equal");
    }
  }
}

// This macro shows off something that will not compile
#define COMPILER_ERROR(...)

} // namespace test
