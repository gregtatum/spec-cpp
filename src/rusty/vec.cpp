#include "vec.h"
#include "option.h"
#include <iostream>
#include "../test.h"

namespace rusty {
namespace vec {

Vec::Vec (int aCapacity) {
  mLength = 0;
  mCapacity = aCapacity;
  mBuffer = new float[mCapacity];
}

Vec::~Vec() {
  delete [] mBuffer;
}

int Vec::capacity () {
  return mCapacity;
}

int Vec::len () {
  return mLength;
}

void Vec::push (float aValue) {
  mLength++;
  if (mLength > mCapacity) {
    // Double the buffer size.
    int oldCapacity = mCapacity;
    int newCapacity = oldCapacity == 0
      ? 1
      : mCapacity * 2;
    float* oldBuffer = mBuffer;
    float* newBuffer = new float[newCapacity];

    // Copy the old buffer to the new buffer.
    for (int i = 0; i < oldCapacity; i++) {
      newBuffer[i] = oldBuffer[i];
    }

    // Remove the old one.
    delete [] oldBuffer;

    // Persist the changed values.
    mBuffer = newBuffer;
    mCapacity = newCapacity;
  }

  // Update the value.
  mBuffer[mLength - 1] = aValue;
}

rusty::option::Option Vec::get (int aIndex) {
  if (aIndex >= 0 && aIndex < mLength) {
    return rusty::option::some(&mBuffer[aIndex]);
  }
  return rusty::option::none();
}

void run_tests() {
  test::suite("rusty::vec", []() {
    test::describe("Vec", []() {
      auto vec = rusty::vec::Vec(0);
      test::equal(vec.capacity(), 0, "The vector starts with 0 capacity");
      test::equal(vec.len(), 0, "The initial length is also 0.");
    });

    test::describe("Vec::push", []() {
      auto vec = rusty::vec::Vec(0);

      vec.push(1);
      test::equal(vec.capacity(), 1, "The capacity is now 1");
      test::equal(vec.len(), 1, "The length is also 1");
      test::equal(*vec.get(0).unwrap(), 1.0f, "The value added is 1");
      test::ok(vec.get(1).isNone(), "The second value is none");

      vec.push(2);
      test::equal(vec.capacity(), 2, "The capacity is doubled after a push");
      test::equal(vec.len(), 2, "The length is also doubled after a push");
      test::equal(*vec.get(0).unwrap(), 1.0f, "The value was added");
      test::equal(*vec.get(1).unwrap(), 2.0f, "The value was added");
      test::ok(vec.get(2).isNone(), "The last value is none");

      vec.push(3);
      test::equal(vec.capacity(), 4, "The capacity is doubled after a push");
      test::equal(vec.len(), 3, "The length is only incremented");
      test::equal(*vec.get(0).unwrap(), 1.0f, "The value was added");
      test::equal(*vec.get(1).unwrap(), 2.0f, "The value was added");
      test::equal(*vec.get(2).unwrap(), 3.0f, "The value was added");
      test::ok(vec.get(3).isNone(), "The last value is none");
    });
  });
}

} // vec
} // rusty
