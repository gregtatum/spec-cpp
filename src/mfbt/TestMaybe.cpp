#include "../../includes/mfbt/Maybe.h"
#include "../test.h"

class SomeValue {
  public:
    int mFloat;
    int mInt;

    SomeValue(float aFloat, int aInt)
      : mFloat(aFloat)
      , mInt(aInt)
      {}
};

namespace mfbt {
namespace TestMaybe {

void run_tests() {
  test::suite("mfbt::TestMaybe", []() {
    test::describe("mozilla::Some", []() {
      mozilla::Maybe<int> myVal = mozilla::Some(5);
      test::assertEq(myVal.isSome(), true, "There is some value");
      test::assertEq(myVal.isNothing(), false, "There is not nothing");
      test::assertEq(myVal.value(), 5, "There is a value");
      test::assertEq(myVal.valueOr(8), 5, "There is a value");
      test::assertEq(*myVal.ptr(), 5, "Can access via pointer");
      test::assertEq(myVal.ref(), 5, "Can access via reference");
    });

    test::describe("mozilla::Nothing", []() {
      mozilla::Maybe<int> myVal = mozilla::Nothing();
      test::assertEq(myVal.isSome(), false, "There is no value");
      test::assertEq(myVal.isNothing(), true, "This is nothing");
      test::assertEq(myVal.valueOr(8), 8, "There is not value, so use default");
    });

    test::describe("Can emplace a value", []() {
      mozilla::Maybe<int> myVal;
      test::assertEq(myVal.isSome(), false, "There is no value");
      myVal.emplace(5);
      test::assertEq(myVal.isSome(), true, "There is now an emplaced value");
    });

    test::describe("Can emplace a value with interesting constructors", []() {
      mozilla::Maybe<SomeValue> myVal;
      myVal.emplace(1.0, 2);
      test::assertEq(myVal.isSome(), true, "There is now an emplaced value");
    });
  });
}

} // TestMaybe
} // mfbt
