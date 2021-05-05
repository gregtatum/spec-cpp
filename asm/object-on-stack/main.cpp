class Object {
public:
  int a = 5;
  __attribute__((optnone)) void blackbox() {}
};

int main() {
  Object obj;
  obj.blackbox();
}
