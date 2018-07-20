#include <vector>
#include <string>
#include "../test.h"

namespace features {
namespace vector {

/**
 * Turn a std::vector into a human readable string.
 */
template<typename T>
std::string toString(T&& vec) {
  std::string output("{ ");
  for (auto value: vec) {
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

/**
 * Do a non-sensical check to demonstrate that a value can be accessed.
 */
bool intExists(int value) {
  return value >= 0 || value < 0;
}

/**
 * This is a helper function that applies toString to vectors and asserts their equality.
 */
template<typename A, typename B>
void assertVecEqual(A&& a, B&& b, const std::string& message) {
  test::equal(toString(a), toString(b), message);
}

void run_tests() {
  test::suite("features::vector", []() {
    test::describe("Can create a vector", []() {
      std::vector<int> vec;
    });

    test::describe("Vectors can be initialized with a size", []() {
      std::vector<int> vec(5);
      test::equal(
        vec.size(),
        size_t(5),
        "Vectors initialized with a size"
      );
    });

    test::describe("toString helper", []() {
      test::equal(
        toString(std::vector<int>({ 1, 2, 3 })),
        std::string("{ 1, 2, 3 }"),
        "It creates a debug string for arrays with values."
      );
      test::equal(
        toString(std::vector<int>({})),
        std::string("{}"),
        "It creates a debug string for arrays without values."
      );
    });

    test::describe("Vectors are initialized to 0", []() {
      assertVecEqual(
        std::vector<int>(5),
        std::vector<int>({ 0, 0, 0, 0, 0 }),
        "Nothing is in the vector yet."
      );
    });

    test::describe("Vectors can be pushed onto", []() {
      std::vector<int> vec {};
      assertVecEqual(vec, std::vector<int>({}), "Nothing is in the vector yet.");
      test::equal(vec.size(), size_t(0), "It has no size");
      test::equal(vec.capacity(), size_t(0), "It has no capacity");

      vec.push_back(1);
      assertVecEqual(vec, std::vector<int>({ 1 }), "One thing is in the vector.");
      test::equal(vec.size(), size_t(1), "Its size matches length");
      test::equal(vec.capacity(), size_t(1), "Its capacity matches the size");

      vec.push_back(2);
      assertVecEqual(vec, std::vector<int>({ 1, 2 }), "Two things are on the vector.");
      test::equal(vec.size(), size_t(2), "Its size matches length");
      test::equal(vec.capacity(), size_t(2), "Its capacity matches the size");

      vec.pop_back();
      assertVecEqual(vec, std::vector<int>({ 1 }), "One thing is in the vector.");
      test::equal(vec.size(), size_t(1), "Its size matches length");
      test::equal(vec.capacity(), size_t(2), "Its capacity has not changed.");
    });

    test::describe("Vectors retain capacity when popping", []() {
      std::vector<int> vec { 1, 2, 3, 4, 5, 6 };

      vec.pop_back();
      vec.pop_back();
      vec.pop_back();
      vec.pop_back();
      vec.pop_back();

      assertVecEqual(vec, std::vector<int>({ 1 }), "One thing is in the vector.");
      test::equal(vec.size(), size_t(1), "Its size matches length");
      test::equal(vec.capacity(), size_t(6), "Its capacity still has the initial sizing.");
    });

    test::describe("Vectors can be resized", []() {
      std::vector<int> vec { 1, 2, 3 };
      vec.resize(6);
      assertVecEqual(vec, std::vector<int>({ 1, 2, 3, 0, 0, 0 }), "Size adds on to the size.");
    });

    test::describe("Vectors can be resized with a value", []() {
      std::vector<int> vec { 1, 2, 3 };
      vec.resize(6, 4);
      assertVecEqual(vec, std::vector<int>({ 1, 2, 3, 4, 4, 4 }), "Size adds on to the size.");
    });

    test::describe("std::vector::at", []() {
      std::vector<int> vec { 11, 22, 33 };
      test::equal(vec.at(0), 11, "Value can be access");
      test::equal(vec.at(1), 22, "Value can be access");
      test::equal(vec.at(2), 33, "Value can be access");
      try {
        // Try to access out of range.
        vec.at(3);
      } catch(const std::exception&) {
        test::ok(true, "This is an out of range exception.");
      }
    });

    test::describe("std::vector::[]", []() {
      std::vector<int> vec { 11, 22, 33 };
      test::equal(vec[0], 11, "Value can be access");
      test::equal(vec[1], 22, "Value can be access");
      test::equal(vec[2], 33, "Value can be access");
      test::ok(intExists(vec[3]), "UNDEFINED VALUE: It allows you to access out of range values.");
    });

    test::describe("std::vector::at can be used as an lvalue", []() {
      std::vector<int> vec { 11, 22, 33 };
      vec.at(0) = 44;
      assertVecEqual(vec, std::vector<int>({ 44, 22, 33 }), "The first item was assigned to.");
    });

    test::describe("std::vector accessors", []() {
      std::vector<int> vec { 11, 22, 33 };
      test::equal(vec.front(), 11, "Can access the front");
      test::equal(vec.back(), 33, "Can access the back");
      test::equal(vec.data()[0], 11, "Can access the underlying data");
      test::equal(vec.data()[1], 22, "Can access the underlying data");
      test::equal(vec.data()[2], 33, "Can access the underlying data");
    });
  });
}
} // vector
} // features
