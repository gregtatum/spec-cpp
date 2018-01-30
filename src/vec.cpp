#include "vec.h"

RustLikeVec::Vec::Vec (int aCapacity) {
  mCapacity = aCapacity;
}

int RustLikeVec::Vec::capacity () {
  return mCapacity;
}
