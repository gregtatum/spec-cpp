#include "../test.h"
#include "./Allocator.h"
#include <unistd.h>
#include <math.h>
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

namespace memory {
namespace allocator {

#define ALLOCATION_BLOCK_SIZE sizeof(class AllocationBlock)

class AllocationBlock {
  // The payload size does not include the ALLOCATION_BLOCK_SIZE/
  size_t mPayloadSize;
public:
  AllocationBlock* next;
  bool isFree;

  AllocationBlock()
    : mPayloadSize(0)
    , next(nullptr)
    , isFree(false)
    {}

  AllocationBlock(size_t aPayloadSize, AllocationBlock* aNext, bool aIsFree)
    : mPayloadSize(aPayloadSize)
    , next(aNext)
    , isFree(aIsFree)
    {}

  size_t blockSize() {
    return mPayloadSize + ALLOCATION_BLOCK_SIZE;
  }

  size_t payloadSize() {
    return mPayloadSize;
  }

  void setBlockSize(size_t aBlockSize) {
    mPayloadSize = aBlockSize - ALLOCATION_BLOCK_SIZE;
  }

  void setPayloadSize(size_t aPayloadSize) {
    mPayloadSize = aPayloadSize;
  }

};

class Allocator {
public:
  AllocationBlock* mRoot;
  size_t mBlockByteSize;
  size_t mTotalBytesAllocated;
  size_t mActiveBytesAllocated;

  Allocator(size_t aBlockByteSize)
    // Create a root allocation block, allocating the required bytes using malloc.
    : mRoot(new (malloc(aBlockByteSize)) AllocationBlock(
      aBlockByteSize - ALLOCATION_BLOCK_SIZE, // payload size
      nullptr,
      true
    ))
    , mBlockByteSize(aBlockByteSize)
    , mTotalBytesAllocated(0)
    , mActiveBytesAllocated(0)
    {}

  ~Allocator() {
    free(reinterpret_cast<void *>(mRoot));
  }

  /**
   * Attempt to free the memory, returns true on success, false on failure.
   */
  bool free(void* pointer) {
    const AllocationBlock* unsafeTargetBlock = reinterpret_cast<AllocationBlock*>(
      reinterpret_cast<uintptr_t>(pointer) - ALLOCATION_BLOCK_SIZE
    );
    // We could naively use the pointer, but let's not trust it and walk
    // the linked list to find the block.
    AllocationBlock* block = mRoot;
    AllocationBlock* previousBlock = nullptr;
    do {
      if (block == unsafeTargetBlock) {
        block->isFree = true;
        auto nextBlock = block->next;
        if (nextBlock && nextBlock->isFree) {
          // The next block is free as well, so combine the two blocks of memory.
          block->next = nextBlock->next;
          block->setPayloadSize(block->payloadSize() + nextBlock->blockSize());
        }

        if (previousBlock && previousBlock->isFree) {
          // The previous block is free as well, so combine the two blocks of memory.
          previousBlock->next = block->next;
          previousBlock->setPayloadSize(
            previousBlock->payloadSize() + block->blockSize()
          );
        }
        return true;
      }
      previousBlock = block;
      block = block->next;
    } while(block);

    // The block wasn't found.
    return false;
  }

  size_t countBlocks() {
    size_t count = 0;
    AllocationBlock* block = mRoot;
    while ((block = block->next)) {
      count++;
    }
    return count;
  }

  void freeAllAllocations() {
    mRoot->setBlockSize(mBlockByteSize);
    mRoot->next= nullptr;
    mRoot->isFree = true;
    mTotalBytesAllocated = 0;
    mActiveBytesAllocated = 0;
  }

  AllocationBlock* findFreeBlock(const size_t payloadSize) {
    AllocationBlock* block = mRoot;

    // Walk the linked list and try to find a new block.
    do {
      if (block->isFree && payloadSize <= block->payloadSize()) {
        // This block is free and big enough to add the data.
        return block;
      }
      block = block->next;
    } while(block);

    return nullptr;
  }

  /**
   * Align the byte value to 8 bits.
   */
  static size_t alignBytes(size_t byteSize) {
    // Sorry for the bitshifting. Given 2^3 == 8, chopping off the least significant bits
    // with bit shifting 3 will align the value to 8 bits.
    return (((byteSize - 1) >> 3) << 3) + 8;
  }

  /**
   * This method allows for simple end-user allocation of values. It relies upon
   * templates to generalize the allocation by the type.
   */
  template<typename AllocatedType, typename... Args>
  AllocatedType* allocate(Args&&... aArgs) {
    // Allocate a block of the appropritate size.
    void* pointer = this->allocateBlock(sizeof(AllocatedType));

    // Use the in-place new operator, and then forward along the args. This preserves
    // the rvalue and lvalue of the args, as created by the callee.
    return new (pointer) AllocatedType(std::forward<Args>(aArgs)...);
  }

