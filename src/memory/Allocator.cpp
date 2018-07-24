#include "../test.h"
#include "./Allocator.h"
#include <unistd.h>
#include <math.h>
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

namespace memory {
namespace allocator {

#define ALLOCATION_BLOCK_SIZE sizeof(class AllocationBlock)

class AllocationBlock {
public:
  // Byte size is the size of the stored data, not including the allocation block.
  size_t byteSize;
  AllocationBlock* next;
  bool isFree;

  AllocationBlock()
    : byteSize(0)
    , next(nullptr)
    , isFree(false)
    {}
};

class Allocator {
public:
  AllocationBlock mRoot;
  AllocationBlock* mTip;
  size_t mTotalBytesAllocated;
  size_t mActiveBytesAllocated;

  Allocator()
    : mRoot(AllocationBlock())
    , mTip(&mRoot)
    , mTotalBytesAllocated(0)
    , mActiveBytesAllocated(0)
    {}

  ~Allocator() {
    this->freeAllAllocations();
  }

  bool free(void* pointer) {
    AllocationBlock* block = &mRoot;
    AllocationBlock* targetBlock = reinterpret_cast<AllocationBlock*>(
      reinterpret_cast<uintptr_t>(pointer) - ALLOCATION_BLOCK_SIZE
    );
    // We could naively use the pointer, but let's not trust it and walk
    // the linked list to find the block.
    do {
      if (block == targetBlock) {
        block->isFree = true;
        return true;
      }
      block = block->next;
    } while(block);

    // The block wasn't found.
    return false;
  }

  void freeAllAllocations() {
    if (this->canResetProgramBreakPointer()) {
      // This doesn't actually work on macOS. It was a fun theory.
      // https://stackoverflow.com/questions/22666728/sbrk-and-negative-parameter
      sbrk(-mTotalBytesAllocated);
    }
    mTip = &mRoot;
    mTotalBytesAllocated = 0;
    mActiveBytesAllocated = 0;
  }

  bool canResetProgramBreakPointer() {
    if (mRoot.next == nullptr) {
      return true;
    }
    if (mTip == &mRoot) {
      // Don't reset the program counter if nothing has been allocated.
      return false;
    }
    // Use pointers to determine the size of the allocations.
    long int allocationsByPointerMath = (
      // Take the tip location.
      reinterpret_cast<uintptr_t>(mTip)
      // Add on the latest allocation block's memory size.
      + mTip->byteSize + ALLOCATION_BLOCK_SIZE
      // Subtract the starting root memory location
      - reinterpret_cast<uintptr_t>(mRoot.next)
    );
    return allocationsByPointerMath == mTotalBytesAllocated;
  }

  AllocationBlock* findFreeBlock(const size_t byteSize) {
    AllocationBlock* block = &mRoot;

    // Walk the linked list and try to find a new block.
    do {
      if (block->isFree && byteSize <= block->byteSize) {
        // This block is free and big enough to add the data.
        return block;
      }
      block = block->next;
    } while(block);

    return this->allocateNewBlock(byteSize);
  }

  AllocationBlock* allocateNewBlock(const size_t byteSize) {
    if (byteSize == 0) {
      // This value doesn't make sense.
      return nullptr;
    }

    // Align this byte value to 8 bits.
    // Sorry for the bitshifting.
    const size_t byteSizeAligned = (((byteSize - 1) >> 3) << 3) + 8;
    // The allocation block is already aligned. Add that on.
    const size_t totalSize = ALLOCATION_BLOCK_SIZE + byteSizeAligned;

    // There isn't any space in the existing blocks. Start to allocate a new one.
    AllocationBlock* newBlock = reinterpret_cast<AllocationBlock*>(sbrk(0));

    // Allocate enough space for the AllocationBlock and the bytes needed.
    const void* response = sbrk(totalSize);
    if (response == (void *) -1) {
      return nullptr;
    }

    // Remember how many bytes we are holding onto.
    mTotalBytesAllocated += totalSize;
    mActiveBytesAllocated += byteSize;

    // Set the fields of the bock.
    newBlock->byteSize = byteSizeAligned;
    newBlock->next = nullptr;
    newBlock->isFree = false;

    // Replace the tip.
    mTip->next = newBlock;
    mTip = newBlock;

    // Offset the pointer to return the free space.
    return newBlock;
  }

