#pragma once

namespace rusty {
namespace option {

class Option {
  bool mHasValue;
  float* mValue;

  public:
    explicit Option(bool, float*);
    float* unwrap();
    bool isSome();
    bool isNone();
};

Option some(float*);
Option none();

} //option
} //rusty
