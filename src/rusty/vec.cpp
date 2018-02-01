#include "vec.h"
#include "option.h"
#include <iostream>

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

} // vec
} // rusty