  void* allocate(const size_t byteSize) {
    AllocationBlock* block = this->findFreeBlock(byteSize);
    if (!block) {
      return nullptr;
    }
    return reinterpret_cast<void*>(
      reinterpret_cast<uintptr_t>(block) + ALLOCATION_BLOCK_SIZE
    );
  }
};

void run_tests() {
  // This code assumes 64bit.
  assert(sizeof(size_t) == 8);

  test::suite("memory::allocator", []() {
    test::describe("calling sbrk", []() {
      void *programBreakMemoryAddress = sbrk(0);
      test::ok(programBreakMemoryAddress, "It returns a memory address");
    });

    test::describe("basic allocation", []() {
      void *start = sbrk(5);
      void *end = sbrk(0);
      test::equal(
        long(end) - long(start),
        long(5),
        "It can allocate 5 bytes"
      );
    });

    test::describe("allocation failures", []() {
      void *failure = sbrk(1000000000);
      test::equal(
        failure,
        reinterpret_cast<void *>(-1),
        "It will fail when given values it can't work with."
      );
    });

    test::describe("A basic allocation", []() {
      Allocator allocator = Allocator();
      test::equal(allocator.mActiveBytesAllocated, size_t(0), "No bytes allocated");
      test::equal(allocator.mTotalBytesAllocated, size_t(0), "No bytes allocated");

      test::ok(allocator.allocate(5), "It was able to allocate something");
      test::equal(allocator.mActiveBytesAllocated, size_t(5), "Allocated some bytes");
      test::equal(
        allocator.mTotalBytesAllocated - ALLOCATION_BLOCK_SIZE,
        size_t(8),
        "It allocated a single aligned block"
      );
    });

    test::describe("Multiple allocations", []() {
      Allocator allocator = Allocator();
      test::ok(allocator.allocate(5), "It was able to allocate something");
      test::ok(allocator.allocate(5), "It was able to allocate something");
      test::ok(allocator.allocate(5), "It was able to allocate something");
      test::equal(allocator.mActiveBytesAllocated, size_t(15), "Allocated some bytes");
      test::equal(
        allocator.mTotalBytesAllocated,
        (size_t(8) + ALLOCATION_BLOCK_SIZE) * 3,
        "It allocated many aligned blocks"
      );
    });

    test::describe("Setting values at allocations", []() {
      Allocator allocator = Allocator();
      int* a = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));
      int* b = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));
      int* c = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));

      *a = 11;
      *b = 22;
      *c = 33;

      test::equal(*a, 11, "a is equal to 11");
      test::equal(*b, 22, "b is equal to 22");
      test::equal(*c, 33, "c is equal to 33");
    });

    test::describe("Freeing all allocations", []() {
      Allocator allocator = Allocator();
      allocator.allocate(5);
      allocator.allocate(5);
      allocator.allocate(5);
      test::ok(allocator.mTotalBytesAllocated > 0, "Some bytes have been allocated.");
      allocator.freeAllAllocations();
      test::ok(allocator.mTotalBytesAllocated == 0, "Freeing the allocations results in no bytes.");
    });

    test::describe("Setting values at allocations", []() {
      Allocator allocator = Allocator();
      int* a1 = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));
      int* b1 = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));
      int* c1 = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));

      *a1 = 11;
      *b1 = 22;
      *c1 = 33;

      allocator.freeAllAllocations();

      int* a2 = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));
      int* b2 = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));
      int* c2 = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));

      test::equal(*a1, 11, "a is equal to 11");
      test::equal(*b1, 22, "b is equal to 22");
      test::equal(*c1, 33, "c is equal to 33");

      *a2 = 44;
      *b2 = 55;
      *c2 = 66;

      test::equal(*a2, 44, "a2 is equal to 44");
      test::equal(*b2, 55, "b2 is equal to 55");
      test::equal(*c2, 66, "c2 is equal to 66");

      /**
       * The program break pointer doesn't actually decrement in macOS.
       * https://stackoverflow.com/questions/22666728/sbrk-and-negative-parameter
       * In theory I should be able to change the values for the older values,
       * but this doesn't seem to do anything.
       */

      // test::equal(*a1, 44, "a1 is equal to 44");
      // test::equal(*b1, 55, "b1 is equal to 55");
      // test::equal(*c1, 66, "c1 is equal to 66");
    });

    test::describe("Values allocated get aligned to 8 bytes.", []() {
      Allocator allocator = Allocator();
      uintptr_t a = reinterpret_cast<uintptr_t>(allocator.allocate(sizeof(int)));
      uintptr_t b = reinterpret_cast<uintptr_t>(allocator.allocate(sizeof(int)));

      test::ok(sizeof(int) < 8, "The int is less than 8 bytes");
      test::equal(
        ALLOCATION_BLOCK_SIZE % 8,
        (unsigned long) 0, // What is the proper way to create an unsigned long?
        "The allocation block is aligned"
      );
      test::equal(
        b - a,
        8 + ALLOCATION_BLOCK_SIZE,
        "The two allocations are aligned 8 bytes apart, plus the allocation block size."
      );
    });

    test::describe("Values can be freed", []() {
      Allocator allocator = Allocator();
      int* a1 = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));
      int* b1 = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));
      int* c1 = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));

      *a1 = 11;
      *b1 = 22;
      *c1 = 33;

      allocator.free(b1);
      int* b2 = reinterpret_cast<int*>(allocator.allocate(sizeof(int)));

      test::equal(*a1, 11, "None of the original values are touched. a is equal to 11");
      test::equal(*b1, 22, "None of the original values are touched. b is equal to 22");
      test::equal(*c1, 33, "None of the original values are touched. c is equal to 33");

      test::equal(b1, b2, "The re-allocated memory re-used the freed spot");

      *b2 = 44;
      test::equal(*b1, 44, "The original spot had its value changed.");
      test::equal(*b2, 44, "The new spot had its value changed.");
    });
  });
}

} // allocator
} // memory