  /**
   * This function sets a block to a payload size. If it has remaining bytes free, it
   * split this block into two blocks, the first with the payload, the second free.
   */
  void static setBlockWithPayload(AllocationBlock* block, size_t payloadSizeToAllocate) {
    // Ensure this is a valid allocation.
    assert(block->payloadSize() >= payloadSizeToAllocate);

    // Is there remaining free space to create a new block?
    auto freeBytesAfterAllocation = block->payloadSize() - payloadSizeToAllocate;
    if (freeBytesAfterAllocation > ALLOCATION_BLOCK_SIZE) {
      // There is enough room in this block to split it into two blocks, where
      // the first contains the payload, and the second is free.
      void* pointerToNextBlock = reinterpret_cast<void *>(
        // Move the pointer forward to the next allocation block.
        reinterpret_cast<uintptr_t>(block) + ALLOCATION_BLOCK_SIZE + payloadSizeToAllocate
      );

      // Set the values for the new split free block.
      auto newFreeBlock = new(pointerToNextBlock) AllocationBlock(
        freeBytesAfterAllocation - ALLOCATION_BLOCK_SIZE, // Free block's payload size.
        block->next,
        true // This block is free.
      );

      // Update the existing block with the new information.
      block->next = newFreeBlock;
      block->setPayloadSize(payloadSizeToAllocate);
    }
    block->isFree = false;
  }

