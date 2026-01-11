#include <mutex>
#include <stdexcept>
#include <climits>

class hierarchical_mutex {
  std::mutex m_;
  unsigned long const hierarchy_value;
  unsigned long prev_hierarchy_value;
  static inline thread_local unsigned long this_thread_hierarchy_value = ULONG_MAX;

  void check_for_hierarchy_violation() {
    // if a thread with a lower hierarchy value tries to acquire our mutex, block
    if (this_thread_hierarchy_value <= hierarchy_value) {
      throw std::logic_error("cannot acquire mutex of higher hierarchy value!"); 
    }
  } 

public:
  explicit hierarchical_mutex(unsigned long val): hierarchy_value(val), prev_hierarchy_value(0) {}
  
  void lock() {
    check_for_hierarchy_violation();
    m_.lock();
    // update thread hierarchy value
    prev_hierarchy_value = this_thread_hierarchy_value;
    this_thread_hierarchy_value = hierarchy_value;
  }
  void unlock() {
    if (this_thread_hierarchy_value != hierarchy_value) {
      throw std::logic_error("cannot unlock mutex of higher hierarchy without unlocking lower mutex first!");
    }
    this_thread_hierarchy_value = prev_hierarchy_value;
    m_.unlock();
  }
  bool try_lock() {
    check_for_hierarchy_violation();
    if (!m_.try_lock()) {
      return false;
    }
    prev_hierarchy_value = this_thread_hierarchy_value;
    this_thread_hierarchy_value = hierarchy_value;
    return true;
  }
};
