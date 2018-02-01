#include <iostream>
#include "rusty/vec.h"
#include "rusty/option.h"
#include "rusty/vec.h"
#include "test.h"

int main()
{
  test::run_tests();
  rusty::option::run_tests();
  rusty::vec::run_tests();
}
