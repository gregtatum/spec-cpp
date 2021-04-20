#include "../../includes/mfbt/Result.h"
#include "../../includes/mfbt/ResultVariant.h"
#include "../test.h"
#include <vector>

namespace mfbt {
namespace TestResult {

enum class Error { ExampleError };

template <typename T> std::string toString(T &&vec) {
  std::string output("{ ");
  for (auto value : vec) {
    output.append(std::to_string(value));
    output.append(", ");
  }
  if (vec.size() > 0) {
    // This vector is non-empty, remove that last ", "
    output.pop_back();
    output.pop_back();
    output.append(" }");
  } else {
    // Make the result "{}"
    output.pop_back();
    output.append("}");
  }

  return output;
}

template <typename A, typename B>
void assertVecEqual(A &&a, B &&b, const std::string &message) {
  test::equal(toString(a), toString(b), message);
}

void run_tests() {
  test::suite("mfbt::TestResult", []() {
    test::describe("Vectors are initialized to 0", []() {
      assertVecEqual(std::vector<int>(5), std::vector<int>({0, 0, 0, 0, 0}),
                     "Nothing is in the vector yet.");
    });

    test::describe("mozilla::Result", []() {
      mozilla::Result<std::vector<int>, Error> vec =
          std::vector<int>({1, 2, 3});
      assertVecEqual(vec.unwrap(), std::vector<int>({1, 2, 3}), "Types match");
    });
  });
}

} // namespace TestResult

} // namespace mfbt
