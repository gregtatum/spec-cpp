#include "../test.h"
#include "rvalue-reference.h"
#include <iostream>

namespace features {
namespace rvalueReference {


struct IntHolder {
  int value;
};

struct RefIntHolder {
  int& value;
};

int addOneByRvalue(int&& value) {
  return value + 1;
}


void run_tests() {
  test::suite("features::rvalueReference");

  test::describe("rvalue references");
  {
    int&& value = (16 + 16);
    test::assertEq(value, 32, "rvalue references point to temporary values");
  }

  test::describe("rvalue references can be passed to functions");
  {
    int&& value = addOneByRvalue(16 + 16);
    test::assertEq(value, 33, "The rvalue reference function added one.");
  }
}

} // rvalueReference
} // features
