#include "option.h"

RustLike::Option::Option (bool aHasValue, float* aValue) {
  mHasValue = aHasValue;
  mValue = aValue;
}

float* RustLike::Option::unwrap() {
  if (mHasValue) {
    return mValue;
  } else {
    throw;
  }
}

RustLike::Option RustLike::some(float* aValue) {
  return RustLike::Option(true, aValue);
}
