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
  // Use an char rather than bool so that we don't used the vector's built in packed
  // version of the bool, which takes extra computation cycles to extract.
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
  class ThreadCommunication {
  public:
    std::atomic<bool> mIsRunning;
    std::atomic<bool> mHasData;
    // Data:
    std::atomic<size_t> mStartingMultipleIndex;
    std::atomic<size_t> mPrimeValue;

    ThreadCommunication()
      : mIsRunning(true)
      , mHasData(false)
      , mStartingMultipleIndex(0)
      , mPrimeValue(0)
      {}
  };

  // Use dynamically allocated arrays rather than vectors, in order to support atomics.
  size_t mPrimeCount;
  size_t mThreadCount;
  std::atomic<bool>* mPrimes;
  // The following are arrays of pointers.
  std::thread** mThreads;
  ThreadCommunication** mThreadCommunications;
public:
  ParallelPrimes(size_t aPrimeCount)
    : mPrimeCount(aPrimeCount)
    , mThreadCount(std::thread::hardware_concurrency())
    , mPrimes(new std::atomic<bool>[aPrimeCount])
    , mThreads(new std::thread* [mThreadCount])
    , mThreadCommunications(new ThreadCommunication* [mThreadCount])
  {}

  ~ParallelPrimes() {
    delete [] mThreads;
    delete [] mThreadCommunications;
    delete [] mPrimes;
  }

  void compute() {
    for (size_t i = 0; i < mPrimeCount; i++) {
      // Initialize the array values.
      mPrimes[i].store(true, std::memory_order_relaxed);
    }
    this->initializeThreads();

    const size_t primeCountSqrt = std::sqrt(mPrimeCount);
    size_t lastKnownMultiple = 0;

    for (size_t i = 2; i < primeCountSqrt; i++) {
      if (i > lastKnownMultiple) {
        // Wait for all writers to finish, we can't yet read from any of the following
        // samples.
        this->awaitAllWriters();

        // Set the NEXT multiple of this value to not being prime, then we can kick
        // of writers for every value from this until the next multiple.
        lastKnownMultiple = i * 2;

        // Is this prime?
        if (mPrimes[i].load(std::memory_order_relaxed)) {
          // Mark the lastKnownMultiple location as a multiple.
          mPrimes[lastKnownMultiple].store(false, std::memory_order_relaxed);

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
    this->stopAllThreads();
  }

  void stopAllThreads() {
    for (size_t i = 0; i < mThreadCount; i++) {
      // Make sure all the threads are joined, so they aren't dangling and
      // still operating.
      mThreadCommunications[i]->mIsRunning = false;
      mThreads[i]->join();
      delete mThreads[i];
      delete mThreadCommunications[i];
    }
  }

  void launchWriter(size_t startingMultipleIndex, size_t primeValue) {
    while (true) {
      // Loop forever looking for a free worker in the thread pool.
      for (
        size_t threadIndex = 0;
        threadIndex < mThreadCount;
        threadIndex++
      ) {
        // Go through each active worker, and examine it's data.
        if (!mThreadCommunications[threadIndex]->mHasData) {
          // This free has no data, set it, and it will in turn find it, and write
          // out the multiples.
          mThreadCommunications[threadIndex]->mStartingMultipleIndex = startingMultipleIndex;
          mThreadCommunications[threadIndex]->mPrimeValue = primeValue;
          // Make sure and set this value last:
          mThreadCommunications[threadIndex]->mHasData = true;
          return;
        }
      }
    }
  }

  void initializeThreads() {
    for (size_t i = 0; i < mThreadCount; i++) {
      mThreadCommunications[i] = new ThreadCommunication {};
      mThreads[i] = new std::thread {
        ParallelPrimes::threadRunner,
        std::ref(mThreadCommunications[i]),
        std::ref(mPrimes),
        mPrimeCount
      };
    }
  }

  static void threadRunner(
    ThreadCommunication*& aThreadCommunication,
    std::atomic<bool>*& primes,
    size_t primeCount
  ) {
    while (aThreadCommunication->mIsRunning) {
      if (aThreadCommunication->mHasData) {
        ParallelPrimes::writeMultiples(
          aThreadCommunication->mStartingMultipleIndex,
          aThreadCommunication->mPrimeValue,
          primeCount,
          primes
        );
        aThreadCommunication->mHasData = false;
      }
    }
  }

  static void writeMultiples(
    size_t startingMultipleIndex,
    size_t primeValue,
    size_t primeCount,
    std::atomic<bool>*& primes
  ) {
    for (
      size_t j = startingMultipleIndex;
      j < primeCount;
      j += primeValue
    ) {
      primes[j].store(false, std::memory_order_relaxed);
    }
  }

  bool isPrime(size_t number) {
    return mPrimes[number];
  }

  void awaitAllWriters() {
    while (true) {
      // Loop forever looking for a free worker in the thread pool.
      bool areThreadsDone = true;
      for (
        size_t threadIndex = 0;
        threadIndex < mThreadCount;
        threadIndex++
      ) {
        areThreadsDone = areThreadsDone && !mThreadCommunications[threadIndex]->mHasData;
      }
      if (areThreadsDone) {
        return;
      }
    }
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
    // size_t timing_count = 1000;
    size_t timing_count = 100000000;
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
    return;
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

    test::describe("compute primes parallel", []() {
      ParallelPrimes primes(1000);
      auto timing = timeExecution([&]() {
        primes.compute();
      });

      printf("    ℹ It took %ld microseconds\n", timing);

      test::equal(primes.isPrime(1), true, "1 is prime");
      test::equal(primes.isPrime(2), true, "2 is prime");
      test::equal(primes.isPrime(3), true, "3 is prime");
      test::equal(primes.isPrime(5), true, "5 is prime");
      test::equal(primes.isPrime(7), true, "7 is prime");
      test::equal(primes.isPrime(11), true, "9 is prime");

      test::equal(primes.isPrime(4), false, "4 is not prime");
      test::equal(primes.isPrime(6), false, "6 is not prime");
      test::equal(primes.isPrime(8), false, "8 is not prime");
      test::equal(primes.isPrime(9), false, "9 is not prime");
      test::equal(primes.isPrime(10), false, "10 is not prime");
    });
  });

  test::describe("serial and parallel both have the same values", [&]() {
    ParallelPrimes parallelPrimes(1000);
    parallelPrimes.compute();
    auto serialPrimes = computePrimesSerially(1000);
    bool doMatch = true;
    for (size_t i = 0; i < serialPrimes.size(); i++) {
      doMatch = doMatch && parallelPrimes.isPrime(i) == (bool) serialPrimes[i];
    }
    test::ok(doMatch, "the results agree");
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
