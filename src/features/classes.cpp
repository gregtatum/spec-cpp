#include "../test.h"

namespace features::classes {

static int shadowedValue = 1;

class NestedOuterClass {
public:
  int mOuterValue;
  static int mStaticValue;
  static int shadowedValue;
  class NestedInnerClass {
  public:
    // This is an error, inner cannot access non-static outer member values.
    // int getOuterValue() {
    //   return mOuterValue;
    // }

    int getOuterStaticValue() { return mStaticValue; }

    int getRootShadowedValue() { return features::classes::shadowedValue; }

    int getOuterShadowedValue() { return shadowedValue; }
  };

  explicit NestedOuterClass(int aOuterValue) : mOuterValue(aOuterValue) {}

  int getOuterValue() { return mOuterValue; }
};

int NestedOuterClass::mStaticValue = 7;
int NestedOuterClass::shadowedValue = 2;

struct Vector {
  double x = 0;
  double y = 0;
};

void run_tests() {
  test::suite("features::classes", []() {
    test::describe("nested classes", []() {
      NestedOuterClass outer(5);
      test::equal(outer.getOuterValue(), 5,
                  "Can create a NestedOuterClass on the stack.");
      NestedOuterClass::NestedInnerClass inner;
      test::equal(inner.getOuterStaticValue(), 7,
                  "Can create a NestedInnerClass, and access a static variable.");
      test::equal(inner.getRootShadowedValue(), 1,
                  "Can create a NestedInnerClass, and access a static variable.");
      test::equal(inner.getOuterShadowedValue(), 2,
                  "Can create a NestedInnerClass, and access a static variable.");
    });

    test::describe("default initialize in-class", []() {
      class DefaultInitializer {
      public:
        int mValue = 5;
        std::string mString{"This works too."};
      };

      DefaultInitializer obj;
      test::equal(obj.mValue, 5, "It can default initialize.");
      test::equal(obj.mString, std::string{"This works too."}, "It creates a string");
    });

    test::describe("inheritance with protected data", []() {
      class Object {
      protected:
        Vector position;
      };
      class Box final : public Object {
      public:
        Vector getPosition() { return position; }
      };
      Box box;
      test::equal(box.getPosition().x, 0.0,
                  "The base class's protected position can be reached");
    });

    test::describe("inheritance with virtual methods", []() {
      class Object {
      public:
        virtual double getWidth() { return 0.0; }
      };

      class Box final : public Object {};
      Object obj;
      test::equal(obj.getWidth(), 0.0,
                  "Virtual methods can be called on the base class.");
      Box box;
      test::equal(box.getWidth(), 0.0,
                  "Virtual methods can be called on the super class.");
    });

    test::describe("inheritance with virtual methods can be overridden", []() {
      class Object {
      public:
        virtual double getWidth() { return 0.0; }
      };

      class Box final : public Object {
      public:
        double getWidth() { return 2.0; }
      };

      Object obj;
      test::equal(obj.getWidth(), 0.0,
                  "Virtual methods can be called on the base class.");
      Box box;
      test::equal(box.getWidth(), 2.0,
                  "Virtual methods can be called on the super class.");
    });

    test::describe("inheritance without virtual methods can be overridden", []() {
      class Object {
      public:
        double getWidth() { return 0.0; }
      };

      class Box final : public Object {
      public:
        double getWidth() { return 2.0; }
      };

      Object obj;
      test::equal(obj.getWidth(), 0.0,
                  "Virtual methods can be called on the base class.");
      Box box;
      test::equal(box.getWidth(), 2.0,
                  "Virtual methods can be called on the super class.");
    });

    test::describe("inheritance with pure virtual methods must be overridden", []() {
      class Object {
      public:
        // The `= 0` syntax is a little obscure, but it declares a pure
        // virtual function, and this class is now magically an abstract
        // base class, and can't be instantiated or "concretized".
        virtual double getWidth() = 0;
      };

      class Box final : public Object {
      public:
        double getWidth() { return 2.0; }
      };

      COMPILER_ERROR(
          // Error:
          Object obj;
          // error: variable type 'Object' is an abstract
          // class
          //       Object obj;
          //              ^
          // note: unimplemented pure virtual method
          // 'getWidth' in 'Object'
          //         virtual double getWidth() = 0;
      );
      Box box;
      test::equal(box.getWidth(), 2.0,
                  "Pure virtual methods can be called on the super class.");
    });

    test::describe("inheriting classes must implement virtual functions", []() {
      class Object {
      public:
        // The `= 0` syntax is a little obscure, but it declares a pure
        // virtual function, and this class is now magically an abstract
        // base class, and can't be instantiated or "concretized".
        virtual double getWidth() = 0;
      };
      COMPILER_ERROR(
          // Error
          class Box final
          : public Object{};
          // unimplemented pure virtual method 'getWidth' in 'Box'
      );
    });

    test::describe("overriding methods can call the base class method", []() {
      class Object {
      public:
        double getWidth() { return size; }

      private:
        double size = 1.0;
      };

      class Box final : public Object {
      public:
        double getWidth() { return Object::getWidth() + 2; }
      };

      Object obj;
      test::equal(obj.getWidth(), 1.0,
                  "Virtual methods can be called on the base class.");
      Box box;
      test::equal(box.getWidth(), 3.0,
                  "Virtual methods can be called on the super class.");
    });

    test::describe("overriding non-virtual methods can have different signatures", []() {
      class Object {
      public:
        double getWidth() { return 0.0; }
      };

      class Box final : public Object {
      public:
        int getWidth() { return 3; }
      };

      Object obj;
      test::equal(obj.getWidth(), 0.0,
                  "Virtual methods can be called on the base class.");
      Box box;
      test::equal(box.getWidth(), 3, "Virtual methods can be called on the super class.");
    });

    test::describe("overriding virtual methods must match signatures", []() {
      class Object {
      public:
        virtual double getWidth() { return 0.0; }
      };

      /**
       * virtual function 'getWidth' has a different return type ('int') than
       * the function it overrides (which has return type 'double')
       */

      //   class Box final : public Object {
      //   public:
      //     int getWidth() { return 3; }
      //   };
    });

    test::describe("overriding virtual methods must match signatures", []() {
      class Object {
      public:
        virtual double getWidth() { return 0.0; }
      };

      class Box final : public Object {
      public:
        // The override specifiers ensures that we are implementing the override
        // correctly.
        double getWidth() override { return 3; }
        //                ^^^^^^^^
      };
    });

    test::describe("override specifier will make sure the base method is virtual", []() {
      class Object {
      public:
        // Error: Should be virtual
        double getWidth() { return 0.0; }
      };

      /**
       * error: only virtual member functions can be marked 'override'
       */
      //  class Box final : public Object {
      //  public:
      //    double getWidth() override { return 3; }
      //    //                ^^^^^^^^
      //  };
    });

    test::describe("override specifier will make sure the base method is virtual", []() {
      class Object {
      public:
        // Error: Should be virtual
        double getWidth() { return 0.0; }
      };

      /**
       * error: only virtual member functions can be marked 'override'
       */
      //  class Box final : public Object {
      //  public:
      //    double getWidth() override { return 3; }
      //    //                ^^^^^^^^
      //  };
    });

    test::describe("Functions call virtual methods on references to base class", []() {
      class Object {
      public:
        virtual double getWidth() { return 0.0; }
      };

      class Box final : public Object {
      public:
        double getWidth() override { return 3; }
      };

      struct Fns {
        static double doDynamicDispatch(Object &obj) { return obj.getWidth(); }
      };

      Object obj;
      Box box;
      test::equal(Fns::doDynamicDispatch(obj), 0.0, "Fn will call the base function.");
      test::equal(Fns::doDynamicDispatch(box), 3.0, "Fn will call the class function.");
    });

    test::describe("Functions call virtual methods on pointers to base class", []() {
      class Object {
      public:
        virtual double getWidth() { return 0.0; }
      };

      class Box final : public Object {
      public:
        double getWidth() override { return 3.0; }
      };

      struct Fns {
        static double doDynamicDispatch(Object *obj) { return obj->getWidth(); }
      };

      Object obj;
      Box box;
      test::equal(Fns::doDynamicDispatch(&obj), 0.0, "Fn will call the base function.");
      test::equal(Fns::doDynamicDispatch(&box), 3.0, "Fn will call the class function.");
    });

    test::describe("pointers are just pointers with dynamic dispatch", []() {
      class Object {
      public:
        virtual double getWidth() { return 0.0; }
      };

      class Box final : public Object {
      public:
        double getWidth() override { return 3.0; }
      };

      struct Fns {
        static size_t doDynamicDispatch(Object *obj) { return sizeof(obj); }
        static size_t doStaticDispatch(Box *obj) { return sizeof(obj); }
      };

      Object obj;
      Box box;
      test::equal(Fns::doDynamicDispatch(&obj), sizeof(uintptr_t), "It's just a pointer");
      test::equal(Fns::doDynamicDispatch(&box), sizeof(uintptr_t), "It's just a pointer");
      test::equal(Fns::doStaticDispatch(&box), sizeof(uintptr_t), "It's just a pointer");
    });

    test::describe("dynamic dispatch affects size with a vpointer member", []() {
      class VectorWrap {
        Vector position;
      };

      class BaseObject {
        Vector position;

      public:
        virtual double getWidth() { return 0.0; }
      };

      class Box final : public BaseObject {
      public:
        double getWidth() override { return 3.0; }
      };

      class ObjectInherited final : public BaseObject {};

      size_t vectorSize = sizeof(Vector);
      size_t vpointerSize = sizeof(uintptr_t);
      test::equal(sizeof(VectorWrap), vectorSize, "VectorWrap size");
      test::equal(sizeof(BaseObject), vectorSize + vpointerSize,
                  "BaseObject contains a vpointer");
      test::equal(sizeof(Box), vectorSize + vpointerSize, "Box contains a vpointer");
      test::equal(sizeof(ObjectInherited), vectorSize + vpointerSize,
                  "ObjectInherited contains a vpointer");
    });

    test::describe("⚠️ deduce the vpointers layout with clang++", []() {
      class BaseObject {
        Vector position;

      public:
        virtual double getWidth() { return 0.0; }
      };

      class Box final : public BaseObject {
      public:
        double getWidth() override { return 3.0; }
      };

      class ObjectInherited final : public BaseObject {};

      BaseObject baseObject1;
      BaseObject baseObject2;
      Box box1;
      Box box2;
      ObjectInherited objInherited1;
      ObjectInherited objInherited2;

      uintptr_t ptrBaseObject1 = *reinterpret_cast<uintptr_t *>(&baseObject1);
      uintptr_t ptrBaseObject2 = *reinterpret_cast<uintptr_t *>(&baseObject2);
      uintptr_t ptrBox1 = *reinterpret_cast<uintptr_t *>(&box1);
      uintptr_t ptrBox2 = *reinterpret_cast<uintptr_t *>(&box2);
      uintptr_t ptrObjInherited1 = *reinterpret_cast<uintptr_t *>(&objInherited1);
      uintptr_t ptrObjInherited2 = *reinterpret_cast<uintptr_t *>(&objInherited2);

      test::equal(ptrBaseObject1, ptrBaseObject2, "BaseObject pointers are the same");
      test::equal(ptrBox1, ptrBox2, "Box pointers are the same");
      test::equal(ptrObjInherited1, ptrObjInherited2,
                  "ObjectInherited pointers are the same");

      test::ok(ptrBaseObject1 != ptrBox1, "Box has a different vpointer from BaseObject");
      test::ok(ptrBaseObject1 != ptrObjInherited1,
               "ObjectInherited shares the same vpointer as BaseObject");
      test::ok(ptrBox1 != ptrObjInherited1,
               "The two inherited classes have different vpointers");
    });

    test::describe("Test simple stack destructing behavior", []() {
      static int constructed;
      static int destructed;
      class AutoObj {
      public:
        AutoObj() { constructed++; }
        ~AutoObj() { destructed++; }
      };
      {
        test::equal(constructed, 0, "No constructor yet");
        test::equal(destructed, 0, "Not destructed");
        AutoObj a;
        test::equal(constructed, 1, "Constructed");
        test::equal(destructed, 0, "Not destructed");
      }
      test::equal(constructed, 1, "Constructed");
      test::equal(destructed, 1, "Destructed");
    });

    test::describe("Test move machinery and destructors", []() {
      static int constructed = 0;
      static int destructed = 0;
      class AutoObj {
      public:
        // Move only
        AutoObj(AutoObj &&other) = default;
        AutoObj &operator=(AutoObj &&other) = default;

        AutoObj() { constructed++; }
        ~AutoObj() { destructed++; }

        static AutoObj *ConstructThenPoint() {
          AutoObj obj;
          return new AutoObj(std::move(obj));
        }
      };

      test::equal(constructed, 0, "Constructed");
      test::equal(destructed, 0, "Destructed");
      AutoObj *obj = AutoObj::ConstructThenPoint();
      test::equal(constructed, 1, "Constructed");
      test::equal(destructed, 1, "Destructed");
      delete obj;
      test::equal(constructed, 1, "Constructed");
      test::equal(destructed, 2, "Destructed");
    });

    test::describe("Test custom move operator", []() {
      static int constructed = 0;
      static int destructed = 0;
      static int moved1 = 0;
      static int moved2 = 0;
      static bool hasmOwnedPointer = false;

      class AutoObj {
      public:
        // Transfer ownership of the UEnumeration in the move constructor.
        AutoObj(AutoObj &&other) noexcept {
          moved1++;
          mOwnedPointer = other.mOwnedPointer;
          other.mOwnedPointer = nullptr;
        }

        // Transfer ownership of the UEnumeration in the move assignment operator.
        AutoObj &operator=(AutoObj &&other) noexcept {
          if (this != &other) {
            if (mOwnedPointer) {
              hasmOwnedPointer = true;
            }
            mOwnedPointer = other.mOwnedPointer;
            other.mOwnedPointer = nullptr;
            moved2++;
          }
          return *this;
        }

        static AutoObj *ConstructThenPoint(int *p) {
          AutoObj obj{p};
          return new AutoObj(std::move(obj));
        }

        int GetValue() { return *mOwnedPointer; }

        AutoObj(int *aOwnedPointer) : mOwnedPointer(aOwnedPointer) { constructed++; }
        ~AutoObj() { destructed++; }

        int *mOwnedPointer = nullptr;
      };

      int i = 0;
      int j = 1;

      test::equal(constructed, 0, "Constructed 0");
      test::equal(destructed, 0, "Destructed 0");
      test::equal(moved1, 0, "Moved 0");

      AutoObj objA = AutoObj{&i};

      test::equal(constructed, 1, "Constructed 1");
      test::equal(destructed, 0, "Destructed 0");
      test::equal(moved1, 0, "Moved 0");

      AutoObj objB = AutoObj{&j};

      test::equal(constructed, 2, "Constructed");
      test::equal(destructed, 0, "Destructed");
      test::equal(moved1, 0, "Moved1");
      test::equal(moved2, 0, "Moved2");
      test::ok(!hasmOwnedPointer, "hasmOwnedPointer false");

      objB = std::move(objA);

      test::equal(constructed, 2, "Constructed");
      test::equal(destructed, 0, "Destructed");
      test::equal(moved1, 0, "Moved1");
      test::equal(moved2, 1, "Moved2");
      test::ok(hasmOwnedPointer, "hasmOwnedPointer true");
    });

    test::describe("Uninitialized pointer values", []() {
      // Why you should initialize pointers to nullptr, just to be safe.
      class Example {
      public:
        void *pointerA = nullptr;
        void *pointerB;
      } example;

      test::equal(example.pointerA, static_cast<void *>(nullptr),
                  "Initialized values are nullptr");
      test::ok(example.pointerB != static_cast<void *>(nullptr),
               "(⚠️ very slightly possible to be equal to nullptr) Un-initialized "
               "values point to random things");
    });
  });
}

} // namespace features::classes
