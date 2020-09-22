#include "arrays.h"
#include "../test.h"
#include <iostream>

namespace features {
namespace arrays {

bool isInt(int &value) { return true; }

bool isArray(int value[]) { return true; }

int &get2ndValue(int (&value)[3]) { return value[1]; }

void addOneToAllMembers(int array[], uint count) {
  for (uint i = 0; i < count; i++) {
    array[i] = array[i] + 1;
  }
}

void addOneToNumber(int *number) { *number = *number + 1; }

int *getArrayMemoryAddress(int array[]) { return array; }

unsigned long getSizeOfArrayAsParameter(int array[]) {
  // The following is an error, because the array degrades to a pointer when
  // passed. You can pass in an array of arbitrary size.

  // "error: sizeof on array function parameter will return size of 'int *'
  // instead of 'int []'"

  // return sizeof(array);

  return sizeof(static_cast<int *>(array));
}

unsigned long getSizeOfArray6AsParameter(int array[6]) {
  // This is still the same error as above, even if the size of the array is
  // explicit.
  // return sizeof(array);
  return sizeof(static_cast<int *>(array));
}

// bool isPointerFixedArray(int[3]* pointerArray) {
//   return true;
// }

int &valueAt(int *array, int position) { return array[position]; }

void run_tests() {
  test::suite("features::arrays", []() {
    test::describe("arrays can be created on the stack.", []() {
      int array[4];
      test::ok(bool(array),
               "An array was created, but it's still uninitialized. Acces");
      test::ok(isInt(array[0]), "The value can be accessed, but its value "
                                "could be anything. UNDEFINED BEHAVIOR");
      array[0] = 11;
      array[1] = 22;
      array[2] = 33;
      array[3] = 44;
      test::equal(array[0], 11, "Value at index 0 can be accessed");
      test::equal(array[1], 22, "Value at index 1 can be accessed");
      test::equal(array[2], 33, "Value at index 2 can be accessed");
      test::equal(array[3], 44, "Value at index 3 can be accessed");
      int index = 7;
      test::ok(isInt(array[index]),
               "The compiler doesn't care if we unintentionally read values "
               "outside of the array. UNDEFINED BEHAVIOR");
    });

    test::describe("arrays can be initialized in one line", []() {
      int array[3] = {11, 22, 33};
      test::equal(array[0], 11, "Value at index 0 can be accessed");
      test::equal(array[1], 22, "Value at index 1 can be accessed");
      test::equal(array[2], 33, "Value at index 2 can be accessed");
    });

    test::describe(
        "arrays can be created through 'universal initialization'", []() {
          int array[3]{11, 22, 33};
          test::equal(array[0], 11, "Value at index 0 can be accessed");
          test::equal(array[1], 22, "Value at index 1 can be accessed");
          test::equal(array[2], 33, "Value at index 2 can be accessed");
        });

    test::describe("Arrays don't need to have every entry initialized", []() {
      int array[2] = {11};
      test::equal(array[0], 11, "Value at index 0 can be accessed");
      test::ok(isInt(array[1]), "Value at index 1 is not initialized");
      // But this is a compiler error:
      // int array [2] = { 11, 22, 33 };
    });

    test::describe("Static arrays are always initialized to 0.", []() {
      static int array[3];
      test::equal(array[0], 0, "The value is 0");
      test::equal(array[1], 0, "The value is 0");
      test::equal(array[2], 0, "The value is 0");
    });

    test::describe("Heap allocated arrays", []() {
      int *array = new int[3]{11, 22, 33};
      test::equal(array[0], 11, "Created 11 on the heap");
      test::equal(array[1], 22, "Created 22 on the heap");
      test::equal(array[2], 33, "Created 33 on the heap");
      delete[] array;
    });

    test::describe("Arrays can be passed to functions", []() {
      int array[3]{11, 22, 33};
      addOneToAllMembers(array, 3);
      test::equal(array[0], 12, "One was added to array");
      test::equal(array[1], 23, "One was added to array");
      test::equal(array[2], 34, "One was added to array");
    });

    test::describe(
        "Arrays can be treated like pointers, and the memory address is copied "
        "to the calling function",
        []() {
          int array[3]{11, 22, 33};
          addOneToNumber(array);
          test::equal(array[0], 12, "One was added to the first element");
          test::equal(array[1], 22, "The rest of the array is not modified");
          test::equal(array[2], 33, "The rest of the array is not modified");
        });

    test::describe(
        "Pointer arithmetic works on arrays coerced to pointers", []() {
          int array[3]{11, 22, 33};
          int *firstElement = array;
          test::equal(*firstElement, 11,
                      "Can read the first element as a pointer");
          test::equal(*(firstElement + 1), 22,
                      "Can read the second element using pointer arithmetic");
          test::equal(*(firstElement + 2), 33,
                      "Can read the second element using pointer arithmetic");
        });

    test::describe(
        "You can pass by references, but only at a fixed array size", []() {
          int array[3]{11, 22, 33};

          // Assign the 2nd value, but reference to the original array.
          get2ndValue(array) = 44;

          test::equal(array[0], 11, "First element not modified");
          test::equal(array[1], 44,
                      "Second element was modified because the function call "
                      "returned an lvalue reference");
          test::equal(array[2], 33, "The third element was not modified");
        });

    test::describe(
        "Passing arrays is mainly type checking by the compiler, but then it "
        "copies the memory address of the array to the callee's arguments.",
        []() {
          int array[3]{11, 22, 33};
          int *arrayAddress = getArrayMemoryAddress(array);
          test::equal(array, arrayAddress, "The memory values are the same.");
        });

    test::describe("array sizeof() and passing arrays to functions", []() {
      int array2[] = {1, 2};
      int array6[] = {1, 2, 3, 4, 5, 6};
      test::equal(sizeof(array2[0]), sizeof(int),
                  "The size of the first member is that of an int");
      test::equal(sizeof(*array2), sizeof(int),
                  "The same is true when looking at as a pointer");
      test::equal(sizeof(array2), 2UL * sizeof(int),
                  "The size of the array element is only accessible on the "
                  "current stack");
      test::equal(sizeof(array6), 6UL * sizeof(int),
                  "Variable lengths can be created, but they are only stored "
                  "on the current stack");
      test::equal(getSizeOfArrayAsParameter(array6), sizeof(void *),
                  "When passing to a function, the stack-allocated array "
                  "degrades to a pointer.");
      test::equal(getSizeOfArray6AsParameter(array6), sizeof(void *),
                  "This also happens even when the size of the array in the "
                  "arguments is explicit, like array[6]");
      test::equal(getSizeOfArray6AsParameter(array2), sizeof(void *),
                  "Size arguments are a lie");
    });
  });
}

} // namespace arrays
} // namespace features
