#include <iostream>
#include "features/arrays.h"
#include "features/lvalue-reference.h"
#include "features/pointers.h"
#include "features/rvalue-reference.h"
#include "features/smart-pointers.h"
#include "features/threads.h"
#include "memory/stack.h"
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

  // These should not stop execution of the rest of the tests, as they may rely
  // upon undefined behavior, or break with compiler optimizations.
  try { memory::stack::run_tests(); } catch (std::string e) {}

  std::cout << "\n All tests passed!\n";
}
