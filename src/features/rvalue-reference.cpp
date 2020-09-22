#include "rvalue-reference.h"
#include "../test.h"
#include <iostream>

namespace features {
namespace rvalueReference {

struct IntHolder {
  int value;
};

struct RefIntHolder {
  int &value;
};

int addOneByRvalue(int &&value) { return value + 1; }

void run_tests() {
  test::suite("features::rvalueReference", []() {
    test::describe("rvalue references", []() {
      int &&value = (16 + 16);
      test::equal(value, 32, "rvalue references point to temporary values");
    });

    test::describe("rvalue references can be passed to functions", []() {
      int &&value = addOneByRvalue(16 + 16);
      test::equal(value, 33, "The rvalue reference function added one.");
    });
  });

  test::suite("features::copymove", []() {
    class Copyable {
    public:
      // Allow copy
      Copyable(const Copyable &other) = default;
      Copyable &operator=(const Copyable &other) = default;
      Copyable(){};
    };

    test::describe("Copyable", []() {
      Copyable a{};
      // "a" can be copied.
      Copyable b{a};
      // "a" can be moved.
      Copyable c{std::move(a)};

      test::ignore(b);
      test::ignore(c);
    });

    class MoveOnly {
    public:
      // Move only
      MoveOnly(MoveOnly &&other) = default;
      MoveOnly &operator=(MoveOnly &&other) = default;
      MoveOnly(){};
      void ignore(){};
    };

    test::describe("MoveOnly", []() {
      MoveOnly a{};

      // "a" cannot be moved.
      // MoveOnly b{a};

      // clang-format off
      // src/features/rvalue-reference.cpp:63:16: error: call to implicitly-deleted copy constructor of 'MoveOnly'
      //       MoveOnly b{a};
      //                ^~~~
      // src/features/rvalue-reference.cpp:54:7: note: copy constructor is implicitly deleted because 'MoveOnly' has a user-declared move constructor
      //       MoveOnly(MoveOnly &&other) = default;
      //       ^
      // clang-format on

      // "a" can be moved.
      MoveOnly c{std::move(a)};

      test::ignoreRValue(std::move(c));
    });

    class DisableCopyMove {
    public:
      // Not copyable or movable
      DisableCopyMove(const DisableCopyMove &) = delete;
      DisableCopyMove &operator=(const DisableCopyMove &) = delete;
      DisableCopyMove(){};
    };

    test::describe("DisableCopyMove", []() {
      DisableCopyMove a{};

      // This is invalid:
      // DisableCopyMove b{a};

      // clang-format off
      // src/features/rvalue-reference.cpp:42:23: error: call to deleted constructor of 'DisableCopyMove'
      //       DisableCopyMove b{a};
      //                       ^~~~
      // src/features/rvalue-reference.cpp:35:7: note: 'DisableCopyMove' has been explicitly marked deleted here
      //       DisableCopyMove(const DisableCopyMove &) = delete;
      //       ^
      // clang-format on

      // This also doesn't work, with the same error as above.
      // DisableCopyMove c{std::move(a)};
    });
  });
}

} // namespace rvalueReference
} // namespace features
