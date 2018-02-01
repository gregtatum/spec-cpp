#pragma once
#include "option.h"

namespace rusty {
namespace vec {

class Vec {
  int mCapacity;
  int mLength;
  float* mBuffer;

  public:
    explicit Vec(int);
    ~Vec();
    int capacity ();
    int len ();
    void push (float);
    rusty::option::Option get (int);
};

} // vec
} // rusty
