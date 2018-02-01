#include "option.h"

namespace rusty {
namespace option {

Option::Option (bool aHasValue, float* aValue) {
  mHasValue = aHasValue;
  mValue = aValue;
}

float* Option::unwrap() {
  if (mHasValue) {
    return mValue;
  } else {
    // TODO - Provide a proper error enum.
    throw 0;
  }
}

bool Option::isSome() {
  return mHasValue;
}

bool Option::isNone() {
  return !mHasValue;
}

Option some(float* aValue) {
  return Option(true, aValue);
}

Option none() {
  return Option(false, nullptr);
}

} // option
} // rusty