  void* allocateBlock(const size_t payloadSize) {
    if (payloadSize == 0) {
      // This value doesn't make sense.
      return nullptr;
    }

    auto payloadSizeToAllocate = Allocator::alignBytes(payloadSize);

    AllocationBlock* block = this->findFreeBlock(payloadSizeToAllocate);
    if (!block) {
      // No space is available for the allocation.
      return nullptr;
    }

    Allocator::setBlockWithPayload(block, payloadSizeToAllocate);

    // Remember how many bytes we are holding onto.
    mTotalBytesAllocated += payloadSizeToAllocate + ALLOCATION_BLOCK_SIZE;
    mActiveBytesAllocated += payloadSize;

    // Return a pointer to the payload.
    return reinterpret_cast<void*>(
      reinterpret_cast<uintptr_t>(block) + ALLOCATION_BLOCK_SIZE
    );
  }
};

void run_tests() {
  // This code assumes 64bit.
  assert(sizeof(size_t) == 8);

  test::suite("memory::allocator", []() {
    test::describe("A basic allocation", []() {
      Allocator allocator = Allocator(1024);
      test::equal(allocator.mActiveBytesAllocated, size_t(0), "No bytes allocated");
      test::equal(allocator.mTotalBytesAllocated, size_t(0), "No bytes allocated");

      test::ok(allocator.allocateBlock(5), "It was able to allocate something");
      test::equal(allocator.mActiveBytesAllocated, size_t(5), "Allocated some bytes");
      test::equal(
        allocator.mTotalBytesAllocated - ALLOCATION_BLOCK_SIZE,
        size_t(8),
        "It allocated a single aligned block"
      );
    });

    test::describe("Multiple allocations", []() {
      Allocator allocator = Allocator(1024);
      test::ok(allocator.allocateBlock(5), "It was able to allocate something");
      test::ok(allocator.allocateBlock(5), "It was able to allocate something");
      test::ok(allocator.allocateBlock(5), "It was able to allocate something");
      test::equal(allocator.mActiveBytesAllocated, size_t(15), "Allocated some bytes");
      test::equal(
        allocator.mTotalBytesAllocated,
        (size_t(8) + ALLOCATION_BLOCK_SIZE) * 3,
        "It allocated many aligned blocks"
      );
    });

    test::describe("Setting values at allocations", []() {
      Allocator allocator = Allocator(1024);
      auto a = allocator.allocate<int>(11);
      auto b = allocator.allocate<int>(22);
      auto c = allocator.allocate<int>(33);

      test::equal(*a, 11, "a is equal to 11");
      test::equal(*b, 22, "b is equal to 22");
      test::equal(*c, 33, "c is equal to 33");
    });

    test::describe("Freeing all allocations", []() {
      Allocator allocator = Allocator(1024);
      allocator.allocateBlock(5);
      allocator.allocateBlock(5);
      allocator.allocateBlock(5);
      test::ok(allocator.mTotalBytesAllocated > 0, "Some bytes have been allocated.");
      allocator.freeAllAllocations();
      test::ok(allocator.mTotalBytesAllocated == 0, "Freeing the allocations results in no bytes.");
    });

    test::describe("Re-using space when freeing up the memory", []() {
      Allocator allocator = Allocator(1024);

      auto a1 = allocator.allocate<int>(11);
      auto b1 = allocator.allocate<int>(22);
      auto c1 = allocator.allocate<int>(33);

      allocator.freeAllAllocations();

      int* a2 = reinterpret_cast<int*>(allocator.allocateBlock(sizeof(int)));
      int* b2 = reinterpret_cast<int*>(allocator.allocateBlock(sizeof(int)));
      int* c2 = reinterpret_cast<int*>(allocator.allocateBlock(sizeof(int)));

      test::equal(*a1, 11, "a is equal to 11");
      test::equal(*b1, 22, "b is equal to 22");
      test::equal(*c1, 33, "c is equal to 33");

      *a2 = 44;
      *b2 = 55;
      *c2 = 66;

      test::equal(*a2, 44, "a2 is equal to 44");
      test::equal(*b2, 55, "b2 is equal to 55");
      test::equal(*c2, 66, "c2 is equal to 66");

      test::equal(*a1, 44, "a1 is equal to 44");
      test::equal(*b1, 55, "b1 is equal to 55");
      test::equal(*c1, 66, "c1 is equal to 66");
    });

    test::describe("Values allocated get aligned to 8 bytes.", []() {
      Allocator allocator = Allocator(1024);
      uintptr_t a = reinterpret_cast<uintptr_t>(allocator.allocateBlock(sizeof(int)));
      uintptr_t b = reinterpret_cast<uintptr_t>(allocator.allocateBlock(sizeof(int)));

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
      Allocator allocator = Allocator(1024);
      auto a1 = allocator.allocate<int>(11);
      auto b1 = allocator.allocate<int>(22);
      auto c1 = allocator.allocate<int>(33);

      allocator.free(b1);

      int* b2 = reinterpret_cast<int*>(allocator.allocateBlock(sizeof(int)));

      test::equal(*a1, 11, "None of the original values are touched. a is equal to 11");
      test::equal(*b1, 22, "None of the original values are touched. b is equal to 22");
      test::equal(*c1, 33, "None of the original values are touched. c is equal to 33");

      test::equal(b1, b2, "The re-allocated memory re-used the freed spot");

      *b2 = 44;
      test::equal(*b1, 44, "The original spot had its value changed.");
      test::equal(*b2, 44, "The new spot had its value changed.");
    });

    test::describe("Freeing a block after an already free block will combine them into one block", []() {
      Allocator allocator = Allocator(1024);

      allocator.allocate<int>();
      auto a = allocator.allocate<int>();
      auto b = allocator.allocate<int>();
      allocator.allocate<int>();

      test::equal(allocator.countBlocks(), (size_t) 4, "Starts out with 4 blocks");
      allocator.free(a);
      allocator.free(b);
      test::equal(
        allocator.countBlocks(),
        (size_t) 3,
        "After freeing the two middle ones, they are combined, and only 3 remain."
      );
    });

    test::describe("Freeing a block before an already free block will combine into one block", []() {
      Allocator allocator = Allocator(1024);

      allocator.allocate<int>();
      auto a = allocator.allocate<int>();
      auto b = allocator.allocate<int>();
      allocator.allocate<int>();

      test::equal(allocator.countBlocks(), (size_t) 4, "Starts out with 4 blocks");
      allocator.free(b);
      allocator.free(a);

      test::equal(
        allocator.countBlocks(),
        (size_t) 3,
        "After freeing the two middle ones, they are combined, and only 3 remain."
      );
    });

    test::describe("Can allocate into a free block", []() {
      Allocator allocator = Allocator(1024);

      auto a = allocator.allocate<int>(11);
      auto b = allocator.allocate<int>(22);
      auto c = allocator.allocate<int>(33);
      auto d = allocator.allocate<int>(44);

      test::equal(*a, 11, "a is equal to 11");
      test::equal(*b, 22, "b is equal to 22");
      test::equal(*c, 33, "c is equal to 33");
      test::equal(*d, 44, "d is equal to 33");

      allocator.free(b);
      allocator.free(c);

      auto e = allocator.allocate<long>(55);

      test::equal(*a, 11, "a is still equal to 11");
      test::equal(*d, 44, "d is still equal to 33");
      test::equal(*e, (long) 55, "e is equal to 55");

      test::ok(
        reinterpret_cast<void*>(a) < reinterpret_cast<void*>(e),
        "The a block is earlier than the d block"
      );
      test::ok(
        reinterpret_cast<void*>(d) > reinterpret_cast<void*>(e),
        "The e block was placed before the d block"
      );
      test::ok(
        reinterpret_cast<void*>(b) == reinterpret_cast<void*>(e),
        "The e block was placed in the b block position"
      );
    });
  });
}

} // allocator
} // memory
