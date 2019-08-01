#include "box.h"
#include "../test.h"

namespace rusty {
namespace box {

class IncrementOnDestruct {
public:
  explicit IncrementOnDestruct(int* aCounter) : mCounter(aCounter) {}
  ~IncrementOnDestruct() {
    ++(*mCounter);
  }
private:
  int* mCounter;
};

void run_tests() {
  test::suite("rusty::box", []() {
    test::describe("Box()", []() {
      auto myBox = Box<int>(new int(5));
      test::equal(*myBox, 5, "Can create a boxed int");
    });

    test::describe("Box destructor called for the pointer", []() {
      int counter = 0;
      {
        auto myBox = Box<IncrementOnDestruct>(new IncrementOnDestruct(&counter));
      }
      test::equal(counter, 1, "The destructor for the box was called");
    });

    test::describe("Can convert to a raw pointer", []() {
      int* myInt = nullptr;
      {
        auto myBox = Box<int>(new int(5));
        myInt = myBox.intoRaw();
      }
      test::equal(*myInt, 5, "Can turn into a normal pointer.");
    });
  });
}

} // box
} // rusty
