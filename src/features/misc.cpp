#include "../test.h"

namespace features {
namespace misc {

const char *getFuncName() { return __func__; }

void run_tests() {
  test::suite("features::initialization", []() {
    test::describe("value initialization", []() {
      int32_t a;
      int32_t b = 0;
      int32_t c{};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
      test::ok(a != 0, "Value is uninitialized.");
#pragma GCC diagnostic pop

      //
      test::ok(b == 0, "Value is assigned.");
      // https://en.cppreference.com/w/cpp/language/value_initialization
      test::ok(c == 0, "Value is value initialized.");
    });

    test::describe("value initialization", []() {
      struct MyStruct {
        int32_t value;
        int32_t valueInitialized = 5;
      };
      MyStruct a;

      test::ok(a.value != 0, "Value is uninitialized.");
      test::equal(a.valueInitialized, 5, "Value is default initialized.");

      MyStruct b{};
      test::equal(b.value, 0, "Value is value initialized: zero-initialized");
      test::equal(b.valueInitialized, 5, "Value is default initialized.");

      // This is a function declaration:
      // MyStruct invalid();

      // This is a function declaration:
      MyStruct c = MyStruct();
      test::equal(c.value, 0, "Value is value initialized: zero-initialized");
      test::equal(c.valueInitialized, 5, "Value is default initialized.");
    });
  });

  test::suite("features::misc", []() {
    test::describe("std::string", []() {
      std::string string{"lorem ipsum"};
      std::string_view string_view{string};

      test::equal(string, std::string{"lorem ipsum"}, "String comparison");
      test::equal(string_view, std::string_view{"lorem ipsum"}, "String view comparison");
    });

    test::describe("constexpr", []() {
      class Rectangle {
      public:
        int h, w;
        constexpr Rectangle(int aH, int aW) : h(aH), w(aW) {}
        constexpr int area() const { return h * w; }
      };

      constexpr Rectangle rect{10, 20};
      test::equal(rect.area(), 200, "The area is computed at compile time.");

      Rectangle mutable_rect{10, 20};
      test::equal(mutable_rect.area(), 200, "The area is computed at run time.");

      mutable_rect.h = 20;
      test::equal(mutable_rect.area(), 400,
                  "The area can be re-computed on changing member values");
    });

    test::describe("using namespace", []() {
      test::info("'using' follows normal scoping rules");
      COMPILER_ERROR(
          //
          string a{""};

          // src/features/misc.cpp:38:7: error: unknown type name
          // 'string'; did you mean 'std::string'?
          //       string a{""};
          //       ^~~~~~
          //       std::string
      );
      {
        using std::string;
        string b{""};
        test::ignore(b);
        test::info("Namespaces can be used individually to simplify namespaces");
      }
      test::info("'using' is bound by lexical scopes");
      COMPILER_ERROR(string c{""});
    });

    test::describe("lambda basic", []() {
      auto fn = []() { return 5; };
      test::equal(fn(), 5, "Lambdas can be defined inline");
    });

    test::describe("lambdas can take arguments", []() {
      auto addFive = [](int a) { return a + 5; };
      test::equal(addFive(3), 8, "3+5=8");
    });

    test::describe("lambdas can explicitly capture values", []() {
      int captureValue = 1;
      auto addFive = [captureValue](int a) { return a + 5 + captureValue; };
      test::equal(addFive(3), 9, "3+5+1=9");
    });

    test::describe("lambdas capture by copying a value", []() {
      int captureValue = 1;
      auto addFive = [captureValue](int a) { return a + 5 + captureValue; };
      captureValue++;
      test::equal(addFive(3), 9, "3+5+1=9");
    });

    test::describe("lambdas can also capture by reference", []() {
      int captureValue = 1;
      auto addFive = [&captureValue](int a) { return a + 5 + captureValue; };
      captureValue++;
      test::equal(addFive(3), 10, "3+5+2=10");
    });

    test::describe("lambdas can implictly capture values", []() {
      int captureValue = 1;
      auto addFive = [=](int a) { return a + 5 + captureValue; };
      test::equal(addFive(3), 9, "3+5+1=9");
    });

    test::describe("lambdas can implictly capture by copy", []() {
      int captureValue = 1;
      auto addFive = [=](int a) { return a + 5 + captureValue; };
      captureValue++;
      test::equal(addFive(3), 9, "3+5+1=9");
    });

    test::describe("lambdas can implictly capture by reference", []() {
      int captureValue = 1;
      auto addFive = [&](int a) { return a + 5 + captureValue; };
      captureValue++;
      test::equal(addFive(3), 10, "3+5+2=10");
    });

    test::describe("capture defaults and explicit copy captures", []() {
      int a = 1;
      int b = 10;
      auto add = [&, b]() { return a + b; };
      test::equal(add(), 11, "It adds the original values.");
      a++;
      b++;
      test::equal(add(), 12, "b is explicitly copied, while a is by reference");
    });
  });

  test::suite("features::identifiers", []() {
    test::describe("std::string", []() {
      test::equal(getFuncName(), "getFuncName", "Gets the name of the function.");
    });
  });
}

} // namespace misc
} // namespace features
