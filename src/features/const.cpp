#include "../test.h"

namespace features::const_test {
void run_tests() {
  test::suite("features::const_test", []() {
    test::describe("object of const type, 'const-qualified type'", []() {
      const int number = 5;
      // src/features/const.cpp:8:14: error: cannot assign to variable 'number'
      // with const-qualified type 'const int'

      // number += 1;

      test::equal(number, 5, "The numbers match");
    });

    test::describe("const specifier can be before and after the type", []() {
      const int number1 = 5;
      int const number2 = 6;
      test::equal(number1, 5, "const before");
      test::equal(number2, 6, "const after");
    });

    test::describe("structs of const type do not have interior mutability", []() {
      struct TestStruct {
        int number = 0;
      };

      const TestStruct testStruct{};
      // src/features/const.cpp:16:25: error: cannot assign to variable 'testStruct'
      // with const-qualified type 'const struct TestStruct'

      // testStruct.number = 5;

      test::equal(testStruct.number, 0, "The numbers match");
    });

    test::describe("const-ness of class definitions", []() {
      class TestObject {
      public:
        // Marking this as const ensures there's no mutability.
        int GetNumber() const { return number; }

        COMPILER_ERROR(
            // error:
            int ModifyNumberWithConst() const {
              number++;
              return number;
            }
            // cannot assign to non-static data member within const member
            // function 'ModifyNumberWithConst'
        )

        int ModifyNumber() {
          number++;
          return number;
        }

      private:
        int number = 0;
      };

      const TestObject testObject{};
      test::equal(testObject.GetNumber(), 0,
                  "GetNumber requires const in the function declaration");

      COMPILER_ERROR(
          // error:
          test::equal(testObject.ModifyNumber(), 1, "");
          //  'this' argument to member function 'ModifyNumber' has
          // type 'const TestObject', but function is not marked const
      );
    });

    test::describe("const struct members", []() {
      struct TestStruct {
        int mutableNumber = 0;
        const int constNumber = 0;
      };

      TestStruct testStruct{};
      testStruct.mutableNumber++;
      COMPILER_ERROR(
          // error:
          testStruct.constNumber++;
          // cannot assign to non-static data member 'constNumber' with
          // const-qualified type 'const int'
      );
    });

    test::describe("const structs with mutable members", []() {
      struct TestStruct {
        mutable int mutableNumber = 0;
        int constNumber = 0;
      };

      const TestStruct testStruct{};
      testStruct.mutableNumber = 5;
      COMPILER_ERROR(testStruct.constNumber = 5;);

      test::equal(testStruct.mutableNumber, 5, "The numbers match");
    });

    test::describe("const pointers cannot have their contents mutated", []() {
      struct TestStruct {
        int number = 0;
      };

      const TestStruct *testStruct = new TestStruct();

      // The pointed to struct cannot be modified.
      COMPILER_ERROR(testStruct->number = 5;);

      // However, it can be re-assigned.
      testStruct = new TestStruct();

      delete testStruct;
      // leak the old testStruct
    });

    test::describe("*const can be mutated but not re-assigned", []() {
      struct TestStruct {
        int number = 0;
      };

      TestStruct *const testStruct = new TestStruct();

      // An object behind *const can be mutated!
      testStruct->number = 5;

      COMPILER_ERROR(
          // However, it can be re-assigned.
          testStruct = new TestStruct();
          // cannot assign to variable 'testStruct' with const-qualified type
          // 'TestStruct *const'
      );

      delete testStruct;
    });

    test::describe("Consts can be combined: const TestStruct *const", []() {
      struct TestStruct {
        int number = 0;
      };

      const TestStruct *const testStruct = new TestStruct();

      COMPILER_ERROR(
          // Cannot mutate
          testStruct->number = 5;
          // cannot assign to variable 'testStruct' with const-qualified type
          // 'const TestStruct *const'
      );
      COMPILER_ERROR(
          // However, it can be re-assigned.
          testStruct = new TestStruct();
          // cannot assign to variable 'testStruct' with const-qualified type
          // 'TestStruct *const'
      );

      delete testStruct;
    });

    test::describe("const as function arguments", []() {
      class Adder {
      public:
        static int addOne(const int number) { return number + 1; }

        COMPILER_ERROR(
            // Error:
            static int addTwo(const int number) { return ++number + 1; }
            // cannot assign to variable 'number' with const-qualified type 'const int'
        );
      };

      test::equal(Adder::addOne(1), 2, "The function can add one.");
    });

    test::describe("const can be cast away", []() {
      struct Value {
        int number = 0;
        static void TakeMut(Value &v) {}
        static void TakeConst(const Value &v) {}
      };

      const Value value{};

      COMPILER_ERROR(
          // Error:
          Value::TakeMut(value);
          // binding reference of type 'Value' to value of type 'const Value'
          // drops 'const' qualifier
      );

      Value::TakeMut(const_cast<Value &>(value));
      Value::TakeConst(value);

      test::equal(value.number, 0, "");
    });
  });
}
} // namespace features::const_test
