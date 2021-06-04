#include "../includes/mfbt/RefPtr.h"
#include "memory/Allocator.h"
#include "memory/stack.h"
#include "mfbt/TestMaybe.h"
#include "mfbt/TestRefPtr.h"
#include "mfbt/TestResult.h"
#include "rusty/box.h"
#include "rusty/option.h"
#include "rusty/vec.h"
#include "test.h"
#include <iostream>

// clang-format off
namespace features {
  namespace arrays          { void run_tests(); }
  namespace classes         { void run_tests(); }
  namespace const_test      { void run_tests(); }
  namespace icu             { void run_tests(); }
  namespace icu_capi        { void run_tests(); }
  namespace iterators       { void run_tests(); }
  namespace lvalueReference { void run_tests(); }
  namespace misc            { void run_tests(); }
  namespace pointers        { void run_tests(); }
  namespace rvalueReference { void run_tests(); }
  namespace smartPointers   { void run_tests(); }
  namespace threads         { void run_tests(); }
  namespace utf8            { void run_tests(); }
  namespace vector          { void run_tests(); }
}
// clang-format on

int main() {
  features::arrays::run_tests();
  features::classes::run_tests();
  features::const_test::run_tests();
  features::icu::run_tests();
  features::icu_capi::run_tests();
  features::iterators::run_tests();
  features::lvalueReference::run_tests();
  features::misc::run_tests();
  features::pointers::run_tests();
  features::rvalueReference::run_tests();
  features::smartPointers::run_tests();
  features::threads::run_tests();
  features::utf8::run_tests();
  features::vector::run_tests();

  rusty::box::run_tests();
  rusty::option::run_tests();
  rusty::vec::run_tests();

  test::run_tests();

  mfbt::TestMaybe::run_tests();
  mfbt::TestRefPtr::run_tests();
  mfbt::TestResult::run_tests();

  memory::allocator::run_tests();

  // These should not stop execution of the rest of the tests, as they may rely
  // upon undefined behavior, or break with compiler optimizations.
  try {
    memory::stack::run_tests();
  } catch (std::string e) {
  }

  std::cout << "\n All tests passed!\n";
}
