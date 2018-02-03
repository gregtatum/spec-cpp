#include "test.h"
#include <iostream>

namespace test {

void suite(
  const std::string& message,
  std::function<void ()> callback
) {
  const char* testName = std::getenv("TEST");
  if (
    // Run it if there is no environment variable.
    !testName ||
    // Compare that test name is the same as environment variable "TEST"
    message.compare(testName) == 0
  ) {
    std::cout << GREEN << "\n" << message << RESET << "\n\n";
    callback();
  }
}

void describe(
  const std::string& message,
  std::function<void ()> callback
) {
  std::cout << BRIGHT_WHITE << "  " << message << RESET << "\n";
  callback();
}

void assert(bool value, const std::string& message) {
  if (!value) {
    std::cout << RED << "    𝘅 " << message << RESET << "\n";
    throw std::string(message);
  } else {
    std::cout << GREEN << "    ✔ " << RESET << WHITE << message << RESET << "\n";
  }
}

void run_tests() {
  test::suite("src::test", []() {

    test::describe("Assertions", []() {
      test::assert(true, "Does not throw");
      // test::assert(false, "Uncomment to throw");
      test::assertEq(true, true, "Equality does work");
      // test::assertEq(true, false, "Inequality throws");
    });

  });
}

} // test
