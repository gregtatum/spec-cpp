#include "../test.h"
#include "mfbt/Maybe.h"
#include <iterator> // std::iterator,std::input_iterator_tag

namespace features::iterators {
using mozilla::Maybe;
using mozilla::Nothing;
using mozilla::Some;

class Range {
public:
  class iterator;
  friend class iterator;

  class iterator : public std::iterator<std::input_iterator_tag, // iterator_category
                                        size_t,                  // value_type
                                        size_t,                  // difference_type
                                        const size_t *,          // pointer
                                        size_t                   // reference
                                        > {
    Range &mRange;
    size_t mNum = 0;

  public:
    explicit iterator(Range &aRange, size_t aNum = 0) : mRange(aRange), mNum(aNum) {}
    iterator &operator++() {
      mNum = mRange.mTo >= mRange.mFrom ? mNum + 1 : mNum - 1;
      return *this;
    }
    iterator operator++(int) {
      iterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(iterator other) const { return mNum == other.mNum; }
    bool operator!=(iterator other) const { return !(*this == other); }
    reference operator*() const { return mNum; }
  };
  Range(size_t aFrom, size_t aTo) : mFrom(aFrom), mTo(aTo) {}
  size_t mFrom = 0;
  size_t mTo = 0;
  iterator begin() { return iterator(*this, mFrom); }
  iterator end() { return iterator(*this, mTo >= mFrom ? mTo + 1 : mTo - 1); }
};

void run_tests() {
  test::suite("features::iterators", []() {
    test::describe("input iterator", []() {
      //
      for (size_t l : Range(3, 5)) {
        std::cout << l << ' '; // 3 4 5
      }
    });

    // test::describe("fibonacci iterator", []() {
    //   class Fibonacci final {

    //   public:
    //     Fibonacci(size_t aCount) : mCount(aCount){};
    //     Fibonacci &operator++() {
    //       --mCount;
    //       int64_t b = mB;
    //       b += mA;
    //       mA = b;
    //       return *this;
    //     }
    //     Maybe<int64_t> operator*() {
    //       if (mCount > 0) {
    //         return Some(mB);
    //       }
    //       return Nothing{};
    //     }
    //     bool operator!=(const Fibonacci &aOther) const { return mCount !=
    //     aOther.mCount; }

    //   private:
    //     size_t mCount = 0;
    //     int64_t mA = 0;
    //     int64_t mB = 1;

    //   public:
    //     class iterator;
    //     friend class iterator;

    //     class iterator : public std::iterator<std::input_iterator_tag, Maybe<int64_t>>
    //     {
    //       Fibonacci &mFibonacci;

    //     public:
    //       typedef Maybe<int64_t> value_type;
    //       iterator(Fibonacci &aFibonacci) : mFibonacci(aFibonacci) {}
    //       bool operator==(const iterator &aOther) const {
    //         return mFibonacci.mCount == aOther.mFibonacci.mCount;
    //       }
    //       bool operator!=(const iterator &aOther) const { return !(*this == aOther); }
    //       value_type operator*() {
    //         if (mFibonacci.mCount > 0) {
    //           return Some(mFibonacci.mB);
    //         };
    //         return Nothing{};
    //       };
    //       iterator &operator++() { return *this; }
    //       iterator operator++(int) { return *this; }
    //     };
    //   };

    //   for (const auto &item : Fibonacci{10}) {
    //     //
    //   }
    // });

    // test::describe("todo", []() {
    //   class Fibonacci final {
    //   public:
    //     Fibonacci(size_t aCount) : mCount(aCount){};
    //     Fibonacci &operator++() {
    //       --mCount;
    //       size_t b = mB;
    //       b += mA;
    //       mA = b;
    //       return *this;
    //     }
    //     Maybe<size_t> operator*() {
    //       if (mCount > 0) {
    //         return Some(mB);
    //       }
    //       return Nothing{};
    //     }
    //     bool operator!=(const Fibonacci &aOther) const { return mCount !=
    //     aOther.mCount; }

    //   private:
    //     size_t mCount = 0;
    //     size_t mA = 0;
    //     size_t mB = 1;
    //   };

    //   for (const auto &item : Fibonacci{10}) {
    //     //
    //   }
    // });

    // test::describe("attempt2", []() {
    //   class Iterator {
    //     using iterator_category = std::input_iterator_tag;
    //     using difference_type = std::ptrdiff_t;
    //     using value_type = int;
    //     using pointer = int *;   // or also value_type*
    //     using reference = int &; // or also value_type&
    //   };
    // });
  });
}

} // namespace features::iterators
