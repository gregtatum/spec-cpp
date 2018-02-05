# C++ Learning Spec

This is a test suite that encapsulates my C++ learning in a series of tests. The building process is done through a `makefile` so from the command line run `make` to build it, and then `./main` to run it. Or do both steps at the same time with `make test`. Individual tests can be run by setting the `TEST` environment variable.

```
make test TEST=features::pointers
```

## Requirements

The build process assumes that `clang++` is on the path with the ability to use the std library `c++1y`.

## Test output
```

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

features::threads

  Created a thread

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

 All tests passed!
```
