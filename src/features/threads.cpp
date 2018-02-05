#include "../test.h"
#include "threads.h"
#include <iostream>
#include <thread>

namespace features {
namespace threads {

void fn() {
}

void run_tests() {
  // TODO
  test::suite("features::threads", []() {
    test::describe("Created a thread", []() {
      std::thread thread(fn);
      thread.join();
    });
  });
}

} // threads
} // features
