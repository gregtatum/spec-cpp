#include "../test.h"

namespace features {
namespace misc {

void run_tests() {
  test::suite("features::misc", []() {
    test::describe("std::string", []() {
      std::string string{"lorem ipsum"};
      std::string_view string_view{string};

      test::equal(string, std::string{"lorem ipsum"}, "String comparison");
      test::equal(string_view, std::string_view{"lorem ipsum"},
                  "String view comparison");
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
      test::equal(mutable_rect.area(), 200,
                  "The area is computed at run time.");

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
        test::info(
            "Namespaces can be used individually to simplify namespaces");
      }
      test::info("'using' is bound by lexical scopes");
      COMPILER_ERROR(string c{""});
    });
  });
}

} // namespace misc
} // namespace features
