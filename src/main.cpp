#include <iostream>
#include "rust-like/vec.h"
#include "rust-like/option.h"
using namespace std;

void describe(const string& message) {
  std::cout << "\n" << message << "\n";
}

void assert(bool value, const string& message) {
  if (!value) {
    std::cout << "  𝘅 " << message << "\n";
    throw string(message);
  } else {
    std::cout << "  ✔ " << message << "\n";
  }
}

template<typename T>
void assertEq(T a, T b, const string& message) {
  if (a != b) {
    std::cout << "  𝘅 " << message << "\n";
    std::cout << "a: " << a << "\n";
    std::cout << "b: " << b << "\n";
    throw string(message);
  } else {
    std::cout << "  ✔ " << message << "\n";
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

  describe("RustLike::Vec");
  {
    auto vec = RustLike::Vec(0);
    assertEq(vec.capacity(), 0, "The vector starts with 0 capacity");
    assertEq(vec.len(), 0, "The initial length is also 0.");
  }

  describe("RustLike::Vec push");
  {
    auto vec = RustLike::Vec(0);

    vec.push(1);
    assertEq(vec.capacity(), 1, "The capacity is now 1");
    assertEq(vec.len(), 1, "The length is also 1");

    vec.push(2);
    assertEq(vec.capacity(), 2, "The capacity is doubled after a push");
    assertEq(vec.len(), 2, "The length is also doubled after a push");

    vec.push(3);
    assertEq(vec.capacity(), 4, "The capacity is doubled after a push");
    assertEq(vec.len(), 3, "The length is only incremented");
  }

  describe("RustLike::Option");
  {
    float pi = 3.14159f;
    auto maybePi = RustLike::some(&pi);
    assertEq(*maybePi.unwrap(), 3.14159f, "Can unwrap a value");
  }
}
