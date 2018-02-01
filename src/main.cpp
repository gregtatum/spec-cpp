#include <iostream>
#include "rusty/vec.h"
#include "rusty/option.h"

// https://en.wikipedia.org/wiki/ANSI_escape_code
static const std::string RED = "\033[31m";
static const std::string GREEN = "\033[32m";
static const std::string WHITE = "\033[27m";
static const std::string BRIGHT_WHITE = "\033[97m";
// static const std::string GREEN = "\033[32m";
// static const std::string GREEN = "\033[32m";
static const std::string RESET = "\033[0m";

void describe(const std::string& message) {
  std::cout << BRIGHT_WHITE << "\n" << message << RESET << "\n";
}

void assert(bool value, const std::string& message) {
  if (!value) {
    std::cout << RED << "  𝘅 " << message << RESET << "\n";
    throw std::string(message);
  } else {
    std::cout << GREEN << "  ✔ " << RESET << WHITE << message << RESET << "\n";
  }
}

template<typename T>
void assertEq(T a, T b, const std::string& message) {
  if (a != b) {
    std::cout << RED << "  𝘅 " << message << RESET << "\n";
    std::cout << "a: " << a << "\n";
    std::cout << "b: " << b << "\n";
    throw std::string(message);
  } else {
    std::cout << GREEN << "  ✔ "<< RESET << WHITE << message << RESET << "\n";
  }
}

int main()
{
  describe("Assertions");
  {
    assert(true, "Does not throw");
    // assert(false, "Uncomment to throw");
    assertEq(true, true, "Equality does work");
    // assertEq(true, false, "Inequality throws");
  }

  describe("rusty::option::Option");
  {
    float pi = 3.14159f;
    auto maybePi = rusty::option::some(&pi);
    assertEq(*maybePi.unwrap(), 3.14159f, "Can unwrap a value");
    assert(maybePi.isSome(), "A some value is Some");
    assert(!maybePi.isNone(), "A some value is not None");

    auto notPi = rusty::option::none();
    try {
      notPi.unwrap();
      assert(false, "Cannot unwrap a None");
    } catch (int e) {
      assert(true, "Cannot unwrap a None");
    }

    assert(!notPi.isSome(), "A none value is not Some");
    assert(notPi.isNone(), "A none value is None");
  }

  describe("rusty::vec::Vec");
  {
    auto vec = rusty::vec::Vec(0);
    assertEq(vec.capacity(), 0, "The vector starts with 0 capacity");
    assertEq(vec.len(), 0, "The initial length is also 0.");
  }

  describe("rusty::vec::Vec::push");
  {
    auto vec = rusty::vec::Vec(0);

    vec.push(1);
    assertEq(vec.capacity(), 1, "The capacity is now 1");
    assertEq(vec.len(), 1, "The length is also 1");
    assertEq(*vec.get(0).unwrap(), 1.0f, "The value added is 1");
    assert(vec.get(1).isNone(), "The second value is none");

    vec.push(2);
    assertEq(vec.capacity(), 2, "The capacity is doubled after a push");
    assertEq(vec.len(), 2, "The length is also doubled after a push");
    assertEq(*vec.get(0).unwrap(), 1.0f, "The value was added");
    assertEq(*vec.get(1).unwrap(), 2.0f, "The value was added");
    assert(vec.get(2).isNone(), "The last value is none");

    vec.push(3);
    assertEq(vec.capacity(), 4, "The capacity is doubled after a push");
    assertEq(vec.len(), 3, "The length is only incremented");
    assertEq(*vec.get(0).unwrap(), 1.0f, "The value was added");
    assertEq(*vec.get(1).unwrap(), 2.0f, "The value was added");
    assertEq(*vec.get(2).unwrap(), 3.0f, "The value was added");
    assert(vec.get(3).isNone(), "The last value is none");
  }
}
