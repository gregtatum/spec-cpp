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

      COMPILER_ERROR(
          // "a" cannot be copied.
          MoveOnly b{a};

          // clang-format off
          // src/features/rvalue-reference.cpp:63:16: error: call to implicitly-deleted copy constructor of 'MoveOnly'
          //       MoveOnly b{a};
          //                ^~~~
          // src/features/rvalue-reference.cpp:54:7: note: copy constructor is implicitly deleted because 'MoveOnly' has a user-declared move constructor
          //       MoveOnly(MoveOnly &&other) = default;
          //       ^
          // clang-format on
      );

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

      COMPILER_ERROR(
          // A cannot be copied.
          DisableCopyMove b{a};

          // clang-format off
          // src/features/rvalue-reference.cpp:42:23: error: call to deleted constructor of 'DisableCopyMove'
          //       DisableCopyMove b{a};
          //                       ^~~~
          // src/features/rvalue-reference.cpp:35:7: note: 'DisableCopyMove' has been explicitly marked deleted here
          //       DisableCopyMove(const DisableCopyMove &) = delete;
          //       ^
          // clang-format on

          // A cannot be moved, either. The same compiler error is generated.
          DisableCopyMove c{std::move(a)};);
    });

    class Unspecified {
    public:
      Unspecified(){};
    };

    test::describe("Unspecified", []() {
      Unspecified a{};
      // "a" can be copied.
      Unspecified b{a};
      // "a" can be moved.
      Unspecified c{std::move(a)};

      test::ignore(b);
      test::ignore(c);
    });

    class MoveOnlyMember {
    public:
      COMPILER_ERROR(
          // Allow copy
          MoveOnlyMember(const MoveOnlyMember &other) = default;
          MoveOnlyMember & operator=(const MoveOnlyMember &other) = default;

          // clang-format off
          // src/features/rvalue-reference.cpp:128:7: error: explicitly defaulted copy constructor is implicitly deleted [-Werror,-Wdefaulted-function-deleted]
          //       MoveOnlyMember(const MoveOnlyMember &other) = default;
          //       ^
          // src/features/rvalue-reference.cpp:134:16: note: copy constructor of 'MoveOnlyMember' is implicitly deleted because field 'mMoveOnly' has a deleted copy constructor
          //       MoveOnly mMoveOnly = {};
          // clang-format on
      )

      MoveOnlyMember(){};
      MoveOnly mMoveOnly = {};
    };

    test::describe("MoveOnlyMember", []() {
      MoveOnlyMember a{};

      COMPILER_ERROR(
          // "a" cannot be copied.
          MoveOnlyMember b{a};

          // clang-format off
          // src/features/rvalue-reference.cpp:133:22: error: call to implicitly-deleted copy constructor of 'MoveOnlyMember'
          //       MoveOnlyMember b{a};
          //                      ^~~~
          // src/features/rvalue-reference.cpp:127:16: note: copy constructor of 'MoveOnlyMember' is implicitly deleted because field 'mMoveOnly' has a deleted copy constructor
          //       MoveOnly mMoveOnly = {};
          //                ^
          // clang-format on
      )

      // "a" can be moved.
      MoveOnlyMember c{std::move(a)};

      test::ignoreRValue(std::move(c));
    });
  });
}

} // namespace rvalueReference
} // namespace features
