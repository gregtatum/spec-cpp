#include "vec.h"
#include <iostream>
using namespace std;

RustLike::Vec::Vec (int aCapacity) {
  mLength = 0;
  mCapacity = aCapacity;
  mBuffer = new float[mCapacity];
}

RustLike::Vec::~Vec() {
  // This doesn't appear to be running?
  std::cout << "Deleting vec\n";
  delete [] mBuffer;
}

int RustLike::Vec::capacity () {
  return mCapacity;
}

int RustLike::Vec::len () {
  return mLength;
}

void RustLike::Vec::push (float aValue) {
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

// std::
// experimental::optional<float> RustLike::Vec::get (int aIndex) {
//   // if (aIndex >= 0 && aIndex < mLength)
//     return mBuffer[aIndex];
//   // }
//   // return {};
// }
