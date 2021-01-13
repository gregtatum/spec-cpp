# C++ Learning Spec

This is a test suite that encapsulates my C++ learning in a series of tests. The building process is done through a `makefile` so from the command line run `make` to build it, and then `./main` to run it. Or do both steps at the same time with `make test`. Individual tests can be run by setting the `TEST` environment variable.

```
make test TEST=features::pointers
```

## Requirements

The build process assumes that `clang++` is on the path with the ability to use the std library `c++1y`.

ICU4C was added as a library, and it must be added to `/usr/local`. This directory is specified in `makefile-icu.mk`. To install the ICU library on macOS:

```
git clone git@github.com:unicode-org/icu.git
cd icu/icu4c/source
./runConfigureICU MacOSX
```

## Test output

```
features::classes

  nested classes
    ✔ Can create a NestedOuterClass on the stack.
    ✔ Can create a NestedInnerClass, and access a static variable.
    ✔ Can create a NestedInnerClass, and access a static variable.
    ✔ Can create a NestedInnerClass, and access a static variable.
  default initialize in-class
    ✔ It can default initialize.
    ✔ It creates a string

features::arrays

  arrays can be created on the stack.
    ✔ An array was created, but it's still uninitialized. Acces
    ✔ The value can be accessed, but its value could be anything. UNDEFINED BEHAVIOR
    ✔ Value at index 0 can be accessed
    ✔ Value at index 1 can be accessed
    ✔ Value at index 2 can be accessed
    ✔ Value at index 3 can be accessed
    ✔ The compiler doesn't care if we unintentionally read values outside of the array. UNDEFINED BEHAVIOR
  arrays can be initialized in one line
    ✔ Value at index 0 can be accessed
    ✔ Value at index 1 can be accessed
    ✔ Value at index 2 can be accessed
  arrays can be created through 'universal initialization'
    ✔ Value at index 0 can be accessed
    ✔ Value at index 1 can be accessed
    ✔ Value at index 2 can be accessed
  Arrays don't need to have every entry initialized
    ✔ Value at index 0 can be accessed
    ✔ Value at index 1 is not initialized
  Static arrays are always initialized to 0.
    ✔ The value is 0
    ✔ The value is 0
    ✔ The value is 0
  Heap allocated arrays
    ✔ Created 11 on the heap
    ✔ Created 22 on the heap
    ✔ Created 33 on the heap
  Arrays can be passed to functions
    ✔ One was added to array
    ✔ One was added to array
    ✔ One was added to array
  Arrays can be treated like pointers, and the memory address is copied to the calling function
    ✔ One was added to the first element
    ✔ The rest of the array is not modified
    ✔ The rest of the array is not modified
  Pointer arithmetic works on arrays coerced to pointers
    ✔ Can read the first element as a pointer
    ✔ Can read the second element using pointer arithmetic
    ✔ Can read the second element using pointer arithmetic
  You can pass by references, but only at a fixed array size
    ✔ First element not modified
    ✔ Second element was modified because the function call returned an lvalue reference
    ✔ The third element was not modified
  Passing arrays is mainly type checking by the compiler, but then it copies the memory address of the array to the callee's arguments.
    ✔ The memory values are the same.
  array sizeof() and passing arrays to functions
    ✔ The size of the first member is that of an int
    ✔ The same is true when looking at as a pointer
    ✔ The size of the array element is only accessible on the current stack
    ✔ Variable lengths can be created, but they are only stored on the current stack
    ✔ When passing to a function, the stack-allocated array degrades to a pointer.
    ✔ This also happens even when the size of the array in the arguments is explicit, like array[6]
    ✔ Size arguments are a lie

features::icu

  basic usage
    ✔ A pattern can be generated.
    ✔ The date can be formatted

features::lvalueReference

  lvalue references
    ✔ The int value is 32
    ✔ The ref value is the same
    ✔ The int value has changed
    ✔ So has the ref value
    ✔ The ref value is changed
    ✔ The original value has changed as well
  const lvalue references
    ✔ The int value is 32
    ✔ The ref value is the same
    ✔ The int value has changed
    ✔ The ref value will change even thought it's const
  passing lvalue references to functions
    ✔ Starts as 32
    ✔ Is mutated and added by one
    ✔ The new value is 34
    ✔ But the original can't be changed
  lvalue references can be returned from another function
    ✔ Got the second entry
    ✔ The array value can be updated
    ✔ The array value can be updated
  lvalue references can be assigned to, as they are lvalues
    ✔ The array value is updated too
  lvalue references can be stored in structs
    ✔ The ref struct holds the value 32
    ✔ The struct holds the value 32
    ✔ The ref struct's value can be changed.
    ✔ The non-ref struct's value remains the same.
    ✔ The original value changes too
  lvalue references in structs can't break out to outer lexical scopes
    ✔ The ref struct can be initialized with a value.
    ✔ The struct  can be initialized with a value.
    ✔ Setting the intHolder in a different lexical scope is fine, as the value is copied.
    ✔ We can't assign a reference to a value in a exterior lexical scope block

features::misc

  std::string
    ✔ String comparison
    ✔ String view comparison
  constexpr
    ✔ The area is computed at compile time.
    ✔ The area is computed at run time.
    ✔ The area can be re-computed on changing member values
  using namespace
    ✔ 'using' follows normal scoping rules
    ✔ Namespaces can be used individually to simplify namespaces
    ✔ 'using' is bound by lexical scopes

features::pointers

  stack values
    ✔ Declaring a simple value puts it on the stack.
  puts a value on the heap
    ✔ 🚧  The value is not initialized, so it could be anything. (possibly intermittent)
    ✔ Values placed on the heap can be accessed through the * operator.
  the value on the stack is the memory address of the heap
    ✔ The value itself is a memory address, so it shouldn't equal our value.
    ✔ The memory address is still in lexical scope and can be re-assigned, but it points at possibly anything in memory. Accesing it is UNDEFINED BEHAVIOR!
  multiple pointers point to the same place in memory
    ✔ The values are in the heap are the same
    ✔ The value on the stack is initially the same as what's on the heap
    ✔ The value on the heap was updated
    ✔ The other pointer points to the same value
    ✔ The value on the stack is a separate value, and is not updated
  references
    ✔ The value is on the stack
    ✔ The reference points to the value on the stack.
    ✔ The value on the stack was updated
    ✔ This is reflected in our own pointer.
  passing around pointers
    ✔ Starts as 32
    ✔ Is mutated and added by one
    ✔ The new value is 34
    ✔ But the original can't be changed

features::rvalueReference

  rvalue references
    ✔ rvalue references point to temporary values
  rvalue references can be passed to functions
    ✔ The rvalue reference function added one.

features::copymove

  Copyable
  MoveOnly
  DisableCopyMove
  Unspecified
  MoveOnlyMember

features::smartPointers

  Create a unique_ptr
    ✔ Can create a smart pointer
    ✔ After leaving it's scope, the reference is removed.
  Delete it before the end of the scope
    ✔ Can create a smart pointer
    ✔ Reseting deletes the object at the reference.
  Release the ownership of the object
    ✔ Can create a smart pointer
    ✔ The object was not removed from the heap
    ✔ The object was manually removed
  Create a shared_ptr
    ✔ A shared pointer is created, and shared with the outer scope
    ✔ The pointer is still shared, so the underlying object still exists.
    ✔ After leaving both scope, the object is destructed
  Create a weak_ptr
    ✔ A shared pointer is created, and shared with the outer scope
    ✔ The weak pointer is not expired
    ✔ The pointer should be deleted, as it was only weakly held
    ✔ The weak pointer is expired
  Create a weak_ptr, but lock it
    ✔ A shared pointer is created, and shared with the outer scope
    ✔ The weak pointer is not expired
    ✔ The pointer is still alive, because the weak ptr locked it
    ✔ The weak pointer is expired
    ✔ The lock and original value are out of scope, the object was deleted
    ✔ The weak pointer is expired

features::primes

  timing in serial
    ℹ It took 32 microseconds to compute 1000 primes in serial
  timing in parallel
    ℹ It took 630 microseconds to compute 1000 primes using concurrent writers

features::threads

  Created a thread

features::vector

  Can create a vector
  Vectors can be initialized with a size
    ✔ Vectors initialized with a size
  toString helper
    ✔ It creates a debug string for arrays with values.
    ✔ It creates a debug string for arrays without values.
  Vectors are initialized to 0
    ✔ Nothing is in the vector yet.
  Vectors can be pushed onto
    ✔ Nothing is in the vector yet.
    ✔ It has no size
    ✔ It has no capacity
    ✔ One thing is in the vector.
    ✔ Its size matches length
    ✔ Its capacity matches the size
    ✔ Two things are on the vector.
    ✔ Its size matches length
    ✔ Its capacity matches the size
    ✔ One thing is in the vector.
    ✔ Its size matches length
    ✔ Its capacity has not changed.
  Vectors retain capacity when popping
    ✔ One thing is in the vector.
    ✔ Its size matches length
    ✔ Its capacity still has the initial sizing.
  Vectors can be resized
    ✔ Size adds on to the size.
  Vectors can be resized with a value
    ✔ Size adds on to the size.
  std::vector::at
    ✔ Value can be access
    ✔ Value can be access
    ✔ Value can be access
    ✔ This is an out of range exception.
  std::vector::[]
    ✔ Value can be access
    ✔ Value can be access
    ✔ Value can be access
    ✔ UNDEFINED VALUE: It allows you to access out of range values.
  std::vector::at can be used as an lvalue
    ✔ The first item was assigned to.
  std::vector accessors
    ✔ Can access the front
    ✔ Can access the back
    ✔ Can access the underlying data
    ✔ Can access the underlying data
    ✔ Can access the underlying data
  std::vector only copies data, it does not modify it
    ✔ It matches the original, not the modified
  std::vector can be passed as a C-style array
24    ✔
  std::vector by default deep-clones in structs
    ✔ deux.a matches
    ✔ deux.b matches
    ✔ trois.a matches
    ✔ trois.b matches
    ✔ deux.a matches
    ✔ deux.b matches
    ✔ trois.a matches
    ✔ trois.b matches
  std::span with std::vector
    ✔ vector matches
    ✔ span matches
    ✔ vector changed
    ✔ the span changed too

rusty::box

  Box()
    ✔ Can create a boxed int
  Box destructor called for the pointer
    ✔ The destructor for the box was called
  Can convert to a raw pointer
    ✔ Can turn into a normal pointer.

rusty::option

  Option
    ✔ Can unwrap a value
    ✔ A some value is Some
    ✔ A some value is not None
    ✔ Cannot unwrap a None
    ✔ A none value is not Some
    ✔ A none value is None

rusty::vec

  Vec
    ✔ The vector starts with 0 capacity
    ✔ The initial length is also 0.
  Vec::push
    ✔ The capacity is now 1
    ✔ The length is also 1
    ✔ The value added is 1
    ✔ The second value is none
    ✔ The capacity is doubled after a push
    ✔ The length is also doubled after a push
    ✔ The value was added
    ✔ The value was added
    ✔ The last value is none
    ✔ The capacity is doubled after a push
    ✔ The length is only incremented
    ✔ The value was added
    ✔ The value was added
    ✔ The value was added
    ✔ The last value is none

src::test

  Assertions
    ✔ Does not throw
    ✔ Equality does work

mfbt::TestRefPtr

  RefPtr works just like a unique_ptr with one reference
    ✔ Has one reference
    ✔ No objects have been killed in the inner scope
    ✔ The reference counted object is killed
  nullptr
    ✔ Uninitialized pointers are false on if checks
    ✔ RefPtr can be assigned to a nullptr
    ✔ RefPtr of course can be assigned.
  reassignment
    ✔ A starts with 1 ref count
    ✔ B starts with 1 ref count
    ✔ The references to A are now 2
    ✔ The references to B are now 2, as it's the same as A
    ✔ One object has been killed
    ✔ Both have been killed
  RefPtr pointer behavior
    ✔ It works like a normal pointer calling member functions.
  RefPtr can have multiple references and properly count them
    ✔ Has one reference
    ✔ Has two reference
    ✔ Has two reference
    ✔ No objects have been killed in the inner scope
    ✔ One reference was removed when it went out of scope
    ✔ No objects have been killed in the middle scope
    ✔ The reference counted object is killed
  observe the refcount by passing a copy of the RefPtr to a function
    ✔ Has one reference at the beginning
    ✔ It had two reference counts in the function call.
    ✔ Has one reference after the function call
    ✔ No objects have been killed in the inner scope
    ✔ The reference counted object is killed
  DANGEROUS! observe the refcount by passing a pointer to the RefPtr to a function
    ✔ Has one reference at the beginning
    ✔ DANGER! Passing by raw pointer obliterates the contract.
    ✔ Has one reference after the function call
    ✔ No objects have been killed in the inner scope
    ✔ The reference counted object is killed
  RefPtr retained by a static
    ✔ Has one reference after initialization
    ✔ Has two references, as one is kept as a static property
    ✔ The RefPtr was still not killed, as it is being retained by the HasStaticObj
    ✔ The reference had to be manually deleted

mfbt::TestMaybe

  mozilla::Some
    ✔ There is some value
    ✔ There is not nothing
    ✔ There is a value
    ✔ There is a value
    ✔ Can access via pointer
    ✔ Can access via reference
  mozilla::Nothing
    ✔ There is no value
    ✔ This is nothing
    ✔ There is not value, so use default
  Can emplace a value
    ✔ There is no value
    ✔ There is now an emplaced value
  Can emplace a value with interesting constructors
    ✔ There is now an emplaced value

memory::allocator

  A basic allocation
    ✔ No bytes allocated
    ✔ No bytes allocated
    ✔ It was able to allocate something
    ✔ Allocated some bytes
    ✔ It allocated a single aligned block
  Multiple allocations
    ✔ It was able to allocate something
    ✔ It was able to allocate something
    ✔ It was able to allocate something
    ✔ Allocated some bytes
    ✔ It allocated many aligned blocks
  Setting values at allocations
    ✔ a is equal to 11
    ✔ b is equal to 22
    ✔ c is equal to 33
  Freeing all allocations
    ✔ Some bytes have been allocated.
    ✔ Freeing the allocations results in no bytes.
  Re-using space when freeing up the memory
    ✔ a is equal to 11
    ✔ b is equal to 22
    ✔ c is equal to 33
    ✔ a2 is equal to 44
    ✔ b2 is equal to 55
    ✔ c2 is equal to 66
    ✔ a1 is equal to 44
    ✔ b1 is equal to 55
    ✔ c1 is equal to 66
  Values allocated get aligned to 8 bytes.
    ✔ The int is less than 8 bytes
    ✔ The allocation block is aligned
    ✔ The two allocations are aligned 8 bytes apart, plus the allocation block size.
  Values can be freed
    ✔ None of the original values are touched. a is equal to 11
    ✔ None of the original values are touched. b is equal to 22
    ✔ None of the original values are touched. c is equal to 33
    ✔ The re-allocated memory re-used the freed spot
    ✔ The original spot had its value changed.
    ✔ The new spot had its value changed.
  Freeing a block after an already free block will combine them into one block
    ✔ Starts out with 4 blocks
    ✔ After freeing the two middle ones, they are combined, and only 3 remain.
  Freeing a block before an already free block will combine into one block
    ✔ Starts out with 4 blocks
    ✔ After freeing the two middle ones, they are combined, and only 3 remain.
  Can allocate into a free block
    ✔ a is equal to 11
    ✔ b is equal to 22
    ✔ c is equal to 33
    ✔ d is equal to 33
    ✔ a is still equal to 11
    ✔ d is still equal to 33
    ✔ e is equal to 55
    ✔ The a block is earlier than the d block
    ✔ The e block was placed before the d block
    ✔ The e block was placed in the b block position

memory::stack

  Stack values are sequentially created backwards on the stack
    ✔ The diff of the addresses match
    ✔ The diff of the addresses match
    ✔ The diff of the addresses match
    ✔ The diff of the addresses match
  Stack values can be accessed using pointer arithmetic
    ✔ Can access the values via pointer arithmetic
    ✔ Can access the values via pointer arithmetic
    ✔ Can access the values via pointer arithmetic
    ✔ Can access the values via pointer arithmetic
    ✔ Can access the values via pointer arithmetic
  Stack values can be accessed using raw memory addresses
    ✔ Can access the values by raw memory access
    ✔ Can access the values by raw memory access
    ✔ Can access the values by raw memory access
    ✔ Can access the values by raw memory access
    ✔ Can access the values by raw memory access
  Stack values can be CHANGED using raw memory addresses, even on const values
    ✔ Value on the stack was changed
    ✔ Value on the stack was changed
    ✔ Value on the stack was changed
  Arrays are stack allocated
    ✔ Value at address matches array value
    ✔ Value at address matches array value
    ✔ Value at address matches array value
    ✔ Value at address matches array value
    ✔ Value at address matches array value
    ✔ Value at address matches array value
    ✔ Value at address matches array value
    ✔ Value at address matches array value
    ✔ Value at address matches array value
    ✔ Array 'a' matches when looking it up
    ✔ Array 'b' matches when looking it up
    ✔ Array 'c' matches when looking it up

 All tests passed!
 ```
