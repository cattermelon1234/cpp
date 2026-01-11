#include <mutex>

class hierarchical_mutex {
private:
  std::mutex m_;
  unsigned long const hierarchy_value;
  unsigned long prev_hierarchy_value;
  static thread_local unsigned long cur_thread_hierarchy_value;

  void check_for_hierarchy_violation() {
    // if a thread with a lower hierarchy value tries to acquire our mutex, block
    if (cur_thread_hierarchy_value <= hierarchy_value) {
      throw std::logic_error("cannot acquire mutex of higher hierarchy value!"); 
    }
  } 

public:
  explicit hierarchical_mutex(unsigned long val): hierarchy_value(val), prev_hierarchy_value(0) {}
  
  void lock() {
    check_for_hierarchy_violation();
    m_.lock();
    // update thread hierarchy value
    prev_hierarchy_value = cur_thread_hierarchy_value;
    cur_thread_hierarchy_value = hierarchy_value;
  }
  void unlock() {
    if (cur_thread_hierarchy_value != hierarchy_value) {
      throw std::logic_error("cannot unlock mutex of higher hierarchy without unlocking lower mutex first!");
    }
    cur_thread_hierarchy_value = prev_hierarchy_value;
    m_.unlock();
  }
  bool try_lock() {
    check_for_hierarchy_violation();
    if (!m_.try_lock()) {
      return false;
    }
    prev_hierarchy_value = cur_thread_hierarchy_value;
    cur_thread_hierarchy_value = hierarchy_value;
    return true;
  }
};
