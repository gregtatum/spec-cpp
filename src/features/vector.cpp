#include "../test.h"
#include <span>
#include <string>
#include <vector>

namespace features {
namespace vector {

/**
 * Turn a std::vector into a human readable string.
 */
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

/**
 * Do a non-sensical check to demonstrate that a value can be accessed.
 */
bool intExists(int value) { return value >= 0 || value < 0; }

/**
 * This is a helper function that applies toString to vectors and asserts their
 * equality.
 */
template <typename A, typename B>
void assertVecEqual(A &&a, B &&b, const std::string &message) {
  test::equal(toString(a), toString(b), message);
}

size_t take_array(int *a) { return sizeof(a); }

void run_tests() {
  test::suite("features::vector", []() {
    test::describe("Can create a vector", []() { std::vector<int> vec; });

    test::describe("Vectors can be initialized with a size", []() {
      std::vector<int> vec(5);
      test::equal(vec.size(), size_t(5), "Vectors initialized with a size");
    });

    test::describe("toString helper", []() {
      test::equal(toString(std::vector<int>({1, 2, 3})),
                  std::string("{ 1, 2, 3 }"),
                  "It creates a debug string for arrays with values.");
      test::equal(toString(std::vector<int>({})), std::string("{}"),
                  "It creates a debug string for arrays without values.");
    });

    test::describe("Vectors are initialized to 0", []() {
      assertVecEqual(std::vector<int>(5), std::vector<int>({0, 0, 0, 0, 0}),
                     "Nothing is in the vector yet.");
    });

    test::describe("Vectors can be pushed onto", []() {
      std::vector<int> vec{};
      assertVecEqual(vec, std::vector<int>({}),
                     "Nothing is in the vector yet.");
      test::equal(vec.size(), size_t(0), "It has no size");
      test::equal(vec.capacity(), size_t(0), "It has no capacity");

      vec.push_back(1);
      assertVecEqual(vec, std::vector<int>({1}), "One thing is in the vector.");
      test::equal(vec.size(), size_t(1), "Its size matches length");
      test::equal(vec.capacity(), size_t(1), "Its capacity matches the size");

      vec.push_back(2);
      assertVecEqual(vec, std::vector<int>({1, 2}),
                     "Two things are on the vector.");
      test::equal(vec.size(), size_t(2), "Its size matches length");
      test::equal(vec.capacity(), size_t(2), "Its capacity matches the size");

      vec.pop_back();
      assertVecEqual(vec, std::vector<int>({1}), "One thing is in the vector.");
      test::equal(vec.size(), size_t(1), "Its size matches length");
      test::equal(vec.capacity(), size_t(2), "Its capacity has not changed.");
    });

    test::describe("Vectors retain capacity when popping", []() {
      std::vector<int> vec{1, 2, 3, 4, 5, 6};

      vec.pop_back();
      vec.pop_back();
      vec.pop_back();
      vec.pop_back();
      vec.pop_back();

      assertVecEqual(vec, std::vector<int>({1}), "One thing is in the vector.");
      test::equal(vec.size(), size_t(1), "Its size matches length");
      test::equal(vec.capacity(), size_t(6),
                  "Its capacity still has the initial sizing.");
    });

    test::describe("Vectors can be resized", []() {
      std::vector<int> vec{1, 2, 3};
      vec.resize(6);
      assertVecEqual(vec, std::vector<int>({1, 2, 3, 0, 0, 0}),
                     "Size adds on to the size.");
    });

    test::describe("Vectors can be resized with a value", []() {
      std::vector<int> vec{1, 2, 3};
      vec.resize(6, 4);
      assertVecEqual(vec, std::vector<int>({1, 2, 3, 4, 4, 4}),
                     "Size adds on to the size.");
    });

    test::describe("std::vector::at", []() {
      std::vector<int> vec{11, 22, 33};
      test::equal(vec.at(0), 11, "Value can be access");
      test::equal(vec.at(1), 22, "Value can be access");
      test::equal(vec.at(2), 33, "Value can be access");
      try {
        // Try to access out of range.
        vec.at(3);
      } catch (const std::exception &) {
        test::ok(true, "This is an out of range exception.");
      }
    });

    test::describe("std::vector::[]", []() {
      std::vector<int> vec{11, 22, 33};
      test::equal(vec[0], 11, "Value can be access");
      test::equal(vec[1], 22, "Value can be access");
      test::equal(vec[2], 33, "Value can be access");
      test::ok(intExists(vec[3]),
               "UNDEFINED VALUE: It allows you to access out of range values.");
    });

    test::describe("std::vector::at can be used as an lvalue", []() {
      std::vector<int> vec{11, 22, 33};
      vec.at(0) = 44;
      assertVecEqual(vec, std::vector<int>({44, 22, 33}),
                     "The first item was assigned to.");
    });

    test::describe("std::vector accessors", []() {
      std::vector<int> vec{11, 22, 33};
      test::equal(vec.front(), 11, "Can access the front");
      test::equal(vec.back(), 33, "Can access the back");
      test::equal(vec.data()[0], 11, "Can access the underlying data");
      test::equal(vec.data()[1], 22, "Can access the underlying data");
      test::equal(vec.data()[2], 33, "Can access the underlying data");
    });

    test::describe("std::vector only copies data, it does not modify it", []() {
      int array[] = {1, 2, 3};
      std::vector<int> vecA(std::begin(array), std::end(array));
      array[1] = 10;
      test::equal(toString(vecA), std::string("{ 1, 2, 3 }"),
                  "It matches the original, not the modified");
    });

    test::describe("std::vector can be passed as a C-style array", []() {
      std::vector<int> vec{1, 2, 3, 4, 5, 6};
      // int *array = &vec[0];
      int array[] = {1, 2, 3, 4, 5, 6};
      printf("%lu", sizeof(array));
      test::equal(sizeof(array), 6UL * 4UL, "");
      // take_array
    });

    struct TwoVectors {
      std::vector<int> a;
      std::vector<int> b;
    };

    test::describe("std::vector by default deep-clones in structs", []() {
      TwoVectors deux{
          {1, 2, 3},
          {4, 5, 6},
      };
      TwoVectors trois{deux};

      test::equal(toString(deux.a), std::string("{ 1, 2, 3 }"),
                  "deux.a matches");
      test::equal(toString(deux.b), std::string("{ 4, 5, 6 }"),
                  "deux.b matches");
      test::equal(toString(trois.a), std::string("{ 1, 2, 3 }"),
                  "trois.a matches");
      test::equal(toString(trois.b), std::string("{ 4, 5, 6 }"),
                  "trois.b matches");

      deux.a[0] = 7;
      deux.a[1] = 8;
      deux.a[2] = 9;

      test::equal(toString(deux.a), std::string("{ 7, 8, 9 }"),
                  "deux.a matches");
      test::equal(toString(deux.b), std::string("{ 4, 5, 6 }"),
                  "deux.b matches");
      test::equal(toString(trois.a), std::string("{ 1, 2, 3 }"),
                  "trois.a matches");
      test::equal(toString(trois.b), std::string("{ 4, 5, 6 }"),
                  "trois.b matches");
    });

    test::describe("std::span with std::vector", []() {
      std::vector<int> vector{1, 2, 3};
      std::span<int> span{vector};

      test::equal(toString(vector), std::string("{ 1, 2, 3 }"),
                  "vector matches");
      test::equal(toString(span), std::string("{ 1, 2, 3 }"), "span matches");

      vector[1] = 9;

      test::equal(toString(vector), std::string("{ 1, 9, 3 }"),
                  "vector changed");
      test::equal(toString(span), std::string("{ 1, 9, 3 }"),
                  "the span changed too");
    });
  });
}
} // namespace vector
} // namespace features
