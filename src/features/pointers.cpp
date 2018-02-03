#include "../test.h"
#include "pointers.h"
#include <iostream>

namespace features {
namespace pointers {

void addOneByMutation(int* value) {
  *value = *value + 1;
}

int addOneByReturns(const int* value) {
  // Uncommenting this generates a compiler error, as the int* is const.
  // *value = *value + 1;;
  return *value + 1;
}

void run_tests() {
  test::suite("features::pointers");
  test::describe("features::pointers - stack values");
  {
    int value = 32;
    test::assertEq(value, 32, "Declaring a simple value puts it on the stack.");
    // Once the lexical scope goes away, the value will be popped off the stack.
  }

  test::describe("features::pointers - puts a value on the heap");
  {
    // Using the new operator reserves the space on the heap.
    int* value = new int;
    test::assert(*value != 32, "🚧  The value is not initialized, so it could be anything. (possibly intermittent)");
    *value = 32;
    test::assertEq(*value, 32, "Values placed on the heap can be accessed through the * operator.");
    // The value must be deleted!
    delete value;
  }

  test::describe("features::pointers - the value on the stack is the memory address of the heap");
  {
    int* value = new int(0);
    test::assert(value != (int*) 0, "The value itself is a memory address, so it shouldn't equal our value.");

    // De-allocate the memory from the heap:
    delete value;

    // Make a copy of the memory address.
    int* deletedValue = value;
    test::assertEq(value, deletedValue, "The memory address is still in lexical scope and can be re-assigned, but it points at possibly anything in memory. Accesing it is UNDEFINED BEHAVIOR!");

    // This deletedValue is unused, but this line stops the compiler from complaining:
    (void)deletedValue;
  }

  test::describe("features::pointers - multiple pointers point to the same place in memory");
  {
    int* a = new int(32);
    int* b = a;
    int c = *a;
    test::assertEq(*a, *b, "The values are in the heap are the same");
    test::assertEq(*a, c, "The value on the stack is initially the same as what's on the heap");

    *a = 35;
    test::assertEq(*a, 35, "The value on the heap was updated");
    test::assertEq(*b, 35, "The other pointer points to the same value");
    test::assertEq(c, 32, "The value on the stack is a separate value, and is not updated");

    // Only one pointer needs to be de-allocated.
    delete a;
    // Running `delete b` would throw an error with malloc:
    // "malloc: *** error for object 0x7f91e7c02980: pointer being freed was not allocated"

    // The memory address stored in `a` and `b` get popped off the stack, as well as the
    // int value at `c`. The int on the heap was de-allocated with the delete command,
    // so we are all clean!
  }

  test::describe("features::pointers - references");
  {
    int a = 32;
    int* a_ref = &a;
    test::assertEq(a, 32, "The value is on the stack");
    test::assertEq(*a_ref, 32, "The reference points to the value on the stack.");

    a = 50;
    test::assertEq(a, 50, "The value on the stack was updated");
    test::assertEq(*a_ref, 50, "This is reflected in our own pointer.");
  }

  test::describe("features::pointers - passing around pointers");
  {
    int* value = new int(32);

    test::assertEq(*value, 32, "Starts as 32");
    addOneByMutation(value);
    test::assertEq(*value, 33, "Is mutated and added by one");

    int value2 = addOneByReturns(value);
    test::assertEq(value2, 34, "The new value is 34");
    test::assertEq(*value, 33, "But the original can't be changed");

    delete value;
  }
}

} // pointers
} // features
