#include "option.h"
#include "../test.h"

namespace rusty {
namespace option {

Option::Option (bool aHasValue, float* aValue) {
  mHasValue = aHasValue;
  mValue = aValue;
}

float* Option::unwrap() {
  if (mHasValue) {
    return mValue;
  } else {
    // TODO - Provide a proper error enum.
    throw 0;
  }
}

bool Option::isSome() {
  return mHasValue;
}

bool Option::isNone() {
  return !mHasValue;
}

Option some(float* aValue) {
  return Option(true, aValue);
}

Option none() {
  return Option(false, nullptr);
}

void run_tests() {
  test::suite("rusty::option", []() {
    test::describe("Option", []() {
      float pi = 3.14159f;
      auto maybePi = rusty::option::some(&pi);
      test::equal(*maybePi.unwrap(), 3.14159f, "Can unwrap a value");
      test::ok(maybePi.isSome(),
       "A some value is Some");
      test::ok(!maybePi.isNone(), "A some value is not None");

      auto notPi = rusty::option::none();
      try {
        notPi.unwrap();
        test::ok(false, "Cannot unwrap a None");
      } catch (int e) {
        test::ok(true, "Cannot unwrap a None");
      }

      test::ok(!notPi.isSome(), "A none value is not Some");
      test::ok(notPi.isNone(), "A none value is None");
    });
  });
}

} // option
} // rusty
