#include "../test.h"
#include "threads.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <chrono>

namespace features {
namespace threads {

// Provide a simple function to trivially test threading.
void fn() {
}

// Using the sieve of Eratosthenese.
std::vector<char> computePrimesSerially(size_t count) {
  // Use an int rather than bool, so we don't pack
  std::vector<char> primes(count);
  std::fill (primes.begin(), primes.end(), true);
  size_t primeCountSqrt = std::sqrt(count);

  for (size_t i = 2; i < primeCountSqrt; i++) {
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

class ParallelPrimes {
  // Use dynamically allocated arrays rather than vectors, in order to support atomics.
  size_t mPrimeCount;
  size_t mWriterCount;
  std::atomic<bool>* mPrimes;
  std::atomic<bool>* mAreWritersFree;
public:
  ParallelPrimes(size_t aPrimeCount)
    : mPrimeCount(aPrimeCount)
    , mWriterCount(std::thread::hardware_concurrency())
    , mPrimes(new std::atomic<bool>[aPrimeCount])
    , mAreWritersFree(new std::atomic<bool>[mWriterCount])
  {
    for (size_t i = 0; i < mPrimeCount; i++) {
      // Initialize the array values.
      mPrimes[i].store(true, std::memory_order_relaxed);
    }
    for (size_t i = 0; i < mWriterCount; i++) {
      // Initialize the array values.
      mAreWritersFree[i].store(true, std::memory_order_relaxed);
    }
  }

  ~ParallelPrimes() {
    delete [] mPrimes;
    delete [] mAreWritersFree;
  }

  void compute() {
    const size_t primeCountSqrt = std::sqrt(mPrimeCount);
    size_t lastWriterMultiple = 0;

    for (size_t i = 2; i < primeCountSqrt; i++) {
      if (i > lastWriterMultiple) {
        // Wait for all writers to finish, we can't yet read from any of the following
        // samples.
        this->awaitAllWriters();

        // Set the NEXT multiple of this value to not being prime, then we can kick
        // of writers for every value from this until the next multiple.
        lastWriterMultiple = i * 2;

        // Is this prime?
        if (mPrimes[i].load(std::memory_order_relaxed)) {
          // The next multiple is prime, mark the lastWriterMultiple location as a multiple.
          mPrimes[lastWriterMultiple].store(false, std::memory_order_relaxed);

          // This value is prime! Kick off more writers.
          // Kick off a writer that will write out the multiples until the end of the
          // primes array.
          this->launchWriter(i * 3, i);
        }
      } else {
        if (mPrimes[i].load(std::memory_order_relaxed)) {
          // This value is prime. Kick off a writer, but await a free writer.
          this->launchWriter(i * 2, i);
        }
      }
    }

    this->awaitAllWriters();
  }

  void launchWriter(size_t aStartingMultipleIndex, size_t aPrimeValue) {
    std::thread writerThread(
      ParallelPrimes::writeMultiples,
      aStartingMultipleIndex,
      aPrimeValue, // primeValue
      mPrimeCount,
      std::ref(mPrimes),
      mAreWritersFree
    );
    writerThread.join();
  }

  std::atomic<bool>* awaitFreeWriter() {
    do {
      for (size_t i = 0; i < mWriterCount; i++) {
        std::atomic<bool>& isWriterFree = mAreWritersFree[i];
        if (isWriterFree) {
          isWriterFree = false;
          return &isWriterFree;
        }
      }
      // Continue looping until one is found.
    } while (true);
  }


  static void writeMultiples(
    size_t startingMultipleIndex,
    size_t primeValue,
    size_t primeCount,
    std::atomic<bool>*& primes,
    std::atomic<bool>* isDone
  ) {
    for (
      size_t j = startingMultipleIndex;
      j < primeCount;
      j += primeValue
    ) {
      primes[j].store(false, std::memory_order_relaxed);
    }
    *isDone = true;
  }

  bool isPrime(size_t number) {
    return mPrimes[number];
  }

  void awaitAllWriters() {
    // The outstandingWriters list is full, try and find a free one.
    do{
      bool areAllWritersDone = true;

      for (size_t i = 0; i < mWriterCount; i++) {
        if (!mAreWritersFree[i]) {
          areAllWritersDone = false;
          break;
        }
      }
      if (areAllWritersDone) {
        return;
      }
      // Continue looping until all of them have finished.
    } while (true);
  }
};

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
    size_t timing_count = 10000;
    test::describe("timing in serial", [&]() {
      auto timing = timeExecution([&]() { computePrimesSerially(timing_count); });
      printf("    ℹ It took %ld microseconds to compute %ld primes in serial\n", timing, timing_count);
    });

    test::describe("timing in parallel", [&]() {
      auto timing = timeExecution([&]() {
        ParallelPrimes primes(timing_count);
        primes.compute();
      });
      printf("    ℹ It took %ld microseconds to compute %ld primes using concurrent writers\n", timing, timing_count);
    });

    test::describe("compute primes serially", []() {
      std::vector<char> primes;

      auto timing = timeExecution([&]() {
        primes = computePrimesSerially(1000);
      });

      printf("    ℹ It took %ld microseconds\n", timing);

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

    test::describe("compute primes serially", []() {
      ParallelPrimes primes(1000);
      auto timing = timeExecution([&]() {
        primes.compute();
      });

      printf("    ℹ It took %ld microseconds\n", timing);

      test::equal((bool) primes.isPrime(1), true, "1 is prime");
      test::equal((bool) primes.isPrime(2), true, "2 is prime");
      test::equal((bool) primes.isPrime(3), true, "3 is prime");
      test::equal((bool) primes.isPrime(5), true, "5 is prime");
      test::equal((bool) primes.isPrime(7), true, "7 is prime");
      test::equal((bool) primes.isPrime(11), true, "9 is prime");

      test::equal((bool) primes.isPrime(4), false, "4 is not prime");
      test::equal((bool) primes.isPrime(6), false, "6 is not prime");
      test::equal((bool) primes.isPrime(8), false, "8 is not prime");
      test::equal((bool) primes.isPrime(9), false, "9 is not prime");
      test::equal((bool) primes.isPrime(10), false, "10 is not prime");
    });
  });

  test::suite("features::threads", []() {
    test::describe("Created a thread", []() {
      std::thread thread(fn);
      thread.join();
    });
  });
}

} // threads
} // features
