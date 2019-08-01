#include <stdio.h>
#include <assert.h>

#pragma once

namespace rusty {
namespace box {

template <typename T>
class Box {
public:
  explicit Box(T* aPointer) : mPointer(aPointer) {
    // A box cannot be a nullptr.
    assert(aPointer);
  }

  ~Box() {
    // Destroy the data when it leaves.
    delete mPointer;
  }

  T* intoRaw() {
    auto pointer = mPointer;
    mPointer = nullptr;
    return pointer;
  }

  T& operator* () {
    assert(mPointer);
    return *mPointer;
  }
private:
  T* mPointer;
};

void run_tests();

} //box
} //rusty
