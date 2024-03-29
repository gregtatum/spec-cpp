#include "../test.h"
#include <iostream>

namespace features {
namespace lvalueReference {

void addOneByMutation(int &value) { value = value + 1; }

int addOneByReturns(const int &value) {
  // Uncommenting this generates a compiler error, as the int& is const.
  // value = value + 1;;
  return value + 1;
}

int &getSecondEntry(int (&values)[5]) { return values[1]; }

struct IntHolder {
  int value;
};

struct RefIntHolder {
  int &value;
};

void run_tests() {
  test::suite("features::lvalueReference", []() {
    test::describe("lvalue references", []() {
      int value = 32;
      int &refValue = value;

      test::equal(value, 32, "The int value is 32");
      test::equal(refValue, 32, "The ref value is the same");

      value = 33;

      test::equal(value, 33, "The int value has changed");
      test::equal(refValue, 33, "So has the ref value");

      refValue = 34;

      test::equal(refValue, 34, "The ref value is changed");
      test::equal(value, 34, "The original value has changed as well");
    });

    test::describe("const lvalue references", []() {
      int value = 32;
      const int &refValue = value;

      test::equal(value, 32, "The int value is 32");
      test::equal(refValue, 32, "The ref value is the same");

      value = 33;

      test::equal(value, 33, "The int value has changed");
      test::equal(refValue, 33,
                  "The ref value will change even thought it's const");

      // This would be a compiler error, as the refValue is a const reference.
      // refValue = 34;
    });

    test::describe("passing lvalue references to functions", []() {
      int value = 32;

      test::equal(value, 32, "Starts as 32");
      addOneByMutation(value);
      test::equal(value, 33, "Is mutated and added by one");

      int value2 = addOneByReturns(value);
      test::equal(value2, 34, "The new value is 34");
      test::equal(value, 33, "But the original can't be changed");
    });

    test::describe(
        "lvalue references can be returned from another function", []() {
          int values[5] = {11, 22, 33, 44, 55};

          int &secondValue = getSecondEntry(values);
          test::equal(secondValue, 22, "Got the second entry");

          secondValue = 222;
          test::equal(secondValue, 222, "The array value can be updated");
          test::equal(values[1], 222, "The array value can be updated");
        });

    test::describe(
        "lvalue references can be assigned to, as they are lvalues", []() {
          int values[5] = {11, 22, 33, 44, 55};
          getSecondEntry(values) = 222;
          test::equal(values[1], 222, "The array value is updated too");
        });

    test::describe("lvalue references can be stored in structs", []() {
      int value = 32;
      RefIntHolder refIntHolder = {value};
      IntHolder intHolder = {value};
      test::equal(refIntHolder.value, 32, "The ref struct holds the value 32");
      test::equal(intHolder.value, 32, "The struct holds the value 32");

      refIntHolder.value = 33;
      test::equal(refIntHolder.value, 33,
                  "The ref struct's value can be changed.");
      test::equal(intHolder.value, 32,
                  "The non-ref struct's value remains the same.");
      test::equal(value, 33, "The original value changes too");
    });

    test::describe(
        "lvalue references in structs can't break out to outer lexical scopes",
        []() {
          int valueOutside = 32;

          RefIntHolder refIntHolder = {valueOutside};
          IntHolder intHolder = {valueOutside};

          test::equal(refIntHolder.value, 32,
                      "The ref struct can be initialized with a value.");
          test::equal(intHolder.value, 32,
                      "The struct  can be initialized with a value.");

          {
            int valueInside = 33;
            intHolder = {valueInside};
            test::equal(intHolder.value, 33,
                        "Setting the intHolder in a different lexical scope is "
                        "fine, as the value is copied.");

            /**
             * The following gives a compiler error, which is nice because we
             * are referring to an int value that would be going out of scope.
             * This would lead to undefined behavior. The error by the compiles
             * is:
             *
             * copy assignment operator of 'RefIntHolder' is implicitly deleted
             * because field 'value' is of reference type 'int &'
             */
            // refIntHolder = { valueInside };
            test::ok(true, "We can't assign a reference to a value in a "
                           "exterior lexical scope block");
          }

          // Stop the compiler from complaining that never used this:
          (void)refIntHolder;
        });
  });
}

} // namespace lvalueReference
} // namespace features
