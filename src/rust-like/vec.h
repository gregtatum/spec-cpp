#pragma once

namespace RustLike {
  class Vec {
    int mCapacity;
    int mLength;
    float* mBuffer;

    public:
      Vec(int);
      ~Vec();
      int capacity ();
      int len ();
      void push (float);
  };
}
