#include <iostream>
#include "features/arrays.h"
#include "features/lvalue-reference.h"
#include "features/pointers.h"
#include "features/rvalue-reference.h"
#include "features/smart-pointers.h"
#include "features/threads.h"
#include "rusty/option.h"
#include "rusty/vec.h"
#include "test.h"

int main()
{
  features::arrays::run_tests();
  features::lvalueReference::run_tests();
  features::pointers::run_tests();
  features::rvalueReference::run_tests();
  features::smartPointers::run_tests();
  features::threads::run_tests();
  rusty::option::run_tests();
  rusty::vec::run_tests();
  test::run_tests();

  std::cout << "\n All tests passed!\n";
}
