#include "../test.h"
#include "threads.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <chrono>

namespace features {
namespace threads {

// Using the sieve of Eratosthenese.
std::vector<bool> computePrimesSerially(size_t count) {
  std::vector<bool> primes(count);
  std::fill (primes.begin(), primes.end(), true);
  size_t countSqrt = std::sqrt(count);

  for (size_t i = 2; i < countSqrt; i++) {
    auto isPrime = primes[i];
    if (isPrime) {
      for (size_t j = i * 2; j < count; j += i) {
        primes[j] = false;
      }
    }
  }

  // Debug information:
  // for (size_t i = 2; i < primes.size(); i++) {
  //   printf("Number: %zu %s\n", i, primes[i] ? "prime" : "");
  // }

  return primes;
}

void fn() {
}

long timeExecution(std::function<void ()> callback) {
  auto before = std::chrono::high_resolution_clock::now();
  callback();
  auto after = std::chrono::high_resolution_clock::now();
  return (
    std::chrono::duration_cast<std::chrono::microseconds>(after - before)
    .count()
  );
}

void run_tests() {
  test::suite("features::primes", []() {
    test::describe("timing", []() {
      auto timing = timeExecution([&]() { computePrimesSerially(1000000); });
      printf("\n\nIt took %ld microseconds\n\n", timing);
    });

    return;
    test::describe("compute primes serially", []() {
      std::vector<bool> primes;

      auto timing = timeExecution([&]() {
        primes = computePrimesSerially(100000);
      });

      printf("\n\nIt took %ld microseconds\n\n", timing);

      test::equal((bool) primes[1], true, "1 is prime");
      test::equal((bool) primes[2], true, "2 is prime");
      test::equal((bool) primes[3], true, "3 is prime");
      test::equal((bool) primes[5], true, "5 is prime");
      test::equal((bool) primes[7], true, "7 is prime");
      test::equal((bool) primes[11], true, "9 is prime");

      test::equal((bool) primes[4], false, "4 is not prime");
      test::equal((bool) primes[6], false, "6 is not prime");
      test::equal((bool) primes[8], false, "8 is not prime");
      test::equal((bool) primes[9], false, "9 is not prime");
      test::equal((bool) primes[10], false, "10 is not prime");
    });
  });
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
