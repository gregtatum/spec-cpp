#include "../test.h"

namespace features {
namespace classes {

static int shadowedValue = 1;

class NestedOuterClass {
public:
  int mOuterValue;
  static int mStaticValue;
  static int shadowedValue;
  class NestedInnerClass {
  public:
    // This is an error, inner cannot access non-static outer member values.
    // int getOuterValue() {
    //   return mOuterValue;
    // }

    int getOuterStaticValue() {
      return mStaticValue;
    }

    int getRootShadowedValue() {
      return features::classes::shadowedValue;
    }

    int getOuterShadowedValue() {
      return shadowedValue;
    }
  };

  explicit NestedOuterClass(int aOuterValue)
    : mOuterValue(aOuterValue)
    {}

  int getOuterValue() {
    return mOuterValue;
  }
};

int NestedOuterClass::mStaticValue = 7;
int NestedOuterClass::shadowedValue = 2;

class DefaultInitializer {
  public:
    int mValue = 5;
    std::string mString { "This works too." };
};

void run_tests() {
  test::suite("features::classes", []() {
    test::describe("nested classes", []() {
      NestedOuterClass outer(5);
      test::equal(outer.getOuterValue(), 5, "Can create a NestedOuterClass on the stack.");
      NestedOuterClass::NestedInnerClass inner;
      test::equal(inner.getOuterStaticValue(), 7, "Can create a NestedInnerClass, and access a static variable.");
      test::equal(inner.getRootShadowedValue(), 1, "Can create a NestedInnerClass, and access a static variable.");
      test::equal(inner.getOuterShadowedValue(), 2, "Can create a NestedInnerClass, and access a static variable.");
    });

    test::describe("default initialize in-class", []() {
      DefaultInitializer obj;
      test::equal(obj.mValue, 5, "It can default initialize.");
      test::equal(obj.mString, std::string { "This works too." }, "It creates a string");
    });
  });
}

} // classes
} // features
