#include <iostream>
#include "features/pointers.h"
#include "features/lvalue-reference.h"
#include "features/rvalue-reference.h"
#include "rusty/vec.h"
#include "rusty/option.h"
#include "rusty/vec.h"
#include "test.h"

int main()
{
  test::run_tests();
  features::pointers::run_tests();
  features::lvalueReference::run_tests();
  features::rvalueReference::run_tests();
  rusty::option::run_tests();
  rusty::vec::run_tests();

  std::cout << "\n All tests passed!\n";
}
