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
      test::equal(myVal.isSome(), true, "There is some value");
      test::equal(myVal.isNothing(), false, "There is not nothing");
      test::equal(myVal.value(), 5, "There is a value");
      test::equal(myVal.valueOr(8), 5, "There is a value");
      test::equal(*myVal.ptr(), 5, "Can access via pointer");
      test::equal(myVal.ref(), 5, "Can access via reference");
    });

    test::describe("mozilla::Nothing", []() {
      mozilla::Maybe<int> myVal = mozilla::Nothing();
      test::equal(myVal.isSome(), false, "There is no value");
      test::equal(myVal.isNothing(), true, "This is nothing");
      test::equal(myVal.valueOr(8), 8, "There is not value, so use default");
    });

    test::describe("Can emplace a value", []() {
      mozilla::Maybe<int> myVal;
      test::equal(myVal.isSome(), false, "There is no value");
      myVal.emplace(5);
      test::equal(myVal.isSome(), true, "There is now an emplaced value");
    });

    test::describe("Can emplace a value with interesting constructors", []() {
      mozilla::Maybe<SomeValue> myVal;
      myVal.emplace(1.0, 2);
      test::equal(myVal.isSome(), true, "There is now an emplaced value");
    });
  });
}

} // TestMaybe
} // mfbt
