#include <iostream>
#include "vec.h"
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

  describe("RustLikeVec::Vec");
  {
    auto vec = RustLikeVec::Vec(0);
    assertEq(vec.capacity(), 0, "The vector starts with 0 capacity");
  }
}
