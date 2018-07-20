#include "../test.h"
#include "stack.h"

namespace memory {
namespace stack {

template<typename T>
unsigned long getAddress(T* value) {
  return (unsigned long) value;
}

void run_tests() {
  test::suite("memory::stack", []() {
    test::describe("Stack values are sequentially created backwards on the stack", []() {
      int a = 11;
      int b = 22;
      int c = 33;
      int d = 44;
      int e = 55;

      // An int is 4 bytes, so the raw memory address is offset by 4 each time.
      test::equal(
        long(getAddress(&b) - getAddress(&a)),
        long(4 * -1),
        "The diff of the addresses match"
      );
      test::equal(
        long(getAddress(&c) - getAddress(&a)),
        long(4 * -2),
        "The diff of the addresses match"
      );
      test::equal(
        long(getAddress(&d) - getAddress(&a)),
        long(4 * -3),
        "The diff of the addresses match"
      );
      test::equal(
        long(getAddress(&e) - getAddress(&a)),
        long(4 * -4),
        "The diff of the addresses match"
      );
    });

    test::describe("Stack values can be accessed using pointer arithmetic", []() {
      const int a = 11;
      const int b = 22; (void) b;
      const int c = 33; (void) c;
      const int d = 44; (void) d;
      const int e = 55; (void) e;

      // Adding numbers to a pointer type of `int*` moves the memory address
      // in increments of 4 bytes.
      test::equal(*(&a - 0), 11, "Can access the values via pointer arithmetic");
      test::equal(*(&a - 1), 22, "Can access the values via pointer arithmetic");
      test::equal(*(&a - 2), 33, "Can access the values via pointer arithmetic");
      test::equal(*(&a - 3), 44, "Can access the values via pointer arithmetic");
      test::equal(*(&a - 4), 55, "Can access the values via pointer arithmetic");
    });

    test::describe("Stack values can be accessed using raw memory addresses", []() {
      const int a = 11;
      const int b = 22; (void) b;
      const int c = 33; (void) c;
      const int d = 44; (void) d;
      const int e = 55; (void) e;

      unsigned long address = (unsigned long) &a;

      // Memory addresses can be manually offset with some math. Each offset is by 4
      // bytes.
      test::equal(*(int*) (address - 4 * 0), 11, "Can access the values by raw memory access");
      test::equal(*(int*) (address - 4 * 1), 22, "Can access the values by raw memory access");
      test::equal(*(int*) (address - 4 * 2), 33, "Can access the values by raw memory access");
      test::equal(*(int*) (address - 4 * 3), 44, "Can access the values by raw memory access");
      test::equal(*(int*) (address - 4 * 4), 55, "Can access the values by raw memory access");
    });

    test::describe("Stack values can be CHANGED using raw memory addresses, even on const values", []() {
      const int a = 11;
      const int b = 22; (void) b;
      const int c = 33; (void) c;

      const int* pA = &a;
      const int* pB = &b;
      const int* pC = &c;

      unsigned long address = (unsigned long) &a;

      // Heyo isn't this fuuuuun.
      *(int*) (address - 4 * 0) = 12;
      *(int*) (address - 4 * 1) = 23;
      *(int*) (address - 4 * 2) = 34;

      // Use pointers to prevent the compiler from optimizing away const values.
      test::equal(*pA, 12, "Value on the stack was changed");
      test::equal(*pB, 23, "Value on the stack was changed");
      test::equal(*pC, 34, "Value on the stack was changed");
    });

    test::describe("Arrays are stack allocated", []() {
      int a[3] { 11, 22, 33 };
      int b[3] { 44, 55, 66 }; (void) b;
      int c[3] { 77, 88, 99 }; (void) c;

      unsigned long address = (unsigned long) &a;

      // Note: This test breaks without the for loop, so something weird is going on.
      // My guess is the compiler is performing optimizations.

      for (int i = 3 * 3; i >= 0; i--) {
        // int offset = - 4 * i + 4 * 1;
        // printf(
        //   "Offset: %d bytes, Value: %d\n",
        //   offset,
        //   *(int*) (((unsigned long) &a) + offset)
        // );
      }

      // a:
      test::equal(*(int*) (address + -24), 77, "Value at address matches array value");
      test::equal(*(int*) (address + -20), 88, "Value at address matches array value");
      test::equal(*(int*) (address + -16), 99, "Value at address matches array value");
      // b:
      test::equal(*(int*) (address + -12), 44, "Value at address matches array value");
      test::equal(*(int*) (address + -8), 55, "Value at address matches array value");
      test::equal(*(int*) (address + -4), 66, "Value at address matches array value");
      // c:
      test::equal(*(int*) (address + 0), 11, "Value at address matches array value");
      test::equal(*(int*) (address + 4), 22, "Value at address matches array value");
      test::equal(*(int*) (address + 8), 33, "Value at address matches array value");


      test::equal((int(*)[3]) (address -  0), &a, "Array 'a' matches when looking it up");
      test::equal((int(*)[3]) (address - 12), &b, "Array 'b' matches when looking it up");
      test::equal((int(*)[3]) (address - 24), &c, "Array 'c' matches when looking it up");
    });

  });
}

} // memory
} // memory
