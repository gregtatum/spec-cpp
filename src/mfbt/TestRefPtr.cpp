#include "../../includes/mfbt/RefCounted.h"
#include "../../includes/mfbt/RefPtr.h"
#include "../test.h"

namespace mfbt {
namespace TestRefPtr {

/**
 * Classes must inherit RefCounted in order to be a candidate for RefPtr.
 */
class Object : public mozilla::RefCounted<Object> {
  int* mKilled;
  public:
    // This macro is required as well:
    MOZ_DECLARE_REFCOUNTED_TYPENAME(Object)

    Object(int* aKilled)
      : mKilled(aKilled)
      {}

    ~Object() {
      *mKilled = *mKilled + 1;
    }

    // This is a simple member function.
    int getOne() {
      return 1;
    }
};

class HasStaticObject {
  public:
    HasStaticObject(RefPtr<Object> aObject) {
      sObject = aObject;
    }
    static RefPtr<Object> sObject;
};

RefPtr<Object> HasStaticObject::sObject = nullptr;

/**
 * This DOES NOT WORK for RefPtr.
 */
class _BareObject {
  int* mKilled;
  public:
    _BareObject(int* aKilled)
      : mKilled(aKilled)
      {}

    ~_BareObject() {
      *mKilled = true;
    }
};

unsigned long getRefCountByCopy(RefPtr<Object> obj) {
  return obj->refCount();
}

unsigned long getRefCountByPointer(RefPtr<Object>* obj) {
  return (*obj)->refCount();
}

void run_tests() {
  test::suite("mfbt::TestRefPtr", []() {
    test::describe("RefPtr works just like a unique_ptr with one reference", []() {
      int killed = 0;
      {
        RefPtr<Object> ref = new Object(&killed);
        test::equal(ref->refCount(), 1ul, "Has one reference");
        test::equal(killed, 0, "No objects have been killed in the inner scope");
      }
      test::equal(killed, 1, "The reference counted object is killed");
    });

    test::describe("nullptr", []() {
      RefPtr<Object> ref;
      test::ok(!ref, "Uninitialized pointers are false on if checks");
      ref = nullptr;
      test::ok(!ref, "RefPtr can be assigned to a nullptr");
      int killed = 0;
      ref = new Object(&killed);
      test::ok(ref, "RefPtr of course can be assigned.");
    });

    test::describe("reassignment", []() {
      int killed = 0;
      {
        RefPtr<Object> refA = new Object(&killed);
        test::equal(refA->refCount(), 1ul, "A starts with 1 ref count");
        RefPtr<Object> refB = new Object(&killed);
        test::equal(refB->refCount(), 1ul, "B starts with 1 ref count");

        refB = refA;
        test::equal(refA->refCount(), 2ul, "The references to A are now 2");
        test::equal(refB->refCount(), 2ul, "The references to B are now 2, as it's the same as A");
        test::equal(killed, 1, "One object has been killed");
      }
      test::equal(killed, 2, "Both have been killed");
    });

    test::describe("RefPtr pointer behavior", []() {
      int killed = 0;
      RefPtr<Object> ref = new Object(&killed);
      test::equal(ref->getOne(), 1, "It works like a normal pointer calling member functions.");
    });

    test::describe("RefPtr can have multiple references and properly count them", []() {
      int killed = 0;
      {
        RefPtr<Object> firstRef;
        {
          firstRef = new Object(&killed);
          test::equal(firstRef->refCount(), 1ul, "Has one reference");
          RefPtr<Object> secondRef = firstRef;
          test::equal(firstRef->refCount(), 2ul, "Has two reference");
          test::equal(secondRef->refCount(), 2ul, "Has two reference");
          test::equal(killed, 0, "No objects have been killed in the inner scope");
        }
        test::equal(firstRef->refCount(), 1ul, "One reference was removed when it went out of scope");
        test::equal(killed, 0, "No objects have been killed in the middle scope");
      }
      test::equal(killed, 1, "The reference counted object is killed");
    });

    test::describe("observe the refcount by passing a copy of the RefPtr to a function", []() {
      int killed = 0;
      {
        RefPtr<Object> ref = new Object(&killed);
        test::equal(ref->refCount(), 1ul, "Has one reference at the beginning");

        unsigned long refCount = getRefCountByCopy(ref);
        test::equal(refCount, 2ul, "It had two reference counts in the function call.");
        test::equal(ref->refCount(), 1ul, "Has one reference after the function call");
        test::equal(killed, 0, "No objects have been killed in the inner scope");
      }
      test::equal(killed, 1, "The reference counted object is killed");
    });

    test::describe("DANGEROUS! observe the refcount by passing a pointer to the RefPtr to a function", []() {
      int killed = 0;
      {
        RefPtr<Object> ref = new Object(&killed);
        test::equal(ref->refCount(), 1ul, "Has one reference at the beginning");

        unsigned long refCount = getRefCountByPointer(&ref);
        test::equal(refCount, 1ul, "DANGER! Passing by raw pointer obliterates the contract.");
        test::equal(ref->refCount(), 1ul, "Has one reference after the function call");
        test::equal(killed, 0, "No objects have been killed in the inner scope");
      }
      test::equal(killed, 1, "The reference counted object is killed");
    });

    test::describe("RefPtr retained by a static", []() {
      int killed = 0;
      {
        RefPtr<Object> ref = new Object(&killed);
        test::equal(ref->refCount(), 1ul, "Has one reference after initialization");
        HasStaticObject hasStatic(ref);
        test::equal(ref->refCount(), 2ul, "Has two references, as one is kept as a static property");
      }
      test::equal(killed, 0, "The RefPtr was still not killed, as it is being retained by the HasStaticObj");
      HasStaticObject::sObject = nullptr;
      test::equal(killed, 1, "The reference had to be manually deleted");
    });

  });
}

} // TestRefPtr
} // mfbt
