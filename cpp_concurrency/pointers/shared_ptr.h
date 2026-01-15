#include <cstddef>

template <typename T>
class shared_pointer {
public:
    // Constructor
    explicit shared_pointer(T* p = nullptr): raw_ptr(p) {
      if (p) {
        count = new std::size_t(1);
      }
      else {
        count = nullptr;
      }
    }

    // Copy Constructor
    shared_pointer(const shared_pointer& other): raw_ptr(other.raw_ptr), count(other.count) {
      incr();
    }

    // Assignment Operator
    shared_pointer& operator=(const shared_pointer& other) {
      raw_ptr = other.raw_ptr;
      count = other.count;
      incr();
    }

    // Destructor
    ~shared_pointer() {
      decr();
    }

    // get(): Returns the raw pointer
    T* get() const {
      return raw_ptr;
    }

    // use_count(): Returns the reference count
    int use_count() const {
      if (!count) {
        return 0;
      }
      return *count;
    }

    // reset(): Releases ownership of the managed object
    void reset(T* new_ptr = nullptr) {
      decr();
      raw_ptr = new_ptr;
      count = new_ptr ? (new std::size_t(1)) : nullptr;
    }

    // Dereference operator
    T& operator*() const {
      return *raw_ptr;
    }

    // Member access operator
    T* operator->() const {
      return raw_ptr;
    }

    void incr() {
      if (count) {
        *count += 1;
      }
    }

    void decr() {
      if (!count) {
        return;
      }
      if (--*count == 0) {
        delete raw_ptr;
        delete count;
      }
      raw_ptr = nullptr;
      count = nullptr;
    }
private:
  T* raw_ptr;
  size_t* count;
};
