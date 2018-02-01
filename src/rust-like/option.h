#pragma once

namespace RustLike {
  class Option {

    bool mHasValue;
    float* mValue;

    public:
      Option(bool, float*);
      float* unwrap();
  };

  Option some(float*);
  Option none();
}
