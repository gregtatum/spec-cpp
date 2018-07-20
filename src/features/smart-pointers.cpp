#include "../test.h"
#include "smart-pointers.h"
#include <iostream>

namespace features {
namespace smartPointers {

void addOneByMutation(int* value) {
  *value = *value + 1;
}

int addOneByReturns(const int* value) {
  // Uncommenting this generates a compiler error, as the int* is const.
  // *value = *value + 1;;
  return *value + 1;
}

class Object {
  bool& mIsDeleted;

  public:
    explicit Object(bool&);
    ~Object();

};

Object::Object(bool& aIsDeleted): mIsDeleted(aIsDeleted) {}

Object::~Object() {
  mIsDeleted = true;
}

void run_tests() {
  test::suite("features::smartPointers", []() {
    test::describe("Create a unique_ptr", []() {
      bool isDeleted = false;
      {
        std::unique_ptr<Object> obj(new Object(isDeleted));
        test::equal(isDeleted, false, "Can create a smart pointer");
        // The value is deleted here.
      }
      test::equal(isDeleted, true, "After leaving it's scope, the reference is removed.");
    });

    test::describe("Delete it before the end of the scope", []() {
      bool isDeleted = false;
      std::unique_ptr<Object> obj(new Object(isDeleted));
      test::equal(isDeleted, false, "Can create a smart pointer");
      obj.reset();
      test::equal(isDeleted, true, "Reseting deletes the object at the reference.");
    });

    test::describe("Release the ownership of the object", []() {
      bool isDeleted = false;
      Object* checker;
      {
        std::unique_ptr<Object> obj(new Object(isDeleted));
        // Save a NEW raw pointer to the object;
        checker = obj.get();
        // Or this, but really?
        // checker = &*obj;
        test::equal(isDeleted, false, "Can create a smart pointer");
        // Let go management of the pointer, it won't be deleted.
        obj.release();
      }
      test::equal(isDeleted, false, "The object was not removed from the heap");
      delete checker;
      test::equal(isDeleted, true, "The object was manually removed");
    });

    test::describe("Create a shared_ptr", []() {
      bool isDeleted = false;
      {
        std::shared_ptr<Object> obj1(nullptr);
        {
          std::shared_ptr<Object> obj2(new Object(isDeleted));
          obj1 = obj2;
          test::equal(isDeleted, false, "A shared pointer is created, and shared with the outer scope");
        }
        test::equal(isDeleted, false, "The pointer is still shared, so the underlying object still exists.");
      }
      test::equal(isDeleted, true, "After leaving both scope, the object is destructed");
    });

    test::describe("Create a weak_ptr", []() {
      bool isDeleted = false;
      std::weak_ptr<Object> weakObj;
      {
        std::shared_ptr<Object> obj(new Object(isDeleted));
        weakObj = obj;
        test::equal(isDeleted, false, "A shared pointer is created, and shared with the outer scope");
        test::equal(weakObj.expired(), false, "The weak pointer is not expired");
      }
      test::equal(isDeleted, true, "The pointer should be deleted, as it was only weakly held");
      test::equal(weakObj.expired(), true, "The weak pointer is expired");
    });

    test::describe("Create a weak_ptr, but lock it", []() {
      bool isDeleted = false;
      std::weak_ptr<Object> weakObj;
      {
        std::shared_ptr<Object> lockObj;
        {
          std::shared_ptr<Object> obj(new Object(isDeleted));
          weakObj = obj;
          test::equal(isDeleted, false, "A shared pointer is created, and shared with the outer scope");
          test::equal(weakObj.expired(), false, "The weak pointer is not expired");
          lockObj = weakObj.lock();
        }
        test::equal(isDeleted, false, "The pointer is still alive, because the weak ptr locked it");
        test::equal(weakObj.expired(), false, "The weak pointer is expired");
      }
      test::equal(isDeleted, true, "The lock and original value are out of scope, the object was deleted");
      test::equal(weakObj.expired(), true, "The weak pointer is expired");
    });

  });
}

} // smartPointers
} // features
